#!/usr/bin/env python3
"""Retrofit the hand-pose table of imported guest models (<Name>-TTT1-arcade-P<n>.3dm),
without a new import. Same remapping as convert.hand_steps: indices 0..3 of each hand's
pose table run from open hand to fist (poses 0, 5, 11 and 16 of TTT1's 17), the others
fall back to pose 0. Short tables (fewer than 8 poses) are left alone.

A table remapped before import version 6 took the last pose as the fist: Baek's, Lee's
and Ganryu's 18th or Armor King's 19th, an open hand after the progression. The pose
lists of a hand are converted one after the other, each the same size, so the poses it
picked (0, 1/3, 2/3, last of n) are evenly spaced and pose 16 is found from them.

    python3 tools/ttt1_retrofit_hands.py workspace/ttt1-import/roster
"""
import struct, sys
from pathlib import Path

REGULAR = 17                  # TTT1's open -> fist progression (convert.REGULAR_POSES)


def steps(n):
    return [round(i * (min(n, REGULAR) - 1) / 3) for i in range(4)]


def remapped_fist(e):
    """The first four entries of a table remapped with the last of n poses as its fist:
    the same four with pose 16 instead, or None when they already have it."""
    for n in range(REGULAR + 1, 2 * REGULAR):
        last = [round(i * (n - 1) / 3) for i in range(4)]
        size, rest = divmod(e[1] - e[0], last[1])
        if size > 0 and not rest and all(e[i] - e[0] == size * last[i] for i in range(4)):
            return [e[0] + size * s for s in steps(n)]
    return None


def retrofit(data):
    """Rewrites `data` (a PS1 3DMK model) in place; number of tables remapped."""
    rows = struct.unpack_from('<I', data, 0)[0]
    done = 0
    for r in range(rows):
        w = struct.unpack_from('<14I', data, 24 + 56 * r)
        if w[13] <= 2 or r not in (13, 17): continue      # the hands only (Devil's wings keep their poses)
        n = 0
        while w[13] + 4 * (n + 1) <= len(data) and struct.unpack_from('<I', data, w[13] + 4 * n)[0] > 0x100: n += 1
        e = list(struct.unpack_from(f'<{n}I', data, w[13]))
        poses = list(dict.fromkeys(e))
        if len(poses) >= 8 and e == poses + [e[0]] * (n - len(poses)):          # TTT1's table, as imported
            new = [poses[s] for s in steps(len(poses))]
        elif n >= 4 and len(set(e[:4])) == 4 and e[4:] == [e[0]] * (n - 4):    # remapped
            new = remapped_fist(e[:4])
            if not new: continue
        else: continue
        struct.pack_into(f'<{n}I', data, w[13], *(new + [new[0]] * (n - 4)))
        done += 1
    return done


def main(paths):
    for root in paths:
        for f in sorted(Path(root).rglob('*-TTT1-arcade-P*.3dm')):
            data = bytearray(f.read_bytes()); n = retrofit(data)
            if n: f.write_bytes(data)
            print(f'{f.name}: {n} table(s) remapped' if n else f'{f.name}: unchanged')


if __name__ == '__main__':
    main(sys.argv[1:] or ['workspace/ttt1-import/roster'])
