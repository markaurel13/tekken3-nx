"""Does an Expanded checkout still have what the TMST add-on builds on?
No disc needed: it reads the Expanded's sources only (CI, check_upstream.cmd).

    python tools/check_expanded.py <expanded folder>

Every item here is listed in docs/COMPAT.md; a failing one points at the
place in src/compat/ or dev/ to fix.
"""
from __future__ import annotations
import re, sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 2:
        print(__doc__); return 2
    root = Path(sys.argv[1])
    def text(rel: str) -> str:
        p = root / rel
        return p.read_text(encoding='utf-8', errors='replace') if p.is_file() else ''

    cmake = text('CMakeLists.txt') + text('psxrecomp/runtime/runtime.cmake')
    api = text('psxrecomp/runtime/include/mod_plugins.h')
    setup = text('launcher/setup_backend.py')
    checks = [
        # (what, ok, where to fix)
        ('game.toml at the root (the shim\'s marker)', (root / 'game.toml').is_file(), 'dev/tmst_inject.cmake'),
        ('target psx-runtime', 'psx-runtime' in cmake, 'dev/tmst_inject.cmake'),
        ('tekken3_wrap_symbol() helper', 'function(tekken3_wrap_symbol' in cmake, 'dev/tmst_inject.cmake (falls back to -Wl,--wrap)'),
        ('packages staged from mods/preloaded/packages to mods/packages', 'mods/packages' in cmake, 'dev/tmst_inject.cmake'),
        ('activation plugins', 'psx_mod_register_activation_plugin(' in api, 'src/compat/tmst_features.cmake'),
        ('VBlank plugins', 'psx_mod_register_vblank_plugin(' in api, 'src/compat/tmst_features.cmake'),
        ('PSX_MOD_CONSTRUCTOR', 'PSX_MOD_CONSTRUCTOR' in api, 'src/'),
        ('setup: tools()', re.search(r'^def tools\(', setup, re.M) is not None, 'dev/build.py'),
        ('setup: kept_mods', re.search(r'^class kept_mods\b', setup, re.M) is not None, 'dev/build.py'),
        ('setup: build_game configure arguments', '-DCMAKE_SUPPRESS_REGENERATION=ON' in setup, 'dev/build.py configure_args'),
    ]
    bad = 0
    for what, ok, fix in checks:
        print(('ok    ' if ok else 'FAIL  ') + what + ('' if ok else f'  -> {fix}'))
        bad += not ok
    print('check_expanded:', 'OK' if not bad else f'{bad} problem(s)')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
