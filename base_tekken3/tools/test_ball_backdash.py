#!/usr/bin/env python3
"""Backdash length and duration, in Practice and in Tekken Ball (B41, GH#94).

    python tools/test_ball_backdash.py [--mode practice|ball] [--fighter jin|kazuya|lee|...]
                                       [--moves t3|ttt1] [--stance rest|neutral|ball] [--trials 5]
                                       [--tap 1] [--hold-back 12] [--far] [--curve]
                                       [--expect-min N] [--expect-max N]
                                       [--build build-rel-lite] [--memcard-dir saves]

The check (the bug back: Ball without the overlay's dash boost, ~745):
    --mode ball --fighter jin --moves ttt1 --stance neutral --tap 1 --hold-back 12 --expect-min 1500
    (same with --fighter kazuya; T3 and TTT1 ~1970), and outside Ball
    --mode practice --fighter jin --moves ttt1 --stance neutral --tap 1 --hold-back 12 --expect-max 1000
The backdash is played as a player does it: back, neutral, back held.
--stance ball forces Tekken Ball's stance (alias 158) instead; --watch,
--start-watch, --literal, --hold observe a fight (see --help).

Boots a private copy of the build (software renderer, headless, x1) on a copy
of the memory card, unlocks Tekken Ball (0x80097F26/27), opens the modes menu
on "Namco presents" (state 3) and takes Practice (line 8, Arcade grid) or
Tekken Ball (line 6, VS grid). Natives are picked on the grid, their moves
forced by TEKKEN3_NATIVE_MOVES (t3: 0, ttt1: 1); a guest takes Jin's place at
the loading screen (0x800ADD5C = 23 + its place in guests.txt). In the
fight, player 1 at rest is saved (savestate slot 10); each trial reloads it
and plays back, neutral, back (input route), then polls the fighter's root
bone (actor +0x908 x, +0x910 z) and move (+0xA0, frame +0x58): peak distance
from the start, frames of the moves. Build: rel-lite (debug server lite).
A full cmake --build drops a copied mods/ttt1 catalogue: copy it again.
"""
import argparse, math, os, shutil, socket, struct, subprocess, sys, tempfile, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'psxrecomp/tools'))
import debug_client

SCREEN, PHASE, MODE, LINE = 0x800ae204, 0x800ae224, 0x800afa88, 0x80097f40
P1_FIGHTER, BLOCKS, LOADING_P1 = 0x80118668, 0x800b8d70, 0x800add5c
ACTOR, STRIDE = 0x800a9228, 0x188c
START, CROSS, RIGHT, LEFT, UP, DOWN, NONE = 0xfff7, 0xbfff, 0xffdf, 0xff7f, 0xffef, 0xffbf, 0xffff
LINES = {'practice': 8, 'ball': 6}
IDS = {'paul': 0, 'law': 1, 'lei': 2, 'king': 3, 'yoshimitsu': 4, 'nina': 5, 'hwoarang': 6,
       'xiaoyu': 7, 'eddy': 8, 'jin': 9, 'bryan': 12, 'heihachi': 13, 'anna': 18}
BALL_CELLS = {'law': (0, 0), 'paul': (6, 0), 'xiaoyu': (0, 1), 'yoshimitsu': (1, 1), 'nina': (2, 1),
              'heihachi': (3, 1), 'king': (4, 1), 'lei': (5, 1), 'jin': (6, 1), 'hwoarang': (0, 2),
              'bryan': (2, 2), 'anna': (5, 2), 'eddy': (6, 2)}
EXE = 'Tekken_3_Expanded.exe' if os.name == 'nt' else 'Tekken_3_Expanded'
SLOT, WINDOW = 10, 70
TAP, HOLD, CURVE = 2, 2, False
STANCES = {'neutral': 3, 'ball': 158}        # 158: Tekken Ball's stance (shared bank, 157 -> 158)


