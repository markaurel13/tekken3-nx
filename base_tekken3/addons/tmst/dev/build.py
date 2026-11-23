"""Developer build of Tekken 3 Expanded with the TMST add-on.

    build.cmd                 the game you play (build-release), with the add-on
    build.cmd --without       the same, without the add-on (the plain Expanded)
    build.cmd <ref>           any tag, branch or commit of the Expanded, in a
                              worktree of its own: dev/build/<ref>/ (compat check)

It builds exactly as the Expanded's setup does (launcher/setup_backend.py:
build_game), with one argument more: -DCMAKE_PROJECT_INCLUDE=<build>/tmst_include.cmake,
a stub written there that includes dev/tmst_inject.cmake while the add-on is
in place (STUB). The Expanded's tracked files are never touched. The setup must have run once
(generated/ holds the game made from the player's disc).

An Expanded with its own add-on hook (1.5.0 on, has_hook) builds the add-on
in by itself (dev/tmst_hook.cmake): then there is no stub, CMAKE_PROJECT_INCLUDE
is cleared and --without is the cache's TMST_WITHOUT (addon.cmake).
"""
from __future__ import annotations
import argparse, json, os, re, shutil, subprocess, sys
from pathlib import Path

DEV = Path(__file__).resolve().parent
ADDON = DEV.parent
ROOT = ADDON.parents[1]                    # <expanded>/addons/tmst/dev
INJECT = DEV / 'tmst_inject.cmake'

# The build's CMAKE_PROJECT_INCLUDE (the cache keeps it, so the Expanded's own
# Update & play configures with it too). It lives in the build folder, not in
# the add-on's: once addons/tmst is deleted without build.cmd --without, it
# deletes the objects whose wrapper dev/tmst_chain.py renamed (they compile
# again as the Expanded wrote them) and the game is built without the add-on.
STUB = '''# Written by addons/tmst/dev/build.py (the TMST add-on): includes the add-on
# while it is in place; once its folder is gone, the Expanded builds as its own.
if(NOT CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    return()
endif()
if(EXISTS "@INJECT@")
    include("@INJECT@")
elseif(NOT DEFINED TMST_GONE)
    set(TMST_GONE 1)
    set(_tmst_state "${CMAKE_BINARY_DIR}/tmst_chain_state.json")
    if(EXISTS "${_tmst_state}")
        # {"symbol": ["<object>", ...]}: every quoted object path in it.
        file(READ "${_tmst_state}" _tmst_json)
        string(REGEX MATCHALL "\\"[^\\"]+\\\\.o(bj)?\\"" _tmst_objs "${_tmst_json}")
        foreach(_tmst_obj IN LISTS _tmst_objs)
            string(REPLACE "\\"" "" _tmst_obj "${_tmst_obj}")
            string(REPLACE "\\\\\\\\" "/" _tmst_obj "${_tmst_obj}")
            file(TO_CMAKE_PATH "${_tmst_obj}" _tmst_obj)
            file(REMOVE "${_tmst_obj}")
            message(STATUS "TMST: ${_tmst_obj} back to the Expanded's own")
        endforeach()
        file(REMOVE "${_tmst_state}")
    endif()
    message(STATUS "TMST: the add-on's folder is gone; the Expanded is built without it")
endif()
'''


def write_stub(build: Path) -> Path:
    build.mkdir(parents=True, exist_ok=True)
    stub = build / 'tmst_include.cmake'
    text = STUB.replace('@INJECT@', INJECT.as_posix())
    if not stub.is_file() or stub.read_text(encoding='utf-8') != text:
        stub.write_text(text, encoding='utf-8')
    return stub


def fail(message: str) -> None:
    sys.exit(f'TMST build: {message}')


