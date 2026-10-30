#!/usr/bin/env python3
"""B51: the KO cry of every TTT1 guest, and its two echoes (TIPS.md section 64).

    python tools/test_ko_echo.py [--build build-rel-lite] [--mods <dir>] [--memcard-dir saves]
                                 [--only arcade-p1,arcade-p2,team,vs,special] [--guests lee,devil]
                                 [--report ko-echo.md]

Boots private copies of a debug-server build (rel-lite: headless, software
renderer, x1, its own settings) on a copy of the memory card, and plays real
fights to a KO:

- arcade-p1: player 1 is the subject, the CPU (Hwoarang, kick strings) KOs it
  twice: the round's KO, then the match's.
- arcade-p2: the CPU is the subject (written at each stage's loading), player 1
  (Jin, Tekken 3 moves) KOs it with lows: the round's KO, then the match's.
- survival: player 1 (Jin) KOs the CPU subjects one after the other.
- team: Team Battle, player 1's five members picked by hand on the Tag page,
  the CPU's team all guests; four CPU members are KOed by player 1, then the
  CPU KOs player 1's five (the last one ends the match).
- vs: a challenger (Start on pad 2) makes two humans: Devil and Lee KO each
  other; the bench drives the attacker.
- special: Tetsujin (no voice bank: a silent cry is expected), Mokujin, Devil
  Jin (Jin with both punches: Jin's own cries, mode 2) and natives on their
  TTT1 moves (mode 4), each as player 1 and as the CPU.

The victim's life is set to 1, the attacker's kept full. Each KO is watched
through the voices' telemetry (src/tekken3_ttt1_voices.c: per player, the
cries' voice at +0..+16, the KO voice at +20..+28) and the log ("<Name> voice:
P<n> command=4xxx", "native KO cry"). Reported per KO: the KO command, how
much of the cry played, the echoes reached (at +16 / +32 frames), what stopped
it early (a later command of the same player, the end of the fight state), and
the commands that followed it. A natives' KO cry is Tekken 3's own (with its
own echo task): only its command is checked. Exit status 1 when a guest's KO
cry or one of its echoes did not play in full (before the fix: a damage cry
sent a few frames after the KO, 0x80041590, cut them on the player's single
voice), or when Tetsujin made a sound.
"""
import argparse, os, random, re, shutil, socket, struct, subprocess, sys, tempfile, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'psxrecomp/tools'))
import debug_client

EXE = 'Tekken_3_Expanded.exe' if os.name == 'nt' else 'Tekken_3_Expanded'
SCREEN, PHASE, MODE, LINE = 0x800ae204, 0x800ae224, 0x800afa88, 0x80097f40
ACTOR, STRIDE, LIFE = 0x800a9228, 0x188c, 0x3f6
P1_CHOICE, IDS = 0x80118668, 0x800add5c
TEAMS, BLOCK = (0x800afae1, 0x800afaee), 0x800b8d70
START, CROSS, SQUARE, R2, BOTH_PUNCHES = 0xfff7, 0xbfff, 0x7fff, 0xfdff, 0x6fff
UP, DOWN, LEFT, RIGHT = 0xffef, 0xffbf, 0xff7f, 0xffdf
B1, B2, B3, B4 = 0x7fff, 0xefff, 0xbfff, 0xdfff
LINES = {'arcade': 0, 'team': 3, 'survival': 5}
ECHO_DELAY, RATE = 44100 * 16 // 60, 44100
ROWS = []            # every KO measured, kept even when a scenario fails later
SLACK = 3000          # the telemetry is sampled every few frames: a position this close to the end is the end
GUEST_BASE, JIN, KING, MOKUJIN, HWOARANG = 23, 9, 3, 15, 6