class Game:
    def __init__(self, build, memcards, env):
        self.dir = Path(tempfile.mkdtemp(prefix='ball-backdash-'))
        run, saves = self.dir / 'run', self.dir / 'saves'
        run.mkdir(); (saves / 'openbios').mkdir(parents=True)
        shutil.copy2(build / EXE, run / EXE)
        for name in ('bios', 'mods', 'assets'):
            if (build / name).is_dir(): shutil.copytree(build / name, run / name)
        for card in ('card1.mcd', 'card2.mcd'):
            if (memcards / card).is_file(): shutil.copy2(memcards / card, saves / card)
        (run / 'settings.toml').write_text('[video]\nsupersampling=1\nantialiasing=false\n'
                                           '[controller]\np1_device="keyboard"\np2_device="none"\n')
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0)); self.port = listener.getsockname()[1]
        self.logfile = self.dir / 'game.log'
        environment = {k: v for k, v in os.environ.items() if not k.startswith('TEKKEN3_')}
        environment.update(env, SDL_AUDIO_DRIVER='dummy')
        with self.logfile.open('w') as out:
            self.process = subprocess.Popen(
                [str(run / EXE), '--game', str(ROOT / 'game.toml'), '--disc', str(ROOT / 'disc/Tekken 3 (USA).cue'),
                 '--no-launcher', '--renderer', 'software', '--headless', '--debug-port', str(self.port),
                 '--memcard-dir', str(saves)], cwd=ROOT, stdout=out, stderr=out, env=environment)
        end = time.monotonic() + 30
        while True:
            if self.process.poll() is not None: raise RuntimeError(f'the game exited at startup; see {self.logfile}')
            try:
                with socket.create_connection(('127.0.0.1', self.port), .2): break
            except OSError:
                if time.monotonic() > end: raise RuntimeError('the debug server did not start')
                time.sleep(.1)

    def q(self, **r):
        result = debug_client.query('127.0.0.1', self.port, r)
        if result.get('ok') is False: raise RuntimeError((r, result))
        return result

    def read(self, a, n): return bytes.fromhex(self.q(cmd='read_ram', addr=f'{a:08x}', len=n)['hex'])
    def value(self, a, n=4): return int.from_bytes(self.read(a, n), 'little')

    def write(self, a, v, n=4):
        for j in range(n): self.q(cmd='write_ram', addr=f'{a + j:08x}', val=f'{v >> (j * 8) & 255:02x}')

    def press(self, buttons): self.q(cmd='press', buttons=buttons, frames=4); time.sleep(.3)
    def frame(self): return self.q(cmd='frame')['frame']

    def wait_frames(self, n):
        end = self.frame() + n
        while self.frame() < end: time.sleep(.02)

    def route(self, steps):
        self.q(cmd='input_route_clear')
        for n, b in steps: self.q(cmd='input_route_append', frames=int(n), buttons=int(b))
        start = self.q(cmd='history')['count']          # the per-frame ring's own numbering
        self.q(cmd='input_route_start')
        return start

    def wait(self, predicate, description, seconds=30):
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            if predicate(): return
            time.sleep(.05)
        raise AssertionError(description)

    def log(self): return self.logfile.read_text(errors='replace')

    def stop(self):
        if self.process.poll() is None:
            self.process.terminate()
            try: self.process.wait(timeout=10)
            except subprocess.TimeoutExpired: self.process.kill()
        shutil.rmtree(self.dir, ignore_errors=True)


def to_mode(g, mode):
    def menu():
        if g.value(SCREEN) == 3:
            g.write(0x80097f26, 1, 1); g.write(0x80097f27, 1, 1)
            g.press(START)
        return g.value(SCREEN) == 4
    g.wait(menu, 'the modes menu never came', 180)
    time.sleep(.3)
    g.write(LINE, LINES[mode]); g.press(CROSS)
    if mode == 'ball':
        g.wait(lambda: g.value(SCREEN) == 10 and g.value(MODE) == 7 and g.value(BLOCKS) == 3, 'TEKKEN BALL SELECT', 20)
    else:
        g.wait(lambda: g.value(SCREEN) == 9 and g.value(MODE) == 5 and g.value(PHASE, 2) <= 1, 'the Practice selector', 20)
    time.sleep(1.5)


