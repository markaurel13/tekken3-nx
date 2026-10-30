#!/usr/bin/env python3
"""Wings flap in the replay of the final KO (B53, TIPS.md § 79).

    python tools/test_wings_replay.py [--guest devil angel devil-jin] [--build build-rel-lite] [--memcard-dir saves]

Boots a private copy of the build (software renderer, headless, x1) on a copy
of the memory card, takes the guest from the Tag page (devil-jin: Jin on the
Arcade page with both punches, the easter egg) against Jin (standing
still, life 1) and wins two rounds with low kicks: the second KO is the final
one, played in slow motion then replayed at half speed. Jin guards every
jumping attack, so at the final KO the wing state (actor +0x128B / +0x128C) is
put mid-flap: the KO itself is a real one. The wing pose (row word 12 of the
wing row: row 1 for Devil and Angel, the row with the 37-pose table for Devil Jin) is then read every frame of the replay
(0x8009BCA8 = 1), with the accessory bones 18..21 (actor +0xF74 + 32 * bone).

Fails if the wings take fewer than MIN_POSES poses in the replay (before the
fix: one, the machine sat at rest because the replay does not play back the
airborne trigger). The accessory bones are printed for the record.
"""
import argparse, struct, sys, time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(Path(__file__).resolve().parent / 'ttt1'))
from test_force_moveset import Game, SCREEN
from test_guest_accessories import to_tag_page, fight, P1, P2, CLOCK, CURSOR_ID, COSTUME, OPPONENT, OPPONENT_COSTUME
from test_force_moveset import LINE, CROSS, START, PHASE

ROOT = Path(__file__).resolve().parent.parent
REPLAY_MODE, MODELS = 0x8009bca8, 0x8009bd28
LIFE = 0x3f6
NONE, FORWARD, DOWN_KICK, RIGHT, SQUARE, BOTH_PUNCHES = 0xffff, 0xffdf, 0xdfbf, 0xffdf, 0x7fff, 0x6fff
JIN = 9
ATTACK = [(60, FORWARD), (4, DOWN_KICK), (10, NONE)] * 4 + [(300, NONE)]
MIN_POSES = 8


def ko(g, last):
    """Walk in and low-kick until Jin falls; at the final KO put the wings mid-flap."""
    g.wait(lambda: g.value(SCREEN) == 8 and g.value(P2 + LIFE, 2) > 1, 'no round', 90)
    time.sleep(1.5)
    g.write(P2 + 0xc5, 0, 1); g.write(P2 + LIFE, 1, 2)
    g.q(cmd='input_route_clear')
    for n, b in ATTACK: g.q(cmd='input_route_append', frames=n, buttons=b)
    g.q(cmd='input_route_start')
    g.wait(lambda: g.value(P2 + LIFE, 2) != 1, 'Jin never fell', 15)
    if last: g.write(P1 + 0x128b, 4, 1); g.write(P1 + 0x128c, 1, 1)


def devil_jin(g):
    """Jin with both punches on the Arcade page (the easter egg, TIPS.md § 30), against Jin."""
    def menu():
        if g.value(SCREEN) == 3: g.press(START)
        return g.value(SCREEN) == 4
    g.wait(menu, 'the modes menu never came', 180)
    time.sleep(.3)
    g.write(LINE, 0); g.press(CROSS)
    g.wait(lambda: g.value(SCREEN) == 9 and g.value(0x80118648) == 21, 'the Arcade selector never came', 20)
    time.sleep(1.5)
    for _ in range(23):
        if g.value(CURSOR_ID) == JIN: break
        g.press(RIGHT)
    # Square first (the punch costume, or the loading screen drops the request), then both punches.
    g.q(cmd='input_route_clear')
    for n, b in [(2, SQUARE), (5, BOTH_PUNCHES), (2, NONE)]: g.q(cmd='input_route_append', frames=n, buttons=b)
    g.q(cmd='input_route_start')
    g.wait(lambda: g.value(SCREEN) == 11, 'loading screen not reached', 15)
    g.write(OPPONENT, JIN, 2); g.write(OPPONENT_COSTUME, 0, 2)
    g.wait(lambda: g.value(SCREEN) == 8 and g.value(PHASE, 2) >= 8, 'the fight never started', 120)
    g.write(P2 + 0xc5, 0, 1)
    time.sleep(2)


