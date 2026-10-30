#!/usr/bin/env python3
"""The cutscene scripts of Tekken Tag Tournament (PS2), read from the game's
executable: the source of truth for what happens in an in-engine ending or
in the Embu, instead of guessing it from captures.

    python tools/ps2/ttt_script.py [--elf SLUS_200.01] [--json OUT.json] [SCENE ...]

SCENE: a character key (armorking, kazuya, bruce...), a scene index (0..37)
or "embu". Without one, every scene. Default executable:
workspace/ttt-ps2/SLUS_200.01 (copied from the disc, never in git).

Found 2026-10-13 (SLUS-20001 v1.00), see tools/ps2/ttt_capture_format.md:
- Ending table 0x51B388, 12 bytes per scene: script pointer, stage number
  (gp-29754, the arena: Armor King 11, Bruce 12), actor list pointer.
  Selector bytes 0x51B578[8 * character + variant] -> scene index (the
  variant: the Theater button / costume; Xiaoyu, Eddy, Kuma, Devil and Roger
  have a second scene). Actor list: (character, costume) pairs of s16,
  costume -1 = the player's costume.
- Script: records of five s16 (frame, command, a, b, c), sorted by frame,
  ended by (length - 1, -1, 0, 0, 0). Read by the ending player in
  0x003AA810 (jump table 0x51B6F0, clock 0x005F6C3C, record pointer
  0x005F6C34, done flag 0x005F6C40).
- The TTT Embu (Theater MOVIE triangle) has its own script at 0x519A90 and
  its own player in 0x003A9A20 (jump table 0x519A50, s16 clock 0x005F6C22,
  record pointer 0x005F6C1C, actor list gp-28312 = 0x437BE0: Paul, Hwoarang,
  Xiaoyu, Eddy, Jin, Jun); same record
  layout, partly different commands.
- Characters: TTT ids, the TTT1 "body" numbers (tools/data/ttt1_characters.json)
  and, for the Tekken 3 cast, the Tekken 3 numbers.
"""
import json, struct, sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ELF_DEFAULT = ROOT / 'workspace/ttt-ps2/SLUS_200.01'

ENDINGS, SELECTOR, SCENES = 0x51B388, 0x51B578, 38
EMBU_SCRIPT, EMBU_ACTORS = 0x519A90, 0x437BE0

CHARACTERS = {
    0: 'paul', 1: 'law', 2: 'lei', 3: 'king', 4: 'yoshimitsu', 5: 'nina', 6: 'hwoarang',
    7: 'xiaoyu', 8: 'eddy', 9: 'jin', 10: 'julia', 11: 'kuma', 12: 'bryan', 13: 'heihachi',
    14: 'ogre', 15: 'tetsujin', 16: 'gunjack', 17: 'roger', 18: 'anna', 19: 'kazuya',
    20: 'trueogre', 21: 'jack2', 22: 'pjack', 23: 'michelle', 24: 'wang', 25: 'lee',
    26: 'armorking', 27: 'ganryu', 28: 'kunimitsu', 29: 'devil', 30: 'jun', 31: 'bruce',
    32: 'baek', 33: 'unknown',
}
# Second scene of a character, by Theater variant (selector byte differs).
VARIANT_KEYS = {(7, 1): 'xiaoyu3', (8, 1): 'tiger', (11, 1): 'panda', (29, 1): 'angel', (17, 1): 'alex'}