def expanded_setup():
    """The Expanded's setup module, for its toolchain and its mods/ keeping.
    Internal to the Expanded (docs/COMPAT.md): checked, never edited."""
    sys.path.insert(0, str(ROOT / 'launcher'))
    try:
        import setup_backend
    except Exception as error:   # noqa: BLE001 - any import failure means: run their setup
        fail(f'cannot load the Expanded setup ({error}); run Setup Tekken 3 first')
    for name in ('tools', 'kept_mods'):
        if not hasattr(setup_backend, name):
            fail(f'this Expanded setup has no {name}() (docs/COMPAT.md)')
    return setup_backend


def environment(setup):
    # Windows: the pinned compilers (downloaded when missing). macOS, Linux:
    # None, after checking that cmake, ninja and a compiler are installed.
    try:
        toolchain = setup.tools()
    except Exception as error:   # noqa: BLE001 - their SetupError says what to install
        fail(str(error))
    env = dict(os.environ)
    env.update(PYTHONUTF8='1', PYTHONNOUSERSITE='1',
               CMAKE_BUILD_PARALLEL_LEVEL=str(min(8, max(2, os.cpu_count() or 2))))
    if toolchain:
        env['PATH'] = str(toolchain / 'bin') + os.pathsep + env.get('PATH', '')
        env.update(PSXRECOMP_TOOLCHAIN_DIR=str(toolchain), RETCOMM_TOOLCHAIN_DIR=str(toolchain))
    return toolchain, env


def has_hook(source: Path) -> bool:
    """The Expanded at source has its own add-on hook (TEKKEN3_ADDON_API,
    its addons/README.md): it includes addons/<id>/addon.cmake by itself."""
    try:
        return 'TEKKEN3_ADDON_API' in (source / 'CMakeLists.txt').read_text(encoding='utf-8', errors='replace')
    except OSError:
        return False


def configure_args(toolchain, source: Path, build: Path, with_addon: bool, emulate: bool = False) -> list[str]:
    cmake = str(toolchain / 'bin/cmake.exe') if toolchain else (shutil.which('cmake') or 'cmake')
    args = [cmake, '-S', str(source), '-B', str(build), '-G', 'Ninja', '-DCMAKE_BUILD_TYPE=Release',
            '-DCMAKE_SUPPRESS_REGENERATION=ON', '-DPython3_EXECUTABLE=' + sys.executable]
    if toolchain:
        args += ['-DCMAKE_C_COMPILER=' + str(toolchain / 'bin/clang.exe'),
                 '-DCMAKE_CXX_COMPILER=' + str(toolchain / 'bin/clang++.exe'),
                 '-DCMAKE_MAKE_PROGRAM=' + str(toolchain / 'bin/ninja.exe'), '-DPSX_STATIC_RUNTIME=ON']
    args += ['-DPSX_DEBUG_TOOLS=OFF', '-DPSX_DEBUG_SERVER_LITE=OFF', '-DPSX_NETPLAY=OFF',
             '-DPSXRECOMP_BIOS_STEMS=OpenBIOS', '-DPSXRECOMP_FORCE_SETUP_HOST=OFF',
             '-DPSXRECOMP_REQUIRE_GAME_C=ON', '-DTEKKEN3_BUILD_PC_PORT=OFF']
    # The TTT1 characters as the player's setup chose them (kept in the cache).
    cache = build / 'CMakeCache.txt'
    ttt1 = 'ON'
    if cache.is_file():
        m = re.search(r'^TEKKEN3_TTT1_CHARACTERS:BOOL=(\w+)', cache.read_text(errors='replace'), re.M)
        if m: ttt1 = m.group(1)
    args.append('-DTEKKEN3_TTT1_CHARACTERS=' + ttt1)
    # --emulate-wraps: the add-on's wraps as macOS builds them (dev/tmst_wraps.cmake).
    args.append('-DTMST_EMULATE_WRAPS=' + ('ON' if emulate else 'OFF'))
    # The cache keeps CMAKE_PROJECT_INCLUDE: set or clear it every time, so that
    # the Expanded's own Update & play builds what was built last.
    if has_hook(source):
        # The hook builds the add-on in (an older cache's stub cleared).
        args += ['-DCMAKE_PROJECT_INCLUDE=', '-DTMST_WITHOUT=' + ('OFF' if with_addon else 'ON')]
    else:
        args.append('-DCMAKE_PROJECT_INCLUDE=' + (write_stub(build).as_posix() if with_addon else ''))
    return args


