#!/usr/bin/env python3
"""The Jukebox's TEKKEN TAG TOURNAMENT CONSOLE music: the 14 PS2 stage pieces
of the Expanded's mods/ttt-ps2-music/ps2-music.ttp (made by its setup from the
player's TTT PS2 disc, tools/ttt_stages/ps2_music_install.py), taken into
addons/tmst/workspace/ttt-music/console/ps2-<stage>.bgm (run by the add-on's
setup, setup/tmst_setup.py).

    python tools/ttt_ps2_music.py --root <expanded folder>

ps2-music.ttp: 'TTP2', u32 format, u32 pieces, then per piece u32 PS2 BGM
number, offset of its first sector, sectors, loop sector, volume (0x3FFF =
full); each 2 048-byte sector is PS-ADPCM, 1 024 bytes left then 1 024 right,
44 100 Hz (the Expanded's src/tekken3_ttt_music.c plays it the same way).

.bgm: an ADPCM pack (tools/adpcm_pack.py, src/jukebox/tmst_adpcm.c), the
game's PS-ADPCM as it is, decoded by the Jukebox as it plays; the driver's
volume is its gain, so the pieces keep the game's levels between them.

The PS2 game's other music (its BGM table, SLUS_200.01 0x50CC00: 10 bytes per
number, u16 volume at +2, u16 TEKKEN.BIN file at +6, s16 loop sector at +8,
< 0 the start), the pieces ps2-music.ttp leaves out, comes from the disc the
Expanded's setup was given (.setup/last-inputs.json's ttt_disc), each read at
its sectors and checked by sha256 (EXTRAS): ps2-embu, ps2-ending and four
pieces ps2-bgm-NN by number. The 2 to 4 s jingles (0x17..0x1B) are left out.

No pack and no disc: nothing is made (and nothing removed). A few seconds.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
import sys
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
EXPANDED = ADDON.parents[1]                 # <expanded>/addons/tmst
sys.path.insert(0, str(ADDON / 'tools'))
import adpcm_pack  # noqa: E402
import jukebox_titles  # noqa: E402

# Where the Expanded keeps the pack: beside the game (its build), then its setup's own copy.
PACKS = ['build-release/mods/ttt-ps2-music/ps2-music.ttp', 'mods/ttt-ps2-music/ps2-music.ttp',
         'workspace/ttt-stages/ps2-music-pack/ps2-music.ttp']
OUT = 'addons/tmst/workspace/ttt-music/console'
# Increase whenever the .bgm files this writes change: the setup makes them again.
VERSION = 3

# PS2 BGM number -> stage (tools/ttt_stages/ps2_music_install.py's PIECES).
STAGES = {0x09: 'paul', 0x0A: 'law', 0x0B: 'yoshimitsu', 0x0C: 'xiaoyu', 0x0D: 'hwoarang',
          0x0E: 'lei', 0x0F: 'ogre', 0x10: 'school', 0x11: 'jin', 0x12: 'nina',
          0x13: 'eddy', 0x14: 'king', 0x15: 'heihachi', 0x16: 'unknown'}
SECTOR, HALF = 2048, 1024
SECTOR_FRAMES = HALF // 16 * 28              # 1 792
FULL = 0x3FFF

# The rest of the BGM table: number, name, first LBA, sectors, loop sector, volume, sha256 of the sectors.
# 0x01 plays before a fight (the Embu, as the Expanded's tools/ps2/ttt_disc_fingerprint.py names its
# sectors); 0x05 the name entry; 0x06 (and 0x07, the same file) the endings (tools/ps2/ttt_to_cine.py: piece 7);
# 0x04 = 0x03. 0x02, 0x03 and 0x08: --disable in data/jukebox/ttt-console.txt, so not made (skipped()).
EXTRAS = [
    (0x01, 'embu', 111953, 922, 0, 13107, '41e8f65f2162f17f09d42efa0a140f957f4483f3a9f2bf7715cab94a69e4f578'),
    (0x02, 'bgm-02', 104579, 2290, 0, 10649, 'be8c0c371506fa241dfeab36ea81dcb7f4da1cc3b118fe9c5f542f7e241c05cd'),
    (0x03, 'bgm-03', 144319, 394, 0, 9011, '9d33e5b4beaa17a350720b44d4996fcd08727689ee0921df4228b7af29b6bca5'),
    (0x05, 'bgm-05', 107120, 338, 0, 10649, 'ab1e3292cbcf58d76f5e98ced18bc57d1b34b36f8f02da2bb4de0e4a6e9e6f6f'),
    (0x06, 'ending', 107458, 4495, 0, 15564, '3675d45faf3b3203bb30704a77156f460839ea08425c95b0b20e23769cd0f741'),
    (0x08, 'bgm-08', 106869, 251, 0, 8192, 'f9ddaa8d24e31a72ed5edbaa4862988f66d307d00afa79f0e1402b5c4569e722'),
]


def find_pack(root: Path) -> Path | None:
    for rel in PACKS:
        if (root / rel).is_file():
            return root / rel
    return None


def find_disc(root: Path) -> Path | None:
    """The TTT PS2 disc the Expanded's setup was last given, if it is still there."""
    try:
        disc = json.loads((root / '.setup/last-inputs.json').read_text(encoding='utf-8')).get('ttt_disc')
    except (OSError, ValueError):
        return None
    return Path(disc) if disc and Path(disc).is_file() else None


