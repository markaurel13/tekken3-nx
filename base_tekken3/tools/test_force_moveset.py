#!/usr/bin/env python3
"""Tekken Force: the MOVESET card, and the player's moves from stage to stage (B40, GH#100).

    python tools/test_force_moveset.py [--native jin|king|...] [--t3] [--bosses N] [--build build-rel-lite]
                                       [--memcard-dir saves]

Boots a private copy of the build (software renderer, headless, x1) on a copy
of the memory card, unlocks Tekken Ball / Force (0x80097F26/27), opens the
modes menu with Start on "Namco presents" (state 3), takes line 7 (Tekken
Force) and walks the Arcade grid (state 9) to the native. Square opens its
MOVESET card, right (unless --t3) switches it to TEKKEN TAG TOURNAMENT, X
takes it. In the stage: player 1's moves header (0x800ADC20) in the TTT1 pack
or not, and d/b+1 (Jin: the TTT1 lunge, else a duck jab) played from the pack
or not. --bosses N then walks right, invincible, the level clock held,
through N bosses (each comes in on phases 21..23; four, the last Heihachi;
~5 min each) and checks the same again against each, and the header at
every stage change on the way. Build: rel-lite (debug server lite).
Before the fix: "square did not open the MOVESET card".
"""
import argparse, os, shutil, socket, subprocess, sys, tempfile, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'psxrecomp/tools'))
import debug_client

SCREEN, PHASE, MODE, LINE = 0x800ae204, 0x800ae224, 0x800afa88, 0x80097f40
P1_STATE, P1_FIGHTER = 0x8011864c, 0x80118668          # Arcade grid, player 1's block
ACTOR, HEADER, LIFE, CLOCK, STAGE = 0x800a9228, 0x800adc20, 0x800a961e, 0x800adbcc, 0x800adc84
START, CROSS, SQUARE, RIGHT, START_SELECT = 0xfff7, 0xbfff, 0x7fff, 0xffdf, 0xfff6
DOWN_BACK_1, WALK, WALK_PUNCH = 0x7f3f, 0xffdf, 0x7fdf   # player 1 faces right
FORCE_LINE = 7
IDS = {'paul': 0, 'law': 1, 'lei': 2, 'king': 3, 'yoshimitsu': 4, 'nina': 5, 'hwoarang': 6,
       'xiaoyu': 7, 'eddy': 8, 'jin': 9, 'julia': 10, 'bryan': 12, 'heihachi': 13, 'anna': 18}
EXE = 'Tekken_3_Expanded.exe' if os.name == 'nt' else 'Tekken_3_Expanded'


def in_pack(address): return 0x9f000000 <= address < 0x9f400000


class Game:
    def __init__(self, build, memcards, env):
        self.dir = Path(tempfile.mkdtemp(prefix='force-moveset-'))
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

    def value(self, a, n=4):
        return int.from_bytes(bytes.fromhex(self.q(cmd='read_ram', addr=f'{a:08x}', len=n)['hex']), 'little')

    def write(self, a, v, n=4):
        for j in range(n): self.q(cmd='write_ram', addr=f'{a + j:08x}', val=f'{v >> (j * 8) & 255:02x}')

    def press(self, buttons): self.q(cmd='press', buttons=buttons, frames=4); time.sleep(.3)

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


def to_force_grid(g):
    # The modes menu (state 4) gives up after two seconds: Start on "Namco
    # presents" (state 3), then the line at once.
    # The first state 3, early in the boot, ignores Start: press it at every
    # state 3 until the menu comes.
    def menu():
        screen = g.value(SCREEN)
        if screen == 3:
            g.write(0x80097f26, 1, 1); g.write(0x80097f27, 1, 1)
            g.press(START)
        return g.value(SCREEN) == 4
    g.wait(menu, 'the modes menu never came', 180)
    time.sleep(.3)
    g.write(LINE, FORCE_LINE); g.press(CROSS)
    g.wait(lambda: g.value(SCREEN) == 9 and g.value(MODE) == 8 and g.value(PHASE, 2) <= 1 and g.value(P1_STATE) == 1,
           'the Tekken Force selector never came', 20)
    time.sleep(1.5)


def held(g, buttons, frames):
    """Records player 1 plays while buttons are held for that many frames."""
    records = set()
    g.q(cmd='set_input', buttons=buttons)
    end = g.q(cmd='frame')['frame'] + frames
    while g.q(cmd='frame')['frame'] < end: records.add(g.value(ACTOR + 0x54))
    g.q(cmd='set_input', buttons=0xffff)
    return records


