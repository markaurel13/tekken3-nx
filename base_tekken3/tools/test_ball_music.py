#!/usr/bin/env python3
"""Tekken Ball: the fight's music with a TTT1 guest (B43, GH#91).

    python tools/test_ball_music.py [--fighter jin|kazuya|...] [--cpu guest|native] [--build build-rel-lite]
                                    [--memcard-dir saves]

Boots a private copy of the build (software renderer, headless, x1) on a copy
of the memory card, unlocks Tekken Ball (0x80097F26/27) and skips the save's
first match against Gon (0x80097F1E = 1: the CPU's fighter is drawn), opens
the modes menu with Start on "Namco presents" (state 3), takes line 6 (Tekken
Ball) and picks player 1's fighter on TEKKEN BALL SELECT (a guest on the Tag
page, R2), then the ball. --cpu guest makes the CPU's draw a guest every time
(TEKKEN3_BALL_GUESTS=1), --cpu native never (=0).

Tekken Ball takes its music from the CPU's fighter (0x8004F67C, the same
player as Arcade: measured, P1 Heihachi against Gon plays Gon's 0x1A), byte
11 of its record (0x80097D40 + ID * 16). For a guest, the stock fighter whose
arena and music it takes (guests.txt, third column; Jin's when none). In the
fight: the music (0x800ADD08 and its copy 0x800AFAA3) must be that one, and
the arena Tekken Ball's (19). Build: rel-lite (debug server lite).
Before the fix (Jin against Kunimitsu, 24, who plays Yoshimitsu's music):
"music 0x1 (copy 0x1), expected 0xb". After: Kazuya drawn, Heihachi's 0x14.
"""
import argparse, os, shutil, socket, subprocess, sys, tempfile, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'psxrecomp/tools'))
import debug_client

SCREEN, PHASE, MODE, LINE, BLOCKS = 0x800ae204, 0x800ae224, 0x800afa88, 0x80097f40, 0x800b8d70
ARENA, ARENA_COPY, MUSIC, MUSIC_COPY, FIGHTERS, RECORDS = 0x800adc84, 0x800afaa2, 0x800add08, 0x800afaa3, 0x800add5c, 0x80097d40
START, CROSS, RIGHT, LEFT, UP, DOWN, R2 = 0xfff7, 0xbfff, 0xffdf, 0xff7f, 0xffef, 0xffbf, 0xfdff
BALL_LINE, BALL_ARENA, GUEST_ID = 6, 19, 23
CELLS = {'law': (0, 0), 'paul': (6, 0), 'xiaoyu': (0, 1), 'yoshimitsu': (1, 1), 'nina': (2, 1),
         'heihachi': (3, 1), 'king': (4, 1), 'lei': (5, 1), 'jin': (6, 1), 'hwoarang': (0, 2),
         'bryan': (2, 2), 'anna': (5, 2), 'eddy': (6, 2)}
IDS = {'paul': 0, 'law': 1, 'lei': 2, 'king': 3, 'yoshimitsu': 4, 'nina': 5, 'hwoarang': 6,
       'xiaoyu': 7, 'eddy': 8, 'jin': 9, 'bryan': 12, 'heihachi': 13, 'anna': 18}
MOKUJIN = 15
EXE = 'Tekken_3_Expanded.exe' if os.name == 'nt' else 'Tekken_3_Expanded'


class Game:
    def __init__(self, build, memcards, env):
        self.dir = Path(tempfile.mkdtemp(prefix='ball-music-'))
        run, saves = self.dir / 'run', self.dir / 'saves'
        run.mkdir(); (saves / 'openbios').mkdir(parents=True)
        shutil.copy2(build / EXE, run / EXE)
        for name in ('bios', 'mods', 'assets'):
            if (build / name).is_dir(): shutil.copytree(build / name, run / name)
        for card in ('card1.mcd', 'card2.mcd'):
            if (memcards / card).is_file(): shutil.copy2(memcards / card, saves / card)
        (run / 'settings.toml').write_text('[video]\nsupersampling=1\nantialiasing=false\n'
                                           '[controller]\np1_device="keyboard"\np2_device="none"\n')
        self.guests = [line.split() for line in (run / 'mods/ttt1/guests.txt').read_text().splitlines() if line.split()]
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0)); self.port = listener.getsockname()[1]
        self.logfile = self.dir / 'game.log'
        environment = {k: v for k, v in os.environ.items() if not k.startswith('TEKKEN3_')}
        environment.update(env, SDL_AUDIO_DRIVER='dummy', TEKKEN3_NATIVE_MOVES='0')   # natives: no MOVESET card
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

    def stop(self):
        if self.process.poll() is None:
            self.process.terminate()
            try: self.process.wait(timeout=10)
            except subprocess.TimeoutExpired: self.process.kill()
        shutil.rmtree(self.dir, ignore_errors=True)


