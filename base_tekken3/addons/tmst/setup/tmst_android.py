"""The Android APK of Tekken 3 Expanded with the TMST add-on (Build Android
APK (TMST).cmd / .command): the Expanded's own APK build
(tools/android/make_apk.py), with the add-on built in as on PC.

    tmst_android.py               the APK (Tekken3Expanded.apk beside the Expanded's files), with
                                  the Jukebox's Tekken 1, 2, TTT and Tekken 5 DR music (about 800 MB)
    tmst_android.py --no-music    without that music (the Jukebox: the Tekken 3 disc's music only)
    tmst_android.py --install     then installs it on the phone connected by USB (adb)
    tmst_android.py --without     the APK without the add-on (the plain Expanded's)

Run after the Expanded's own setup (Setup Tekken 3, or Build Android APK for
the Android game alone) and after Setup TMST on the PC game: the add-on's
setup steps run again first (nothing is made twice; a Tekken 2 disc it knows
is used), then the APK is built. What the game reads from addons/tmst goes in
the APK at the same place (the Tournament's and Photo Mode's pictures, the
Jukebox's lists and, unless --no-music, its music); the add-on's mods and
the Tekken 2 stages are staged by the build itself (dev/tmst_hook.cmake with
the Expanded's add-on hook, 1.5.0 on; dev/tmst_inject.cmake before it).

The add-on is kept in build-android's CMake cache (CMAKE_PROJECT_INCLUDE before
the hook, TMST_WITHOUT=OFF with it), so the Expanded's own Build Android APK
also builds it in afterwards, as its Update & play does on PC; --without takes
it out again.
"""
from __future__ import annotations
import argparse, os, subprocess, sys
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
ROOT = ADDON.parents[1]
BUILD = ROOT / 'build-android'
sys.path.insert(0, str(ROOT / 'tools/android'))
sys.path.insert(0, str(ROOT / 'launcher'))

# What the game reads from addons/tmst, as "here|in the APK's data" (the
# runtime's folder on the phone is the data folder, with game.toml in it).
# The Tournament's backgrounds and stories, Photo Mode's logo and the Jukebox's
# title lists are staged by the build under mods/tmst-data (addon.cmake
# TMST_STAGED), so they are in the APK already, as in the Expanded's own.
DATA = (
    'workspace/jukebox-defaults.txt',
    'workspace/tournament',            # the portraits the game made (a cache)
)
MUSIC = (
    'workspace/t2-music',              # TEKKEN 1 and 2 (Tekken 2 Content's Music)
    'workspace/ttt-music',             # TEKKEN TAG TOURNAMENT
    'workspace/t5dr-music',            # TEKKEN 5 DR
)


def say(message: str, detail: str = '') -> None:
    print(f'TMST Android: {message}' + (f'  {detail}' if detail else ''), flush=True)


def data_args(music: bool) -> list[str]:
    args = []
    for name in DATA + (MUSIC if music else ()):
        source = ADDON / name
        if source.exists():
            args += ['--data', f'{source}=addons/tmst/{name}']
        elif name in MUSIC:
            say(f'{name}: not made (Setup TMST makes it), its Jukebox category stays empty')
    return args


def addon_setup() -> None:
    """The add-on's own steps (setup/tmst_setup.py --no-build): current ones are skipped."""
    code = subprocess.call([sys.executable, '-u', str(ADDON / 'setup/tmst_setup.py'), '--no-build'], cwd=ADDON)
    if code:
        say('the add-on\'s setup stopped (see above); the APK is built with what is there')


def main() -> int:
    a = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    a.add_argument('--no-music', dest='music', action='store_false',
                   help="without the Jukebox's Tekken 1, 2, TTT and Tekken 5 DR music (about 800 MB)")
    # --music: the music was left out by default before (as PCM, about 2 GB); kept for old commands.
    a.add_argument('--music', dest='music', action='store_true', help=argparse.SUPPRESS)
    a.add_argument('--install', action='store_true', help='install it on the phone connected by USB (adb)')
    a.add_argument('--without', action='store_true', help='the APK without the add-on')
    a.add_argument('--out', type=Path, help='the APK file (default: the Expanded\'s Tekken3Expanded.apk)')
    args = a.parse_args()

    import make_apk
    import importlib.util               # dev/build.py: the include stub, the chains' objects
    spec = importlib.util.spec_from_file_location('tmst_dev_build', ADDON / 'dev/build.py')
    tmst_build = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(tmst_build)
    try:
        make_apk.up_to_date()           # never an APK of an older install: the Expanded's update first
        if not args.without:
            addon_setup()
        hook = tmst_build.has_hook(ROOT)
        tmst_build.forget_chains(BUILD, set() if args.without or hook else tmst_build.chains_listed())
        if hook:    # the Expanded's add-on hook builds the add-on in (addon.cmake)
            defines = ['CMAKE_PROJECT_INCLUDE=', 'TMST_WITHOUT=' + ('ON' if args.without else 'OFF')]
        else:
            defines = ['CMAKE_PROJECT_INCLUDE=' + ('' if args.without else tmst_build.write_stub(BUILD).as_posix())]
        extra = [] if args.without else data_args(args.music)

        def run(command, env=None):
            # build_apk.py's command: the add-on's data beside the game's.
            if any(str(c).endswith('build_apk.py') for c in command):
                command = list(command) + extra
            make_apk.run_command(command, env)

        apk = make_apk.build(out=(args.out or make_apk.DEFAULT_OUT).resolve(), run=run,
                             defines=defines)
        say(f'APK ready: {apk} ({apk.stat().st_size / 1e6:.0f} MB), '
            + ('without the add-on' if args.without
               else 'with the add-on' + (' and its music' if args.music else ', without its music')))
        if args.install:
            say('installing on the phone (USB debugging must be on)')
            make_apk.run_command([make_apk.adb(), 'install', '-r', apk])
            say('installed: open Tekken 3 Expanded on the phone')
        else:
            print(make_apk.install_hint(apk))
        return 0
    except make_apk.Stop as stop:
        say(str(stop))
        return 1


if __name__ == '__main__':
    os.environ.setdefault('PYTHONUTF8', '1')
    sys.exit(main())
