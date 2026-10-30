#!/usr/bin/env python3
"""The Jacks' "Tarzan" win pose ends on its crash (B50, GH#65).

    python3 tools/test_tarzan_crash.py --build build-rel-lite --who jack2|gunjack-ttt1|gunjack-t3
        [--button cross|circle] [--tag NAME]

Cold start, Arcade selector: player 1 takes Jack-2 (Tag page, R2) or Gun Jack
(stock page; TEKKEN3_NATIVE_MOVES=1 puts him on his TTT1 moves, 0 on T3's).
Once the round runs, player 2's life goes to 0 and --button is held through
the K.O. and the replay: the win pose it picks plays its event script. Every
sound event from the K.O. on is logged (TEKKEN3_TTT1_SOUND_LOG).

TTT1's script 490 (the Tarzan pose of Gun Jack and Jack-2) beats the chest
(0x108D, 21 times) and ends on the crash 0x7098 at frame 172. Its entry 13
steps back in time (frame 100 after 103), which both engines skip (TTT1
0x8011454C, T3 0x80041428): the importer used to stop the script there, so
the pose lost its last eight beats and the crash. Fails when a TTT1 Jack's
pose beats its chest without the crash. On T3's moves, Gun Jack's own pose is
printed as the reference.
"""
import argparse, os, re, shutil, socket, subprocess, sys, time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / 'tools'))
import test_devil_jin as T

T.WORK = ROOT / 'workspace/tarzan-crash'


class Game(T.Game):
    """T.Game run from the build folder itself: no links (Windows refuses
    symbolic links without developer mode), cards in a fresh folder, headless
    software renderer. The build's settings.toml stays as it is."""
    def __init__(self, build, tag, env=None):
        cards = T.WORK / f'cards-{os.getpid()}-{tag}'
        shutil.rmtree(cards, ignore_errors=True); cards.mkdir(parents=True)
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0)); self.port = listener.getsockname()[1]
        self.log_path = T.WORK / f'{tag}.log'
        full = {k: v for k, v in os.environ.items() if not k.startswith('TEKKEN3_')}
        full.update(env or {}); full['SDL_AUDIO_DRIVER'] = 'dummy'
        exe = next(p for p in (build / 'Tekken_3_Expanded.exe', build / 'Tekken_3_Expanded') if p.is_file())
        args = [str(exe), '--game', str(ROOT / 'game.toml'), '--disc', str(ROOT / 'disc/Tekken 3 (USA).cue'),
                '--no-launcher', '--renderer', 'software', '--headless',
                '--debug-port', str(self.port), '--memcard-dir', str(cards)]
        with self.log_path.open('w') as out:
            self.process = subprocess.Popen(args, cwd=ROOT, stdout=out, stderr=out, env=full)
        self.run, self.tag = cards, tag
        deadline = time.monotonic() + float(os.environ.get('GAME_START_TIMEOUT', 60))
        while True:
            if self.process.poll() is not None: raise RuntimeError(f'game exited; see {self.log_path}')
            try:
                with socket.create_connection(('127.0.0.1', self.port), .2): break
            except OSError:
                if time.monotonic() > deadline: raise RuntimeError('debug endpoint did not start')
                time.sleep(.1)
