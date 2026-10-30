#!/usr/bin/env python3
"""Find the actors of a TTT (PS2) in-engine cutscene in a PCSX2 save state.

    python3 tools/ps2/ttt_find_actors.py STATE.p2s [...]

An actor is an array of 0xC0-byte nodes: world matrix at +0x00 (affine,
+60 = 1.0), parent node pointer at +0xB0 (inside the array, maybe a later node, except for
the root's, which points at the actor object). Prints the cutscene counter (0x00447F24) and every array of 60
nodes or more: the addresses for ttt_capture.py --actors.
"""
import struct, sys, zipfile

COUNTER = 0x447F24


def actors(ee, minimum=60):
    found, a = [], 0x100000
    while a < len(ee) - 0xC0:
        if struct.unpack_from('<f', ee, a + 60)[0] != 1.0:
            a += 16; continue
        n = 0
        while a + (n + 1) * 0xC0 <= len(ee):
            o = a + n * 0xC0
            if struct.unpack_from('<f', ee, o + 60)[0] != 1.0: break
            par = struct.unpack_from('<I', ee, o + 0xB0)[0]
            if n and not (a <= par < a + 160 * 0xC0): break
            n += 1
        if n >= minimum: found.append((a, n)); a += n * 0xC0
        else: a += 16
    return found


if __name__ == '__main__':
    for path in sys.argv[1:]:
        ee = zipfile.ZipFile(path).read('eeMemory.bin')
        print(path, 'counter', struct.unpack_from('<I', ee, COUNTER)[0],
              ' '.join(f'{a:#x}:{n}' for a, n in actors(ee)))