def pick(g, mode, native):
    if mode == 'ball':
        col, row = BALL_CELLS[native]
        for _ in range(20):
            x, y = g.value(BLOCKS + 8), g.value(BLOCKS + 12)
            if (x, y) == (col, row): break
            g.press(DOWN if y < row else UP if y > row else RIGHT if x < col else LEFT)
        else: raise AssertionError(f'the grid cursor never reached {native}')
        g.press(CROSS)
        g.wait(lambda: g.value(BLOCKS) == 15, 'the ball choice never came', 10)
    else:
        for _ in range(30):
            if g.value(P1_FIGHTER) == IDS[native]: break
            g.press(RIGHT)
        else: raise AssertionError(f'the grid cursor never reached {native}')
    # X takes the fighter (Practice: then the dummy) or the ball; the loading
    # screen (11) is short: X held, then a tight loop on the screen.
    for _ in range(4):
        g.q(cmd='set_input', buttons=CROSS)
        end = time.monotonic() + 3
        while g.value(SCREEN) != 11 and time.monotonic() < end: pass
        g.q(cmd='set_input', buttons=NONE)
        if g.value(SCREEN) == 11: return
        time.sleep(.2)
    raise AssertionError('the loading screen never came')


def signed(v): return v - (1 << 32) if v >= 1 << 31 else v


def guest_id(build, name):
    names = [l.split()[0] for l in (build / 'mods/ttt1/guests.txt').read_text().splitlines() if l.split()]
    return 23 + names.index(name)


def trial(g, back):
    """One backdash from the saved rest. The lite server keeps no per-frame
    ring and answers about one query per frame: poll the fighter's root
    (actor +0 x, +8 z), move (+0xA0) and move frame (+0x58) as fast as it
    answers; the move frames give the duration. The root bone (+0x908 x,
    +0x910 z) follows the move; +0 / +8 only take it at the move's end."""
    g.q(cmd='savestate', op='load', slot=SLOT)
    g.wait_frames(6)
    g.q(cmd='input_route_clear')
    for n, b in [(TAP, back), (TAP, NONE), (HOLD, back), (WINDOW, NONE)]:
        g.q(cmd='input_route_append', frames=n, buttons=b)
    f0 = g.frame()
    g.q(cmd='input_route_start')
    rows, end, curve = [], time.monotonic() + 3, []
    while time.monotonic() < end:
        block = g.read(ACTOR, 0x914)
        x, _, z = struct.unpack_from('<3i', block, 0x908)
        curve.append((g.frame() - f0, x, z, struct.unpack_from('<H', block, 0xa0)[0]))
        rows.append((x, z, struct.unpack_from('<H', block, 0xa0)[0], struct.unpack_from('<H', block, 0x58)[0]))
        if not g.q(cmd='input_route_status').get('active'): break
    x0, z0 = rows[0][:2]
    dist = [math.hypot(x - x0, z - z0) for x, z, _, _ in rows]
    moves = {}                                   # move -> its last frame seen
    for _, _, m, f in rows:
        if m not in (rows[0][2], 3): moves[m] = max(moves.get(m, 0), f)
    if CURVE:
        print('    curve (frame: distance, move): ' + ' '.join(
            f'{f}:{round(math.hypot(x - x0, z - z0))}/{m}' for f, x, z, m in curve), flush=True)
    return round(max(dist)), round(dist[-1]), sum(moves.values()), moves


