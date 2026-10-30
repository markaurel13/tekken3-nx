#!/usr/bin/env python3
"""Jin's Omen (TTT1): Force (b+1+2), then 2,2,3, on Jin's TTT1 moves (F01, TIPS § 88).

    python tools/test_jin_omen.py [--build build-rel-lite] [--memcard-dir saves] [--shots DIR]
                                  [--cpu-right N] [--recoil]

Boots a private copy of the build (software renderer, headless, x1) on a copy
of the memory card, opens Practice from the modes menu, picks Jin with square,
switches his MOVESET card to TEKKEN TAG TOURNAMENT and takes it; the CPU is
whoever player 1's pad lands on (--cpu-right N cells further). In the fight,
player 1 plays Force then 2,2,3 (frame-exact, the debug server's input
route), and the records he goes through are named by their TTT1 alias, read
from the pack (Jin-TTT1-combat.jmv) at the base the game log gives. Checks:
the Omen stance (alias 0x313) is reached, crouching leaves it, and from it
each Omen move starts in its default version (Corpse Hitter, 7 Hit Combo to
its last hit, Dragon Uppercut, Rising Uppercut, Hell Brusher, Stonehead).
The Omen's rules on the opponent: Kazuya's (--cpu-tag: the Tag page, Kazuya
first) Corpse Hitter 0x324 and Rising Uppercut 0x328 (more damage), Jin's and
Heihachi's Rising Uppercut 0x329, and with the dummy on GUARD ALL a guarded
Corpse Hitter deflected (0xED2) by them only.
--recoil instead: Arcade (life not held), Jin's life before and after a
Corpse Hitter out of reach (TTT1: 9). Build: rel-lite (debug server lite).
Before the fix: "Force, 2,2,3 never reached the Omen stance".
"""
import argparse, os, re, shutil, socket, struct, subprocess, sys, tempfile, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'psxrecomp/tools'))
import debug_client

SCREEN, PHASE, MODE, LINE = 0x800ae204, 0x800ae224, 0x800afa88, 0x80097f40
P1_FIGHTER, ACTOR, STRIDE, LIFE = 0x80118668, 0x800a9228, 0x188c, 0x800a961e
TOTAL_DMG = 0x800b8b94                                # Practice's TOTAL DMG (TIPS § 6)
DUMMY, GUARD_ALL = 0x800b8bdd, 4                       # Practice dummy action (TIPS § 6)
DEFLECTED, DEFLECTORS = 0xed2, {9, 13}                 # (Devil, 27, is a guest: key not readable)
NONE, START, CROSS, SQUARE, RIGHT, LEFT, DOWN, R2 = 0xffff, 0xfff7, 0xbfff, 0x7fff, 0xffdf, 0xff7f, 0xffbf, 0xfdff
# Tekken buttons on the pad (active low): 1 square, 2 triangle, 3 cross, 4 circle.
B1, B2, B3, B4 = 0x8000, 0x1000, 0x4000, 0x2000
D_UP, D_RIGHT, D_DOWN, D_LEFT = 0x10, 0x20, 0x40, 0x80
PRACTICE_LINE, JIN = 8, 9          # all lines open (TIPS Â§ 6): Ball 6, Force 7, Practice 8
EXE = 'Tekken_3_Expanded.exe' if os.name == 'nt' else 'Tekken_3_Expanded'
# TTT1 aliases (captures/unknown@jin-select-ram.bin): Force, the Omen
# transformation, its stance, and its own moves.
FORCE, OMEN_IN, OMEN = 0x2af, 0x312, 0x313


def pad(*bits):
    v = NONE
    for b in bits: v &= ~b
    return v


def pack_records(path, ram_aliases):
    """Record index in the pack -> its TTT1 alias numbers (the metadata keeps
    each record's TTT1 address)."""
    blob = Path(path).read_bytes()
    magic, _, _, count = struct.unpack_from('<4I', blob)
    assert magic == 0x314d554a, path
    by_address = {}
    for i, a in enumerate(ram_aliases): by_address.setdefault(a, set()).add(i)
    return [by_address.get(struct.unpack_from('<I', blob, 32 + i * 16 + 4)[0], set()) for i in range(count)]


