#!/usr/bin/env python3
"""B46 (GH#70): a guest's loading-screen (VS) portrait gets its own gradient.

    python3 tools/test_vs_background.py [--build build-rel-lite] [--guests kazuya,jun]

The loading screen (state 11) draws each portrait over a gradient whose
colour comes from 0x8004B790 (ID, costume): the upper 24 bits of the word it
points at, kept in the portrait object (type 4, +0x14; objects of 0x28 bytes
at 0x800B8D60). Every guest used to get one pale tint, 0xB3D8EA (a white
background). This boots a headless copy of the build on a copy of the
memory cards, opens Arcade, picks the guest on the Tag page as a player
does (player 1, against the CPU) and at the loading screen reads player 1's
portrait object: its colour must be the guest's in guest_tints
(src/tekken3_ttt1_roster.c), never the old tint, and two guests must differ.
One fresh game per guest: the lean debug server has no pad 2, and writing
the CPU's choice races the screen's setup.
Needs a build with the debug server (PSX_DEBUG_SERVER_LITE=ON is enough).
"""
import argparse, os, re, shutil, socket, struct, subprocess, sys, tempfile, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'psxrecomp/tools'))
import debug_client

GUEST_ID, OLD_TINT = 23, 0xb3d8ea
STATE, MODE = 0x800ae204, 0x800afa88
OBJECTS, OBJECT_SIZE, PORTRAIT = 0x800b8d60, 0x28, 4
CHOICE = 0x800add5c                    # loading choice, + 2 * player
PAD_START, PAD_CROSS, PAD_SQUARE, PAD_R2 = 0xfff7, 0xbfff, 0x7fff, 0xfdff
PAD_UP, PAD_RIGHT, PAD_DOWN, PAD_LEFT = 0xffef, 0xffdf, 0xffbf, 0xff7f


def tints():
    """{key: 0xRRGGBB} from the runtime's table, the single source of truth."""
    src = (ROOT / 'src/tekken3_ttt1_roster.c').read_text()
    if 'guest_tints[]' not in src: return {}
    table = src[src.index('guest_tints[]'):]
    table = table[:table.index('};')]
    return {k: int(v, 16) for k, v in re.findall(r'\{"(\w+)",0x([0-9a-f]+)\}', table)}


def bgr(rgb): return (rgb & 0xff) << 16 | rgb & 0xff00 | rgb >> 16


class Game:
    def __init__(self, build):
        self.tmp = Path(tempfile.mkdtemp(prefix='vs-background-'))
        exe = next(p for p in (build / 'Tekken_3_Expanded.exe', build / 'Tekken_3_Expanded') if p.exists())
        run = self.tmp / 'run'; run.mkdir()
        shutil.copy2(exe, run / exe.name)
        for name in ('bios', 'mods'): shutil.copytree(build / name, run / name)
        (run / 'settings.toml').write_text('[video]\nsupersampling=1\n[controller]\np1_device="keyboard"\np2_device="none"\n')
        saves = self.tmp / 'saves'                     # a copy: never the player's cards
        if (ROOT / 'saves').is_dir(): shutil.copytree(ROOT / 'saves', saves)
        else: saves.mkdir()
        self.catalogue = [l.split()[0] for l in (run / 'mods/ttt1/guests.txt').read_text().splitlines() if l.split()]
        with socket.socket() as s:
            s.bind(('127.0.0.1', 0)); self.port = s.getsockname()[1]
        env = {k: v for k, v in os.environ.items() if not k.startswith('TEKKEN3_')}
        env['SDL_AUDIO_DRIVER'] = 'dummy'
        self.log = (self.tmp / 'game.log').open('w')
        self.process = subprocess.Popen(
            [str(run / exe.name), '--game', str(ROOT / 'game.toml'), '--disc', str(ROOT / 'disc/Tekken 3 (USA).cue'),
             '--no-launcher', '--renderer', 'software', '--headless', '--debug-port', str(self.port),
             '--memcard-dir', str(saves)], cwd=ROOT, stdout=self.log, stderr=subprocess.STDOUT, env=env)
        end = time.monotonic() + 60
        while True:
            if self.process.poll() is not None: raise RuntimeError('the game exited during startup')
            try:
                socket.create_connection(('127.0.0.1', self.port), .2).close(); break
            except OSError:
                if time.monotonic() > end: raise RuntimeError('debug endpoint did not start')
                time.sleep(.1)

    def q(self, **r):
        result = debug_client.query('127.0.0.1', self.port, r)
        if result.get('ok') is False: raise RuntimeError((r, result))
        return result

    def read(self, a, n): return bytes.fromhex(self.q(cmd='read_ram', addr=f'{a:08x}', len=n)['hex'])
    def value(self, a, n=4): return int.from_bytes(self.read(a, n), 'little')
    def write(self, a, v, n):
        for j in range(n): self.q(cmd='write_ram', addr=f'{a + j:08x}', val=f'{v >> 8 * j & 255:02x}')

    def press(self, buttons):
        """Frame-exact: the headless game runs faster than real time."""
        self.q(cmd='input_route_clear')
        self.q(cmd='input_route_append', frames=4, buttons=buttons)
        self.q(cmd='input_route_append', frames=6, buttons=0xffff)
        self.q(cmd='input_route_start'); time.sleep(.25)

    def cells(self): return self.value(0x80118648)      # cells on the shown page

    def wait(self, predicate, what, seconds):
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            if predicate(): return
            time.sleep(.05)
        raise AssertionError(what)

    def stop(self):
        if self.process.poll() is None:
            self.process.kill(); self.process.wait()
        self.log.close()
        shutil.rmtree(self.tmp, ignore_errors=True)