class Game:
    def __init__(self, build, mods, memcards, env):
        self.dir = Path(tempfile.mkdtemp(prefix='ko-echo-'))
        run, saves = self.dir / 'run', self.dir / 'saves'
        (run / 'mods').mkdir(parents=True); (saves / 'openbios').mkdir(parents=True)
        shutil.copy2(build / EXE, run / EXE)
        for name in ('bios', 'assets'):
            if (build / name).is_dir(): shutil.copytree(build / name, run / name)
        # The mods: the build's, then --mods over them; folders are linked, not
        # copied (the TTT1 catalogue is hundreds of megabytes) and never written.
        for source in (build / 'mods', mods):
            if not source or not source.is_dir(): continue
            for entry in source.iterdir():
                target = run / 'mods' / entry.name
                if target.exists() or os.path.lexists(target): self.unlink(target)
                if entry.is_dir(): self.link(entry.resolve(), target)
                else: shutil.copy2(entry, target)
        for card in ('card1.mcd', 'card2.mcd'):
            if (memcards / card).is_file(): shutil.copy2(memcards / card, saves / card)
        (run / 'settings.toml').write_text('[video]\nsupersampling=1\nantialiasing=false\n'
                                           '[controller]\np1_device="keyboard"\n')
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

    @staticmethod
    def link(source, target):
        if os.name == 'nt': subprocess.run(['cmd', '/c', 'mklink', '/J', str(target), str(source)], check=True, capture_output=True)
        else: target.symlink_to(source)

    @staticmethod
    def unlink(target):
        # A junction or a link goes, never what it points to.
        if os.path.islink(target) or (hasattr(os.path, 'isjunction') and os.path.isjunction(target)):
            os.unlink(target) if os.path.islink(target) else os.rmdir(target)
        elif target.is_dir(): shutil.rmtree(target)
        else: target.unlink()

    def q(self, **r):
        for _ in range(400):
            try: result = debug_client.query('127.0.0.1', self.port, r)
            except (OSError, ValueError):
                if self.process.poll() is not None: raise RuntimeError(f'the game exited; see {self.logfile}')
                time.sleep(.1); continue
            if result.get('err') != 'emu busy or frozen': break
            time.sleep(.05)
        if result.get('ok') is False: raise RuntimeError((r, result))
        return result

    def read(self, a, n): return bytes.fromhex(self.q(cmd='read_ram', addr=f'{a:08x}', len=n)['hex'])
    def value(self, a, n=4): return int.from_bytes(self.read(a, n), 'little')

    def write(self, a, v, n=4):
        for j in range(n): self.q(cmd='write_ram', addr=f'{a + j:08x}', val=f'{v >> (j * 8) & 255:02x}')

    def press(self, buttons, port=1, settle=.3):
        self.q(cmd='press', buttons=buttons, frames=4, port=port); time.sleep(settle)
    def frame(self): return self.q(cmd='frame')['frame']
    def log(self): return self.logfile.read_text(errors='replace')

    def wait(self, predicate, description, seconds=30, step=None):
        end = time.monotonic() + seconds
        while time.monotonic() < end:
            if predicate(): return
            if step: step()
            time.sleep(.05)
        raise AssertionError(description)

    def stop(self, keep=None):
        if keep: keep.mkdir(parents=True, exist_ok=True); shutil.copy2(self.logfile, keep / f'{self.dir.name}.log')
        if self.process.poll() is None:
            self.process.terminate()
            try: self.process.wait(timeout=10)
            except subprocess.TimeoutExpired: self.process.kill()
        for entry in (self.dir / 'run/mods').iterdir():
            if os.path.islink(entry) or (hasattr(os.path, 'isjunction') and os.path.isjunction(entry)): self.unlink(entry)
        shutil.rmtree(self.dir, ignore_errors=True)


def to_menu_line(g, line):
    # The modes menu (state 4) gives up after two seconds: Start on "Namco
    # presents" (state 3), the line written, X at once (TIPS.md section 76).
    g.wait(lambda: g.value(SCREEN) >= 2, 'the game did not boot', 90)
    g.q(cmd='press', buttons=0xffff, frames=2, port=2)       # pad 2 plugged: VS and challengers
    def menu():
        if g.value(SCREEN) == 3:
            g.write(0x80097f26, 1, 1); g.write(0x80097f27, 1, 1)
            g.press(START)
        return g.value(SCREEN) == 4
    g.wait(menu, 'the modes menu never came', 180)
    time.sleep(.3)
    g.write(LINE, line); g.press(CROSS)


def load_fight(g, ids, description, seconds, nudge=None):
    """Through the loading screen (11) to the fight, writing the fighters
    (0x800ADD5C, one half per player; None: the game's choice) from the
    moment the selector is left until the screen has read them. No sleeping:
    headless, the game runs several times faster than real time and the
    screen lasts a few frames. nudge() is called about twice a second while
    the selector is still up."""
    end, last, seen = time.monotonic() + seconds, 0, False
    while time.monotonic() < end:
        screen = g.value(SCREEN)
        if screen == 11: seen = True
        if seen and (screen == 8 or (screen == 11 and g.value(PHASE, 2) >= 6)): return
        if screen not in (8, 9):
            for p, cid in enumerate(ids):
                if cid is not None: g.write(IDS + 2 * p, cid, 2)
        elif nudge and time.monotonic() - last > .5: nudge(); last = time.monotonic()
    raise AssertionError(f'{description} (screen {g.value(SCREEN)}, phase {g.value(PHASE, 2)})')


