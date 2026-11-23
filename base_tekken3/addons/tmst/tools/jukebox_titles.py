#!/usr/bin/env python3
"""The Jukebox's title lists: addons/tmst/workspace/jukebox/<category>.txt (the
add-on's own folder, which the Expanded's setup never cleans up) (run by the add-on's setup, setup/tmst_setup.py).

    python tools/jukebox_titles.py --root <expanded folder>

One file per category of the Jukebox (src/jukebox/tmst_jukebox.c), each line

    file = title

for the music this install has now: the .bgm files of the category's
folders, and for Tekken 3 the music the disc's executable names (xa-NN, an
XA stream of TEKKEN3.XAS; cd-track-N, a CD audio track). A new file gets a
line whose title is its name (without .bgm), the new files' lines in the
order of their titles. A title with "--disable" in it keeps that music out of
the game's list. The game lists the music in the order of these lines, then
the rest by name.

data/jukebox/<category>.txt (this add-on) are the lists every install starts
with: a list not there yet starts as that file, and a new file it names gets
its line from it (its title) rather than its name. A list the setup wrote and
nobody changed since (WRITTEN) is made again from that file, so a new title or
"--disable" there reaches it; a list written by hand keeps its lines (never
changed or reordered, the lines of music that is gone too: the game leaves
those out), but for those of the pieces the setup does not make (skipped()).

Tekken 1's two lists are only what they name: its music is the Tekken 2
folders' files the Tekken 2 disc plays again, so a new file there gets a
line in the Tekken 2 list, never in Tekken 1's.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import re
import struct
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
EXPANDED = ADDON.parents[1]                 # <expanded>/addons/tmst

# id, file, folders under the Expanded's folder; disc mode for Tekken 3.
# No folders: the list is only what it names (Tekken 1, the game's only_listed).
CATEGORIES = [
    ('t1-console', 'tekken1-console.txt', [], None),
    ('t1-arcade', 'tekken1-arcade.txt', [], None),
    ('t2-console', 'tekken2-console.txt', ['addons/tmst/workspace/t2-music/console'], None),
    ('t2-arcade', 'tekken2-arcade.txt', ['addons/tmst/workspace/t2-music/arcade'], None),
    ('t3-console', 'tekken3-console.txt', [], 0),
    ('t3-arcade', 'tekken3-arcade.txt', [], 1),
    ('ttt-console', 'ttt-console.txt', ['addons/tmst/workspace/ttt-music/console'], None),
    ('ttt-arcade', 'ttt-arcade.txt', ['addons/tmst/workspace/ttt-music/arcade'], None),
    ('t5dr', 'tekken5-dr.txt', ['addons/tmst/workspace/t5dr-music'], None),
]
DEFAULTS = ADDON / 'data/jukebox'
LISTS = 'addons/tmst/workspace/jukebox'
# What the setup last wrote into each list (sha256 by file): a list still the
# same is the setup's, made again from DEFAULTS when those change; a list
# written by hand since stays.
WRITTEN = 'addons/tmst/workspace/jukebox-titles.json'

# The music folders whose pieces the setup makes, each with the lists that name
# them (Tekken 1's name the Tekken 2 folders' files). A piece "--disable" in a
# list there and enabled in none is not made at all (tools/tekken2_music.py,
# ttt_ps2_music.py, tekken5dr_music.py), and its line is left out of the lists
# the setup writes: it would only take room. KEEP: the pieces the add-on plays
# outside the Jukebox too (src/tournament/tmst_tournament_stages.c's
# BGM SELECT), made even when the Jukebox leaves them out.
MADE = {
    't2-music/console': ('tekken2-console.txt', 'tekken1-console.txt'),
    't2-music/arcade': ('tekken2-arcade.txt', 'tekken1-arcade.txt'),
    'ttt-music/console': ('ttt-console.txt',),
    'ttt-music/arcade': ('ttt-arcade.txt',),
    't5dr-music': ('tekken5-dr.txt',),
}
KEEP = {
    't2-music/console': {'bruce.bgm', 'devil.bgm', 'jack2.bgm', 'lee.bgm', 'michelle.bgm', 'pjack.bgm'}
    | {f't2-{n}.bgm' for n in ('anna', 'armor-king', 'baek', 'ganryu', 'heihachi', 'jun', 'kazuya', 'king',
                               'kuma', 'kunimitsu', 'law', 'lei', 'nina', 'paul', 'roger', 'wang', 'yoshimitsu')},
    't2-music/arcade': {f't2-{n}.bgm' for n in (
        'anna', 'armor-king', 'baek', 'bruce', 'devil', 'ganryu', 'heihachi', 'jack-2', 'jun', 'kazuya', 'king',
        'kuma', 'kunimitsu', 'law', 'lee', 'lei', 'michelle', 'nina', 'paul', 'pjack', 'roger', 'wang',
        'yoshimitsu')},
    'ttt-music/console': {f'ps2-{n}.bgm' for n in (
        'paul', 'law', 'yoshimitsu', 'xiaoyu', 'hwoarang', 'lei', 'ogre', 'school', 'jin', 'nina', 'eddy', 'king',
        'heihachi', 'unknown')},
    'ttt-music/arcade': {f'arcade-{n}.bgm' for n in (
        'paul', 'law', 'yoshimitsu', 'xiaoyu', 'hwoarang', 'lei', 'ogre', 'school', 'jin', 'nina', 'eddy', 'king',
        'heihachi', 'unknown')},
}


def skipped(folder: str) -> list[str]:
    """The pieces of workspace/<folder> not made (see MADE), by name."""
    off, on = set(), set()
    for file in MADE[folder]:
        for name, line in default_lines(file).items():
            name = name if name.endswith('.bgm') else name + '.bgm'
            (off if '--disable' in line.split('=', 1)[1] else on).add(name)
    return sorted(off - on - KEEP.get(folder, set()))


def skipped_in(file: str) -> set[str]:
    """The pieces a list's lines are left out for: those of the folders it names."""
    return {name for folder, files in MADE.items() if file in files for name in skipped(folder)}
