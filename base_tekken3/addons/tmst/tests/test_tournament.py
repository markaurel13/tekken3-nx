"""src/tournament: the pools, the bracket and the progress file, built alone with
the C compiler at hand (skipped without one) through tests/c/tournament_main.c."""
import os, shutil, struct, subprocess, tempfile, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
SOURCES = ['tmst_bracket.c', 'tmst_tournament_pools.c', 'tmst_tournament_progress.c', 'tmst_tournament_stages.c',
           'tmst_tournament_cine.c', 'tmst_tournament_story.c']
# The roster keys the Expanded has today: the stock fighters, Panda and Tiger, its guests.
STOCK = ['paul', 'law', 'lei', 'king', 'yoshimitsu', 'nina', 'hwoarang', 'xiaoyu', 'eddy', 'jin', 'julia', 'kuma',
         'bryan', 'heihachi', 'ogre', 'mokujin', 'gunjack', 'gon', 'anna', 'drb', 'trueogre', 'panda', 'tiger']
GUESTS = ['alex', 'angel', 'armorking', 'baek', 'bruce', 'devil', 'ganryu', 'jack2', 'jun', 'kazuya', 'kunimitsu',
          'lee', 'michelle', 'pjack', 'roger', 'tetsujin', 'unknown', 'wang']


def compiler():
    for name in (os.environ.get('CC'), 'clang', 'gcc', 'cc'):
        if name and shutil.which(name):
            return shutil.which(name)
    return None