def telemetry(g):
    found = re.findall(r'telemetry=([0-9A-F]+)', g.log())
    return int(found[-1], 16) if found else 0


def round_ready(g): return g.value(SCREEN) == 8 and g.value(PHASE, 2) == 8


def drive(g, attacker, victim, step):
    """Player input for a human attacker: walk in, lows (a standing fighter
    guards highs and mids), now and then a string."""
    a = struct.unpack('<i', g.read(ACTOR + attacker * STRIDE, 4))[0]
    v = struct.unpack('<i', g.read(ACTOR + victim * STRIDE, 4))[0]
    forward = RIGHT if a < v else LEFT
    pattern = (forward, forward, DOWN & B4, 0xffff, DOWN & B3, 0xffff, forward, DOWN & B4, B1, B2, DOWN & B4, 0xffff)
    g.q(cmd='set_input', buttons=pattern[step % len(pattern)], port=attacker + 1)


def ko(g, victim, human_attacker, label):
    """KO the victim once and follow its KO cry. Returns the case's row."""
    attacker = 1 - victim
    # A new round: the previous KO's aftermath (still state 8, phase 8) left,
    # then the fight running and the victim's life above 1 (in Team Battle the
    # winner stays, with the life it kept).
    if getattr(g, 'kos', 0): g.wait(lambda: not round_ready(g), f'{label}: the previous round did not end', 150)
    g.kos = getattr(g, 'kos', 0) + 1
    g.wait(lambda: round_ready(g) and 1 < g.value(ACTOR + victim * STRIDE + LIFE, 2) < 1000,
           f'{label}: the round did not start', 150)
    time.sleep(.3)
    actor = struct.unpack('<5H', g.read(ACTOR + 0x14 + victim * STRIDE, 10))
    tel = telemetry(g)
    full = g.value(ACTOR + attacker * STRIDE + 0x3fa, 2) or 140
    mark = len(g.log())
    g.write(ACTOR + victim * STRIDE + LIFE, 1, 2)
    samples, ko_frame, step, end = [], None, 0, time.monotonic() + 90
    while time.monotonic() < end:
        frame = g.frame()
        life = g.value(ACTOR + victim * STRIDE + LIFE, 2)
        screen, phase = g.value(SCREEN), g.value(PHASE, 2)
        t = struct.unpack('<8I', g.read(tel + victim * 32, 32)) if tel else (0,) * 8
        samples.append((frame, screen, phase, t))
        if ko_frame is None:
            if life == 0 or life > 1000: ko_frame = frame
            else:
                g.write(ACTOR + attacker * STRIDE + LIFE, full, 2)
                if human_attacker: drive(g, attacker, victim, step); step += 1
        else:
            if human_attacker: g.q(cmd='set_input', buttons=0xffff, port=attacker + 1)
            if frame - ko_frame > 200 or screen != 8: break
    for port in (1, 2): g.q(cmd='set_input', buttons=0xffff, port=port)
    log = g.log()[mark:]
    row = analyse(label, victim, actor, ko_frame, samples, log)
    row['_log'] = log
    ROWS.append(row)
    return row


