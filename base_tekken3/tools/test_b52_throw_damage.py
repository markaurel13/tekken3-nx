#!/usr/bin/env python3
"""Damage of a guest's side and back throws (B52), without a player.

    python tools/test_b52_throw_damage.py [--build build-rel-lite] [--guest ganryu]
                                         [--victims 9,devil] [--sides front,left,right,back] [--exe <before>] [--visible]

Cold start (a blank memory card), Arcade selector, player 1 takes the guest on
the Tag page; player 2's fighter is written while the fight loads (0x800ADD5E,
TIPS.md section 25: a stock ID, or a guest's name) and left idle (+0xC5 = 0).
Both are placed face to face once in their stances (savestate 1); each try
reloads it, refills the round's clock (0x800ADBCC), turns the victim so that
it is grabbed from its front, left, right or back (as tools/test_ttt1_solo.py
does) and holds 1+3 four frames. The headless game runs unthrottled: waits
count frames. It records the attacker's records (pack numbers) and the
victim's life before and after.

TTT1 (800FB534) deals a throw's damage as soon as the attacker's timed
property 24 (T3 property 11: +0xE4 = 1) fires, whatever the victim does. T3
(800449B4) deals it only to a victim still flagged as thrown (+0x74 < 0).
Ganryu's left and right throws let their victim go (a 0x0010 entry, type 6,
which clears +0x74) at frames 60 / 51, before their damage at 66 / 58: the
throw went through and dealt nothing (B52). Checks: each throw reaches its
pack record and takes the sum of its damage properties from the victim's
life (Kunimitsu's right throw: twenty of 1, then 20).
Exit status 1 when a throw takes the wrong damage, 2 when one never caught
(the press missed three times: nothing measured).
"""
import argparse, os, shutil, socket, struct, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'psxrecomp/tools'))
import debug_client

WORK = ROOT / 'out/b52'
STATE, PHASE, CURSOR_CHAR, CELL, GRID = 0x800ae204, 0x800ae224, 0x80118668, 0x80118650, 0x80118648
ACTOR, STRIDE, CLOCK = 0x800a9228, 0x188c, 0x800adbcc
START, CROSS, SQUARE, R2, RIGHT, LEFT, DOWN, UP = 0xfff7, 0xbfff, 0x7fff, 0xfdff, 0xffdf, 0xff7f, 0xffbf, 0xffef
THROW_13 = 0x3fff                       # Square + Cross: 1+3
GUEST_ID, PACK = 23, 8192
# Turn of the victim from facing the attacker (its idle stance at contact).
SIDES = {'front': 0, 'left': 0x4000, 'right': 0xc000, 'back': 0x8000}


class Game:
    def __init__(self, build, tag, visible=False, exe=None):
        run = WORK / f'run-{os.getpid()}-{tag}'
        shutil.rmtree(run, ignore_errors=True); run.mkdir(parents=True)
        built = next(p for p in (build / 'Tekken_3_Expanded.exe', build / 'Tekken_3_Expanded') if p.exists())
        shutil.copy2(exe or built, run / built.name)
        exe = built
        for name in ('bios', 'mods'):       # links, not copies: the catalogue is large
            if os.name == 'nt':
                subprocess.run(['cmd', '/c', 'mklink', '/J', str(run / name), str(build / name)], check=True, capture_output=True)
            else: (run / name).symlink_to(build / name)
        (run / 'settings.toml').write_text('[video]\nsupersampling=1\nantialiasing=false\n'
                                           '[controller]\np1_device="keyboard"\np2_device="none"\n')
        cards = run / 'cards'; cards.mkdir()
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0)); self.port = listener.getsockname()[1]
        self.log_path = WORK / f'{tag}.log'
        env = {k: v for k, v in os.environ.items() if not k.startswith('TEKKEN3_')}
        env['SDL_AUDIO_DRIVER'] = 'dummy'
        args = [str(run / exe.name), '--game', str(ROOT / 'game.toml'), '--disc', str(ROOT / 'disc/Tekken 3 (USA).cue'),
                '--no-launcher', '--renderer', 'opengl' if visible else 'software', '--debug-port', str(self.port),
                '--memcard-dir', str(cards)] + ([] if visible else ['--headless'])
        self.out = self.log_path.open('w')
        self.process = subprocess.Popen(args, cwd=ROOT, stdout=self.out, stderr=self.out, env=env)
        self.run = run
        deadline = time.monotonic() + 60
        while True:
            if self.process.poll() is not None: raise RuntimeError(f'game exited; see {self.log_path}')
            try:
                with socket.create_connection(('127.0.0.1', self.port), .2): break
            except OSError:
                if time.monotonic() > deadline: raise RuntimeError('debug endpoint did not start')
                time.sleep(.2)

    def q(self, **r):
        for _ in range(100):
            result = debug_client.query('127.0.0.1', self.port, r)
            if result.get('err') != 'emu busy or frozen': break
            time.sleep(.2)
        if result.get('ok') is False: raise RuntimeError((r, result))
        return result

    def read(self, a, n=4): return bytes.fromhex(self.q(cmd='read_ram', addr=f'{a:08x}', len=n)['hex'])
    def value(self, a, n=4): return int.from_bytes(self.read(a, n), 'little')
    def write(self, a, data): self.q(cmd='write_ram', addr=f'{a:08x}', bytes=data.hex())
    def press(self, b, frames=4): self.q(cmd='press', buttons=b, frames=frames); time.sleep(.25)
    def frame(self): return self.q(cmd='frame')['frame']
    def frames(self, n):
        # The headless game runs unthrottled (three times as fast or more):
        # count frames, not seconds.
        end = self.frame() + n
        while self.frame() < end: time.sleep(.002)

    def wait(self, predicate, what, seconds=120, step=None):
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            if predicate(): return
            if step: step()
            time.sleep(.1)
        raise AssertionError(what)

    def stop(self):
        if self.process.poll() is None:
            self.process.terminate()
            try: self.process.wait(timeout=10)
            except subprocess.TimeoutExpired: self.process.kill()
        self.out.close()
        for name in ('bios', 'mods'):
            if os.name == 'nt': subprocess.run(['cmd', '/c', 'rmdir', str(self.run / name)], capture_output=True)
            else: (self.run / name).unlink(missing_ok=True)
        shutil.rmtree(self.run, ignore_errors=True)