def to_ball_grid(g):
    # The modes menu (state 4) gives up after two seconds, and the first
    # state 3 of the boot ignores Start: press it at every state 3.
    def menu():
        if g.value(SCREEN) == 3:
            g.write(0x80097f26, 1, 1); g.write(0x80097f27, 1, 1)
            g.write(0x80097f1e, 1, 1)                 # past the first match's Gon
            g.press(START)
        return g.value(SCREEN) == 4
    g.wait(menu, 'the modes menu never came', 180)
    time.sleep(.3)
    g.write(LINE, BALL_LINE); g.press(CROSS)
    g.wait(lambda: g.value(SCREEN) == 10 and g.value(MODE) == 7 and g.value(BLOCKS) == 3, 'TEKKEN BALL SELECT never came', 20)
    time.sleep(1)


def pick(g, fighter):
    """The fighter's cell: a guest on the Tag page (k % 6, k / 6). Returns its ID."""
    names = [guest[0] for guest in g.guests]
    if fighter in names:
        k = names.index(fighter)
        g.press(R2); time.sleep(1.5)
        cell, want = (k % 6, k // 6), GUEST_ID + k
    else: cell, want = CELLS[fighter], IDS[fighter]
    for _ in range(20):
        x, y = g.value(BLOCKS + 8), g.value(BLOCKS + 12)
        if (x, y) == cell: break
        g.press(DOWN if y < cell[1] else UP if y > cell[1] else RIGHT if x < cell[0] else LEFT)
    else: raise AssertionError(f'the grid cursor never reached {fighter}')
    g.press(CROSS)
    g.wait(lambda: g.value(BLOCKS) == 15, 'the ball choice never came', 10)
    assert g.value(BLOCKS + 56) >> 2 == want, ('another fighter was picked', g.value(BLOCKS + 56) >> 2, want)
    return want


def fighter_id(g, name):
    names = [guest[0] for guest in g.guests]
    return GUEST_ID + names.index(name) if name in names else IDS[name]


def expected_music(g, fighter_id):
    """Byte 11 of the record of the stock fighter whose music this one plays."""
    owner = fighter_id
    if fighter_id >= GUEST_ID:
        column = g.guests[fighter_id - GUEST_ID][2] if len(g.guests[fighter_id - GUEST_ID]) > 2 else ''
        owner = int(column) if column.isdigit() and int(column) < 23 else 9
    return g.value(g.value(RECORDS + owner * 16) + 11, 1), owner


def run(g, fighter, cpu):
    to_ball_grid(g)
    me = pick(g, fighter)
    # X takes the ball; the loading screen (11) lasts some twenty frames.
    g.q(cmd='set_input', buttons=CROSS)
    end = time.monotonic() + 10
    while g.value(SCREEN) != 11:
        if time.monotonic() > end: raise AssertionError('the loading screen never came')
    g.q(cmd='set_input', buttons=0xffff)
    g.wait(lambda: g.value(SCREEN) == 8 and g.value(PHASE, 2) >= 6, 'the fight never started', 90)
    time.sleep(2)
    fighters = g.value(FIGHTERS, 2), g.value(FIGHTERS + 2, 2)
    music, copy, arena = g.value(MUSIC, 2), g.value(MUSIC_COPY, 1), g.value(ARENA, 2)
    if cpu == 'guest' and fighters[1] == MOKUJIN: return False   # the guests' pool has Mokujin too
    want, owner = expected_music(g, fighters[1])
    print(f'fighters {fighters}: arena {arena} (copy {g.value(ARENA_COPY, 1)}), music {music:#x} (copy {copy:#x}); '
          f'CPU fighter {fighters[1]} plays the music of fighter {owner}: {want:#x}', flush=True)
    assert fighters[0] == me, ('player 1 is not the fighter picked', fighters)
    assert (fighters[1] >= GUEST_ID) == (cpu == 'guest'), ('the CPU drew', fighters[1])
    assert arena == BALL_ARENA, f'arena {arena}, expected {BALL_ARENA} (Tekken Ball)'
    assert music == want and copy == want, f'music {music:#x} (copy {copy:#x}), expected {want:#x}'
    return True


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--fighter', default='jin', help='a guest of guests.txt or a native of the grid')
    ap.add_argument('--cpu', default='guest', choices=['guest', 'native'])
    ap.add_argument('--build', default=str(ROOT / 'build-rel-lite'))
    ap.add_argument('--memcard-dir', default=str(ROOT / 'saves'), help='copied, never written')
    a = ap.parse_args()
    for _ in range(4):
        g = Game(Path(a.build), Path(a.memcard_dir), {'TEKKEN3_BALL_GUESTS': '1' if a.cpu == 'guest' else '0'})
        try:
            if run(g, a.fighter, a.cpu): break
            print('the CPU drew Mokujin, not a guest: again', flush=True)
        finally:
            g.stop()
    else: raise AssertionError('the CPU never drew a guest')
    print('OK')


if __name__ == '__main__':
    main()