def chains_listed() -> set[str]:
    text = (ADDON / 'addon.cmake').read_text(encoding='utf-8')
    m = re.search(r'set\(\s*TMST_CHAINS\b(.*?)\)', text, re.S)
    body = re.sub(r'#.*', '', m.group(1)) if m else ''
    return {t.strip('"').split('|')[0] for t in body.split()}


def forget_chains(build: Path, keep: set[str]) -> None:
    """Objects whose wrapper dev/tmst_chain.py renamed, for symbols no longer
    chained (all of them without the add-on): deleted, so that Ninja compiles
    them again as the Expanded wrote them."""
    state_file = build / 'tmst_chain_state.json'
    try: state = json.loads(state_file.read_text(encoding='utf-8'))
    except (OSError, ValueError): return
    for sym in [s for s in state if s not in keep]:
        for obj in state.pop(sym):
            Path(obj).unlink(missing_ok=True)
            print(f'TMST build: {Path(obj).name} back to the Expanded\'s own (no chain on {sym})')
    state_file.write_text(json.dumps(state, indent=1), encoding='utf-8')


def run(args, env):
    print('>', ' '.join(str(a) for a in args), flush=True)
    if subprocess.run(args, env=env).returncode:
        fail('the build stopped (see above)')


def worktree(ref: str) -> tuple[Path, Path]:
    """A detached worktree of the Expanded at ref, with the player's generated
    game (and workspace, if kept) linked in: git never sees those folders."""
    safe = re.sub(r'[^A-Za-z0-9._-]', '_', ref)
    base = DEV / 'build' / safe
    source, build = base / 'src', base / 'out'
    if not (ROOT / 'generated').is_dir():
        fail('no generated/ in the Expanded folder: run Setup Tekken 3 first')
    subprocess.run(['git', '-C', str(ROOT), 'fetch', '--tags', '--quiet'])
    if source.is_dir():
        run(['git', '-C', str(source), 'checkout', '--detach', '--quiet', ref], os.environ)
    else:
        base.mkdir(parents=True, exist_ok=True)
        run(['git', '-C', str(ROOT), 'worktree', 'add', '--detach', str(source), ref], os.environ)
    for folder in GENERATED_FOLDERS:
        target, link = ROOT / folder, source / folder
        if target.is_dir() and not link.exists():
            if os.name == 'nt':
                subprocess.run(['cmd', '/c', 'mklink', '/J', str(link), str(target)], check=True,
                               stdout=subprocess.DEVNULL)
            else:
                link.symlink_to(target, target_is_directory=True)
    for file in GENERATED_FILES:
        if (ROOT / file).is_file():
            shutil.copy2(ROOT / file, source / file)
    # A ref with the add-on hook finds add-ons in its own addons/: this one
    # copied there (its sources and lists; not dev/, workspace/ or .git).
    if has_hook(source):
        copy_addon(source / 'addons' / ADDON.name)
    return source, build


def copy_addon(target: Path) -> None:
    """This add-on in another ref's addons/, for that ref's hook: a copy, never
    a link (a link would put dev/build inside itself). workspace/ (what the
    setup made, gigabytes) stays an empty folder: a compat build stages no
    Tekken 2 stages."""
    skip = shutil.ignore_patterns('.git', 'build', 'workspace', '__pycache__', 'screenshots', 'tests')
    if target.exists():
        shutil.rmtree(target)
    shutil.copytree(ADDON, target, ignore=skip)
    (target / 'workspace').mkdir(exist_ok=True)


