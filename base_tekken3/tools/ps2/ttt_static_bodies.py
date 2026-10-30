#!/usr/bin/env python3
"""The static bodies of a TTT (PS2) ending (Bruce's last shot: the beaten
Tekken Force soldiers, Kazuya and Heihachi), from a PCSX2 save state taken
while they are on screen.

    python3 tools/ps2/ttt_static_bodies.py STATE.p2s OUT.json [INDEX ...]

A list of 0x90-byte records at 0x01FBF590 (empty until the shot): world
matrix +0x00 (no rotation in Bruce's: only the position), then +0x40 a
descriptor (0x0044D140 table), +0x44 a small table, +0x48 the pose block,
+0x4C base position (3 floats), +0x5C angle, +0x64 scale (1.2), +0x6C index
<< 16, +0x70 type << 16 (1 Kazuya, 2 Heihachi, 0 a soldier), +0x80 next.
The pose block (0xE8 bytes, 0x00FCD310 + 0xE8 * index) is read by
0x00367CA0(block, out, frame, 57): u16 frame count, then frame 0's 57
channels as they are (a TTT1 pose: 3 root channels, 3 angles for each of
18 bones; the whole orientation is in it), then compressed deltas. INDEX:
the records to keep (default all; the pile around Heihachi and Kazuya is
1-7 and 25-28).
"""
import json, struct, sys, zipfile

LIST, STRIDE, COUNT = 0x01FBF590, 0x90, 30
TYPES = {0: 'soldier', 1: 'kazuya', 2: 'heihachi'}


def bodies(ee, keep=None):
    out = []
    for i in range(COUNT):
        rec = LIST + STRIDE * i
        if keep and i not in keep: continue
        matrix = list(struct.unpack_from('<16f', ee, rec))
        block = struct.unpack_from('<I', ee, rec + 0x48)[0]
        if not block: continue
        out.append(dict(index=i, type=TYPES.get(struct.unpack_from('<I', ee, rec + 0x70)[0] >> 16, 'other'),
                        position=matrix[12:15], matrix=matrix,
                        base=list(struct.unpack_from('<3f', ee, rec + 0x4C)),
                        angle=struct.unpack_from('<f', ee, rec + 0x5C)[0],
                        scale=struct.unpack_from('<f', ee, rec + 0x64)[0],
                        frames=struct.unpack_from('<H', ee, block)[0],
                        pose=list(struct.unpack_from('<57h', ee, block + 2))))
    return out


def main():
    state, out = sys.argv[1], sys.argv[2]
    keep = {int(v) for v in sys.argv[3:]}
    ee = zipfile.ZipFile(state).read('eeMemory.bin')
    b = bodies(ee, keep)
    with open(out, 'w') as f: json.dump(dict(state=state, bodies=b), f, indent=1)
    print(out, len(b), 'bodies:', ', '.join(f"{x['index']} {x['type']}" for x in b))


if __name__ == '__main__':
    main()
