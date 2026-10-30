#!/usr/bin/env python3
"""Springs of the accessory bones (tails, hair, skirts, coat tails...) of a TTT1
model slot, as the arcade swings them (F03, TIPS.md § 78).

TTT1's accessory solver (0x80163B68) takes its parameters from a table of six
pointers per model slot at 0x8019D674 (one per bone 18..23, 0 = rigid), loaded
by 0x801648E0 into actor +0x12D4. Each block is eight halfwords (+0 switch, 32
in every guest block; +2 smoothing of the tip; +4 / +6 stiffness and damping;
+8; +10 sway; +12 collision mode, 0 for none) and four more when +12 is not 0
(+14 bone collided with, +16..+20 offset, +22 radius). Tekken 3's solver
(0x80037CBC) reads the same fields from +4 on.

The converter (tools/ttt1/model/convert.py) puts TTT1 accessory rows 24..29
(bones 18..23) on PS1 parts 18..21 (rows 21..24), the rest into another part
(second mesh, rigid). For each PS1 part 18..21 the .acc file gives the TTT1
bone drawn there and its block:

    'ACC1', u16 4, u16 entry size (28), then per PS1 bone 18..21:
    u16 TTT1 bone (0xFFFF: rigid), u16 0, 12 x s16 block (zeros past +16 without collision)

Read at load by read_accessories() (src/tekken3_ttt1_mod.c). No file: every
accessory bone stays rigid.

    python tools/ttt1/accessories.py <select RAM> <slot> <TTT1 .3dm> <out .acc>
"""
import struct, sys
from pathlib import Path

TABLE = 0x19D674                         # 0x8019D674 in a RAM image from 0x80000000
ENTRY, BONES = 28, (18, 19, 20, 21)
PS1_ROW = {b: b + 3 for b in BONES}      # PS1 part 18..21 -> row 21..24


def blocks(ram, slot):
    """TTT1 bone 18..23 -> its 12-halfword block, for this model slot."""
    out = {}
    for i in range(6):
        p = struct.unpack_from('<I', ram, TABLE + 24 * slot + 4 * i)[0]
        if not p: continue
        b = list(struct.unpack_from('<12h', ram, p & 0x3FFFFF))
        if not b[6]: b[8:] = [0, 0, 0, 0]          # no collision: a 16-byte block
        out[18 + i] = b
    return out


def entries(ram, slot, s2t):
    """PS1 bone 18..21 -> (TTT1 bone, block), with the layout of the conversion
    (convert.S2T: PS1 row -> TTT1 row)."""
    table, out = blocks(ram, slot), {}
    for b in BONES:
        t = s2t.get(PS1_ROW[b])
        if t is not None and t >= 24 and t - 6 in table: out[b] = (t - 6, table[t - 6])
    return out


def pack(entries_):
    data = bytearray(b'ACC1' + struct.pack('<HH', len(BONES), ENTRY))
    for b in BONES:
        bone, block = entries_.get(b, (0xFFFF, [0] * 12))
        data += struct.pack('<HH12h', bone, 0, *block)
    return bytes(data)


def read(path):
    """{PS1 bone: {'ttt1_bone', 'block'}} of a .acc file; {} without one."""
    path = Path(path)
    if not path.is_file(): return {}
    d = path.read_bytes()
    assert d[:4] == b'ACC1', path
    count, size = struct.unpack_from('<HH', d, 4)
    out = {}
    for i in range(count):
        bone, _, *block = struct.unpack_from('<HH12h', d, 8 + size * i)
        if bone != 0xFFFF: out[BONES[i]] = dict(ttt1_bone=bone, block=block)
    return out


def write(path, ram, slot, s2t):
    """Writes the .acc of a converted model; returns the report entry."""
    e = entries(ram, slot, s2t)
    Path(path).write_bytes(pack(e))
    table = blocks(ram, slot)
    drawn = {t - 6 for t in s2t.values() if t >= 24}
    return dict(swinging={str(b): t for b, (t, _) in e.items()},
                rigid=sorted(set(table) - {t for t, _ in e.values()}),      # baked into another part, or not drawn
                not_drawn=sorted(set(table) - drawn))


def main(argv=sys.argv[1:]):
    sys.path.insert(0, str(Path(__file__).resolve().parent / 'model'))
    import convert
    from fmt import load
    ram, slot, model, out = Path(argv[0]).read_bytes(), int(argv[1]), load(argv[2]), argv[3]
    convert.convert(model)
    print(write(out, ram, slot, {s: t for s, t in convert.S2T.items() if s > 20}))


if __name__ == '__main__':
    main()