def disc_image(disc: Path) -> tuple[Path, int, int]:
    """(image file, sector size, data offset) of a .cue (its first FILE), .bin or .iso."""
    if disc.suffix.lower() == '.cue':
        line = next(l for l in disc.read_text(errors='replace').splitlines() if l.strip().upper().startswith('FILE'))
        disc = disc.parent / line.split('"')[1]
    with disc.open('rb') as f:
        raw = f.read(12) == b'\0' + b'\xff' * 10 + b'\0'
    return disc, (2352 if raw else 2048), (24 if raw else 0)


def disc_sectors(disc: Path, lba: int, count: int) -> bytes:
    path, size, offset = disc_image(disc)
    out = bytearray()
    with path.open('rb') as f:
        for n in range(lba, lba + count):
            f.seek(n * size + offset)
            out += f.read(SECTOR)
    return bytes(out)


def pieces(pack: Path) -> list[tuple[int, int, int, int, int]]:
    """(number, offset, sectors, loop sector, volume) of each piece."""
    with pack.open('rb') as f:
        head = f.read(12)
        if len(head) < 12 or head[:4] != b'TTP2':
            raise ValueError(f'{pack}: not a TTP2 pack')
        count = struct.unpack_from('<I', head, 8)[0]
        table = f.read(20 * count)
    out = [struct.unpack_from('<5I', table, 20 * i) for i in range(count)]
    for number, offset, sectors, loop, volume in out:
        if not sectors or loop >= sectors:
            raise ValueError(f'{pack}: piece 0x{number:02X} is not whole')
    return out


def convert_piece(pack: str, piece: tuple, out: str) -> str:
    number, offset, sectors, loop, volume = piece
    with open(pack, 'rb') as f:
        f.seek(offset)
        data = f.read(sectors * SECTOR)
    if len(data) != sectors * SECTOR:
        raise ValueError(f'{pack}: piece 0x{number:02X} is cut short')
    return write_piece(data, sectors, loop, volume, out)


def extra_piece(disc: str, extra: tuple, out: str) -> str | None:
    """One of EXTRAS from the disc; None when its sectors are not the ones expected."""
    number, name, lba, sectors, loop, volume, sha = extra
    try:
        data = disc_sectors(Path(disc), lba, sectors)
    except (OSError, ValueError, StopIteration, IndexError):
        return None
    if hashlib.sha256(data).hexdigest() != sha:
        return None
    return write_piece(data, sectors, loop, volume, out)


def write_piece(data: bytes, sectors: int, loop: int, volume: int, out: str) -> str:
    """The sectors as an ADPCM pack (the game's bytes; the driver's volume as its gain)."""
    left = b''.join(data[s:s + HALF] for s in range(0, len(data), SECTOR))
    right = b''.join(data[s + HALF:s + SECTOR] for s in range(0, len(data), SECTOR))
    adpcm_pack.write(out, left, right, loop * SECTOR_FRAMES, volume / FULL)
    return Path(out).name


def skipped() -> list[str]:
    """The pieces not made: --disable in the title list and played nowhere
    else (tools/jukebox_titles.py's skipped())."""
    return jukebox_titles.skipped('ttt-music/console')


def name_of(number: int) -> str:
    return f'ps2-{STAGES.get(number, f"{number:02x}")}.bgm'


def convert(pack: Path | None, out: Path, disc: Path | None = None, say=print, skip=None) -> list[str]:
    """Every piece of pack as out/ps2-<stage>.bgm, and EXTRAS from disc as
    out/ps2-<name>.bgm, but those of skip (default skipped(), their files
    removed); the names written."""
    out.mkdir(parents=True, exist_ok=True)
    skip = set(skipped() if skip is None else skip)
    for name in skip:
        (out / name).unlink(missing_ok=True)
    jobs = [p for p in pieces(pack) if name_of(p[0]) not in skip] if pack else []
    extras = [e for e in EXTRAS if f'ps2-{e[1]}.bgm' not in skip]
    with ProcessPoolExecutor() as pool:
        names = pool.map(convert_piece, [str(pack)] * len(jobs), jobs, [str(out / name_of(p[0])) for p in jobs])
        extras = pool.map(extra_piece, [str(disc)] * len(extras), extras,
                          [str(out / f'ps2-{e[1]}.bgm') for e in extras]) if disc else []
        names, extras = list(names), list(extras)
    if disc and not all(extras):
        say(f'TTT PS2 music: {disc.name} is not the TTT USA disc (SLUS-20001, 1.00) expected, its other music left out')
    names += [n for n in extras if n]
    say(f'TTT PS2 music: {len(names)} piece(s) in {out}')
    return names


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--root', type=Path, default=EXPANDED, help='the Expanded folder (default: the one this add-on is in)')
    root = ap.parse_args(argv).root
    pack, disc = find_pack(root), find_disc(root)
    if not pack and not disc:
        print('TTT PS2 music: no mods/ttt-ps2-music/ps2-music.ttp and no TTT PS2 disc (the Expanded setup takes it)')
        return 0
    convert(pack, root / OUT, disc)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
