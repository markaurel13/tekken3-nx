"""tools/jukebox_titles.py, on a temporary Expanded folder (no disc)."""
import importlib.util, re, tempfile, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
spec = importlib.util.spec_from_file_location('jukebox_titles', ADDON / 'tools/jukebox_titles.py')
jukebox_titles = importlib.util.module_from_spec(spec)
spec.loader.exec_module(jukebox_titles)


class JukeboxTitlesTest(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.root = Path(self.tmp.name)
        music = self.root / 'addons/tmst/workspace/t2-music/arcade'
        music.mkdir(parents=True)
        for name in ('t2-paul.bgm', 't2-new.bgm'):
            (music / name).write_bytes(b'T3BGM001')
        self.defaults = jukebox_titles.DEFAULTS
        jukebox_titles.DEFAULTS = self.root / 'defaults'
        jukebox_titles.DEFAULTS.mkdir()
        (jukebox_titles.DEFAULTS / 'tekken2-arcade.txt').write_text('t2-paul.bgm = Paul (T2 Arcade)\n', encoding='utf-8')

    def tearDown(self) -> None:
        jukebox_titles.DEFAULTS = self.defaults
        self.tmp.cleanup()

    def lines(self, name: str) -> list[str]:
        text = (self.root / 'addons/tmst/workspace/jukebox' / name).read_text(encoding='utf-8')
        return [line for line in text.splitlines() if line and not line.startswith('#')]

    def test_defaults_then_names(self) -> None:
        jukebox_titles.update(self.root)
        self.assertEqual(self.lines('tekken2-arcade.txt'), ['t2-paul.bgm = Paul (T2 Arcade)', 't2-new.bgm = t2-new'])

    def test_user_lines_stay(self) -> None:
        lists = self.root / 'addons/tmst/workspace/jukebox'
        lists.mkdir(parents=True)
        (lists / 'tekken2-arcade.txt').write_text('# mine\nt2-paul = Mine --disable\ngone.bgm = Gone\n', encoding='utf-8')
        jukebox_titles.update(self.root)
        self.assertEqual(self.lines('tekken2-arcade.txt'), ['t2-paul = Mine --disable', 'gone.bgm = Gone', 't2-new.bgm = t2-new'])

    def test_made_again_while_nobody_changed_it(self) -> None:
        jukebox_titles.update(self.root)
        (jukebox_titles.DEFAULTS / 'tekken2-arcade.txt').write_text(
            't2-paul.bgm = Paul, new title\nt2-new.bgm = New\n', encoding='utf-8')
        jukebox_titles.update(self.root)
        self.assertEqual(self.lines('tekken2-arcade.txt'), ['t2-paul.bgm = Paul, new title', 't2-new.bgm = New'])
        path = self.root / 'addons/tmst/workspace/jukebox/tekken2-arcade.txt'
        path.write_text(path.read_text(encoding='utf-8') + 'mine.bgm = Mine\n', encoding='utf-8')
        (jukebox_titles.DEFAULTS / 'tekken2-arcade.txt').write_text('t2-paul.bgm = Paul again\n', encoding='utf-8')
        jukebox_titles.update(self.root)                    # written by hand: it stays
        self.assertEqual(self.lines('tekken2-arcade.txt'), ['t2-paul.bgm = Paul, new title', 't2-new.bgm = New', 'mine.bgm = Mine'])

    def test_disabled_pieces_left_out(self) -> None:
        (jukebox_titles.DEFAULTS / 'tekken2-arcade.txt').write_text(
            't2-paul.bgm = Paul --disable\nt2-new.bgm = New --disable\nt2-music-10.bgm = Ten\n', encoding='utf-8')
        (jukebox_titles.DEFAULTS / 'tekken1-arcade.txt').write_text('t2-new.bgm = New (T1)\n', encoding='utf-8')
        (jukebox_titles.DEFAULTS / 'tekken2-console.txt').write_text('t2-music-10.bgm = Ten --disable\n', encoding='utf-8')
        # t2-paul: the Tournament's (KEEP); t2-new: Tekken 1's list has it on.
        self.assertEqual(jukebox_titles.skipped('t2-music/arcade'), [])
        self.assertEqual(jukebox_titles.skipped('t2-music/console'), ['t2-music-10.bgm'])
        (jukebox_titles.DEFAULTS / 'tekken1-arcade.txt').unlink()
        self.assertEqual(jukebox_titles.skipped('t2-music/arcade'), ['t2-new.bgm'])
        lists = self.root / 'addons/tmst/workspace/jukebox'
        lists.mkdir(parents=True)
        (lists / 'tekken2-arcade.txt').write_text('t2-new.bgm = Mine\nt2-paul.bgm = Mine too\n', encoding='utf-8')
        jukebox_titles.update(self.root)
        self.assertEqual(self.lines('tekken2-arcade.txt'), ['t2-paul.bgm = Mine too'])
        self.assertEqual(self.lines('tekken2-console.txt'), [])

    def test_keep_is_what_the_addon_plays(self) -> None:
        played = set()
        for path in (ADDON / 'src').rglob('*.c'):
            if 'jukebox' not in path.parts:
                played |= set(re.findall(r'"([a-z0-9-]+\.bgm)"', path.read_text(encoding='utf-8')))
        kept = set().union(*jukebox_titles.KEEP.values())
        self.assertTrue(played)
        self.assertEqual(played - kept, set())
        jukebox_titles.DEFAULTS = self.defaults             # the add-on's own lists
        for folder in jukebox_titles.MADE:
            self.assertEqual(set(jukebox_titles.skipped(folder)) & played, set(), folder)

    def test_tekken1_only_what_it_names(self) -> None:
        jukebox_titles.update(self.root)
        self.assertEqual(self.lines('tekken1-arcade.txt'), [])


if __name__ == '__main__':
    unittest.main()