def tag_place(k): return divmod(k, 8) if k < 16 else (k - 16, 8)


def to_fight(g, guest_index, victim):
    g.q(cmd='turbo', enabled=1)
    g.wait(lambda: g.value(STATE) == 4, 'mode menu not reached', 240, lambda: g.press(START))
    g.q(cmd='turbo', enabled=0)
    time.sleep(1)
    g.wait(lambda: g.value(STATE) == 9 and g.value(GRID) == 21, 'selector not reached', 90,
           lambda: (g.press(CROSS), time.sleep(.4)))
    time.sleep(1.5)
    g.press(R2); time.sleep(2.5)
    row, col = tag_place(guest_index)
    for _ in range(14):
        r, c = tag_place(g.value(CELL))
        if (r, c) == (row, col): break
        g.press(DOWN if r < row else UP if r > row else RIGHT if c < col else LEFT)
    g.wait(lambda: g.value(CURSOR_CHAR) == GUEST_ID + guest_index, 'player 1 did not reach the guest', 10)
    g.press(SQUARE)
    g.wait(lambda: g.value(STATE) == 11, 'loading screen not reached', 30)
    g.write(0x800add5e, struct.pack('<H', victim)); g.write(0x800add9a, b'\0\0')
    g.wait(lambda: g.value(STATE) == 8 and g.value(PHASE, 2) >= 6, 'round did not start', 120)
    g.wait(lambda: g.value(ACTOR + STRIDE + 0x16, 1) == (GUEST_ID if victim >= GUEST_ID else victim) or
           g.value(ACTOR + STRIDE + 0x14, 1) == victim, 'player 2 is not the victim', 10)
    g.write(ACTOR + STRIDE + 0xc5, b'\0')     # the CPU stays idle
    time.sleep(3)                       # past the intros


def life(g): return struct.unpack('<i', g.read(ACTOR + STRIDE + 0x3f4))[0] / 65536


def gated(g, writes):
    """Actor fields written with both actors' updates held (test_ttt1_solo)."""
    for p in (0, 1): g.write(ACTOR + p * STRIDE + 0xc3, b'\0')
    for a, data in writes: g.write(a, data)
    for p in (0, 1): g.write(ACTOR + p * STRIDE + 0xc3, b'\1')