def arcade_selector(g):
    end = time.monotonic() + 180
    while g.value(STATE) != 9:
        if time.monotonic() > end: raise AssertionError('the Arcade selector never opened')
        g.press(PAD_CROSS if g.value(STATE) == 4 else PAD_START)      # ARCADE is the first entry
    g.write(0x8011863c, 0x10ff, 2)                                    # selector timer up
    time.sleep(1)


def pick_tag_cell(g, cell, cid):
    """Walk player 1's cursor to a Tag cell and confirm it."""
    def place(c): return divmod(c, 8) if c < 16 else (c - 16, 8)
    for _ in range(40):
        if g.value(0x80118668) == cid: break
        here = g.value(0x80118650)
        if here >= g.cells(): continue                 # off the page while it switches
        (r, c), (R, C) = place(here), place(cell)
        g.press(PAD_DOWN if r < R else PAD_UP if r > R else PAD_RIGHT if c < C else PAD_LEFT)
    else: raise AssertionError('the cursor did not reach the guest cell')
    g.press(PAD_SQUARE)


def portrait_colours(g):
    """{player: 0xBBGGRR} of the loading screen's portrait objects (player 1 left)."""
    objects = g.read(OBJECTS, 64 * OBJECT_SIZE)
    found = {}
    for i in range(64):
        o = objects[i * OBJECT_SIZE:(i + 1) * OBJECT_SIZE]
        if struct.unpack_from('<H', o)[0] == PORTRAIT:
            x = struct.unpack_from('<h', o, 4)[0]
            found[0 if x < 100 else 1] = struct.unpack_from('<I', o, 0x14)[0] & 0xffffff
    return found


def guest_colour(build, guest):
    """Player 1's loading-screen gradient (0xBBGGRR) with guest picked on the
    Tag page against the Arcade CPU, in a fresh game."""
    g = Game(build)
    try:
        cid = GUEST_ID + g.catalogue.index(guest)
        arcade_selector(g)
        g.press(PAD_R2)                                            # the Tag page
        g.wait(lambda: g.cells() == len(g.catalogue), 'R2 did not open the Tag page', 10)
        time.sleep(1.5)
        pick_tag_cell(g, g.catalogue.index(guest), cid)
        g.wait(lambda: len(portrait_colours(g)) == 2, 'no portrait objects on the loading screen', 30)
        time.sleep(.5)
        seen, ids = portrait_colours(g), [g.value(CHOICE, 2), g.value(CHOICE + 2, 2)]
        print(f'{guest}: loading screen IDs {ids}, colours', {p + 1: f'{c:06x}' for p, c in seen.items()})
        assert ids[0] == cid, ids
        return seen[0]
    finally:
        g.stop()


def main():
    a = argparse.ArgumentParser()
    a.add_argument('--build', default='build-rel-lite')
    a.add_argument('--guests', default='kazuya,jun', help='one fresh game per guest, player 1')
    args = a.parse_args()
    table, seen = tints(), {}
    for guest in args.guests.split(','):
        colour = guest_colour(ROOT / args.build, guest)
        assert colour != bgr(OLD_TINT), f'{guest}: the old pale tint (white background) is back'
        if guest in table: assert colour == bgr(table[guest]), (guest, f'{colour:06x}', f'{bgr(table[guest]):06x}')
        seen[guest] = colour
    assert len(set(seen.values())) == len(seen), f'guests share one gradient: {seen}'
    print('OK: each guest has its own loading-screen gradient')


if __name__ == '__main__':
    main()
