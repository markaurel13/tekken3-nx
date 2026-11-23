#!/usr/bin/env python3
"""The Jukebox's TEKKEN 2 CONSOLE / ARCADE music (and with it TEKKEN 1's: the
ten Tekken 1 stage pieces the Tekken 2 disc plays again as its sub-bosses'
themes), from the player's Tekken 2 PlayStation disc (tools/tekken2_disc.py,
remembered by the add-on's setup in workspace/tekken2-disc.json) into
addons/tmst/workspace/t2-music/{console,arcade}/*.bgm (run by the add-on's
setup, setup/tmst_setup.py).

    python tools/tekken2_music.py --disc <Tekken 2 disc> [--root <expanded folder>]

The disc (Europe SCES-00255 and USA SLUS-00213 alike: the same tables at the
same offsets of the program, the same music):
- TEKKEN2/TEKKEN2.XAS, the music as XA audio: eight interleaved channels,
  each sector 18 sound groups of 128 bytes (4-bit ADPCM, stereo, 37 800 Hz);
- the program's XA table (file offset 0x1C90): 12 bytes a track, its first
  and last sector in TEKKEN2.XAS and its channel (the first sector is already
  the channel's: every eighth sector from it);
- its music table (file offset 0x2088, RAM 0x80011888): per music id two
  4-byte entries, the arranged music (Console) and the arcade's: flag (1: a CD
  audio track), volume, XA track (s16);
- two CD audio tracks (2 and 3), the music flagged 1.
Both tables are checked by their SHA-256: another version's music is left out.

Which file is which (PIECES): the file names of the add-on's title lists
(data/jukebox/tekken1|2-console|arcade.txt), each a music id of a mode, a
fighter's (its record's music id) or one no fighter has (t2-music-NN).

.bgm: an XA pack (tools/adpcm_pack.py's write_xa, src/jukebox/tmst_adpcm.c),
the track's sound groups as they are, which the Jukebox decodes and brings to
44 100 Hz (a Hann-windowed sinc, resample()'s) as it plays, looping whole; its
gain to Tekken 3's music loudness (the RMS of the Jukebox's other packs),
measured here on the frames the Jukebox will play. A CD audio track (and XA
that is not stereo 37 800 Hz) is the frames themselves ('T3BGM001', u32
44 100, u32 frames, u32 loop frame 0, s16 left and right), a CD track's
silence either side cut.

No disc: nothing is made (and nothing removed). Pure Python with numpy (the
Expanded's setup has it), one process per piece: a minute or two.
"""
from __future__ import annotations

import argparse
import hashlib
import os
import shutil
import struct
import sys
from array import array
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

import numpy as np

ADDON = Path(__file__).resolve().parents[1]
EXPANDED = ADDON.parents[1]                 # <expanded>/addons/tmst
sys.path.insert(0, str(ADDON / 'tools'))
from disc_image import DiscError, open_disc  # noqa: E402
from tekken2_disc import REGIONS  # noqa: E402
import adpcm_pack  # noqa: E402
import jukebox_titles  # noqa: E402

OUT = 'addons/tmst/workspace/t2-music'
# Increase whenever the .bgm files this writes change: the setup makes them again.
VERSION = 2

XA_FILE = 'TEKKEN2/TEKKEN2.XAS'
XA_TABLE, XA_TRACKS = 0x1C90, 56            # the music's tracks (from 56 the films' sound)
MUSIC_TABLE, MUSIC_IDS = 0x2088, 28
TABLES_SHA256 = ('1cb97b06c9c8a02a0db24e6d3ced308d0e3855710a6ae362943d81a56a875a64',
                 'ae03a8fab7ed5496d34459a238a8e009fcdf518475feb1d9743deabe9dc9ce14')
