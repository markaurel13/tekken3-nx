"""src/core/tmst_files.c's tmst_files_addon, built alone with the C compiler at
hand (skipped without one), the game's folder given through a stand-in
SDL_GetBasePath (tests/c/sdl_stub): addons/tmst first, then the copy the
build stages under the game's mods/tmst-data (the Expanded's own APK)."""
import os, shutil, subprocess, tempfile, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
REL = 'data/tournament/story/t3.txt'


def compiler():
    for name in (os.environ.get('CC'), 'clang', 'gcc', 'cc'):
        if name and shutil.which(name):
            return shutil.which(name)
    return None


def touch(path: Path) -> Path:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text('x', encoding='utf-8')
    return path


def same(a: str, b: Path) -> bool:
    return os.path.normcase(os.path.realpath(a)) == os.path.normcase(os.path.realpath(b))


@unittest.skipUnless(compiler(), 'no C compiler')
class FilesTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.tmp = tempfile.TemporaryDirectory()
        cls.exe = Path(cls.tmp.name) / 'files_main.exe'
        subprocess.run([compiler(), '-O2', '-I', str(ADDON / 'tests/c/sdl_stub'), '-I', str(ADDON / 'src/core'),
                        str(ADDON / 'tests/c/files_main.c'), str(ADDON / 'src/core/tmst_files.c'),
                        '-o', str(cls.exe)], check=True)

    @classmethod
    def tearDownClass(cls):
        cls.tmp.cleanup()

    def setUp(self):
        self.dir = Path(tempfile.mkdtemp(dir=self.tmp.name))

    def found(self, game: Path) -> str:
        out = subprocess.run([str(self.exe), REL], capture_output=True, text=True, check=True,
                             env=dict(os.environ, TMST_TEST_BASE=str(game) + os.sep)).stdout
        return out.splitlines()[2]

    def pc(self):
        """The player's install: the Expanded's folder with game.toml, the game in build-release."""
        root = self.dir / 'expanded'
        touch(root / 'game.toml')
        game = root / 'build-release'
        game.mkdir(parents=True)
        return root, game

    def test_addon_folder_first(self):
        root, game = self.pc()
        own = touch(root / 'addons/tmst' / REL)
        touch(game / 'mods/tmst-data' / REL)
        self.assertTrue(same(self.found(game), own))

    def test_staged_copy_without_the_addon_folder(self):
        root, game = self.pc()
        staged = touch(game / 'mods/tmst-data' / REL)
        self.assertTrue(same(self.found(game), staged))

    def test_neither_gives_the_addon_path(self):
        root, game = self.pc()
        self.assertTrue(same(self.found(game), root / 'addons/tmst' / REL))

    def test_android_data_folder(self):
        """The APK's data folder: game.toml and mods/ in the game's own folder."""
        game = self.dir / 'data'
        touch(game / 'game.toml')
        staged = touch(game / 'mods/tmst-data' / REL)
        self.assertTrue(same(self.found(game), staged))


if __name__ == '__main__':
    unittest.main()
