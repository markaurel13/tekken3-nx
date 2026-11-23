"""Disc-free tests of the add-on's Python side (CI)."""
import importlib.util, sys, unittest
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]


def load(rel: str):
    spec = importlib.util.spec_from_file_location(Path(rel).stem, ADDON / rel)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


class CheckRepo(unittest.TestCase):
    def test_cmake_list(self):
        check = load('tools/check_repo.py')
        text = 'set(TMST_SOURCES\n    a.c  # note\n    src/b.c\n)\nset(TMST_WRAPS\n)'
        self.assertEqual(check.cmake_list(text, 'TMST_SOURCES'), ['a.c', 'src/b.c'])
        self.assertEqual(check.cmake_list(text, 'TMST_WRAPS'), [])

    def test_repo_is_clean(self):
        check = load('tools/check_repo.py')
        self.assertEqual(check.main(), 0)


if __name__ == '__main__':
    unittest.main()