def check_moves(g, ttt1, where):
    header = g.value(HEADER)
    assert in_pack(header) == ttt1, f'{where}: player 1 moves header {header:#x}, expected {"TTT1" if ttt1 else "T3"}'
    time.sleep(.5)
    records = held(g, DOWN_BACK_1, 60)
    time.sleep(1)
    from_pack = sorted(hex(r) for r in records if in_pack(r))
    print(f'{where}: header {header:#x}, d/b+1 records {sorted(hex(r) for r in records)}', flush=True)
    if ttt1: assert from_pack, f'{where}: d/b+1 played no TTT1 record'
    else: assert not from_pack, f'{where}: d/b+1 played TTT1 records on T3 moves: {from_pack}'


def run(g, native, ttt1, bosses):
    to_force_grid(g)
    for _ in range(30):
        if g.value(P1_FIGHTER) == IDS[native]: break
        g.press(RIGHT)
    else: raise AssertionError(f'the grid cursor never reached {native}')
    g.press(SQUARE)
    g.wait(lambda: 'P1 opens the MOVESET card' in g.log(), 'square did not open the MOVESET card (the bug)', 5)
    if ttt1:
        g.press(RIGHT)
        g.wait(lambda: 'P1 highlights TTT1 moves' in g.log(), 'right did not switch to TTT1 moves', 5)
    g.press(CROSS)
    g.wait(lambda: f'P1 takes {"TTT1" if ttt1 else "T3"} moves' in g.log(), 'X did not take the card', 5)
    g.wait(lambda: g.value(SCREEN) == 8 and g.value(PHASE, 2) >= 8, 'the stage never started', 90)
    assert g.value(ACTOR + 0x18, 2) == IDS[native], ('another fighter is playing', g.value(ACTOR + 0x18, 2))
    name = native.capitalize()
    assert (f'P1 {name} on the TTT1 moveset' in g.log()) == ttt1, f'player 1 is not on the {"TTT1" if ttt1 else "T3"} moves'
    check_moves(g, ttt1, 'stage start')
    # Walk right through the level, invincible, the clock held; a boss comes
    # in on phases 21..23 (TIPS § 56), then the fight goes on in phase 8.
    stage, full, i, met, in_boss, last = g.value(STAGE, 2), g.value(LIFE, 2), 0, 0, False, None
    end = time.monotonic() + 420 * bosses
    while met < bosses:
        if time.monotonic() > end: raise AssertionError(f'boss {met + 1} never came')
        screen, phase = g.value(SCREEN), g.value(PHASE, 2)
        if screen == 8 and phase >= 6:
            if g.value(LIFE, 2) < full: g.write(LIFE, full, 2)
            g.write(CLOCK, 0x7000, 2)
        if screen == 8 and g.value(STAGE, 2) != stage:
            stage = g.value(STAGE, 2)
            print(f'stage {stage:#x}: header {g.value(HEADER):#x}', flush=True)
            assert in_pack(g.value(HEADER)) == ttt1, f'stage {stage:#x}: player 1 lost the moves taken'
        boss_entry = screen == 8 and 21 <= phase <= 23
        if boss_entry and not in_boss:
            met += 1
            g.q(cmd='set_input', buttons=0xffff)
            g.wait(lambda: g.value(SCREEN) == 8 and g.value(PHASE, 2) == 8, f'boss {met}: the fight never started', 60)
            time.sleep(2)
            fighters = [g.value(ACTOR + k * 0x188c + 0x18, 2) for k in range(1, 4)]
            check_moves(g, ttt1, f'boss {met} (stage {g.value(STAGE, 2):#x}, actors 1..3: {fighters})')
        if (screen, phase) != last:
            last = (screen, phase); print(f'  screen {screen} phase {phase} stage {g.value(STAGE, 2):#x}', flush=True)
        in_boss = boss_entry
        i += 1
        g.q(cmd='set_input', buttons=WALK_PUNCH if i % 4 == 0 else WALK)
        time.sleep(.05)
    g.q(cmd='set_input', buttons=0xffff)
    if ttt1:
        log = g.log().split('P1 takes TTT1 moves', 1)[1]
        assert 'P1 back to the native moveset' not in log, 'the TTT1 moves were dropped on the way'


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--native', default='jin', choices=sorted(IDS))
    ap.add_argument('--t3', action='store_true')
    ap.add_argument('--bosses', type=int, default=0, help='go on through that many bosses (4: to Heihachi)')
    ap.add_argument('--build', default=str(ROOT / 'build-rel-lite'))
    ap.add_argument('--memcard-dir', default=str(ROOT / 'saves'), help='copied, never written')
    a = ap.parse_args()
    g = Game(Path(a.build), Path(a.memcard_dir), {'TEKKEN3_NATIVE_MOVES': ''})
    try:
        run(g, a.native, not a.t3, a.bosses)
        print('OK')
    finally:
        g.stop()


if __name__ == '__main__':
    main()
