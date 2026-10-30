#!/usr/bin/env python3
"""The CPU's Team Battle team: each member fights as announced (B38, GH#89, GH#67).

    python3 tools/test_team_cpu_moves.py --build build-rel-lite [--seed 1] [--fights 8]

Cold start, Team Battle 8 against 8: Start draws both teams, then the CPU's
team is written on the loading screen, a guest first (Michelle, or the
first of GUESTS that player 1's team leaves free) and natives with TTT1
moves after it. The mod tosses each native's moves for the match
(src/tekken3_ttt1_roster.c, team_tick: "P2's X fights on TTT1 moves
(toss)"). Player 1 wins every fight (the CPU's life to 0 once the round
runs) until --fights CPU members have fought.

Checks, in the log:
  - each CPU native fights on the moves its team tossed (its "P2 ... on the
    TTT1 moveset" / "P2 back to the native moveset" state at the round),
    never a draw of its own ("CPU P2 fighter N draws");
  - once the guest has left, player 2 reloads nothing of it: no
    "<guest> model:" line, no "+ <guest>)" in the reaction tables merged.
Build: a PSX_DEBUG_SERVER_LITE build (build-rel-lite).
"""
import argparse, os, re, shutil, socket, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import test_devil_jin as T

WORK = ROOT / 'workspace/team-cpu-moves'
STATE, PHASE, MODE, RUNNING = 0x800ae204, 0x800ae224, 0x800afa88, 0x80096f2c
TEAMS = (0x800afae1, 0x800afaee)                  # 0x800AFA88 + 89 + 13 * player: ID * 4 + costume, size at +12
FIRST, FIRST_COSTUME = 0x800add5c, 0x800add98     # the loading screen's fighters (+2 * player)
ACTOR_ID, STRIDE, LIFE = 0x800a9240, 0x188c, 0x800aaeaa   # P2's life: actor 0x800AAAB4 + 0x3F6
START, CROSS, RIGHT = 0xfff7, 0xbfff, 0xffdf
GUEST_ID = 23
ARENA, CHOOSER, RECORDS = 0x800adc84, 0x800b053e, 0x80097d40   # TIPS.md section 25; __wrap_func_8004F3B0
GUESTS = ['michelle', 'lee', 'baek', 'bruce', 'kunimitsu', 'wang']
# Natives with TTT1 moves (natives.txt), after the guest.
NATIVES = [1, 18, 13, 9, 10, 12, 14]              # Law, Anna, Heihachi, Jin, Julia, Bryan, Ogre
STOCK = ['Paul', 'Law', 'Lei', 'King', 'Yoshimitsu', 'Nina', 'Hwoarang', 'Xiaoyu', 'Eddy', 'Jin', 'Julia',
         'Kuma', 'Bryan', 'Heihachi', 'Ogre', 'Mokujin', 'Gun Jack', 'Gon', 'Anna', 'Dr. Bosconovitch',
         'True Ogre', 'Crow']
MOVES = re.compile(r'TTT1 characters: P2 (?:\S.*? on the TTT1 moveset|back to the native moveset)')