PAD_R2 = 0xfdff
BUTTONS = dict(circle=T.pad(T.CI), cross=T.pad(T.CR))
WHO = {'jack2': (29, None), 'gunjack-ttt1': (16, '1'), 'gunjack-t3': (16, '0')}
BEAT, CRASH = 0x108d, 0x7098      # TTT1 events; the pack plays them with bit 0x800 (0x188D, 0x7898)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', required=True)
    ap.add_argument('--who', choices=WHO, required=True)
    ap.add_argument('--button', choices=BUTTONS, default='cross')
    ap.add_argument('--tag', default=None)
    a = ap.parse_args()
    T.WORK.mkdir(parents=True, exist_ok=True)
    cid, native_moves = WHO[a.who]
    env = {'TEKKEN3_TTT1_SOUND_LOG': '1'}
    if native_moves is not None: env['TEKKEN3_NATIVE_MOVES'] = native_moves
    tag = a.tag or f'{a.who}-{a.button}'
    g = Game(Path(a.build).resolve(), tag, env)
    try:
        T.to_selector(g)
        if cid >= 23:
            g.press(PAD_R2); time.sleep(1.5)
            T.cursor_to(g, cid)
        else:
            # Gun Jack is not on the cursor's path: take Paul, then write the
            # ID the loading screen reads (0x800ADD5C) until the fight (§ 67).
            # Slow steps: a native's TTT1 moveset loads under the cursor.
            seen = []
            for _ in range(120):
                seen.append(g.value(T.CURSOR_CHAR))
                if seen[-1] == 0: break
                g.q(cmd='press', buttons=T.RIGHT, frames=6, port=1); time.sleep(.5)
            else: raise AssertionError(f'cursor did not reach Paul: {seen[-24:]}')
            # The pick handler (0x8010DAB8) reads the fighter at +28 of the
            # player's block: Gun Jack there, then the button at once.
            g.q(cmd='write_ram', addr=f'{T.CURSOR_CHAR:08x}', bytes=cid.to_bytes(2, 'little').hex())
        g.press(T.SQUARE, frames=12)
        def step():
            state = g.value(T.STATE)
            if state == 9: g.press(T.SQUARE, frames=12)
            if state == 11 and cid < 23: g.q(cmd='write_ram', addr='800add5c', bytes=cid.to_bytes(2, 'little').hex())
        g.wait(lambda: g.value(T.STATE) == 8 and g.value(T.PHASE, 2) >= 8, 'round did not start', 150, step)
        assert T.actor(g, 0)[2] == cid, f'P1 is {T.actor(g, 0)[2]}, not {cid}'
        print('P1 actor +0x14,+0x16,+0x18 =', T.actor(g, 0)[:3])
        g.shot('start')                 # a pack the runtime rejects shows its message here
        time.sleep(1.5)
        mark = len(g.log())
        g.q(cmd='write_ram', addr=f'{T.ACTOR + T.STRIDE + T.LIFE:08x}', bytes='0000')
        end = time.monotonic() + 16
        while time.monotonic() < end:
            g.q(cmd='press', buttons=BUTTONS[a.button], frames=20); time.sleep(.3)
        g.shot('pose')
        lines = [l for l in g.log()[mark:].splitlines() if 'SOUNDLOG P1' in l or 'TTT1 characters: P1' in l]
        (T.WORK / f'{tag}.sounds.txt').write_text('\n'.join(lines))
        events = [int(m.group(1), 16) for l in lines for m in [re.search(r'SOUNDLOG P1 ([0-9A-F]{4})', l)] if m]
        beats = sum(e & ~0x800 == BEAT for e in events)
        crash = any(e & ~0x800 == CRASH for e in events)
        # TTT1 moves mark their script sounds (bit 0x800): T3's own beats mean
        # the TTT1 tables were not taken (the runtime rejects a bad pack).
        marked = all(e & 0x800 for e in events if e & ~0x800 in (BEAT, CRASH))
        print(f'{len(lines)} P1 sound lines after the K.O.; chest beats {beats}, crash {crash}, TTT1 events {marked}')
        for l in lines[:120]: print('  ', l)
        if a.who == 'gunjack-t3': return
        rejected = [l for l in g.log().splitlines() if 'tables rejected' in l]
        if rejected or not marked: raise SystemExit(f'FAIL: not on the TTT1 moves {rejected[:1]}')
        if not beats: raise SystemExit(f'FAIL: no Tarzan pose (no {BEAT:#06x}): try the other --button')
        if not crash: raise SystemExit(f'FAIL: Tarzan pose without its crash ({CRASH:#06x})')
        print('PASS: Tarzan pose ends on its crash')
    finally:
        g.stop()


if __name__ == '__main__':
    main()