# What the Expanded's setup makes beside its tracked files and its build reads
# (git status --ignored): linked or copied into another ref's worktree.
GENERATED_FOLDERS = ('generated', 'psxrecomp/generated', 'workspace', 'disc')
GENERATED_FILES = ('psxrecomp/runtime/include/overlay_codegen_hash.h',)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('ref', nargs='?', help='a tag, branch or commit of the Expanded (default: the game you play)')
    parser.add_argument('--without', action='store_true', help='build without the add-on')
    parser.add_argument('--emulate-wraps', action='store_true',
                        help='the wraps without -Wl,--wrap, as on macOS (to try that emulation here)')
    parser.add_argument('--debug', action='store_true',
                        help='a debug build of the installed Expanded (debug server: screenshots, input) in dev/build/debug')
    a = parser.parse_args()

    setup = expanded_setup()
    toolchain, env = environment(setup)
    with_addon = not a.without
    # With the hook, nothing is renamed after compiling: every object
    # dev/tmst_chain.py renamed in an older build compiles again.
    keep = chains_listed() if with_addon and not has_hook(ROOT) else set()
    if a.debug:
        build = DEV / 'build' / 'debug'
        forget_chains(build, keep)
        args = configure_args(toolchain, ROOT, build, with_addon, a.emulate_wraps)
        args = [x for x in args if not x.startswith('-DPSX_DEBUG_TOOLS=')] + ['-DPSX_DEBUG_TOOLS=ON']
        run(args, env)
        run([args[0], '--build', str(build), '--target', 'psx-runtime', '--parallel', env['CMAKE_BUILD_PARALLEL_LEVEL']], env)
        print(f'TMST build: debug build in {build}')
        return
    if a.ref:
        source, build = worktree(a.ref)
        forget_chains(build, keep if not has_hook(source) else set())
        run(configure_args(toolchain, source, build, with_addon, a.emulate_wraps), env)
        run([configure_args(toolchain, source, build, with_addon, a.emulate_wraps)[0], '--build', str(build),
             '--target', 'psx-runtime', '--parallel', env['CMAKE_BUILD_PARALLEL_LEVEL']], env)
        print(f'TMST build: {a.ref} built in {build}')
        return
    build = ROOT / 'build-release'
    if not (build / 'CMakeCache.txt').is_file():
        fail('no build-release yet: run Setup Tekken 3 first')
    # The framework empties build-release/mods at every link: the Expanded's
    # setup keeps the player's mod settings and data aside meanwhile.
    forget_chains(build, keep)
    was_ready = expanded_ready(setup)
    with setup.kept_mods():
        run(configure_args(toolchain, ROOT, build, with_addon, a.emulate_wraps), env)
        run([configure_args(toolchain, ROOT, build, with_addon, a.emulate_wraps)[0], '--build', str(build),
             '--target', 'psx-runtime', '--parallel', env['CMAKE_BUILD_PARALLEL_LEVEL']], env)
    if was_ready:
        keep_ready(setup)
    print('TMST build: build-release ' + ('with' if with_addon else 'WITHOUT') + ' the add-on')


def expanded_ready(setup) -> bool:
    """The Expanded's setup is done but for the exe's digest (an earlier
    build of ours, or none yet): its ready() with the digest it expects."""
    digest = setup.digest
    try:
        expected = setup.load_json(setup.STATE / 'ready.json').get('exe_sha256')
        setup.digest = lambda path: expected
        return bool(expected) and bool(setup.ready())
    except Exception:   # noqa: BLE001 - another Expanded setup: its own launcher decides
        return False
    finally:
        setup.digest = digest


def keep_ready(setup) -> None:
    """The Expanded's setup is done when the game's exe is the one it built
    (.setup/ready.json exe_sha256): the exe built here takes its place, or its
    Tekken 3 Expanded.exe would ask for an update (and build once more)."""
    try:
        state = setup.load_json(setup.STATE / 'ready.json')
        state['exe_sha256'] = setup.digest(setup.EXE)
        setup.save_json(setup.STATE / 'ready.json', state)
    except Exception as error:   # noqa: BLE001 - the game still plays; its launcher rebuilds once
        print(f'TMST build: the Expanded setup state is not updated ({error})')


if __name__ == '__main__':
    main()