def analyse(label, victim, actor, ko_frame, samples, log):
    row = dict(case=label, victim=f'J{victim + 1}', fighter=actor[2], command='', cry='', echoes='', cut='', after='')
    if ko_frame is None:
        row.update(cry='pas de K.O.', verdict='?'); return row
    mine = [l for l in log.splitlines() if f'voice: P{victim + 1} command=' in l]
    commands = [re.search(r'command=([0-9A-F]{4})', l).group(1) for l in mine]
    native = re.findall(rf'P{victim + 1} native KO cry ([0-9A-F]{{4}})', log)
    kos = [i for i, c in enumerate(commands) if c.startswith('4')]
    if native and not kos:
        row.update(command=native[0], cry='Tekken 3', echoes='Tekken 3', verdict='ok'); return row
    if not kos and actor[2] < GUEST_BASE:
        row.update(command='non journalisée', cry='Tekken 3', echoes='Tekken 3', verdict='?'); return row
    if not kos:
        row.update(command='aucune', cry='muet', echoes='-', after=' '.join(commands[:4]),
                   verdict='muet'); return row
    first = kos[0]
    row['command'] = commands[first]
    following = commands[first + 1:kos[1]] if len(kos) > 1 else commands[first + 1:]
    row['after'] = ' '.join(following[:4])
    # The KO voice: +20 commands, +24 position, +28 end (fixed build); else
    # the single voice, +0 commands, +8 category, +12 position, +16 frames.
    split = any(t[5] for _, _, _, t in samples)
    track, start, best, total, stop = None, None, 0, 0, ''
    for frame, screen, phase, t in samples:
        if split:
            calls, pos, end, active = t[5], t[6], t[7], t[7] != 0
            frames = end - 2 * ECHO_DELAY if end else 0
        else:
            calls, pos, active, frames = t[0], t[3], t[2] == 4, t[4]
            end = frames + 2 * ECHO_DELAY
        if track is None:
            if active: track, start, total = calls, frame, end; best = pos
            continue
        if calls != track:
            if best < total: stop = 'commande ' + (following[0] if following else '?')
            break
        if not active:
            # The mixer silences a voice at its end, or when the fight state
            # is left (src/tekken3_ttt1_voices.c): still fighting, it ended.
            if screen == 8 and phase == 8: best = total
            elif best < total - SLACK: stop = f'état {screen}/{phase}'
            break
        best = max(best, pos)
    if track is None:
        row.update(cry='coupé aussitôt', echoes='0', cut='commande ' + (following[0] if following else '?'), verdict='KO')
        return row
    if not stop and best < total - SLACK and samples and samples[-1][0] - start < 150: stop = 'fin de mesure'
    frames = total - 2 * ECHO_DELAY
    heard = min(best, frames)
    echoes = sum(1 for e in (1, 2) if best >= e * ECHO_DELAY + min(frames, 2205))
    row['cry'] = 'entier' if best >= frames - SLACK else f'{heard * 1000 // RATE} / {frames * 1000 // RATE} ms'
    row['echoes'] = str(echoes)
    row['cut'] = stop
    row['verdict'] = 'ok' if best >= frames - SLACK and echoes == 2 else 'KO'
    return row


# ---------------------------------------------------------------- scenarios

def arcade_start(g, picks=SQUARE, cursor=None):
    """The Arcade grid, the cursor on `cursor` (an ID) if given, `picks` pressed."""
    to_menu_line(g, LINES['arcade'])
    g.wait(lambda: g.value(SCREEN) == 9 and g.value(MODE) == 0, 'the Arcade selector never came', 30)
    time.sleep(1.5)
    if cursor is not None:
        for _ in range(30):
            if g.value(P1_CHOICE) == cursor: break
            g.press(RIGHT)
        else: raise AssertionError(f'the cursor never reached {cursor}')
    g.press(picks)


def arcade_p1(g, subject, name):
    """Player 1 is the subject, the CPU KOs it twice."""
    arcade_start(g, *((BOTH_PUNCHES, JIN) if subject == 'deviljin' else (SQUARE, None)))
    cid = JIN if subject == 'deviljin' else subject
    if subject == 'deviljin':
        g.wait(lambda: 'Devil Jin: P1 requested' in g.log(), 'both punches did not request Devil Jin', 5)
    load_fight(g, (cid, HWOARANG), 'no loading screen', 30, lambda: g.press(SQUARE, settle=0) if g.value(SCREEN) == 9 else None)
    return [ko(g, 0, False, f'Arcade J1 {name} manche'), ko(g, 0, False, f'Arcade J1 {name} match')]


def arcade_p2(g, subjects):
    """Player 1 (Jin) KOs the CPU, one subject per stage."""
    arcade_start(g, SQUARE, JIN)
    rows = []
    for subject, name in subjects:
        load_fight(g, (JIN, subject), f'no loading screen for {name}', 120,
                   lambda: g.press(START, settle=0) if g.value(SCREEN) == 9 else None)
        rows.append(ko(g, 1, True, f'Arcade J2 {name} manche'))
        rows.append(ko(g, 1, True, f'Arcade J2 {name} match'))
    return rows