def wing_slot(g):
    """Row word 12 of the row whose table has the most poses (wings_step, src/tekken3_ttt1_mod.c).
    The debug server does not read the mods' memory (0x9F000000...), where Devil Jin's table
    lives: then the one row other than the hands' with a table."""
    model, best, others = g.value(MODELS), (0, None), []
    for r in range(27):
        if r in (13, 17): continue
        table = g.value(model + 24 + 56 * r + 52)
        if 0x9f000000 <= table < 0x9f400000: others.append(model + 24 + 56 * r + 48)
        if not 0x80000000 <= table < 0x80200000: continue
        words = struct.unpack('<47I', bytes.fromhex(g.q(cmd='read_ram', addr=f'{table:08x}', len=47 * 4)['hex']))
        n = next((i for i in range(1, 47) if words[i] == words[0]), 47)
        if n >= 24 and n > best[0]: best = (n, model + 24 + 56 * r + 48)
    return best[1] or (others[0] if len(others) == 1 else None)


def replay(g, slot):
    g.wait(lambda: g.value(REPLAY_MODE) == 1, 'no replay after the final KO', 20)
    poses, bones, last = [], [], -1
    while g.value(REPLAY_MODE) == 1:
        f = g.q(cmd='frame')['frame']
        if f == last: continue
        last = f
        poses.append(g.value(slot))
        raw = bytes.fromhex(g.q(cmd='read_ram', addr=f'{P1 + 0xf74 + 32 * 18:08x}', len=32 * 4)['hex'])
        bones.append([struct.unpack_from('<9h', raw, 32 * i) for i in range(4)])
    return poses, bones


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--guest', nargs='*', default=['devil', 'angel'])
    ap.add_argument('--build', default=str(ROOT / 'build-rel-lite'))
    ap.add_argument('--memcard-dir', default=str(ROOT / 'saves'), help='copied, never written')
    a = ap.parse_args()
    build = Path(a.build)
    catalogue = [l.split()[0] for l in (build / 'mods/ttt1/guests.txt').read_text().splitlines() if l.split()]
    failed = []
    for key in a.guest:
        g = Game(build, Path(a.memcard_dir), {})
        try:
            if key == 'devil-jin': devil_jin(g)
            else:
                to_tag_page(g, len(catalogue))
                fight(g, catalogue.index(key), len(catalogue), 1)
            ko(g, False)
            g.write(CLOCK, 0x7000, 2)
            g.wait(lambda: g.value(SCREEN) == 8 and g.value(P2 + LIFE, 2) > 1, 'no second round', 90)
            slots = []                                       # Devil Jin's model is patched in after the round starts
            g.wait(lambda: slots.append(wing_slot(g)) or slots[-1] is not None, 'no wing row on this model', 15)
            slot = slots[-1]
            ko(g, True)
            poses, bones = replay(g, slot)
        except Exception as e:
            print(f'{key}: {e}', flush=True); failed.append(key); continue
        finally:
            g.stop()
        distinct = len(set(poses))
        moving = [18 + i for i in range(4) if len({b[i] for b in bones}) > 1]
        print(f'{key}: {len(poses)} replay frames, {distinct} wing poses; accessory bones moving: {moving or "none"}'
              + ('  FROZEN' if distinct < MIN_POSES else ''), flush=True)
        if distinct < MIN_POSES: failed.append(key)
    if failed: sys.exit(f'wings frozen in the KO replay: {", ".join(failed)}')


if __name__ == '__main__':
    main()