# Ending player (0x003AA810). Fades are tasks of 0x003AC538: kind 0 from
# black, 1 to black, 2 from white (b = start level, 128 = full), 3 to white;
# a = duration in frames.
ENDING_COMMANDS = {
    0: ('model', 'actor 0 becomes actor-list entry a (model reloaded: Angel -> Kazuya, Jin -> Devil)'),
    1: ('model', 'actor 1 becomes actor-list entry a'),
    2: ('motion', 'actor 0 plays scene motion a from frame b, c frames (0x368190 on a new motion)'),
    3: ('motion', 'actor 1 plays scene motion a from frame b, c frames'),
    4: ('camera', 'camera track a, b (0x346588 restart, gp-25508/-25504)'),
    5: ('fade', 'from white: a frames, from level b (128 = full white)'),
    6: ('fade', 'to white over a frames'),
    10: ('spark', 'hit spark of actor a\'s kind at node b of actor c (b = 16387: a stage point) (0x3A2B38)'),
    12: ('devil', 'actor a: devil textures off (b = 0) / on (b = 1) (0x3A8640)'),
    15: ('task', 'task 0x3AC4C0: actor a & 1, value b'),
    16: ('special', 'per-actor state of character 29 (Devil/Angel wings?): actor a, b, c (0x3782E0)'),
    17: ('sprite', 'effect sprite between two nodes: a bit 0 actor, bits 1-2 kind; b, c nodes (16386: the camera)'),
    18: ('sprite', 'effect sprite kind 3 at node b of actor a & 1 (98 root, 16384 0x3ACE48, 16385 a fixed point, 16386 the camera), size c'),
    19: ('fade', 'from black over a frames'),
    20: ('fade', 'to black over a frames'),
    21: ('fade', 'task 0x3AC728: kind a, level b, c frames (coloured fade?)'),
    22: ('screen', 'screen task 0x3AC818: a, b, c >> 8, c & 0xFF'),
    23: ('screen', 'screen task 0x3AC8E0: 220, a frames'),
    24: ('screen', 'screen task 0x3ACA18: a, b'),
    25: ('screen', 'screen task 0x3ACA70: a (flag), b, c frames'),
    26: ('flag', 'gp-28292 = a & 1'),
    27: ('camera', 'camera restart (0x346588)'),
    28: ('special', 'actor a & 1, characters 20 / 29 only: state b (+0x8000+12370)'),
    29: ('screen', 'screen task 0x3AA420: a'),
    30: ('screen', 'screen task 0x3AD140: a'),
    31: ('flag', '0x34CAE0 for actor 0 (a bit 0) / 1 (a bit 1)'),
    32: ('screen', 'screen task 0x3AA378: a'),
}
# Embu player (0x003A9A20).
EMBU_COMMANDS = {
    0: ('model', 'actor 0 becomes actor-list entry a'),
    1: ('model', 'actor 1 becomes actor-list entry a'),
    2: ('motion', 'actor 0 plays scene motion a, frames b to c'),
    3: ('motion', 'actor 1 plays scene motion a, frames b to c'),
    4: ('camera', 'camera shot a (gp-25544), c (gp-25540)'),
    5: ('fade', 'from white over a frames (0x3A6738, colour 255,255,255)'),
    6: ('fade', 'to white over a frames (0x3A66D8)'),
    7: ('stage', 'stage effect a, b (0x3A7F10, stage 4 only: the storm\'s lightning)'),
    8: ('nop', ''),
    9: ('flag', '0x3A6B28: *0x008C9BE0 = 1'),
    10: ('spark', 'hit spark of actor a at the hit point b of actor a & 1 (+1640 + 80 b)'),
    11: ('fade', 'white flash held a frames (0x3A6838)'),
    12: ('devil', 'actor a: devil off (b = 0) / on (b = 1), with two tasks 0x3AB620'),
    13: ('music', 'sound queue command 0x40 1 (0x39A038): the music'),
    14: ('model', 'as 0'),
}


class Elf:
    SECTIONS = ((0x340000, 0x1000, 0xD55BC), (0x415600, 0xD6600, 0xE0800), (0x4FB780, 0x1BC780, 0xF9828))

    def __init__(self, path):
        self.data = Path(path).read_bytes()

    def off(self, a):
        for va, o, n in self.SECTIONS:
            if va <= a < va + n: return a - va + o
        raise ValueError(f'{a:#x} not in the executable')

    def h(self, a, n=1): return struct.unpack_from(f'<{n}h', self.data, self.off(a))
    def w(self, a, n=1): return struct.unpack_from(f'<{n}I', self.data, self.off(a))
    def b(self, a, n=1): return struct.unpack_from(f'<{n}B', self.data, self.off(a))