def survival(g, subjects):
    """Survival: player 1 (Jin) KOs one CPU subject after another, each KO
    ending its match; the next opponent comes in on its own loading screen."""
    to_menu_line(g, LINES['survival'])
    g.wait(lambda: g.value(SCREEN) == 9 and g.value(MODE) == 4, 'the Survival selector never came', 30)
    time.sleep(1.5)
    for _ in range(30):
        if g.value(P1_CHOICE) == JIN: break
        g.press(RIGHT)
    g.press(SQUARE)
    rows = []
    for subject, name in subjects:
        load_fight(g, (JIN, subject), f'no loading screen for {name}', 120,
                   lambda: g.press(SQUARE, settle=0) if g.value(SCREEN) == 9 else None)
        rows.append(ko(g, 1, True, f'Survival J2 {name}'))
    return rows


def team(g, picks, names):
    """Team Battle: player 1's five by hand (Tag page), the CPU's team guests."""
    to_menu_line(g, LINES['team'])
    g.wait(lambda: g.value(SCREEN) == 10 and g.value(MODE) == 2, 'the Team Battle grid never came', 30)
    time.sleep(2)
    for _ in range(8): g.press(LEFT)         # one member, then five
    for _ in range(4): g.press(RIGHT)
    g.press(CROSS); time.sleep(.8)
    g.press(R2); time.sleep(1.5)
    for k in picks:
        row, col = divmod(k, 6)              # the Tag page in rows of six (roster.c, VS_TAG_COLS)
        for _ in range(20):
            y, x = divmod(g.value(BLOCK + 12) * 7 + g.value(BLOCK + 8), 6)
            if (x, y) == (col, row): break
            g.press(DOWN if y < row else UP if y > row else RIGHT if x < col else LEFT)
        else: raise AssertionError('the Team grid cursor never reached the guest')
        g.press(SQUARE); time.sleep(.8)
    for _ in range(8):
        if g.value(SCREEN) != 10: break
        g.press(START); time.sleep(.8)
    g.wait(lambda: g.value(SCREEN) == 8, 'the first fight never came', 150)
    sizes = [g.value(TEAMS[p] + 12, 1) for p in range(2)]
    rows = []
    for i in range(min(4, sizes[1] - 1)): rows.append(ko(g, 1, True, f'Team J2 membre {i + 1}'))
    for i in range(sizes[0]):
        name = names[i] if i < len(names) else 'membre au hasard'
        rows.append(ko(g, 0, False, f'Team J1 {name} ' + ('match' if i == sizes[0] - 1 else f'membre {i + 1}')))
    return rows


def versus(g, a, b, names):
    """A challenger: player 2 joins with Start, both pick, both humans."""
    to_menu_line(g, LINES['arcade'])
    g.wait(lambda: g.value(SCREEN) == 9 and g.value(MODE) == 0, 'the Arcade selector never came', 30)
    time.sleep(1.5)
    g.press(START, port=2); time.sleep(2)
    if g.value(P1_CHOICE + 0x7c) == 22:
        raise AssertionError('player 2 did not join: no pad 2 injection in this build (PSX_DEBUG_TOOLS=OFF)')
    load_fight(g, (a, b), 'no loading screen', 30,
               lambda: (g.press(SQUARE, settle=0), g.press(SQUARE, port=2, settle=0)) if g.value(SCREEN) == 9 else None)
    return [ko(g, 0, True, f'VS J1 {names[0]} manche'), ko(g, 1, True, f'VS J2 {names[1]} manche'),
            ko(g, 0, True, f'VS J1 {names[0]} match')]


# ---------------------------------------------------------------- main

def catalogue(mods):
    path = mods / 'ttt1/guests.txt'
    return [line.split()[0] for line in path.read_text().splitlines() if line.split()]