@unittest.skipUnless(compiler(), 'no C compiler')
class TournamentTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        src = ADDON / 'src/tournament'
        cls.exe, cls.locked = (Path(cls.tmp.name) / n for n in ('t_open.exe', 't_locked.exe'))
        for exe, flag in ((cls.exe, ['-DTMST_TOURNAMENT_ALL_OPEN=1']), (cls.locked, [])):
            subprocess.run([compiler(), '-O2', '-Wall', '-Werror', *flag, '-I', str(src),
                            str(ADDON / 'tests/c/tournament_main.c'), *(str(src / s) for s in SOURCES), '-o', str(exe)],
                           check=True)

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def run_exe(self, *args, exe=None):
        return subprocess.run([str(exe or self.exe), *map(str, args)], check=True, capture_output=True,
                              text=True).stdout.splitlines()

    # ---- pools -------------------------------------------------------------------
    def pools(self):
        out = {}; self.out = {}
        for line in self.run_exe('pools'):
            tid, boss, keys = line.split('|')
            out[tid] = (boss, keys.split() if not keys.startswith('*') else None)
            if keys.startswith('*'): self.out[tid] = [k[1:] for k in keys.split()[1:]]
        return out

    def test_pools(self):
        p = self.pools()
        self.assertEqual(list(p), ['t1', 't2', 't3', 'ttt', 'free'])
        self.assertEqual({t: p[t][0] for t in p},
                         {'t1': 'heihachi', 't2': 'kazuya+devil', 't3': 'ogre+trueogre', 'ttt': 'unknown', 'free': '-'})
        self.assertEqual([len(p[t][1]) for t in ('t1', 't2', 't3')], [17, 25, 23])
        self.assertIsNone(p['ttt'][1]); self.assertIsNone(p['free'][1])
        self.assertEqual(self.out, {'ttt': ['gon', 'drb'], 'free': []})
        self.assertTrue(set(p['t1'][1]) < set(p['t2'][1]))
        for t in ('t1', 't2', 't3'):
            keys = p[t][1]
            self.assertEqual(len(keys), len(set(keys)), t)
            bosses = p[t][0].split('+')
            for b in bosses: self.assertIn(b, keys, t)
            self.assertTrue(set(keys) <= set(STOCK + GUESTS), set(keys) - set(STOCK + GUESTS))
            self.assertGreaterEqual(len(keys) - 1 - len(bosses), 15, t)   # the player and the bosses out: 15 others left

    # ---- bracket -----------------------------------------------------------------
    def bracket(self, seed, results, player=3, boss=7, n=30):
        lines = self.run_exe('bracket', seed, player, boss, n, results)
        if lines == ['refused']: return None
        slots = list(map(int, lines[0].split()[1:]))
        steps = [l.split() for l in lines[1:-1]]
        final = list(map(int, steps[-1][2:]))
        return slots, steps, final, lines[-1].split()

    def test_slots(self):
        for seed in range(1, 60):
            slots, *_ = self.bracket(seed, '')
            self.assertEqual(len(slots), 16)
            self.assertEqual(len(set(slots)), 16, slots)
            self.assertIn(3, slots); self.assertNotIn(7, slots)
            self.assertTrue(all(0 <= s < 30 for s in slots))

    def test_player_place_varies(self):
        self.assertGreater(len({self.bracket(seed, '')[0].index(3) for seed in range(1, 80)}), 8)

    def test_pool_too_small(self):
        self.assertIsNone(self.bracket(1, '', n=16))        # 16 minus player and boss: 14
        self.assertIsNotNone(self.bracket(1, '', n=17))

    def test_champion_path(self):
        for seed in range(1, 40):
            slots, steps, final, end = self.bracket(seed, 'WWWWW')
            played = [s for s in steps if s[0] == 'match']
            self.assertEqual([int(s[1]) for s in played][-1], 15)
            self.assertEqual(len(played), 5)
            self.assertEqual(end[0], 'champion')
            self.assertEqual(final[15], 3)
            self.assertEqual(int(played[-1][3]), 7)          # the boss in the Grand Final
            self.check_consistent(slots, final)

    def test_boss_only_in_grand_final(self):
        for seed in range(1, 40):
            slots, steps, final, _ = self.bracket(seed, 'LLLLL')
            self.assertNotIn(7, final[:15])
            self.assertEqual(sum(s[0] == 'match' for s in steps), 1)

    def test_loss_each_round(self):
        for k in range(5):
            for seed in range(1, 15):
                slots, steps, final, end = self.bracket(seed, 'W' * k + 'L')
                self.assertEqual(end[0], 'out')
                lost = int(end[3])
                played = [int(s[1]) for s in steps if s[0] == 'match']
                self.assertEqual(len(played), k + 1)
                self.assertEqual(lost, played[-1])
                self.assertNotEqual(final[lost], 3)
                self.assertTrue(all(w >= 0 for w in final))
                self.check_consistent(slots, final)

    def test_round_drawn_after_player(self):
        slots, steps, *_ = self.bracket(5, 'W')
        step = [s for s in steps if s[0] == 'step'][0]
        self.assertEqual(int(step[1]), 0xff)                 # the whole first round, the player's match included
        winners = list(map(int, step[2:]))
        self.assertTrue(all(w >= 0 for w in winners[:8]) and all(w < 0 for w in winners[8:]))

    def check_consistent(self, slots, w):
        def fighters(m):
            if m < 8: return slots[2 * m], slots[2 * m + 1]
            if m < 12: return w[(m - 8) * 2], w[(m - 8) * 2 + 1]
            if m < 14: return w[8 + (m - 12) * 2], w[9 + (m - 12) * 2]
            if m == 14: return w[12], w[13]
            return w[14], 7
        for m in range(16):
            self.assertIn(w[m], fighters(m), m)
        for m in range(8):                                   # sides stay apart until the final
            left = m < 4
            self.assertEqual(slots.index(w[m]) < 8, left)

    def test_draws_even(self):
        wins = 0
        for seed in range(1, 200):
            slots, steps, final, _ = self.bracket(seed, '')
            wins += final[0] == slots[0]
        self.assertTrue(70 < wins < 130, wins)

    # ---- progress ----------------------------------------------------------------
    def test_progress_all_open_while_testing(self):
        path = Path(self.tmp.name) / 'open.txt'
        self.assertEqual(self.run_exe('progress', path, 'load'), ['won 0 open 0 1 2 3 4'])
        env = dict(os.environ, TMST_TOURNAMENT_ALL_OPEN='1')     # the headless tests' way, the default build
        out = subprocess.run([str(self.locked), 'progress', str(path), 'load'], check=True, capture_output=True,
                             text=True, env=env).stdout.splitlines()
        self.assertEqual(out, ['won 0 open 0 1 2 3 4'])
        self.assertEqual(self.run_exe('progress', path, 'load', exe=self.locked), ['won 0 open 0'])

    def test_progress_chain(self):
        path = Path(self.tmp.name) / 'tournament.txt'
        if path.exists(): path.unlink()
        run = lambda op: self.run_exe('progress', path, op, exe=self.locked)[0]
        self.assertEqual(run('load'), 'won 0 open 0')
        self.assertEqual(run('win:0'), 'won 1 open 0 1 opens 1')
        self.assertEqual(run('win:0'), 'won 1 open 0 1 opens -1')
        self.assertEqual(run('win:1'), 'won 3 open 0 1 2 opens 2')
        self.assertEqual(run('win:2'), 'won 7 open 0 1 2 3 opens 3')
        self.assertEqual(run('win:3'), 'won 15 open 0 1 2 3 4 opens 4')
        self.assertEqual(run('win:4'), 'won 31 open 0 1 2 3 4 opens -1')
        self.assertIn('won = t1, t2, t3, ttt, free', path.read_text())

    def test_progress_text(self):
        path = Path(self.tmp.name) / 'hand.txt'
        path.write_text('# edited\n  won =  t2 ,ttt, bogus\n')
        self.assertEqual(self.run_exe('progress', path, 'load', exe=self.locked), ['won 10 open 0 1 2 3 4'])

    # ---- stages ------------------------------------------------------------------
    NONE, T2, T3, TTT = range(4)

    def stages(self, t, keys, school=0):
        return {k: (int(a), int(b)) for k, a, b, _ in
                (l.split() for l in self.run_exe('stage', t, school, *keys))}

    def music(self, t, keys, school=0, arcade=False):
        return {k: m for k, _, _, m in
                (l.split() for l in self.run_exe('stage-arcade' if arcade else 'stage', t, school, *keys))}

    def test_music_arcade(self):
        """Issue #9: BGM SELECT's ARCADE (RANDOM's draw): T1, T2 and TTT the Arcade pieces, every one in its list;
        T3 still the Console XA (the game side maps it by the game's table)."""
        lists = {}
        for cat, files in (('t2-arcade', ['tekken1-arcade', 'tekken2-arcade']), ('ttt-arcade', ['ttt-arcade'])):
            lists[cat] = {l.split('=')[0].strip() for f in files
                          for l in (ADDON / 'data/jukebox' / (f + '.txt')).read_text().splitlines()
                          if '=' in l and not l.startswith('#')}
        p = self.pools(); everyone = STOCK + GUESTS
        for t, cat in ((0, 't2-arcade'), (1, 't2-arcade'), (3, 'ttt-arcade')):
            tid = ['t1', 't2', 't3', 'ttt'][t]
            keys = p[tid][1] or [k for k in everyone if k not in self.out[tid]]
            for school in (0, 1):
                for k, m in self.music(t, keys, school, True).items():
                    c, name = m.split('/')
                    self.assertEqual(c, cat, (tid, k)); self.assertIn(name, lists[cat], (tid, k))
        self.assertEqual(self.music(1, ['bruce', 'jack2', 'devil'], arcade=True),
                         {'bruce': 't2-arcade/t2-bruce.bgm', 'jack2': 't2-arcade/t2-jack-2.bgm',
                          'devil': 't2-arcade/t2-devil.bgm'})
        self.assertEqual(self.music(3, ['xiaoyu'], 1, True), {'xiaoyu': 'ttt-arcade/arcade-school.bgm'})
        self.assertEqual(self.music(2, ['anna'], arcade=True), {'anna': 't3-console/xa-21'})

    def test_music(self):
        lists = {}
        for cat, files in (('t2-console', ['tekken1-console', 'tekken2-console']), ('t3-console', ['tekken3-console']),
                           ('ttt-console', ['ttt-console'])):
            lists[cat] = {l.split('=')[0].strip() for f in files
                          for l in (ADDON / 'data/jukebox' / (f + '.txt')).read_text().splitlines()
                          if '=' in l and not l.startswith('#')}
        p = self.pools(); everyone = STOCK + GUESTS
        for t, cat in ((0, 't2-console'), (1, 't2-console'), (2, 't3-console'), (3, 'ttt-console')):
            tid = ['t1', 't2', 't3', 'ttt'][t]
            keys = p[tid][1] or [k for k in everyone if k not in self.out[tid]]
            for school in (0, 1):
                for k, m in self.music(t, keys, school).items():
                    c, name = m.split('/')
                    self.assertEqual(c, cat, (tid, k)); self.assertIn(name, lists[cat], (tid, k))
        self.assertEqual(set(self.music(4, everyone).values()), {'-/-'})
        self.assertEqual(self.music(0, ['paul', 'wang']), {'paul': 't2-console/t2-paul.bgm', 'wang': 't2-console/t2-wang.bgm'})
        self.assertEqual(self.music(1, ['alex', 'angel', 'kazuya', 'devil']),
                         {'alex': 't2-console/t2-kuma.bgm', 'angel': 't2-console/devil.bgm',
                          'kazuya': 't2-console/t2-kazuya.bgm', 'devil': 't2-console/devil.bgm'})
        self.assertEqual(self.music(2, ['anna', 'gunjack', 'nina', 'panda', 'trueogre']),
                         {'anna': 't3-console/xa-21', 'gunjack': 't3-console/xa-27', 'nina': 't3-console/xa-38',
                          'panda': 't3-console/xa-34', 'trueogre': 't3-console/xa-41'})
        self.assertEqual(self.music(3, ['bryan', 'unknown', 'xiaoyu']),
                         {'bryan': 'ttt-console/ps2-paul.bgm', 'unknown': 'ttt-console/ps2-unknown.bgm',
                          'xiaoyu': 'ttt-console/ps2-xiaoyu.bgm'})
        self.assertEqual(self.music(3, ['xiaoyu'], 1), {'xiaoyu': 'ttt-console/ps2-school.bgm'})

    def test_stages_cover_pools(self):
        p = self.pools()
        everyone = STOCK + GUESTS
        for t, kind, top in ((0, self.T2, 22), (1, self.T2, 22), (2, self.T3, 14), (3, self.TTT, 14)):
            tid = ['t1', 't2', 't3', 'ttt'][t]
            keys = p[tid][1] or [k for k in everyone if k not in self.out[tid]]
            for school in (0, 1):
                for k, (got, stage) in self.stages(t, keys, school).items():
                    self.assertEqual(got, kind, (tid, k))
                    self.assertTrue(0 <= stage <= top, (tid, k, stage))
        self.assertEqual(set(self.stages(4, everyone).values()), {(self.NONE, -1)})   # Free: the system's

    def test_stages_lists(self):
        self.assertEqual(self.stages(0, ['heihachi', 'kazuya', 'lei', 'bruce']),
                         {'heihachi': (self.T2, 6), 'kazuya': (self.T2, 21), 'lei': (self.NONE, -1),
                          'bruce': (self.NONE, -1)})
        self.assertEqual(self.stages(1, ['devil', 'angel', 'roger', 'alex', 'bruce', 'lei']),
                         {'devil': (self.T2, 22), 'angel': (self.T2, 22), 'roger': (self.T2, 17),
                          'alex': (self.T2, 14), 'bruce': (self.T2, 0), 'lei': (self.T2, 5)})
        # T1's stages are T2's, for its own fighters
        t1, t2 = self.stages(0, GUESTS + STOCK), self.stages(1, GUESTS + STOCK)
        for k, v in t1.items():
            if v[0] != self.NONE: self.assertEqual(v, t2[k], k)
        self.assertEqual(self.stages(2, ['trueogre', 'gon', 'drb', 'tiger', 'panda', 'heihachi']),
                         {'trueogre': (self.T3, 6), 'gon': (self.T3, 13), 'drb': (self.T3, 14),
                          'tiger': (self.T3, 10), 'panda': (self.T3, 1), 'heihachi': (self.T3, 12)})
        self.assertEqual(self.stages(3, ['unknown', 'tetsujin', 'kazuya', 'jun']),
                         {'unknown': (self.TTT, 14), 'tetsujin': (self.TTT, 2), 'kazuya': (self.TTT, 12),
                          'jun': (self.TTT, 8)})

    def test_stages_school(self):
        for t, kind in ((2, self.T3), (3, self.TTT)):
            self.assertEqual(self.stages(t, ['xiaoyu', 'jin', 'paul']),
                             {'xiaoyu': (kind, 3), 'jin': (kind, 8), 'paul': (kind, 0)})
            self.assertEqual(self.stages(t, ['xiaoyu', 'jin', 'paul'], 1),
                             {'xiaoyu': (kind, 7), 'jin': (kind, 7), 'paul': (kind, 0)})
        self.assertEqual(self.stages(1, ['jun'], 1), {'jun': (self.T2, 2)})   # T2: no school stage

    # ---- GAME OPTION's STAGES = TEKKEN 2 and RANDOM (issue #11) ----
    def option_stages(self, keys, school=0):
        return {k: (int(a), int(b), int(c)) for k, a, b, c in
                (l.split() for l in self.run_exe('option-stage', school, *keys))}

    # the user's list (2026-11-13): (kind, stage)
    OPTION = {
        'bryan': (3, 0), 'mokujin': (3, 2), 'xiaoyu': (3, 3), 'hwoarang': (3, 4), 'ogre': (3, 6),
        'trueogre': (3, 6), 'jin': (3, 8), 'gunjack': (3, 9), 'eddy': (3, 10), 'julia': (3, 12), 'gon': (3, 13),
        'drb': (3, 14),
        'panda': (4, 1), 'tetsujin': (4, 2), 'tiger': (4, 13), 'unknown': (4, 14),
        'bruce': (2, 0), 'wang': (2, 1), 'jun': (2, 2), 'paul': (2, 3), 'king': (2, 4), 'lei': (2, 5),
        'heihachi': (2, 6), 'yoshimitsu': (2, 7), 'jack2': (2, 8), 'law': (2, 9), 'kunimitsu': (2, 10),
        'nina': (2, 11), 'armorking': (2, 12), 'lee': (2, 13), 'alex': (2, 14), 'kuma': (2, 14), 'ganryu': (2, 15),
        'anna': (2, 16), 'roger': (2, 17), 'pjack': (2, 18), 'michelle': (2, 19), 'baek': (2, 20),
        'kazuya': (2, 21), 'devil': (2, 22), 'angel': (2, 22)}

    def test_option_stages_list(self):
        kinds = {2: self.T2, 3: self.T3, 4: self.TTT}
        got = self.option_stages(STOCK + GUESTS)
        self.assertEqual(set(got), set(self.OPTION))          # every fighter on the list, none missing
        for k, (kind, stage) in self.OPTION.items():
            self.assertEqual(got[k][:2], (kinds[kind], stage), k)
        self.assertEqual(self.option_stages(['nobody']), {'nobody': (self.NONE, -1, -1)})

    # RANDOM's Tekken 2 lot for Tekken 3's newcomers (issue #11, the user's list, 2026-11-14)
    RANDOM_T2 = {'xiaoyu': 1, 'panda': 1, 'jin': 2, 'bryan': 3, 'mokujin': 7, 'tetsujin': 7, 'gunjack': 8,
                 'drb': 8, 'gon': 17, 'eddy': 17, 'tiger': 17, 'julia': 19, 'hwoarang': 20, 'ogre': 22,
                 'trueogre': 22, 'unknown': 22}

    def test_option_stages_school(self):
        got = self.option_stages(['xiaoyu', 'jin', 'paul', 'panda'], 1)
        self.assertEqual(got['xiaoyu'][:2], (self.T3, 7))
        self.assertEqual(got['jin'][:2], (self.T3, 7))
        self.assertEqual(got['paul'][:2], (self.T2, 3))       # the outfit counts only for Xiaoyu and Jin
        self.assertEqual(got['panda'][:2], (self.TTT, 1))

    def test_option_random_pool(self):
        got = self.option_stages(STOCK + GUESTS)
        for k, (kind, stage, lot) in got.items():
            # RANDOM's Tekken 2 lot: Tekken 2's fighters on their own Tekken 2 stage,
            # Tekken 3's newcomers on the user's list (issue #11, 2026-11-14)
            self.assertEqual(lot, stage if kind == self.T2 else self.RANDOM_T2.get(k, -1), k)
        self.assertEqual(got['paul'][2], 3)
        self.assertEqual(got['panda'][2], 1)
        self.assertEqual(got['roger'][2], 17)
        # every stock fighter and guest has a lot (the user's lists, 2026-11-14)
        for k, (kind, stage, lot) in got.items(): self.assertGreaterEqual(lot, 0, k)
        # the school outfit: T3 or TTT only
        school = self.option_stages(['xiaoyu', 'jin', 'paul', 'panda'], 1)
        self.assertEqual((school['xiaoyu'][2], school['jin'][2]), (-1, -1))
        self.assertEqual((school['paul'][2], school['panda'][2]), (3, 1))
        # the T2 tournament's stages are the option's
        for k, v in self.stages(1, STOCK + GUESTS).items():
            if v[0] == self.T2: self.assertEqual(got[k][:2], v, k)

    # ---- the boss intros (tmst_tournament_cine.c) ----
    @staticmethod
    def pack(records=4, clips=None, ch3=0, sounds=None):
        """A TTT1 combat pack: record 0 the neutral (3 still frames), the
        records in `clips` ({record: (frames, step)}) on their own: heading
        0x8000, distance step a frame, channel 3 from `ch3` on, wrapping
        past 0xFFFF; `sounds` ({record: (event1, event2, frame)}) their
        sound lists (the others 0xFFFF, 0xFFFF, 0)."""
        def clip(frames, step):
            rows = []
            for f in range(frames):
                row = [0] * 57
                row[0], row[1], row[2], row[3] = 0x8000, 1040, f * step, (ch3 + f * 8) & 0xffff
                rows.append(struct.pack('<57H', *row))
            return struct.pack('<II', 0x4a554e00 | frames, 57) + b''.join(rows)
        clips, sounds = clips or {}, sounds or {}
        table = 32 + records * 16
        at = table + records * 56
        blobs, offsets, lists = [clip(3, 0)], {None: at}, {}
        at += len(blobs[0])
        for r, (frames, step) in sorted(clips.items()):
            blobs.append(clip(frames, step)); offsets[r] = at; at += len(blobs[-1])
        for r in range(records):
            blobs.append(struct.pack('<3H', *sounds.get(r, (0xffff, 0xffff, 0))) + b'\xff\xff'); lists[r] = at; at += 8
        head = struct.pack('<8I', 0x314d554a, 4, 0, records, 0, 0, 0, table)
        entries = b''.join(struct.pack('<4I', 0, 0x80000000 + i, table + i * 56, 0) for i in range(records))
        recs = b''.join(struct.pack('<I', offsets.get(i, offsets[None])) + b'\0' * 24 + struct.pack('<I', lists[i])
                        + b'\0' * 24 for i in range(records))
        out = bytearray(head + entries + recs + b''.join(blobs))
        struct.pack_into('<I', out, 8, len(out))
        return bytes(out)

    def cine(self, key, own, cmd='cine', **others):
        args, d = [], Path(self.tmp.name)
        (d / 'own.jmv').write_bytes(own)
        for stem, data in others.items():
            (d / (stem + '.jmv')).write_bytes(data)
            args.append('%s-TTT1=%s' % (stem, d / (stem + '.jmv')))
        r = subprocess.run([str(self.exe), cmd, key, str(d / 'own.jmv'), *args], capture_output=True, text=True)
        return r.returncode, r.stdout.splitlines()

    @staticmethod
    def blocks(lines):
        """The .cine's header words, its camera rows, its actors' pose rows."""
        head, cam, poses, i = {}, [], {}, 1
        while i < len(lines):
            w = lines[i].split()
            if w[0] == 'camera':
                cam = [list(map(int, l.split())) for l in lines[i + 1:i + 1 + int(w[2])]]; i += 1 + int(w[2]); continue
            if w[0] == 'poses':
                poses[int(w[1])] = [list(map(int, l.split())) for l in lines[i + 1:i + 1 + int(w[3])]]; i += 1 + int(w[3]); continue
            head.setdefault(w[0], []).append(w[1:]); i += 1
        return head, cam, poses

    @staticmethod
    def turn(a, b): return min(abs(b - a), 65536 - abs(b - a))

    def devil(self, step=20, sounds=None):
        """Devil's intro on test packs: Kazuya's stance and tag move (record 1),
        Devil's stance; Kazuya's channel 3 from 1000, Devil's from 30000."""
        kazuya = self.pack(clips={1: (50, step)}, ch3=1000, sounds=sounds)
        return self.cine('devil', self.pack(ch3=30000), Kazuya=kazuya)

    def test_intro_devil(self):
        code, out = self.devil()
        self.assertEqual((code, out[0]), (0, 'entry devil'))
        head, cam, poses = self.blocks(out)
        n = int(head['length'][0][0])
        self.assertEqual((len(cam), len(poses[1]), len(poses[0])), (n, n, 1))
        # Kazuya first, Devil (the boss, 33 costume 0) from the swap on
        self.assertEqual(head['actor'], [['0', '7', '1'], ['1', 'kazuya', '0']])
        swap = int(head['swap'][0][3])
        self.assertEqual(head['swap'][0][:3], ['1', '33', '0'])
        self.assertEqual(head['hide'], [['0', '0']])
        self.assertEqual(head['dark'], [['0']])
        # the flash: up to white before the swap, down after; black at both ends
        fades = [list(map(int, f)) for f in head['fade']]
        self.assertEqual(fades[0][2:], [-256, 0])
        self.assertIn([swap - 6, swap - 1, 0, 256], fades)
        self.assertIn([swap, swap + 12, 256, 0], fades)
        self.assertEqual(fades[-1][1:], [n - 1, 0, -256])
        # the lightning on the actor, round the swap
        aura = list(map(int, head['aura'][0]))
        self.assertEqual(aura[0], 1)
        self.assertTrue(aura[1] < swap < aura[2])
        self.assertTrue(all(len(p) == 57 for p in poses[1]))
        # Kazuya's channel 3 first (from 1000), Devil's stance at the end (30000)
        c3 = [p[3] for p in poses[1]]
        self.assertTrue(1000 <= c3[0] < 1100)
        self.assertTrue(30000 <= c3[-1] < 30100)
        self.assertTrue(all(self.turn(a, b) <= 2500 for a, b in zip(c3, c3[1:])))   # blended, no jump
        for a, b in zip(cam, cam[1:]):                                               # the camera eased
            self.assertTrue(all(abs(x - y) <= 40 for x, y in zip(a, b)), (a, b))
        self.assertGreater(poses[0][0][2], 10000)                                    # the hidden player far away

    def test_intro_parts_go_on(self):
        # the tag move walks forward: never back, and Devil's stance stays where it
        # left him, on its heading (no jump of the root across the parts)
        code, out = self.devil(step=10)
        p = self.blocks(out)[2][1]
        d = [r[2] for r in p]
        self.assertEqual(code, 0)
        self.assertTrue(all(0 <= b - a <= 20 for a, b in zip(d, d[1:])), d)
        self.assertGreater(d[-1], 300)
        self.assertTrue(all(r[0] == 0x8000 for r in p[100:]))

    def test_intro_sounds(self):
        # the tag move's own sound (event 0x705C at its frame 34): index 92 of
        # Kazuya's pack, at the intro's frame of that clip frame (70 %)
        kazuya = self.pack(clips={1: (50, 0)}, sounds={1: (0x705c, 0xffff, 34)})
        code, out = self.cine('devil', self.pack(), 'sounds', Kazuya=kazuya)
        self.assertEqual(code, 0)
        self.assertEqual(out, ['entry devil', '%d Kazuya-TTT1 92' % (80 + 34 * 100 // 70)])
        # a sound past the part's end, none; the boss's own pack in its place
        kazuya = self.pack(clips={1: (50, 0)}, sounds={1: (0x705c, 0x7001, 49)})
        self.assertEqual(self.cine('devil', self.pack(), 'sounds', Kazuya=kazuya)[1], ['entry devil'])
        self.assertEqual(self.cine('devil', self.pack(sounds={0: (0x7010, 0xffff, 5)}), 'sounds')[1],
                         ['entry devil', '5 - 16', '87 - 16', '155 - 16'])   # 80 + 5 / 0.7, 150 + 5

    def test_intro_heihachi(self):
        # a native alone: his own pack by its stem (Heihachi-TTT1), not P2's
        # moves' (the boss's pack here, channel 3 from 30000, never used)
        hei = self.pack(records=830, clips={815: (150, 4), 826: (60, 0)}, ch3=5000,
                        sounds={826: (0x7021, 0xffff, 10)})
        code, out = self.cine('heihachi', self.pack(ch3=30000), Heihachi=hei)
        self.assertEqual((code, out[0]), (0, 'entry heihachi'))
        head, cam, poses = self.blocks(out)
        n = int(head['length'][0][0])
        self.assertEqual((len(cam), len(poses[1])), (n, n))
        self.assertEqual(head['actor'][1], ['1', '33', '0'])                 # the boss all along
        self.assertNotIn('swap', head)
        self.assertEqual(len(head['fade']), 2)                               # no flash
        aura = list(map(int, head['aura'][0]))
        self.assertEqual(aura[:4], [1, 221, 266, 2])                         # in the turn (826 from frame 220)
        c3 = [p[3] for p in poses[1]]
        self.assertTrue(all(5000 <= v < 5000 + 150 * 8 for v in c3), (min(c3), max(c3)))
        self.assertTrue(all(self.turn(a, b) <= 2500 for a, b in zip(c3, c3[1:])))
        for a, b in zip(cam, cam[1:]):
            self.assertTrue(all(abs(x - y) <= 40 for x, y in zip(a, b)), (a, b))
        # his sound from his pack, at the turn's frame 10
        self.assertEqual(self.cine('heihachi', self.pack(), 'sounds', Heihachi=hei)[1],
                         ['entry heihachi', '230 Heihachi-TTT1 33'])
        # without his pack: not written (no intro)
        self.assertEqual(self.cine('heihachi', self.pack())[0], 1)

    def test_intro_trueogre(self):
        # two natives: Ogre's parts from his pack (Ogre-TTT1, channel 3 from
        # 1000), True Ogre's from his own (Trueogre-TTT1, from 5000), not P2's
        ogre = self.pack(records=820, clips={812: (75, 0), 2: (126, 0), 733: (150, 0)}, ch3=1000)
        to = self.pack(records=820, clips={735: (150, 0)}, ch3=5000)
        code, out = self.cine('trueogre', self.pack(ch3=30000), Ogre=ogre, Trueogre=to)
        self.assertEqual((code, out[0]), (0, 'entry trueogre'))
        head, cam, poses = self.blocks(out)
        n = int(head['length'][0][0])
        self.assertEqual((len(cam), len(poses[1])), (n, n))
        # Ogre first, True Ogre (the boss) from the swap on, in a white flash
        self.assertEqual(head['actor'][1], ['1', 'ogre', '0'])
        swap = int(head['swap'][0][3])
        self.assertEqual(head['swap'][0][:3], ['1', '33', '0'])
        fades = [list(map(int, f)) for f in head['fade']]
        self.assertIn([swap - 16, swap - 1, 0, 256], fades)
        self.assertIn([swap, swap + 32, 256, 0], fades)
        # his lightning from his arms spread, on through the flash
        aura = list(map(int, head['aura'][0]))
        self.assertEqual(aura[:2], [1, 147])
        self.assertGreater(aura[2], swap + 32)
        c3 = [p[3] for p in poses[1]]
        self.assertTrue(1000 <= c3[0] < 1100)
        self.assertTrue(all(1000 <= v < 1000 + 150 * 8 for v in c3[:swap]), (min(c3[:swap]), max(c3[:swap])))
        self.assertTrue(all(5000 <= v < 5000 + 150 * 8 for v in c3[swap + 12:]))
        self.assertTrue(all(self.turn(a, b) <= 2500 for a, b in zip(c3, c3[1:])))   # blended, no jump
        for a, b in zip(cam, cam[1:]):                                               # the camera eased
            self.assertTrue(all(abs(x - y) <= 40 for x, y in zip(a, b)), (a, b))
        # without Ogre's pack: still written (True Ogre's stance in its place);
        # without True Ogre's: not (no intro)
        self.assertEqual(self.cine('trueogre', self.pack(), Trueogre=to)[0], 0)
        self.assertEqual(self.cine('trueogre', self.pack(), Ogre=ogre)[0], 1)

    def test_intro_fallback(self):
        own = self.pack()
        code, out = self.cine('-', own)
        self.assertEqual((code, out[0]), (0, 'entry -'))
        self.assertEqual(self.cine('lei', own)[1][0], 'entry -')
        head, cam, poses = self.blocks(out)
        self.assertNotIn('swap', head)
        self.assertNotIn('aura', head)
        self.assertEqual(head['actor'][1], ['1', '33', '0'])
        self.assertEqual(len(head['fade']), 2)                                # no flash
        self.assertTrue(all(p[2] == 0 for p in poses[1]))                     # the stance, standing
        # Kazuya's pack missing: the boss's neutral instead, still written
        code, out = self.cine('devil', own)
        self.assertEqual(code, 0)
        self.assertTrue(all(p[3] < 24 for p in self.blocks(out)[2][1]))
        # not a pack
        self.assertEqual(self.cine('devil', b'\0' * 64)[0], 1)

    # ---- the story before the draw ------------------------------------------------
    def story(self, text, width):
        path = Path(self.tmp.name) / 'story.txt'
        path.write_bytes(text.encode('utf-8'))
        return subprocess.run([str(self.exe), 'story', str(path), str(width)], check=True, capture_output=True,
                              encoding='utf-8').stdout.splitlines()

    def test_story_parse_and_wrap(self):
        out = self.story('\ufeff\r\n  TEKKEN 2  \r\none two three\r\nfour\r\n\r\n\r\n\u201cfive\u201d \u2013 six\r\n', 9)
        self.assertEqual(out, ['title|TEKKEN 2', '|one two', '|three', '|four', '|',
                               '|\u201cfive\u201d \u2013', '|six'])

    def test_story_long_word_alone(self):
        self.assertEqual(self.story('T\nab abcdefghij cd', 5), ['title|T', '|ab', '|abcdefghij', '|cd'])

    def test_story_empty(self):
        self.assertEqual(self.story('\n\n', 10), ['none'])

    def test_story_files(self):
        for tid, title in (('t1', 'TEKKEN'), ('t2', 'TEKKEN 2'), ('t3', 'TEKKEN 3')):
            text = (ADDON / 'data/tournament/story' / f'{tid}.txt').read_text(encoding='utf-8')
            out = self.story(text, 80)
            self.assertEqual(out[0], 'title|' + title)
            self.assertTrue(all(len(line) - 1 <= 80 for line in out[1:]))
            self.assertTrue(set(title.replace(' ', '')) <= set('ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789'))
        for tid in ('ttt', 'free'):
            self.assertFalse((ADDON / 'data/tournament/story' / f'{tid}.txt').exists())


if __name__ == '__main__':
    unittest.main()