XA_RATE, RATE, TAPS = 37800, 44100, 16
RMS = 3912 * 10000 // 7600                  # Tekken 3's music before the Jukebox's FILE_LEVEL
K0, K1 = (0, 60, 115, 98, 122), (0, 0, -52, -55, -60)
CHUNK = 1 << 18                             # resample()'s share at a time
# A piece's resampling takes a few hundred MB: half the cores, at most 6.
JOBS = max(1, min(6, (os.cpu_count() or 2) // 2))

# The fighters' music ids (the program's fighter records, RAM 0x800A39E4: byte 7).
FIGHTERS = {'heihachi': 0, 'paul': 1, 'law': 2, 'jack-2': 3, 'nina': 4, 'king': 5, 'yoshimitsu': 6,
            'michelle': 7, 'jun': 8, 'lei': 9, 'kazuya': 11, 'devil': 12, 'roger': 13, 'baek': 14,
            'kunimitsu': 19, 'anna': 20, 'lee': 21, 'wang': 22, 'armor-king': 23, 'kuma': 24,
            'ganryu': 25, 'bruce': 26, 'pjack': 27}
EXTRA_MUSIC = (10, 15, 17, 18)              # no fighter's
# Console: the arranged music (mode 0); a few files keep the names the title lists gave them.
CONSOLE_NAMES = {'devil': 'devil', 'jack-2': 'jack2', 'michelle': 'michelle', 'pjack': 'pjack',
                 'lee': 'lee', 'bruce': 'bruce'}
CD_TRACKS = (2, 3)


def pieces() -> list[tuple[str, str, tuple]]:
    """(folder, file, source): source ('xa', music id, mode) or ('cd', track)."""
    out = []
    for folder, mode in (('console', 0), ('arcade', 1)):
        names = CONSOLE_NAMES if mode == 0 else {}
        for fighter, music_id in FIGHTERS.items():
            out.append((folder, f'{names.get(fighter, "t2-" + fighter)}.bgm', ('xa', music_id, mode)))
        out += [(folder, f't2-music-{m}.bgm', ('xa', m, mode)) for m in EXTRA_MUSIC]
    out += [('console', f't2-cd-track-{t}.bgm', ('cd', t)) for t in CD_TRACKS]
    return out


def skipped() -> list[str]:
    """The pieces not made ('<folder>/<file>'): --disable in the title lists
    and played nowhere else (tools/jukebox_titles.py's skipped())."""
    return [f'{folder}/{name}' for folder in ('console', 'arcade')
            for name in jukebox_titles.skipped(f't2-music/{folder}')]


def find_disc(state: Path) -> Path | None:
    """The Tekken 2 disc the add-on's setup remembered (workspace/tekken2-disc.json), if it is still there."""
    import json
    try:
        disc = json.loads((state / 'tekken2-disc.json').read_text(encoding='utf-8')).get('disc') or {}
    except (OSError, ValueError):
        return None
    path = disc.get('path')
    return Path(path) if path and Path(path).is_file() else None


def tables(disc) -> tuple[list, dict]:
    """The XA tracks [(first, last, channel)] and the music {(id, mode): (flag, volume, track)}."""
    exe = next((e for e in (disc.read_file(name) for name in REGIONS) if e), None)
    if exe is None:
        raise DiscError('not a Tekken 2 disc (no SCES_002.55 or SLUS_002.13)')
    xa = exe[XA_TABLE:XA_TABLE + 12 * XA_TRACKS]
    music = exe[MUSIC_TABLE:MUSIC_TABLE + 8 * MUSIC_IDS]
    if (hashlib.sha256(xa).hexdigest(), hashlib.sha256(music).hexdigest()) != TABLES_SHA256:
        raise DiscError('a Tekken 2 version whose music tables are not the ones known '
                        '(Europe SCES-00255, USA SLUS-00213)')
    tracks = [struct.unpack_from('<3I', xa, 12 * i) for i in range(XA_TRACKS)]
    entries = {(m, mode): struct.unpack_from('<BBh', music, 8 * m + 4 * mode)
               for m in range(MUSIC_IDS) for mode in (0, 1)}
    return tracks, entries


def groups(disc, xas: int, track: tuple) -> tuple[bytes, bool]:
    """An XA track's sound groups: every eighth sector from its first, those of
    its channel; and whether they are all stereo 4-bit 37 800 Hz (coding byte 1)."""
    first, last, channel = track
    out = bytearray()
    plain = True
    for n in range(first, last + 1, 8):
        raw = disc.raw(xas + n)
        if raw[17] == channel and raw[18] & 4:
            out += raw[24:24 + 18 * 128]
            plain = plain and raw[19] & 0x3F == 1
    return bytes(out), plain


def decode_groups(data: bytes) -> np.ndarray:
    """Sound groups as stereo frames at 37 800 Hz: each group 8 units of 28
    samples (4-bit; even units left, odd right), filter and shift from the
    group's header bytes 4..11; the filter (integer, clamped to 16 bits) runs
    on from unit to unit of a side."""
    g = np.frombuffer(data, np.uint8).reshape(-1, 128)
    nibbles = g[:, 16:].reshape(-1, 28, 4).transpose(0, 2, 1)           # group, unit >> 1, sample
    sides = []
    for side in (0, 1):
        p = g[:, 4 + side:12:2].reshape(-1).astype(np.int64)            # units side, side + 2, ...
        v = (nibbles >> 4 if side else nibbles & 15).reshape(-1, 28).astype(np.int64)
        v[v >= 8] -= 16
        shift = p & 15
        v = (v << 12) >> np.where(shift > 12, 9, shift)[:, None]       # 13..15 as 9 (the PlayStation's)
        out = array('h')
        put = out.append
        s1 = s2 = 0
        for row, f in zip(v.tolist(), (p >> 4 & 3).tolist()):
            k0, k1 = K0[f], K1[f]
            for r in row:
                r += (s1 * k0 + s2 * k1 + 32) >> 6
                r = 32767 if r > 32767 else -32768 if r < -32768 else r
                put(r)
                s2 = s1
                s1 = r
        sides.append(np.frombuffer(out, np.int16))
    return np.stack(sides, 1).astype(np.float64)


def resample(x: np.ndarray) -> np.ndarray:
    """37 800 -> 44 100 Hz: each output sample from the 2 * TAPS input samples
    round it, a Hann-windowed sinc; CHUNK samples at a time."""
    n = len(x) * RATE // XA_RATE
    padded = np.pad(x, ((TAPS, TAPS + 1), (0, 0)))
    y = np.empty((n, x.shape[1]))
    for a in range(0, n, CHUNK):
        pos = np.arange(a, min(n, a + CHUNK)) * (XA_RATE / RATE)
        base = np.floor(pos).astype(np.int64)
        values, which = np.unique(pos - base, return_inverse=True)
        part = np.zeros((len(pos), x.shape[1]))
        for k in range(-TAPS + 1, TAPS + 1):
            d = values - k
            w = np.sinc(d) * (0.5 + 0.5 * np.cos(np.pi * d / TAPS))
            part += padded[base + k + TAPS] * w[which][:, None]
        y[a:a + len(pos)] = part
    return y


def gain_of(pcm: np.ndarray) -> float:
    """The gain to Tekken 3's music loudness (no frame clipped)."""
    return min(RMS / max(1.0, float(np.sqrt((pcm ** 2).mean()))), 32767 / max(1.0, float(np.abs(pcm).max())))


def write(path: Path, pcm: np.ndarray) -> None:
    """A .bgm of the frames themselves, at Tekken 3's music loudness, looping whole."""
    frames = np.clip(np.round(pcm * gain_of(pcm)), -32768, 32767).astype('<i2')
    temp = path.with_name(path.name + '.new')
    temp.write_bytes(b'T3BGM001' + struct.pack('<3I', RATE, len(frames), 0) + frames.tobytes())
    temp.replace(path)


def make(disc_path: str, source: tuple, out: str) -> str:
    """One piece (in a worker process): read from the disc, decoded, written."""
    with open_disc(disc_path) as disc:
        if source[0] == 'cd':
            data = disc.audio(source[1])
            pcm = np.frombuffer(data[:len(data) // 4 * 4], '<i2').reshape(-1, 2).astype(np.float64)
            audible = np.nonzero(np.abs(pcm).max(1) > 0)[0]
            if not len(audible):
                raise DiscError(f'CD audio track {source[1]} is silent')
            pcm = pcm[audible[0]:audible[-1] + 1]
        else:
            tracks, _ = tables(disc)
            xas = disc.locate(XA_FILE)[0]
            data, plain = groups(disc, xas, tracks[source[1]])
            pcm = resample(decode_groups(data))
            if plain:                               # the XA as it is, its gain measured as the Jukebox plays it
                adpcm_pack.write_xa(out, data, gain_of(pcm))
                return Path(out).name
    write(Path(out), pcm)
    return Path(out).name


def convert(disc: Path, out: Path, say=print, jobs: int = JOBS, skip=None) -> list[str]:
    """Every piece of PIECES as out/<folder>/<file> but those of skip (default
    skipped(), their files removed); the files written ('<folder>/<file>').
    DiscError when the disc's music is not the one known."""
    skip = set(skipped() if skip is None else skip)
    with open_disc(disc) as d:
        if not d.locate(XA_FILE):
            raise DiscError(f'{disc.name} has no {XA_FILE}')
        tracks, music = tables(d)
    # The same XA track (or CD track) is made once, then copied.
    todo: dict[tuple, list[Path]] = {}
    for folder, name, source in pieces():
        if f'{folder}/{name}' in skip:
            (out / folder / name).unlink(missing_ok=True)
            continue
        if source[0] == 'xa':
            flag, _, track = music[source[1], source[2]]
            if flag or not 0 <= track < len(tracks):
                say(f'Tekken 2 music: {name} ({folder}) is not an XA track, left out')
                continue
            key = ('xa', track)
        else:
            key = source
        todo.setdefault(key, []).append(out / folder / name)
    for folder in ('console', 'arcade'):
        (out / folder).mkdir(parents=True, exist_ok=True)
    # The longest first, so the workers end together.
    order = sorted(todo, key=lambda k: -(tracks[k[1]][1] - tracks[k[1]][0] if k[0] == 'xa' else 1 << 30))
    names = []
    with ProcessPoolExecutor(max_workers=max(1, jobs)) as pool:
        futures = [(key, pool.submit(make, str(disc), ('xa', key[1]) if key[0] == 'xa' else key, str(todo[key][0])))
                   for key in order]
        for key, future in futures:
            future.result()
            first, *others = todo[key]
            for other in others:
                shutil.copyfile(first, other)
            names += [f'{p.parent.name}/{p.name}' for p in todo[key]]
    say(f'Tekken 2 music: {len(names)} piece(s) in {out}')
    return sorted(names)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--root', type=Path, default=EXPANDED, help='the Expanded folder (default: the one this add-on is in)')
    ap.add_argument('--disc', type=Path, help='the Tekken 2 disc (default: the one the add-on setup remembered)')
    ap.add_argument('--jobs', type=int, default=JOBS, help=f'pieces made at a time (default {JOBS})')
    args = ap.parse_args(argv)
    disc = args.disc or find_disc(args.root / 'addons/tmst/workspace')
    if not disc:
        print('Tekken 2 music: no Tekken 2 disc (Setup TMST.cmd asks for it)')
        return 0
    convert(disc, args.root / OUT, jobs=args.jobs)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