def main():
    if hasattr(sys.stdout, 'reconfigure'): sys.stdout.reconfigure(encoding='utf-8')
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--build', default=str(ROOT / 'build-rel-lite'))
    ap.add_argument('--mods', help='a mods folder over the build\'s (a release build\'s, with the TTT1 catalogue)')
    ap.add_argument('--memcard-dir', default=str(ROOT / 'saves'), help='copied, never written')
    ap.add_argument('--only', default='arcade-p1,arcade-p2,survival,team,vs,special')
    ap.add_argument('--guests', help='catalogue keys (default: all)')
    ap.add_argument('--logs', help='keep the log of each game in that folder (with the sound log)')
    ap.add_argument('--report', help='write the table (Markdown) there too')
    a = ap.parse_args()
    build, memcards = Path(a.build).resolve(), Path(a.memcard_dir)
    mods = Path(a.mods).resolve() if a.mods else None
    keys = catalogue(mods if mods and (mods / 'ttt1').is_dir() else build / 'mods')
    wanted = a.guests.split(',') if a.guests else [k for k in keys if k != 'tetsujin']
    subjects = [(GUEST_BASE + keys.index(k), k) for k in wanted]
    only = set(a.only.split(','))
    rows = ROWS

    def run(env, body):
        extra = {'TEKKEN3_TTT1_SOUND_LOG': '1'} if a.logs else {}
        g = Game(build, mods, memcards, {'TEKKEN3_NATIVE_MOVES': '0', 'TEKKEN3_CPU_MOVESET': 'original', **extra, **env})
        try: body(g)
        except (AssertionError, RuntimeError) as error:
            rows.append(dict(case=f'erreur : {error}', victim='', fighter='',
                             command='', cry='erreur', echoes='', cut='', after='', verdict='?'))
        finally: g.stop(Path(a.logs) if a.logs else None)
        print(table(rows[-6:]), flush=True)

    if 'arcade-p1' in only:
        for cid, name in subjects: run({}, lambda g, c=cid, n=name: arcade_p1(g, c, n))
    if 'arcade-p2' in only:
        for i in range(0, len(subjects), 8): run({}, lambda g, s=subjects[i:i + 8]: arcade_p2(g, s))
    if 'survival' in only:
        for i in range(0, len(subjects), 9): run({}, lambda g, s=subjects[i:i + 9]: survival(g, s))
    if 'team' in only:
        guests = [(cid, n) for cid, n in subjects]
        for i in range(0, len(guests), 5):
            chunk = guests[i:i + 5]
            while len(chunk) < 5: chunk.append(guests[len(chunk) % len(guests)])
            run({'TEKKEN3_TEAM_GUESTS': '1,8'}, lambda g, c=chunk: team(g, [cid - GUEST_BASE for cid, _ in c], [n for _, n in c]))
    if 'vs' in only and 'devil' in keys and 'lee' in keys:
        run({}, lambda g: versus(g, GUEST_BASE + keys.index('devil'), GUEST_BASE + keys.index('lee'), ('devil', 'lee')))
    if 'special' in only:
        tetsujin = GUEST_BASE + keys.index('tetsujin') if 'tetsujin' in keys else None
        if tetsujin:
            run({}, lambda g: arcade_p1(g, tetsujin, 'tetsujin'))
            run({}, lambda g: arcade_p2(g, [(tetsujin, 'tetsujin')]))
        run({}, lambda g: arcade_p1(g, MOKUJIN, 'mokujin'))
        run({}, lambda g: arcade_p2(g, [(MOKUJIN, 'mokujin')]))
        run({}, lambda g: arcade_p1(g, 'deviljin', 'devil jin'))
        run({'TEKKEN3_NATIVE_MOVES': '1'}, lambda g: arcade_p1(g, JIN, 'jin (TTT1)'))
        run({'TEKKEN3_NATIVE_MOVES': '1'}, lambda g: arcade_p2(g, [(KING, 'king (TTT1)')]))

    text = table(rows)
    print('\n' + text)
    if a.report: Path(a.report).write_text(text + '\n', encoding='utf-8')
    # Tetsujin has no voice bank: his KO is silent (TIPS.md section 39).
    tetsujin = GUEST_BASE + keys.index('tetsujin') if 'tetsujin' in keys else -1
    bad = [r for r in rows if r['verdict'] == 'KO' or (r['verdict'] == 'muet') != (r['fighter'] == tetsujin)
           and r['verdict'] != '?']
    unknown = [r for r in rows if r['verdict'] == '?']
    if a.logs:
        for i, r in enumerate(bad):
            Path(a.logs, f'ko-{i}.log').write_text(r['case'] + '\n' + r.get('_log', ''), encoding='utf-8')
    print(f'\n{len(rows)} cas, {len(bad)} en échec, {len(unknown)} non mesurés')
    sys.exit(1 if bad else 0)


def table(rows):
    head = '| cas | victime | perso | commande K.O. | cri | échos | coupé par | commandes suivantes | verdict |'
    lines = [head, '|' + '---|' * 9]
    for r in rows:
        lines.append('| ' + ' | '.join(str(r.get(k, '')) for k in
                     ('case', 'victim', 'fighter', 'command', 'cry', 'echoes', 'cut', 'after', 'verdict')) + ' |')
    return '\n'.join(lines)


if __name__ == '__main__':
    main()