def run(g, a, build):
    to_mode(g, a.mode)
    native = a.fighter if a.fighter in IDS else 'jin'
    pick(g, a.mode, native)
    if a.fighter not in IDS:
        gid = guest_id(build, a.fighter)
        for _ in range(30): g.write(LOADING_P1, gid, 2); time.sleep(.01)
    g.wait(lambda: g.value(SCREEN) == 8 and g.value(PHASE, 2) >= 6 and g.value(ACTOR + 0x54) != 0,
           'the fight never started', 90)
    if a.literal:
        # GH#94 as written: from the fight start, b, n, b as soon as player 1
        # is free, again and again; each one's moves and root displacement.
        g.wait(lambda: g.value(ACTOR + 0xa0, 2) == 3, 'player 1 never free', 20)
        back = LEFT if signed(g.value(ACTOR)) < signed(g.value(ACTOR + STRIDE)) else RIGHT
        for k in range(a.literal):
            x0, _, z0 = struct.unpack('<3i', g.read(ACTOR + 0x908, 12))
            g.q(cmd='input_route_clear')
            for n, b in [(2, back), (2, NONE), (2, back), (70, NONE)]:
                g.q(cmd='input_route_append', frames=n, buttons=b)
            g.q(cmd='input_route_start')
            moves, peak = [], 0
            while g.q(cmd='input_route_status').get('active'):
                block = g.read(ACTOR, 0x914)
                x, _, z = struct.unpack_from('<3i', block, 0x908); m = struct.unpack_from('<H', block, 0xa0)[0]
                peak = max(peak, math.hypot(x - x0, z - z0))
                if not moves or moves[-1] != m: moves.append(m)
            print(f'  backdash {k + 1}: from x {x0}, peak {round(peak)}, moves {moves}', flush=True)
        return []
    if a.start_watch:
        seen, last, end = [], None, time.monotonic() + a.start_watch
        while time.monotonic() < end:
            m = g.value(ACTOR + 0xa0, 2)
            if m != last: seen.append((m, g.frame())); last = m
        print(f'START {a.moves}: {seen[:40]}', flush=True)
        return []
    time.sleep(4)
    if a.mode == 'practice':                     # MODE SELECT: 1P FREESTYLE, then OK
        for _ in range(2): g.press(CROSS); time.sleep(1.2)
    # Walk forward to room behind (Tekken Ball starts player 1 against the
    # back of its court, where a backdash goes nowhere).
    x = g.value(ACTOR)
    if a.far: x = None                           # keep the start distance (Practice: 1996, far variant 32 / 33)
    else: g.q(cmd='set_input', buttons=RIGHT if signed(x) < signed(g.value(ACTOR + STRIDE)) else LEFT)
    end = g.frame() + (0 if a.far else 150)
    while g.frame() < end and abs(signed(g.value(ACTOR)) - signed(g.value(ACTOR + STRIDE))) > 2600: time.sleep(.02)
    g.q(cmd='set_input', buttons=NONE); g.wait_frames(60)
    assert a.far or g.value(ACTOR) != x, 'player 1 does not walk: still in a menu?'
    fighter = g.value(ACTOR + 0x18, 2)
    me, other = struct.unpack('<3i', g.read(ACTOR, 12)), struct.unpack('<3i', g.read(ACTOR + STRIDE, 12))
    # Back is away from player 2 along x (both modes put the fighters on the x axis).
    back = LEFT if me[0] < other[0] else RIGHT
    g.q(cmd='turbo', enabled=0)
    if a.stance != 'rest':
        # The stance asked for, through the engine's pending transition
        # (+0x1A0 alias, +0x1A4 record from the fighter's alias table).
        alias = STANCES[a.stance]
        table = g.value(g.value(0x800adc20) + 12)
        g.wait(lambda: g.value(ACTOR + 0x1a4) == 0, 'player 1 never settled', 5)
        pending = struct.pack('<HHI', alias, g.value(ACTOR + 0x1a2, 2), g.value(table + alias * 4))
        g.q(cmd='write_ram', addr=f'{ACTOR + 0x1a0:08x}', bytes=pending.hex())   # one write: one frame
        g.wait_frames(20)
    g.q(cmd='savestate', op='save', slot=SLOT)
    header = g.value(0x800adc20)
    rest = g.value(ACTOR + 0xa0, 2), g.value(ACTOR + 0x54)
    print(f'{a.mode} {a.fighter} moves={a.moves} actor fighter {fighter} header {header:#x} rest move {rest[0]} record {rest[1]:#x} '
          f'P1 {me} P2 {other} back={"left" if back == LEFT else "right"}', flush=True)
    if a.watch:
        # Player 1 left alone: which moves it stands in, how long (samples).
        g.q(cmd='savestate', op='load', slot=SLOT)
        seen, runs, last, end = {}, [], None, time.monotonic() + a.watch
        k = 0
        while time.monotonic() < end:
            k += 1
            if a.mash and k % 25 == 0: g.q(cmd='press', buttons=int(a.mash, 16), frames=3)
            m = g.value(ACTOR + 0xa0, 2)
            seen[m] = seen.get(m, 0) + 1
            if m != last: runs.append(m); last = m
        total = sum(seen.values())
        top = sorted(seen.items(), key=lambda kv: -kv[1])[:8]
        print(f'WATCH {a.moves}: ' + ', '.join(f'{m}: {100 * n / total:.0f}%' for m, n in top), flush=True)
        print(f'  sequence: {runs[:160]}', flush=True)
        return []
    if a.hold:
        print(f'HOLD port {g.port} pid {g.process.pid}', flush=True)
        while g.process.poll() is None: time.sleep(1)
    results = []
    for k in range(a.trials):
        r = trial(g, back)
        results.append(r)
        print(f'  trial {k + 1}: max {r[0]}, end {r[1]}, frames of the moves {r[2]}, moves (last frame) {r[3]}', flush=True)
    peaks = [r[0] for r in results]
    print(f'RESULT {a.mode} {a.fighter} {a.moves} stance={a.stance}: max distance {min(peaks)}..{max(peaks)}, '
          f'frames {sorted({r[2] for r in results})}', flush=True)
    if a.expect_min: assert min(peaks) >= a.expect_min, f'backdash {min(peaks)} < {a.expect_min} (the bug)'
    if a.expect_max: assert max(peaks) <= a.expect_max, f'backdash {max(peaks)} > {a.expect_max}'
    return results


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--mode', default='ball', choices=sorted(LINES))
    ap.add_argument('--fighter', default='jin')
    ap.add_argument('--moves', default='t3', choices=('t3', 'ttt1'))
    ap.add_argument('--trials', type=int, default=5)
    ap.add_argument('--watch', type=float, default=0, help='seconds of player 1 left alone: moves it stands in')
    ap.add_argument('--mash', help='with --watch: pad mask (hex) pressed every 25 samples, e.g. 7fff (square)')
    ap.add_argument('--start-watch', type=float, default=0, help='seconds: player 1 moves from the fight start, untouched')
    ap.add_argument('--literal', type=int, default=0, help='GH#94 as written: N backdashes from the fight start')
    ap.add_argument('--hold', action='store_true', help='leave the game running at the saved rest')
    ap.add_argument('--stance', default='rest', choices=('rest',) + tuple(STANCES),
                    help='stance before the backdash: as found, neutral (3) or the Ball stance (158)')
    ap.add_argument('--env', action='append', default=[], help='KEY=VALUE for the game (repeatable)')
    ap.add_argument('--build', default=str(ROOT / 'build-rel-lite'))
    ap.add_argument('--memcard-dir', default=str(ROOT / 'saves'))
    ap.add_argument('--tap', type=int, default=2, help='frames of each back / neutral / back press')
    ap.add_argument('--hold-back', type=int, default=0, help='frames the second back is held (GH#94: held, ~10)')
    ap.add_argument('--expect-min', type=int, default=0, help='fail if a trial goes less far (Ball: 1500)')
    ap.add_argument('--expect-max', type=int, default=0, help='fail if a trial goes farther (Practice: 1000)')
    ap.add_argument('--far', action='store_true', help='no walk forward before the save (start distance)')
    ap.add_argument('--curve', action='store_true', help='print each trial frame by frame')
    a = ap.parse_args()
    global TAP, HOLD, CURVE
    TAP, HOLD, CURVE = a.tap, a.hold_back or a.tap, a.curve
    build = Path(a.build)
    # A full cmake --build re-prepares mods/ and drops a copied catalogue.
    assert (build / 'mods/ttt1/guests.txt').is_file(), f'no TTT1 catalogue in {build}/mods/ttt1'
    env = {'TEKKEN3_NATIVE_MOVES': '1' if a.moves == 'ttt1' else '0'}
    env.update(e.split('=', 1) for e in a.env)
    g = Game(build, Path(a.memcard_dir), env)
    try:
        run(g, a, build)
        for line in g.log().splitlines():
            if line.startswith('B41') or 'Native moves' in line or 'alias table' in line: print(line)
    except BaseException:
        print(g.log()[-3000:]); raise
    finally: g.stop()


if __name__ == '__main__':
    main()
