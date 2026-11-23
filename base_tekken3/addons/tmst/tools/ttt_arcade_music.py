#!/usr/bin/env python3
"""The Jukebox's TEKKEN TAG TOURNAMENT ARCADE music: the 14 arcade stage
pieces of the Expanded's mods/ttt-music/music.ttm (made by its setup from the
player's tektagt.zip, tools/ttt_stages/music_install.py), rendered into
addons/tmst/workspace/ttt-music/arcade/arcade-<stage>.bgm (run by the add-on's
setup, setup/tmst_setup.py).

    python tools/ttt_arcade_music.py --root <expanded folder> [--cc COMPILER]

music.ttm holds what TTT's sound driver wrote to its C352 sound chip and the
chip's sample banks, not audio: tools/ttm_render.c replays the writes on the
Expanded's own C352 (src/tekken3_ttt_music.c), built here with the C compiler
given (the setup passes the Expanded's pinned clang), into .bgm packs at the
game's level for these pieces (TEKKEN3_TTT_MUSIC_VOLUME's 500 %). This file
inflates music.ttm ('TTTM', u32 format, u32 inflated size, zlib) for it.

No music.ttm: nothing is made (and nothing removed). About ten seconds.
"""
from __future__ import annotations

import argparse
import os
import shutil
import struct
import subprocess
import sys
import tempfile
import zlib
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
EXPANDED = ADDON.parents[1]                 # <expanded>/addons/tmst

# Where the Expanded keeps the pack: beside the game (its build), then its setup's own copy.
PACKS = ['build-release/mods/ttt-music/music.ttm', 'mods/ttt-music/music.ttm',
         'workspace/ttt-stages/music-pack/music.ttm']
OUT = 'addons/tmst/workspace/ttt-music/arcade'
RENDER = ADDON / 'tools/ttm_render.c'
FORMAT = 1
GAIN = 500                                   # src/tekken3_ttt_music.c's gain_percent()
# Increase whenever the .bgm files this writes change: the setup makes them again.
VERSION = 1

# Piece number -> stage (STAGE_PIECES of src/tekken3_ttt_music.c; Eddy-A shares Eddy's).
STAGES = {0x11: 'paul', 0x1E: 'law', 0x16: 'yoshimitsu', 0x10: 'xiaoyu', 0x19: 'hwoarang',
          0x13: 'lei', 0x1C: 'ogre', 0x1B: 'school', 0x17: 'jin', 0x15: 'nina',
          0x18: 'eddy', 0x14: 'king', 0x1A: 'heihachi', 0x12: 'unknown'}


def find_pack(root: Path) -> Path | None:
    for rel in PACKS:
        if (root / rel).is_file():
            return root / rel
    return None


def compiler(given: str | None = None) -> str | None:
    for name in (given, os.environ.get('CC'), 'clang', 'gcc', 'cc'):
        if name and (Path(name).is_file() or shutil.which(name)):
            return name
    return None


def inflate(pack: Path) -> bytes:
    data = pack.read_bytes()
    if len(data) < 12 or data[:4] != b'TTTM' or struct.unpack_from('<I', data, 4)[0] != FORMAT:
        raise ValueError(f'{pack}: not a music file of format {FORMAT}')
    body = zlib.decompress(data[12:])
    if len(body) != struct.unpack_from('<I', data, 8)[0]:
        raise ValueError(f'{pack}: does not inflate whole')
    return body


def name_of(number: int) -> str:
    return f'arcade-{STAGES.get(number, f"{number:02x}")}.bgm'


def convert(pack: Path, out: Path, cc: str, say=print) -> list[str]:
    """Every piece of pack as out/arcade-<stage>.bgm; the names written."""
    body = inflate(pack)
    out.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='tmst-ttm-') as tmp:
        tmp = Path(tmp)
        exe = tmp / ('ttm_render.exe' if os.name == 'nt' else 'ttm_render')
        built = subprocess.run([cc, '-O2', '-o', str(exe), str(RENDER)], capture_output=True, text=True)
        if built.returncode:
            raise RuntimeError(f'{RENDER.name} does not build with {cc}:\n{built.stderr.strip()}')
        (tmp / 'body.bin').write_bytes(body)
        (tmp / 'out').mkdir()
        ran = subprocess.run([str(exe), str(tmp / 'body.bin'), str(tmp / 'out'), str(GAIN)],
                             capture_output=True, text=True)
        if ran.returncode:
            raise RuntimeError(f'{RENDER.name}: {ran.stderr.strip()}')
        names = []
        for line in ran.stdout.split('\n'):
            if not line.strip():
                continue
            number = int(line.split()[0], 16)
            name = name_of(number)
            # shutil.move: the temp folder may be on another drive than the game.
            shutil.move(str(tmp / 'out' / f'piece-{number:02X}.bgm'), str(out / name))
            names.append(name)
    say(f'TTT arcade music: {len(names)} piece(s) in {out}')
    return names


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--root', type=Path, default=EXPANDED, help='the Expanded folder (default: the one this add-on is in)')
    ap.add_argument('--cc', help='the C compiler (default: CC, then clang, gcc, cc)')
    args = ap.parse_args(argv)
    pack = find_pack(args.root)
    if not pack:
        print('TTT arcade music: no mods/ttt-music/music.ttm (the Expanded setup makes it from tektagt.zip)')
        return 0
    cc = compiler(args.cc)
    if not cc:
        print('TTT arcade music: no C compiler', file=sys.stderr)
        return 1
    convert(pack, args.root / OUT, cc)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