def records(elf, a):
    out = []
    while True:
        r = elf.h(a, 5); a += 10
        if r[1] == -1: return out, r[0] + 1
        out.append(r)
        if len(out) > 1000: raise ValueError('script without end')


def actor_list(elf, a, n=4):
    pairs = elf.h(a, 2 * n)
    out = []
    for i in range(n):
        c, k = pairs[2 * i], pairs[2 * i + 1]
        if c < 0 or c > 40: continue          # an empty entry (Bruce's list has holes)
        out.append(dict(entry=i, character=c, key=CHARACTERS.get(c, f'id{c}'), costume=k))
    return out


def event(r, table):
    t, cmd, a, b, c = r
    kind, doc = table.get(cmd, ('unknown', ''))
    return dict(t=t, cmd=cmd, kind=kind, a=a, b=b, c=c)


def scenes(elf):
    sel = elf.b(SELECTOR, 8 * 36)
    keys = {}
    for ch in range(34):
        variants = sel[8 * ch:8 * ch + 8]
        first = variants[0]
        keys.setdefault(first, CHARACTERS.get(ch, f'id{ch}'))
        for v in variants:
            if v != first: keys.setdefault(v, VARIANT_KEYS.get((ch, 1), CHARACTERS.get(ch) + '2'))
    out = []
    for i in range(SCENES):
        sp, stage, al = elf.w(ENDINGS + 12 * i, 3)
        recs, length = records(elf, sp)
        out.append(dict(scene=i, key=keys.get(i), stage=stage, length=length, script=hex(sp),
                        actors=actor_list(elf, al), events=[event(r, ENDING_COMMANDS) for r in recs]))
    recs, length = records(elf, EMBU_SCRIPT)
    out.append(dict(scene='embu', key='embu', stage=4, length=length, script=hex(EMBU_SCRIPT),
                    actors=actor_list(elf, EMBU_ACTORS, 6), events=[event(r, EMBU_COMMANDS) for r in recs]))
    return out


def find(all_scenes, name):
    for s in all_scenes:
        if str(s['scene']) == name or s['key'] == name: return s
    raise SystemExit(f'no scene {name!r}: ' + ', '.join(str(s['key']) for s in all_scenes))


def show(s):
    table = EMBU_COMMANDS if s['scene'] == 'embu' else ENDING_COMMANDS
    actors = ', '.join(f"entry {a['entry']} = {a['key']} costume {a['costume']}" for a in s['actors'] or [])
    print(f"scene {s['scene']} ({s['key']}): stage {s['stage']}, {s['length']} frames; {actors}")
    for e in s['events']:
        kind, doc = table.get(e['cmd'], ('unknown', ''))
        print(f"  {e['t']:5d}  cmd {e['cmd']:2d} {kind:8s} {e['a']:6d} {e['b']:6d} {e['c']:6d}  {doc}")


def main():
    args = sys.argv[1:]
    elf_path, out = ELF_DEFAULT, None
    if '--elf' in args:
        i = args.index('--elf'); elf_path = Path(args[i + 1]); del args[i:i + 2]
    if '--json' in args:
        i = args.index('--json'); out = Path(args[i + 1]); del args[i:i + 2]
    every = scenes(Elf(elf_path))
    chosen = [find(every, a) for a in args] if args else every
    if out:
        out.write_text(json.dumps(dict(source=str(elf_path.name), scenes=chosen), indent=1))
        print(f'{out}: {len(chosen)} scenes')
    else:
        for s in chosen: show(s)


if __name__ == '__main__':
    main()
