"""setup/tmst_setup.py without installing anything: the pinned numpy it takes
from the Expanded's requirements, the steps' order."""
import sys, tempfile, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'setup'))
import tmst_setup  # noqa: E402

REQUIREMENTS = '''# Written by tools/update_requirements.py
pillow==12.0.0 ; python_version < "3.15" \\
    --hash=sha256:aaaa
numpy==2.2.6 ; python_version < "3.11" \\
    --hash=sha256:bbbb \\
    --hash=sha256:cccc
numpy==2.3.5 ; python_version >= "3.11" \\
    --hash=sha256:dddd
numpyx==1.0 \\
    --hash=sha256:eeee
'''


class SetupTest(unittest.TestCase):
    def test_pinned_takes_every_version_with_its_hashes(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / 'requirements-import.txt'
            path.write_text(REQUIREMENTS, encoding='utf-8')
            text = tmst_setup.pinned('numpy', path)
        self.assertEqual(text.splitlines(), [
            'numpy==2.2.6 ; python_version < "3.11" \\', '    --hash=sha256:bbbb \\', '    --hash=sha256:cccc',
            'numpy==2.3.5 ; python_version >= "3.11" \\', '    --hash=sha256:dddd'])
        self.assertEqual(tmst_setup.pinned('numpy', Path(tmp) / 'gone.txt'), '')

    def test_the_expanded_pins_numpy(self):
        text = tmst_setup.pinned('numpy', tmst_setup.ROOT / 'tools/requirements-import.txt')
        if not (tmst_setup.ROOT / 'tools/requirements-import.txt').is_file():
            self.skipTest('no Expanded beside the add-on')
        self.assertIn('numpy==', text)
        self.assertIn('--hash=sha256:', text)
        self.assertIn('pillow==', tmst_setup.pinned('pillow', tmst_setup.ROOT / 'tools/requirements-import.txt'))

    def test_modules_first_then_the_disc_then_what_needs_it(self):
        names = [s.__name__ for s in tmst_setup.STEPS]
        self.assertEqual(names[:2], ['python_packages', 'tekken2_disc'])
        self.assertLess(names.index('tekken2_music'), names.index('jukebox_titles'))
        self.assertLess(names.index('tekken5dr_music'), names.index('jukebox_titles'))
        self.assertIn('tekken2_stages', names)
        self.assertLess(names.index('tekken2_stages'), names.index('tekken2_remake_pictures'))
        self.assertLess(names.index('tekken2_remake_pictures'), names.index('tekken2_remake_stages'))


if __name__ == '__main__':
    unittest.main()
