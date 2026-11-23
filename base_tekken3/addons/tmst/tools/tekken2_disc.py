"""The player's Tekken 2 PlayStation disc, for what the add-on imports from it
(the setup checks it and remembers it; tools/tekken2_music.py takes its music).

Supported: Tekken 2 USA (SLUS-00213, every version) and Europe (SCES-00255),
as a .cue with its .bin files, a .bin (with its .cue beside it, or a
single-track one) or a .chd (tools/disc_image.py). The disc is told by the
program its SYSTEM.CNF boots.
"""
from __future__ import annotations
import re
from pathlib import Path
from disc_image import DiscError, EXTENSIONS, open_disc

VERSION = 1
REGIONS = {'SLUS_002.13': 'USA', 'SCES_002.55': 'Europe'}
EXPECTED = ('Expected: Tekken 2 for PlayStation, USA (SLUS-00213) or Europe (SCES-00255), '
            'as a .cue with its .bin files, a .bin or a .chd.')


def identify(path) -> dict:
    """{'path', 'region', 'serial', 'tracks'} of a supported disc; DiscError otherwise."""
    path = Path(path)
    if path.suffix.lower() not in EXTENSIONS:
        raise DiscError(f'{path.name} is not a disc image. {EXPECTED}')
    with open_disc(path) as disc:
        cnf = disc.read_file('SYSTEM.CNF')
        if cnf is None:
            raise DiscError(f'{path.name} is not a PlayStation disc. {EXPECTED}')
        m = re.search(rb'BOOT\s*=\s*cdrom:\\?(?:.*\\)?([A-Z]{4}_\d{3}\.\d{2})', cnf, re.I)
        boot = m[1].decode('ascii').upper() if m else '?'
        if boot not in REGIONS:
            raise DiscError(f'{path.name} boots {boot.replace("_", "-").replace(".", "")}, not Tekken 2. {EXPECTED}')
        return {'path': str(path.resolve()), 'region': REGIONS[boot],
                'serial': boot.replace('_', '-').replace('.', ''), 'tracks': len(disc.tracks)}


def search(folders) -> list[Path]:
    """Disc images in the given folders (not below) whose name says Tekken 2."""
    found = []
    for folder in folders:
        try:
            files = [f for f in sorted(Path(folder).iterdir())
                     if f.suffix.lower() in EXTENSIONS and re.search(r'tekken[ _-]*(2|ii)\b', f.stem, re.I)]
        except OSError:
            continue
        # A .cue or .chd first; a .bin only where neither is (its own .cue is found from it).
        found += [f for f in files if f.suffix.lower() != '.bin'] or files
    return found