HEADER = ('# Jukebox titles, {name}: one line per music file, "file = title".\n'
          '# Write the title you want after the "="; the setup only adds lines for new files.\n'
          '# "--disable" after the "=" (with or without a title) leaves that music out of the list.\n')

# SLUS-00402 (load address 0x80010000, header 0x800): the XA table
# (0x80024D78, 50 x 12 bytes) and the music table (0x80024FD0, per music id
# 4 bytes: the XA of mode 0 (arranged) and its volume, then mode 1 (arcade);
# bit 7 of the XA byte: a CD audio track).
EXE_BASE = 0x80010000 - 0x800
XA_TABLE, XA_COUNT, MUSIC_TABLE, MUSIC_IDS = 0x80024D78, 50, 0x80024FD0, 32


def disc_music(root: Path, mode: int) -> list[str]:
    """The Tekken 3 music the executable names, from the copy the Expanded's
    setup keeps in disc/ (SLUS_004.02)."""
    exe = root / 'disc' / 'SLUS_004.02'
    if not exe.is_file():
        return []
    data = exe.read_bytes()
    xa = set()
    for music in range(MUSIC_IDS):
        b = data[MUSIC_TABLE - EXE_BASE + music * 4 + mode * 2]
        if not b & 0x80 and b < XA_COUNT:
            first, last, channel = struct.unpack_from('<3I', data, XA_TABLE - EXE_BASE + b * 12)
            if channel < 8 and first <= last:
                xa.add(b)
    names = [f'xa-{i:02d}' for i in sorted(xa)]
    if mode == 0:                       # the disc's CD audio, as the game lists it
        names += [f'cd-track-{t}' for t in audio_tracks(root) if t >= 2]
    return names


