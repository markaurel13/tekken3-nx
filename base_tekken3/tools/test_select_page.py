#!/usr/bin/env python3
"""Switching to the Tag page at the selectors (B44: GH#59, GH#97).

    python3 tools/test_select_page.py [--build build-rel-lite] [--saves DIR]

Cold boot of a private copy of the build (debug server, full or lite,
headless, software renderer, 4:3, a copy of the memory card), real pad
input from the modes menu. Three checks, read from the GPU packets:

  - Arcade selector (state 9), R2: the cells' row lies under a drawing area
    that stops at y 305; the screen keeps what was last drawn there
    unclipped, which the overlay does for two frames after a cursor move
    (0x80118634). The page switch must do the same, or in 4:3 the stock
    cells stayed until the next move (GH#97; 16:9 redraws them every frame).
    Its cells must be centred: nine columns from x 28 to 340 (they started
    at 46, 10 px from the right edge).
  - Tekken Ball grid (state 10), R2: player 2 has not joined (block state
    8); his small portrait must stay the stock page's "?" (it became the
    first guest, Kazuya: GH#59).
  - Same grid: the two narrow gradients either side of the cells (POLY_G4,
    32 wide, y 67) must hug the six columns of the Tag page (x 43 and 291),
    and come back to the stock page's (25 and 309) on L2 (GH#59).

Exit status 0 = all checks passed. The menu has QUIT GAME on line 1:
the script never presses X there.
"""
import argparse, os, shutil, socket, subprocess, sys, tempfile, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'psxrecomp/tools'))
import debug_client

START, CROSS, DOWN, UP, R2, L2, IDLE = 0xfff7, 0xbfff, 0xffbf, 0xffef, 0xfdff, 0xfeff, 0xffff
STATE, MENU_LINE, MODE = 0x800ae204, 0x80097f40, 0x800afa88
ARCADE_LINE, BALL_LINE = 0, 6
P2_BLOCK = 0x800b8d70 + 172
EXE = 'Tekken_3_Expanded' + ('.exe' if os.name == 'nt' else '')


class Game:
    def __init__(self, build, saves):
        self.tmp = Path(tempfile.mkdtemp(prefix='select-page-'))
        run = self.tmp / 'run'
        run.mkdir()
        shutil.copy2(build / EXE, run / EXE)
        shutil.copytree(build / 'bios', run / 'bios')
        (run / 'mods').mkdir()
        for entry in (build / 'mods').iterdir():
            if entry.name != 'state.toml': link(entry, run / 'mods' / entry.name)
        # 4:3: the defect only shows on the native picture.
        state = (build / 'mods/state.toml').read_text()
        (run / 'mods/state.toml').write_text(state.replace(
            'id = "true-widescreen"\nenabled = true', 'id = "true-widescreen"\nenabled = false'))
        (run / 'settings.toml').write_text('[video]\nsupersampling=1\nantialiasing=false\n'
                                           '[controller]\np1_device="keyboard"\np2_device="keyboard"\n')
        shutil.copytree(saves, self.tmp / 'saves')
        with socket.socket() as s:
            s.bind(('127.0.0.1', 0)); self.port = s.getsockname()[1]
        env = {k: v for k, v in os.environ.items() if not k.startswith('TEKKEN3_')}
        env['SDL_AUDIO_DRIVER'] = 'dummy'
        self.log = self.tmp / 'game.log'
        self.proc = subprocess.Popen(
            [str(run / EXE), '--game', str(ROOT / 'game.toml'), '--disc', str(ROOT / 'disc/Tekken 3 (USA).cue'),
             '--no-launcher', '--renderer', 'software', '--debug-port', str(self.port),
             '--memcard-dir', str(self.tmp / 'saves'), '--headless'],
            cwd=run, stdout=self.log.open('w'), stderr=subprocess.STDOUT, env=env)
        for _ in range(300):
            try: socket.create_connection(('127.0.0.1', self.port), .2).close(); break
            except OSError: time.sleep(.1)

    def q(self, **r): return debug_client.query('127.0.0.1', self.port, r)
    def value(self, a, n=4): return int.from_bytes(bytes.fromhex(self.q(cmd='read_ram', addr=f'{a:08x}', len=n)['hex']), 'little')
    def write(self, a, v, n=4): self.q(cmd='write_ram', addr=f'{a:08x}', bytes=v.to_bytes(n, 'little').hex())
    def frame(self): return self.q(cmd='frame')['frame']
    def press(self, b): self.q(cmd='press', buttons=b, frames=4, port=1); time.sleep(.4)

    def wait(self, predicate, what, seconds=60):
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            if predicate(): return
            time.sleep(.1)
        raise AssertionError(what)

    def to_mode(self, line):
        """Start until the modes menu answers Down, then the line, then X."""
        for _ in range(120):
            if self.value(STATE) == 4 and self.value(MENU_LINE) == 0:
                self.press(DOWN)
                if self.value(MENU_LINE) == 1:
                    while self.value(MENU_LINE) < line: self.press(DOWN)
                    while self.value(MENU_LINE) > line: self.press(UP)
                    self.press(CROSS)
                    return
            elif self.value(STATE) != 4:
                self.press(START)
            time.sleep(.5)
        raise AssertionError('the modes menu never came')

    def packets(self, frame):
        """(drawing area bottom, opcode, words) for each primitive of a frame."""
        out, bottom = [], None
        for e in self.q(cmd='gpu_frame_dump', frame=frame, count=8192).get('entries', []):
            w = [int(x, 16) for x in e['w']]
            if e['op'] == '0xE4': bottom = w[0] >> 10 & 0x3ff
            out.append((bottom, int(e['op'], 16), w))
        return out

    def close(self):
        if self.proc.poll() is None: self.proc.kill()
        self.proc.wait()
        # Links first, without following them: they point into the build.
        for entry in (self.tmp / 'run/mods').iterdir():
            if entry.is_symlink() or getattr(os.path, 'isjunction', lambda p: False)(entry): os.rmdir(entry) if entry.is_dir() else entry.unlink()
        shutil.rmtree(self.tmp, ignore_errors=True)