class Game(T.Game):
    """Run from the build folder itself (Windows refuses symbolic links
    without developer mode), cards in a fresh folder, headless software."""
    def __init__(self, build, tag, env=None):
        cards = WORK / f'cards-{os.getpid()}-{tag}'
        shutil.rmtree(cards, ignore_errors=True); cards.mkdir(parents=True)
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0)); self.port = listener.getsockname()[1]
        self.log_path = WORK / f'{tag}.log'
        full = {k: v for k, v in os.environ.items() if not k.startswith('TEKKEN3_')}
        full.update(env or {}); full['SDL_AUDIO_DRIVER'] = 'dummy'
        exe = next(p for p in (build / 'Tekken_3_Expanded.exe', build / 'Tekken_3_Expanded') if p.is_file())
        args = [str(exe), '--game', str(ROOT / 'game.toml'), '--disc', str(ROOT / 'disc/Tekken 3 (USA).cue'),
                '--no-launcher', '--renderer', 'software', '--headless',
                '--debug-port', str(self.port), '--memcard-dir', str(cards)]
        with self.log_path.open('w') as out:
            self.process = subprocess.Popen(args, cwd=ROOT, stdout=out, stderr=out, env=full)
        self.run, self.tag = cards, tag
        deadline = time.monotonic() + 60
        while True:
            if self.process.poll() is not None: raise RuntimeError(f'game exited; see {self.log_path}')
            try:
                with socket.create_connection(('127.0.0.1', self.port), .2): break
            except OSError:
                if time.monotonic() > deadline: raise RuntimeError('debug endpoint did not start')
                time.sleep(.1)

    def write(self, a, data):
        self.q(cmd='write_ram', addr=f'{a:08x}', bytes=bytes(data).hex())

    def read(self, a, n):
        return bytes.fromhex(self.q(cmd='read_ram', addr=f'{a:08x}', len=n)['hex'])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', required=True)
    ap.add_argument('--seed', default='1')
    ap.add_argument('--fights', type=int, default=8)
    ap.add_argument('--tag', default='team')
    a = ap.parse_args()
    WORK.mkdir(parents=True, exist_ok=True)
    build = Path(a.build).resolve()
    names = [l.split()[0] for l in (build / 'mods/ttt1/guests.txt').read_text().splitlines() if l.split()]
    env = {'TEKKEN3_CPU_MOVESET': 'random', 'TEKKEN3_CPU_MOVESET_SEED': a.seed, 'TEKKEN3_TEAM_GUESTS': '1,0'}
    g = Game(build, a.tag, env)
    fights, guest = [], None
    try:
        T.to_selector(g)
        g.write(MODE, [2, 0, 0, 0]); g.write(PHASE, [0, 0]); g.write(STATE, [10, 0])
        time.sleep(3)
        for _ in range(7): g.press(RIGHT)
        g.press(CROSS); time.sleep(.8)
        start = len(g.log())
        for _ in range(6):
            g.press(START)                                  # Start: the teams are drawn
            if g.value(STATE) != 10: break
            time.sleep(.8)
        # As soon as the loading screen shows the teams: the first fighters
        # load meanwhile, and the mod tosses only on this screen.
        end = time.monotonic() + 30
        while not (g.value(STATE, 2) == 11 and g.read(TEAMS[1] + 12, 1)[0] == 8):
            if time.monotonic() > end: raise AssertionError('teams not built')
        p1 = {m >> 2 for m in g.read(TEAMS[0], 8)}
        guest = next(n for n in GUESTS if GUEST_ID + names.index(n) not in p1)
        gid = GUEST_ID + names.index(guest)
        team = [gid] + NATIVES
        # One command each: the mod never sees half a team.
        g.write(TEAMS[1], [i * 4 for i in team])
        g.write(FIRST + 2, [gid, 0]); g.write(FIRST_COSTUME + 2, [0, 0])
        assert g.value(STATE, 2) == 11, 'the loading screen ended before the CPU team was written'
        time.sleep(1.5)
        print('CPU team:', [STOCK[i] if i < GUEST_ID else guest for i in team])
        killed, seen, t0 = False, -1, time.monotonic()
        while len(fights) < a.fights and time.monotonic() - t0 < 900:
            if g.process.poll() is not None: raise RuntimeError('the game exited')
            state = g.value(STATE, 2)
            if state == 8 and g.value(RUNNING) == 1:
                if not killed and g.value(LIFE, 2) > 0:
                    pid = g.value(ACTOR_ID + STRIDE, 2)
                    if fights and fights[-1][0] == pid: g.write(LIFE, [0, 0]); killed = True; continue
                    log = g.log()
                    last = MOVES.findall(log)
                    moves = 'TTT1' if last and 'on the TTT1 moveset' in last[-1] else 'T3'
                    if not fights:
                        chooser = g.value(CHOOSER, 1)
                        arena = (g.value(ARENA, 2), chooser, g.value(ACTOR_ID + chooser * STRIDE, 2),
                                 [g.value(g.value(RECORDS + i * 16) + 10, 1) for i in range(len(STOCK))])
                    fights.append((pid, moves, len(log)))
                    print('fight', len(fights), 'P2', STOCK[pid] if pid < GUEST_ID else names[pid - GUEST_ID], moves)
                    time.sleep(.3)
                    g.write(LIFE, [0, 0]); killed = True
            else:
                killed = False
                if state not in (8, 9, 10, 11): g.press(START)
            time.sleep(.2)
        log = g.log()
    finally:
        g.stop()
    shutil.copy2(g.log_path, WORK / f'{a.tag}-last.log')
    failures = []
    after = log[start:]
    # The toss of the team written: the last block of team lines (Start's
    # own may fall after `start` too).
    blocks, block = [], []
    for line in after.splitlines():
        if line.startswith('[FPS]'): continue
        if re.match(r"(Native moves: P\d's |TTT1 characters: .* joins )", line): block.append(line)
        elif block: blocks.append(block); block = []
    tossed = set(re.findall(r"Native moves: P2's (.*?) fights on TTT1 moves \(toss\)", '\n'.join(blocks[-1] if blocks else [])))
    print('tossed TTT1:', sorted(tossed))
    if not fights or fights[0][0] != gid: failures.append(f'the first CPU fighter is not {guest}: the bench wrote too late')
    draws = re.findall(r'Native moves: CPU P2 fighter (\d+) draws \d+ choice\(s\) -> (\S+)', after)
    if draws: failures.append(f'the CPU drew for its team members: {draws}')
    for pid, moves, at in fights:
        if pid >= GUEST_ID: continue
        want = 'TTT1' if STOCK[pid] in tossed else 'T3'
        if moves != want: failures.append(f'{STOCK[pid]} fought on {moves}, its team tossed {want}')
    gone = next((at for pid, moves, at in fights if pid < GUEST_ID), None)
    label = guest.capitalize()
    if gone is not None:
        tail = log[gone:]
        stale = [l for l in tail.splitlines() if re.match(rf'(?i){guest}\S* model:', l) or re.search(rf'(?i)\+ {guest}\)', l)]
        if stale: failures.append(f'{label} reloaded after leaving: {stale[:4]}')
    # The first fight's arena: the chooser's fighter's (a guest's: the
    # fighter its catalogue line names, Jin's when none).
    if fights:
        got, chooser, who, table = arena
        owner = who
        if who >= GUEST_ID:
            column = (build / 'mods/ttt1/guests.txt').read_text().splitlines()[who - GUEST_ID].split()
            owner = int(column[2]) if len(column) > 2 and column[2].isdigit() else 9
        print(f'first fight: arena {got}, chooser P{chooser + 1} fighter {who}, arena owner {owner} ({table[owner]})')
        if got != table[owner]: failures.append(f'first fight on arena {got}, not {table[owner]} (fighter {owner})')
    print('fights:', [(STOCK[p] if p < GUEST_ID else label, m) for p, m, _ in fights])
    if len(fights) < a.fights: failures.append(f'only {len(fights)} fights of {a.fights}')
    for f in failures: print('FAIL:', f)
    if failures: sys.exit(1)
    print('ok: every CPU member fought on its tossed moves; nothing of', label, 'reloaded after it left')


if __name__ == '__main__': main()