def audio_tracks(root: Path) -> list[int]:
    cues = sorted((root / 'disc').glob('*.cue'))
    if not cues:
        return []
    tracks = re.findall(r'TRACK\s+(\d+)\s+AUDIO', cues[0].read_text(errors='replace'), re.I)
    return [int(t) for t in tracks]


def music_files(root: Path, folders) -> list[str]:
    names = []
    for folder in folders:
        d = root / folder
        if d.is_dir():
            names += [p.name for p in sorted(d.glob('*.bgm')) if p.name not in names]
    return names


def listed(text: str) -> set[str]:
    names = set()
    for line in text.splitlines():
        line = line.strip()
        if line and not line.startswith('#') and '=' in line:
            name = line.split('=', 1)[0].strip()
            names.add(name)
            if not name.endswith('.bgm'):
                names.add(name + '.bgm')
    return names


def default_lines(file: str) -> dict[str, str]:
    """data/jukebox/<file>'s lines by the music file they name."""
    path = DEFAULTS / file
    lines = {}
    for line in (path.read_text(encoding='utf-8').splitlines() if path.is_file() else []):
        if line.strip() and not line.lstrip().startswith('#') and '=' in line:
            lines[line.split('=', 1)[0].strip()] = line.strip()
    return lines


def without(text: str, names: set[str]) -> str:
    """text without the lines of names (with or without .bgm)."""
    def named(line: str) -> str:
        name = line.split('=', 1)[0].strip()
        return name if name.endswith('.bgm') else name + '.bgm'
    return ''.join(line for line in text.splitlines(keepends=True)
                   if not (line.strip() and not line.lstrip().startswith('#') and '=' in line and named(line) in names))


def digest(text: str) -> str:
    return hashlib.sha256(text.encode('utf-8')).hexdigest()


def update(root: Path) -> int:
    out = root / LISTS
    out.mkdir(parents=True, exist_ok=True)
    state = root / WRITTEN
    try:
        written = json.loads(state.read_text(encoding='utf-8'))
    except (OSError, ValueError):
        written = {}
    added = 0
    for cid, file, folders, mode in CATEGORIES:
        names = disc_music(root, mode) if mode is not None else music_files(root, folders)
        path = out / file
        old = path.read_text(encoding='utf-8') if path.is_file() else None
        ours = old is None or written.get(file) == digest(old)
        if not ours:                    # written by hand: it stays
            text = old
        elif (DEFAULTS / file).is_file():
            text = (DEFAULTS / file).read_text(encoding='utf-8')
        else:
            text = old if old is not None else HEADER.format(name=cid)
        skip = skipped_in(file)
        text = without(text, skip)
        have = listed(text)
        new = [n for n in names if n not in have and n not in skip]
        if text and not text.endswith('\n'):
            text += '\n'
        defaults = default_lines(file)
        lines = [defaults.get(n, f'{n} = {n[:-4] if n.endswith(".bgm") else n}') for n in new]
        if mode is None:                # files: in the order of their titles (the disc's music: its own)
            lines.sort(key=lambda line: line.split('=', 1)[1].strip().casefold())
        text += ''.join(line + '\n' for line in lines)
        if ours:
            written[file] = digest(text)
        if text == old:
            continue
        path.write_text(text, encoding='utf-8')
        added += len(new)
        print(f'{path.relative_to(root)}: {"made again from " + str(DEFAULTS.name) if ours and old else "written"}'
              f', {len(new)} new line(s)')
    state.write_text(json.dumps(written, indent=1), encoding='utf-8')
    print(f'Jukebox titles: {added} new line(s)')
    return 0


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--root', type=Path, default=EXPANDED, help='the Expanded folder (default: the one this add-on is in)')
    return update(ap.parse_args(argv).root)


if __name__ == '__main__':
    raise SystemExit(main())