class Game:
    def __init__(self, build, memcards, env, scale=1, renderer='software', visible=False):
        self.dir = Path(tempfile.mkdtemp(prefix='jin-omen-'))
        run, saves = self.dir / 'run', self.dir / 'saves'
        run.mkdir(); (saves / 'openbios').mkdir(parents=True)
        shutil.copy2(build / EXE, run / EXE)
        for name in ('bios', 'mods', 'assets'):
            if (build / name).is_dir(): shutil.copytree(build / name, run / name)
        for card in ('card1.mcd', 'card2.mcd'):
            if (memcards / card).is_file(): shutil.copy2(memcards / card, saves / card)
        if visible and (build / 'settings.toml').is_file():     # the player's own settings
            shutil.copy2(build / 'settings.toml', run / 'settings.toml')
        else:
            (run / 'settings.toml').write_text(f'[video]\nsupersampling={scale}\nantialiasing=false\n'
                                               '[controller]\np1_device="keyboard"\np2_device="none"\n')
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0)); self.port = listener.getsockname()[1]
        self.logfile = self.dir / 'game.log'
        environment = {k: v for k, v in os.environ.items() if not k.startswith('TEKKEN3_')}
        environment.update(env, **({} if visible else {'SDL_AUDIO_DRIVER': 'dummy'}))
        with self.logfile.open('w') as out:
            self.process = subprocess.Popen(
                [str(run / EXE), '--game', str(ROOT / 'game.toml'), '--disc', str(ROOT / 'disc/Tekken 3 (USA).cue'),
                 '--no-launcher', *([] if visible else ['--renderer', renderer, '--headless']),
                 '--debug-port', str(self.port),
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

    def wait(self, predicate, description, seconds=30):
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            if predicate(): return
            time.sleep(.05)
        raise AssertionError(description)

    def log(self): return self.logfile.read_text(errors='replace')

    def shot(self, path): self.q(cmd='screenshot_file', path=str(path))

    def stop(self):
        if self.process.poll() is None:
            self.process.terminate()
            try: self.process.wait(timeout=10)
            except subprocess.TimeoutExpired: self.process.kill()
        shutil.rmtree(self.dir, ignore_errors=True)


def to_mode(g, line, mode):
    def menu():
        if g.value(SCREEN) == 3:
            g.write(0x80097f26, 1, 1); g.write(0x80097f27, 1, 1)
            g.press(START)
        return g.value(SCREEN) == 4
    g.wait(menu, 'the modes menu never came', 180)
    time.sleep(.3)
    g.write(LINE, line); g.press(CROSS)
    g.wait(lambda: g.value(SCREEN) == 9 and g.value(MODE) == mode, f'the selector of mode {mode} never came', 20)
    time.sleep(1.5)


def pick_jin(g, cpu_right=0, cpu_tag=False):
    for _ in range(30):
        if g.value(P1_FIGHTER) == JIN: break
        g.press(RIGHT)
    else: raise AssertionError('the grid cursor never reached Jin')
    before = g.log().count('P1 opens the MOVESET card')
    g.press(SQUARE)
    g.wait(lambda: g.log().count('P1 opens the MOVESET card') > before, 'square did not open the MOVESET card', 5)
    g.press(RIGHT)
    g.wait(lambda: 'P1 highlights TTT1 moves' in g.log(), 'right did not switch to TTT1 moves', 5)
    g.press(CROSS)
    g.wait(lambda: 'P1 takes TTT1 moves' in g.log(), 'X did not take the card', 5)
    # Practice: player 1's pad then picks the CPU, `cpu_right` cells right.
    time.sleep(1.5)
    if cpu_tag:                                       # R2: the Tag page (guests; Kazuya first)
        g.press(R2); time.sleep(1.5)
    for _ in range(cpu_right): g.press(RIGHT)
    for _ in range(10):
        if g.value(SCREEN) == 8: break
        g.press(CROSS); time.sleep(.7)
    g.wait(lambda: g.value(SCREEN) == 8 and g.value(PHASE, 2) >= 8, 'the fight never started', 90)
    time.sleep(3)


class Moves:
    def __init__(self, g, table):
        self.g, self.table, self.base = g, table, None

    def aliases(self):
        """Player 1's record and its TTT1 aliases; the pack's records start
        where the runtime says (game log: "installed P1 alias table ... records=")."""
        if self.base is None:
            found = re.findall(r'installed P1 alias table.*records=([0-9A-F]{8})', self.g.log())
            assert found, 'the game log does not give player 1 records'
            self.base = int(found[-1], 16)
        record = self.g.value(ACTOR + 0x54)
        k, r = divmod(record - self.base, 56)
        return record, self.table[k] if not r and 0 <= k < len(self.table) else set()

    def play(self, steps, settle=40):
        """steps: (buttons, frames) held in turn, frame-exact (the debug
        server's input route); then `settle` idle frames. Returns the TTT1
        aliases player 1 went through, in order (sampled)."""
        g, seen = self.g, []
        g.q(cmd='input_route_clear')
        for buttons, frames in steps + [(NONE, settle)]:
            g.q(cmd='input_route_append', buttons=buttons, frames=frames)
        g.q(cmd='input_route_start')
        # Until the route has played out (its last step is the settle).
        while g.q(cmd='input_route_status').get('active'):
            _, al = self.aliases()
            k = min(al) if al else None
            if k is not None and (not seen or seen[-1] != k): seen.append(k)
        return seen


def force_omen(lead=14):
    """Force (b+1+2), then 2,2,3 two frames each: the entry holds on Force's
    second record (alias 0x2AF) frames 1..20, the sequence in the last 50
    frames; 3 pressed 14 to 26 frames after b+1+2 (measured; 8 and 32: no)."""
    return [(pad(D_LEFT, B1, B2), 2), (NONE, lead), (pad(B2), 2), (NONE, 2), (pad(B2), 2), (NONE, 2), (pad(B3), 2)]


# From the stance: name, inputs, the aliases one of which must be played
# (TTT1 aliases; Dragon Uppercut and Stonehead are records Jin shares).
# f, N, d, d/f each move the stance on (0x314, 0x316, 0x317, 0x318); d/f
# alone, then the button (0x318, frames 1..19). The 7 Hit Combo: the first
# two chain within frames 1..10, the next ones near frame 17 of the hit before.
FNDDF = [(pad(D_RIGHT), 2), (NONE, 2), (pad(D_DOWN), 2), (pad(D_DOWN, D_RIGHT), 2)]
SEVEN = [(pad(B1), 2), (NONE, 4), (pad(B2), 2), (NONE, 4), (pad(B2), 2), (NONE, 12), (pad(B3), 2), (NONE, 14),
         (pad(B2), 2), (NONE, 14), (pad(B4), 2), (NONE, 16), (pad(B3), 2)]
OMEN_MOVES = [
    ('1+2 Corpse Hitter', [(pad(B1, B2), 3)], {0x323, 0x324}),   # 0x324: against Kazuya
    ('1,2,2,3,2,4,3 7 Hit Combo', SEVEN, {0x32e}),
    ('f,N,d,d/f+1 Dragon Uppercut', FNDDF + [(pad(D_DOWN, D_RIGHT, B1), 3)], {0x9c}),
    ('f,N,d,d/f+2 Rising Uppercut', FNDDF + [(pad(D_DOWN, D_RIGHT, B2), 3)], {0x321, 0x328, 0x329}),
    ('f,N,d,d/f+4 Hell Brusher', FNDDF + [(pad(D_DOWN, D_RIGHT, B4), 3)], {0x32b}),
    ('f,f+1+2 Stonehead', [(pad(D_RIGHT), 2), (NONE, 2), (pad(D_RIGHT, B1, B2), 3)], {0x16d}),
]


def recoil(g, m):
    """Arcade (life not held): Jin's life before and after an Omen Corpse
    Hitter thrown at once, out of reach (the opponent is still far)."""
    to_mode(g, 0, 0)
    pick_jin(g)
    for attempt in range(3):
        before = g.value(LIFE, 2)
        seen = m.play(force_omen() + [(NONE, 45), (pad(B1, B2), 3)], settle=90)
        after = g.value(LIFE, 2)
        print(f'Arcade, Corpse Hitter: {[hex(a) for a in seen]} ; P1 life {before} -> {after}', flush=True)
        if 0x323 in seen: return before - after
        time.sleep(1)
    raise AssertionError('no Corpse Hitter in Arcade')


# The Omen's rules on the opponent (TTT1 moveset keys): its Corpse Hitter and
# Rising Uppercut versions. Natives carry their key at actor +0x16 (<= 18);
# a guest's (23) is its own TTT1 moveset, not readable here: not checked.
VARIANTS = {'1+2 Corpse Hitter': {18: 0x324},
            'f,N,d,d/f+2 Rising Uppercut': {18: 0x328, 28: 0x328, 9: 0x329, 13: 0x329, 27: 0x329}}
DEFAULTS = {'1+2 Corpse Hitter': 0x323, 'f,N,d,d/f+2 Rising Uppercut': 0x321}


def run(g, table, shots, cpu_right, cpu_tag=False):
    to_mode(g, PRACTICE_LINE, 5)
    pick_jin(g, cpu_right, cpu_tag)
    cpu_key = g.value(ACTOR + STRIDE + 0x16, 2)
    print(f'CPU: character {g.value(ACTOR + STRIDE + 0x18, 2)}, key {cpu_key}', flush=True)
    assert g.value(ACTOR + 0x18, 2) == JIN, 'player 1 is not Jin'
    assert 'P1 Jin on the TTT1 moveset' in g.log(), 'Jin is not on his TTT1 moves'
    m = Moves(g, table)
    hexes = lambda s: [hex(a) for a in s]
    for attempt in range(3):          # the fight's first input after the bench's wait can be lost
        seen = m.play(force_omen(), settle=60)
        print(f'Force, 2,2,3: {hexes(seen)}', flush=True)
        if OMEN in seen: break
        time.sleep(1.5)
    assert OMEN in seen, 'Force, 2,2,3 never reached the Omen stance'
    if shots: g.shot(Path(shots) / 'omen-stance.png')
    seen = m.play([(pad(D_DOWN), 20)], settle=30)
    print(f'crouch from the stance: {hexes(seen)}', flush=True)
    assert seen and seen[-1] != OMEN, 'crouching did not leave the Omen stance'
    failed = []
    # Deflection: the dummy guards everything; a guarded Omen move turns into
    # alias 0xED2 against Jin, Heihachi and Devil only. At the starting
    # distance (the moves below move both fighters).
    if cpu_key <= 18:
        g.write(DUMMY, GUARD_ALL, 1)
        time.sleep(1)
        m.play([(pad(D_DOWN), 20)], settle=40)
        seen = m.play(force_omen() + [(NONE, 45), (pad(B1, B2), 3)], settle=100)
        g.write(DUMMY, 0, 1)
        deflected = DEFLECTED in seen
        print(f'guarded Corpse Hitter against key {cpu_key}: {hexes(seen)} ; deflected {deflected}', flush=True)
        if deflected != (cpu_key in DEFLECTORS): failed.append(f'deflection against key {cpu_key}')
    for name, steps, want in OMEN_MOVES:
        # Two tries: a tight window (f,N,d,d/f) is sometimes missed by a frame.
        for attempt in range(2):
            time.sleep(1.5)
            total = g.value(TOTAL_DMG, 2)
            m.play([(pad(D_DOWN), 20)], settle=40)     # out of the stance a previous move left
            # Force ends ~20 frames after the 3, then the transformation (alias
            # 0x312, 20 frames) takes no input: the move once in the stance.
            seen = m.play(force_omen() + [(NONE, 45)] + steps, settle=100)
            print(f'{name}{" (again)" if attempt else ""}: {hexes(seen)} ; damage {g.value(TOTAL_DMG, 2) - total}', flush=True)
            after = seen[seen.index(OMEN) + 1:] if OMEN in seen else []
            if want & set(after): break
        if shots: g.shot(Path(shots) / f'omen-{name.split()[-1].lower()}.png')
        if not want & set(after):
            failed.append(name)
        elif name in VARIANTS and cpu_key <= 18:
            expected = VARIANTS[name].get(cpu_key, DEFAULTS[name])
            if expected not in after: failed.append(f'{name} (expected {expected:#x} against key {cpu_key})')
    assert not failed, f'Omen moves not played: {failed}'


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--build', default=str(ROOT / 'build-rel-lite'))
    ap.add_argument('--memcard-dir', default=str(ROOT / 'saves'), help='copied, never written')
    ap.add_argument('--capture', default=str(ROOT / 'workspace/ttt1-import/captures/unknown@jin-select-ram.bin'))
    ap.add_argument('--shots', help='folder for screenshots')
    ap.add_argument('--recoil', action='store_true', help='Arcade instead: the recoil of a Corpse Hitter')
    ap.add_argument('--cpu-tag', action='store_true', help='the CPU from the Tag page (guests)')
    ap.add_argument('--cpu-right', type=int, default=0, help='move the CPU cursor right that many cells')
    a = ap.parse_args()
    ram = Path(a.capture).read_bytes()
    table = pack_records(Path(a.build) / 'mods/ttt1/Jin-TTT1-combat.jmv', struct.unpack_from('<5515I', ram, 0x2a7350))
    g = Game(Path(a.build), Path(a.memcard_dir), {'TEKKEN3_NATIVE_MOVES': ''})
    try:
        if a.recoil: print(f'recoil: {recoil(g, Moves(g, table))}')
        else: run(g, table, a.shots, a.cpu_right, a.cpu_tag)
        print('OK')
    finally:
        g.stop()


if __name__ == '__main__':
    main()