def attempt(g, side):
    g.q(cmd='savestate', op='load', slot=1); time.sleep(1.5)
    # The round's clock (frames, as in Tekken Force): a round over takes no
    # damage, and the walk into contact may have used most of it.
    g.write(CLOCK, struct.pack('<H', 3600))
    v = ACTOR + STRIDE
    table = g.value(g.value(0x800adc24) + 12)
    turn = SIDES[side] if side in SIDES else int(side, 16)
    facing = struct.pack('<H', (g.value(v + 0xe, 2) + turn) & 0xffff)
    # Back to its stance (alias 3), then turned: grabbed from that side.
    g.q(cmd='clear_input')
    gated(g, [(v + 0x6e, b'\0\0'), (v + 0x198, struct.pack('<5H', 1, 6, 0, 0, 3)), (v + 0x1a4, g.read(table + 12))])
    g.frames(4)
    gated(g, [(v + 0xe, facing), (v + 0x2c, facing), (v + 0xb9, b'\0')])
    g.frames(2)
    before = life(g)
    g.q(cmd='set_input', buttons=f'{THROW_13:04x}'); g.frames(4); g.q(cmd='clear_input')
    moves, end = [], g.frame() + 240
    while g.frame() < end:
        m = g.value(ACTOR + 0xa0, 2)
        if not moves or moves[-1] != m: moves.append(m)
        time.sleep(.01)
    print(f"  life {before:g} -> {life(g):g}", flush=True)
    return moves, before - life(g)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', default='build-rel-lite')
    ap.add_argument('--guest', default='ganryu')
    ap.add_argument('--victims', default='9,devil', help='stock IDs (Jin = 9) or guest names')
    ap.add_argument('--sides', default='front,left,right,back')
    ap.add_argument('--exe', help="another executable for the build's data (the build before a change)")
    ap.add_argument('--visible', action='store_true')
    a = ap.parse_args()
    build = (ROOT / a.build).resolve()
    catalogue = [l.split()[0] for l in (build / 'mods/ttt1/guests.txt').read_text().splitlines() if l.split()]
    pack = (build / f'mods/ttt1/{a.guest.capitalize()}-TTT1-combat.jmv').read_bytes()
    n, rb = struct.unpack_from('<8I', pack)[3], struct.unpack_from('<8I', pack)[7]
    damage = {}         # what a throw's record takes: its timed damage properties (T3 11)
    for i in range(n):
        r = struct.unpack_from('<14I', pack, rb + i * 56)
        at, total = r[8], 0
        while at and struct.unpack_from('<H', pack, at)[0]:
            op = struct.unpack_from('<H', pack, at + 2)[0]
            if op >> 8 == 0x0b: total += (op & 255) or r[5] & 0xffff
            at += 4
        if total: damage[PACK + i] = total
    WORK.mkdir(parents=True, exist_ok=True)
    ok, missed = True, False      # a miss (no throw caught) says nothing of the damage
    for victim in a.victims.split(','):
        vid = int(victim) if victim.isdigit() else GUEST_ID + catalogue.index(victim)
        g = Game(build, f'{a.guest}-{victim}', a.visible, a.exe and Path(a.exe))
        try:
            to_fight(g, catalogue.index(a.guest), vid)
            x = lambda p: struct.unpack('<i', g.read(ACTOR + p * STRIDE, 4))[0]
            # Face to face, 900 apart (test_ttt1_solo align()), both in their
            # stance: a walk into contact stopped anywhere between 700 and 1300,
            # and a guest placed during its intro walked off again (Roger -520).
            g.wait(lambda: g.value(ACTOR + 0xa0, 2) == 3, 'player 1 did not reach its stance', 30)
            for _ in range(10):
                writes = []
                for p, xp, facing in ((0, -450, 0x4000), (1, 450, 0xc000)):
                    a_ = ACTOR + p * STRIDE
                    writes += [(a_, struct.pack('<3i', xp, 0, 0)), (a_ + 0xf68, struct.pack('<3i', xp, 0, 0)),
                               (a_ + 0xe, struct.pack('<H', facing)), (a_ + 0x2c, struct.pack('<H', facing))]
                gated(g, writes)
                g.frames(30)
                if abs(x(1) - x(0)) < 1600 and g.value(ACTOR + 0xa0, 2) == 3: break
            else: raise AssertionError(f'could not place the fighters ({x(0)}, {x(1)})')
            g.q(cmd='savestate', op='save', slot=1); time.sleep(1.5)
            for side in a.sides.split(','):
                for go in range(3):     # the press now and then misses the throw: again
                    moves, lost = attempt(g, side)
                    throw = [m for m in moves if m in damage]
                    if throw: break
                want = damage[throw[0]] if throw else None
                verdict = 'MISS' if not throw else 'OK' if abs(lost - want) < .5 else 'FAIL'
                missed |= verdict == 'MISS'; ok &= verdict != 'FAIL'
                print(f'{a.guest} {side} on {victim}:', verdict,
                      f'record {throw[0] - PACK if throw else None}, throw damage {want}, life lost {lost:g}',
                      f'(moves {[m - PACK if m >= PACK else hex(m) for m in moves[:6]]})', flush=True)
        finally:
            g.stop()
    print('B52:', 'FAIL' if not ok else 'INCOMPLETE (a throw missed)' if missed else 'OK')
    sys.exit(1 if not ok else 2 if missed else 0)


if __name__ == '__main__':
    main()