def link(source, target):
    if os.name == 'nt':
        subprocess.run(['cmd', '/c', 'mklink', '/J', str(target), str(source)], capture_output=True, check=True)
    else:
        os.symlink(source, target)


def xy(word): return word & 0xffff, word >> 16


def cells_drawn(packets):
    """Cells of the Arcade selector's row (32 x 58 sprites at y 336) drawn
    under a drawing area that reaches them."""
    return sum(1 for bottom, op, w in packets
               if op == 0x65 and xy(w[1])[1] == 336 and w[3] & 0xffff == 32 and bottom and bottom >= 394)


def gradients(packets):
    """Left x of the two narrow gradients beside the state-10 grid."""
    return sorted(xy(w[1])[0] for _, op, w in packets
                  if op == 0x3a and xy(w[1])[1] == 67 and xy(w[3])[0] - xy(w[1])[0] == 32)


def p2_portrait(packets):
    """Player 2's small portrait under the grid: (texture word, CLUT/uv word)."""
    return [(w[2], w[3]) for _, op, w in packets
            if op == 0x65 and w[3] & 0xffff == 32 and 300 <= xy(w[1])[1] <= 400 and xy(w[1])[0] >= 240]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', default='build-rel-lite')
    ap.add_argument('--saves', default=str(ROOT / 'saves'), help='memory card folder (copied, never written)')
    args = ap.parse_args()
    build = (ROOT / args.build).resolve()
    failures = []
    def check(ok, what):
        print(('ok   ' if ok else 'FAIL ') + what, flush=True)
        if not ok: failures.append(what)

    g = Game(build, Path(args.saves))
    try:
        g.to_mode(ARCADE_LINE)
        g.wait(lambda: g.value(STATE) == 9, 'the Arcade selector', 20); time.sleep(2)
        g.write(0x8011863c, 5000)                       # the countdown
        f0 = g.frame(); time.sleep(.3)
        steady = sum(cells_drawn(g.packets(f)) > 0 for f in range(f0, g.frame()))
        check(steady == 0, f'Arcade: the row is not redrawn while the cursor rests ({steady} frames)')
        f0 = g.frame(); g.press(R2); time.sleep(.3)
        drawn = sum(cells_drawn(g.packets(f)) > 0 for f in range(f0, g.frame()))
        check(drawn >= 1, f'Arcade: R2 redraws the Tag page\'s row unclipped ({drawn} frames)')
        xs = sorted({xy(w[1])[0] for _, op, w in g.packets(g.frame() - 1)
                     if op == 0x65 and w[3] & 0xffff == 32 and xy(w[1])[1] in (336, 398)})
        check(bool(xs) and xs[0] == 368 - (xs[-1] + 32),
              f'Arcade: the Tag page is centred (cells x {xs[0] if xs else None}..{xs[-1] + 32 if xs else None} of 368)')
    finally:
        g.close()

    g = Game(build, Path(args.saves))
    try:
        g.write(0x80097f26, 1, 1); g.write(0x80097f27, 1, 1)    # Tekken Ball on the menu
        g.to_mode(BALL_LINE)
        g.wait(lambda: g.value(STATE) == 10 and g.value(MODE) == 7, 'TEKKEN BALL SELECT', 20); time.sleep(1.5)
        p2_state = g.value(P2_BLOCK)
        stock = g.packets(g.frame() - 1)
        question, stock_gradients = p2_portrait(stock), gradients(stock)
        g.press(R2); time.sleep(.5)
        tag = g.packets(g.frame() - 1)
        check(p2_state != 3 and question and p2_portrait(tag) == question,
              f'Ball: player 2 (block state {p2_state}, not joined) keeps "?" on the Tag page')
        check(stock_gradients == [25, 309], f'Ball: stock page gradients at x {stock_gradients}')
        check(gradients(tag) == [43, 291], f'Ball: Tag page gradients at x {gradients(tag)} (43, 291)')
        g.press(L2); time.sleep(.5)
        back = gradients(g.packets(g.frame() - 1))
        check(back == [25, 309], f'Ball: back on the stock page, gradients at x {back}')
    finally:
        g.close()
    print('OK' if not failures else f'{len(failures)} check(s) failed')
    return 1 if failures else 0


if __name__ == '__main__':
    sys.exit(main())
