"""The TMST add-on's setup: runs after the Expanded's (never inside it) and
makes, on the player's machine and from the player's own discs, what the
add-on's mods need. Nothing it makes is ever committed.

Each step is a function in STEPS: it checks whether its output is current
(a version stamp under addons/tmst/workspace/) and does its work only when not.
Steps: the Python modules the tools need (numpy, Pillow), the Tekken 2 disc
(checked and remembered), the Jukebox's Tekken 2 (and Tekken 1) music from it,
the Tekken 2 stages from it (the Classic's packs, the Remake's pictures and packs), the
Jukebox's TTT console and arcade music, then its title lists, then Tournament
Mode's portraits of the stock fighters from the Tekken 3 disc. Then the game is
built again with the add-on (dev/build.py).

It always runs with the Expanded's own Python (.setup/venv, which its setup
gives numpy): started with another one, it starts itself again with that one.

    tmst_setup.py                    the steps, asking nothing, then the build
    tmst_setup.py --interactive      asks for the Tekken 2 disc in a window when
                                     none is known (without setup/tmst_gui.py)
    tmst_setup.py --t2-disc <file>   that Tekken 2 disc (.cue, .bin or .chd)
    tmst_setup.py --no-t2-disc       no Tekken 2 disc (forgets the one known)
    tmst_setup.py --no-build         the steps only (setup/tmst_gui.py builds on
                                     its own, with its progress; the Expanded's hook)
"""
from __future__ import annotations
import argparse, json, os, subprocess, sys
from pathlib import Path
import tmst_platform as platform

ADDON = Path(__file__).resolve().parents[1]
ROOT = ADDON.parents[1]                     # <expanded>/addons/tmst/setup
# The add-on's own data: in its folder, which the Expanded's setup (its
# free-space step deletes workspace/) never touches.
STATE = ADDON / 'workspace'
OPTIONS = argparse.Namespace(interactive=False, t2_disc=None, no_t2_disc=False, no_build=False)
# Python modules the setup installs itself when the Python running it has
# none (pip install --target): the tools' processes find them on sys.path.
PACKAGES = STATE / 'python'
VENV = ROOT / '.setup/venv'


class SetupFailed(Exception):
    pass


def log(message: str) -> None:
    print(f'TMST setup: {message}', flush=True)


def stamp(name: str) -> dict:
    try:
        return json.loads((STATE / f'{name}.json').read_text(encoding='utf-8'))
    except (OSError, ValueError):
        return {}


def save_stamp(name: str, data: dict) -> None:
    STATE.mkdir(parents=True, exist_ok=True)
    (STATE / f'{name}.json').write_text(json.dumps(data, indent=1), encoding='utf-8')


def music_pack_step(music, name: str, label: str, source: str, convert) -> None:
    """A Jukebox music folder made from one of the Expanded's music packs (and,
    for a tool with find_disc, the disc the Expanded's setup was given): again
    when one of them or the tool's VERSION changed or a file is gone; nothing
    without any."""
    pack = music.find_pack(ROOT)
    disc = music.find_disc(ROOT) if hasattr(music, 'find_disc') else None
    if not pack and not disc:
        log(f'{label}: no pack (the Expanded setup makes it from {source}), skipped')
        return
    want = {'version': music.VERSION}
    if hasattr(music, 'skipped'):                  # the pieces the title lists leave out
        want['skip'] = music.skipped()
    for key, path in (('pack', pack), ('disc', disc)):
        if path:
            st = path.stat()
            want[key] = {'path': str(path), 'size': st.st_size, 'mtime': int(st.st_mtime)}
    have = stamp(name)
    out = ROOT / music.OUT
    if have.get('stamp') == want and all((out / n).is_file() for n in have.get('files', [])):
        return
    log(f'{label}: making the pieces')
    names = convert(pack, out, disc)
    if names is not None:
        save_stamp(name, {'stamp': want, 'files': names})


def file_stamp(path: Path) -> dict:
    st = path.stat()
    return {'size': st.st_size, 'mtime': int(st.st_mtime)}


def expanded_source_folders() -> list[Path]:
    """The folders of the files the Expanded's setup was last given (its
    .setup/last-inputs.json), then the Expanded's own folder."""
    try:
        inputs = json.loads((ROOT / '.setup/last-inputs.json').read_text(encoding='utf-8'))
    except (OSError, ValueError):
        inputs = {}
    folders = []
    for value in inputs.values():
        if isinstance(value, str) and value:
            folder = Path(value).parent
            if folder not in folders:
                folders.append(folder)
    return folders + [ROOT]


def ask_t2_disc() -> Path | None:
    """The Tekken 2 disc chosen in a file window; None when cancelled (or no window)."""
    try:
        import tkinter
        from tkinter import filedialog
        window = tkinter.Tk()
        window.withdraw()
        window.attributes('-topmost', True)
        chosen = filedialog.askopenfilename(
            parent=window, title='Tekken 2 disc (optional): Cancel to go on without it',
            filetypes=[('Tekken 2 disc image', '*.cue *.bin *.chd'), ('All files', '*.*')])
        window.destroy()
    except Exception as error:   # noqa: BLE001 - no Tk, no display: go on without asking
        log(f'Tekken 2 disc: cannot open a window ({error})')
        return None
    return Path(chosen) if chosen else None


def venv_python() -> Path:
    return VENV / ('Scripts/python.exe' if os.name == 'nt' else 'bin/python')


def in_venv() -> bool:
    try:
        return Path(sys.prefix).resolve() == VENV.resolve()
    except OSError:
        return False


def pinned(name: str, requirements: Path) -> str:
    """The lines of a module in a pip requirements file with hashes (the
    Expanded's tools/requirements-import.txt): each of its versions (the
    markers choose) and their hashes; '' when it has none."""
    try:
        lines = requirements.read_text(encoding='utf-8').splitlines()
    except OSError:
        return ''
    out, inside = [], False
    for line in lines:
        if not line.startswith((' ', '\t')):
            inside = line.split('==')[0].strip().lower() == name.lower()
        if inside:
            out.append(line)
    return '\n'.join(out) + '\n' if out else ''


def python_packages() -> None:
    """numpy and Pillow, for tools/tekken2_music.py, tekken2_stages.py and
    t2_remake_stages.py: the Expanded's setup puts them in its .setup/venv,
    which runs this. For those it has not, the Expanded's own pinned versions
    (tools/requirements-import.txt, by their hashes, wheels only) into
    workspace/python; none of the Expanded's files change. Without them the
    Tekken 2 music and stages are left out (their steps say so)."""
    import importlib, importlib.util
    missing = [(module, name) for module, name in (('numpy', 'numpy'), ('PIL', 'pillow'))
               if importlib.util.find_spec(module) is None]
    if not missing:
        return
    names = ', '.join(name for _, name in missing)
    requirement = ''.join(pinned(name, ROOT / 'tools/requirements-import.txt') for _, name in missing)
    if not requirement:
        log(f'{names}: missing, and the Expanded pins no version of them; the Tekken 2 music and stages are left out')
        return
    STATE.mkdir(parents=True, exist_ok=True)
    listing = STATE / 'python-requirements.txt'
    listing.write_text(requirement, encoding='utf-8')
    log(f'{names}: installing the Expanded’s versions into {PACKAGES}')
    result = subprocess.run([sys.executable, '-m', 'pip', 'install', '--disable-pip-version-check', '--no-input',
                             '--require-hashes', '--only-binary=:all:', '--upgrade', '--target', str(PACKAGES),
                             '-r', str(listing)])
    listing.unlink(missing_ok=True)
    if result.returncode:
        log(f'{names}: could not be installed; the Tekken 2 music and stages are left out')
        return
    if str(PACKAGES) not in sys.path:
        sys.path.insert(0, str(PACKAGES))
    importlib.invalidate_caches()
    for module, name in missing:
        try:
            loaded = importlib.import_module(module)
            log(f'{name}: {getattr(loaded, "__version__", "installed")}')
        except ImportError as error:
            log(f'{name}: installed but cannot be loaded ({error}); the Tekken 2 music and stages are left out')


def build() -> int:
    """The game built again with the add-on (dev/build.py, as Setup TMST does)."""
    log('building the game with the add-on (dev/build.py)')
    return subprocess.call([sys.executable, '-u', str(ADDON / 'dev/build.py')], cwd=ADDON)


def tekken2_disc() -> None:
    """The player's Tekken 2 disc (tools/tekken2_disc.py), checked and kept in
    workspace/tekken2-disc.json for the steps after it. Optional: without it
    the setup goes on; a disc given on the command line that is not Tekken 2
    stops it."""
    sys.path.insert(0, str(ADDON / 'tools'))
    import tekken2_disc as t2
    from disc_image import DiscError
    name = 'tekken2-disc'

    def keep(info: dict, how: str) -> None:
        save_stamp(name, {'version': t2.VERSION, 'disc': info, 'file': file_stamp(Path(info['path']))})
        log(f'Tekken 2 disc: {info["region"]} ({info["serial"]}), {how}: {info["path"]}')

    if OPTIONS.no_t2_disc:
        save_stamp(name, {'version': t2.VERSION, 'declined': True})
        log('Tekken 2 disc: none (as asked)')
        return
    if OPTIONS.t2_disc:
        try:
            keep(t2.identify(OPTIONS.t2_disc), 'given')
        except (DiscError, OSError) as error:
            raise SetupFailed(f'Tekken 2 disc: {error}') from None
        return
    have = stamp(name)
    known = have.get('disc')
    if known:
        path = Path(known['path'])
        try:
            if have.get('version') == t2.VERSION and have.get('file') == file_stamp(path):
                log(f'Tekken 2 disc: {known["region"]} ({known["serial"]}): {path}')
                return
            keep(t2.identify(path), 'checked again')
            return
        except (DiscError, OSError) as error:
            log(f'Tekken 2 disc: the one known cannot be used any more ({error})')
    elif have.get('declined') and not OPTIONS.interactive:
        return
    for candidate in t2.search(expanded_source_folders()):
        try:
            keep(t2.identify(candidate), 'found')
            return
        except (DiscError, OSError) as error:
            log(f'Tekken 2 disc: {candidate.name} not used ({error})')
    if have.get('declined') or not OPTIONS.interactive:
        log(f'Tekken 2 disc: none (optional; {platform.script("Setup TMST")} asks for it{", or drop the disc on it" if platform.WINDOWS else ""})')
        return
    log('Tekken 2 disc: choose it in the window (optional: Cancel goes on without it)')
    while True:
        chosen = ask_t2_disc()
        if not chosen:
            save_stamp(name, {'version': t2.VERSION, 'declined': True})
            log(f'Tekken 2 disc: none (run {platform.script("Setup TMST")} again to add it later)')
            return
        try:
            keep(t2.identify(chosen), 'chosen')
            return
        except (DiscError, OSError) as error:
            log(f'Tekken 2 disc: {error}')


def tekken2_music() -> None:
    """The Jukebox's Tekken 2 (and with it Tekken 1) music (tools/tekken2_music.py),
    from the Tekken 2 disc the step before kept: again when the disc or the
    tool's VERSION changed or a file is gone; nothing without a disc."""
    sys.path.insert(0, str(ADDON / 'tools'))
    from disc_image import DiscError
    try:
        import tekken2_music as music
    except ImportError as error:                    # numpy: the Expanded's setup installs it
        log(f'Tekken 2 music: {error}, skipped')
        return
    disc = music.find_disc(STATE)
    if not disc:
        log('Tekken 2 music: no Tekken 2 disc, skipped')
        return
    want = {'version': music.VERSION, 'disc': {'path': str(disc), **file_stamp(disc)}, 'skip': music.skipped()}
    have = stamp('tekken2-music')
    out = ROOT / music.OUT
    if have.get('stamp') == want and all((out / n).is_file() for n in have.get('files', [])):
        return
    log('Tekken 2 music: taking it from the disc (a minute or two)')
    try:
        names = music.convert(disc, out, say=log)
    except (DiscError, OSError) as error:
        log(f'Tekken 2 music: {error}; left out')
        return
    save_stamp('tekken2-music', {'stamp': want, 'files': names})


def tekken2_stages() -> None:
    """Tekken 2 Content's Stages, Classic (tools/tekken2_stages.py), from the
    Tekken 2 disc the first step kept: again when the disc or the tool's
    VERSION changed or a pack is gone; nothing without a disc. No pictures
    (the tool makes them when run by hand)."""
    sys.path.insert(0, str(ADDON / 'tools'))
    from disc_image import DiscError
    import tekken2_stages as stages
    disc = stages.find_disc(STATE)
    if not disc:
        log('Tekken 2 stages: no Tekken 2 disc, skipped')
        return
    want = {'version': stages.VERSION, 'disc': {'path': str(disc), **file_stamp(disc)}}
    have = stamp('tekken2-stages')
    out = ROOT / stages.OUT
    if have.get('stamp') == want and all((out / n).is_file() for n in have.get('files', [])):
        return
    log('Tekken 2 stages: taking them from the disc (a minute or two)')
    try:
        names = stages.convert(disc, out, say=log, previews=False)
    except ImportError as error:                    # numpy (the floor's halves): python_packages
        log(f'Tekken 2 stages: {error}; left out')
        return
    except (DiscError, OSError, ValueError) as error:
        log(f'Tekken 2 stages: {error}; left out')
        return
    save_stamp('tekken2-stages', {'stamp': want, 'files': names})


def tekken2_remake_pictures() -> None:
    """Tekken 2 Content's Stages, Remake: the user's backgrounds, from the
    patches in tools/data/t2-remake-patches (tools/t2_remake_patch.py),
    which only the Tekken 2 disc opens: again when the disc, the tool's
    VERSION or a patch changed or a picture is gone. A picture that does not
    open: that stage's Remake on Tekken 2's own panorama (said)."""
    sys.path.insert(0, str(ADDON / 'tools'))
    import hashlib
    from disc_image import DiscError
    import t2_remake_patch as patch
    import tekken2_stages as stages
    disc = stages.find_disc(STATE)
    if not disc:
        log('Tekken 2 Remake pictures: no Tekken 2 disc, skipped')
        return
    patches = hashlib.sha256()
    for f in patch.patch_files():
        patches.update(f.name.encode('utf-8') + f.read_bytes())
    want = {'version': patch.VERSION, 'patches': patches.hexdigest()[:16],
            'disc': {'path': str(disc), **file_stamp(disc)}}
    have = stamp('tekken2-remake-pictures')
    out = ROOT / stages.OUT
    if have.get('stamp') == want and all((out / n).is_file() for n in have.get('files', [])):
        return
    log('Tekken 2 Remake pictures: opening them with your Tekken 2 disc')
    try:
        names = patch.apply(disc, out, say=log)
    except ImportError as error:                    # numpy, Pillow: python_packages
        log(f'Tekken 2 Remake pictures: {error}; left out')
        return
    except (DiscError, OSError, ValueError) as error:
        log(f'Tekken 2 Remake pictures: {error}; left out')
        return
    if len(names) < len(patch.patch_files()):
        log(f'Tekken 2 Remake pictures: {len(names)} of {len(patch.patch_files())}; '
            'the others’ stages on Tekken 2’s own panorama')
    save_stamp('tekken2-remake-pictures', {'stamp': want, 'files': names})


def tekken2_remake_stages() -> None:
    """Tekken 2 Content's Stages, Remake: the 23 packs (tools/t2_remake_stages.py)
    from the disc, tekken3.zip (the Expanded setup's) and the pictures: again
    when any of them, the generator or its recipes changed or a pack is gone.
    Without tekken3.zip every stage is the Classic's pack (no Remake)."""
    sys.path.insert(0, str(ADDON / 'tools'))
    import hashlib
    from disc_image import DiscError
    import t2_remake_stages as remake
    import tekken2_stages as stages
    disc = stages.find_disc(STATE)
    if not disc:
        log('Tekken 2 Remake stages: no Tekken 2 disc, skipped')
        return
    out = ROOT / stages.OUT
    if not all((out / n).is_file() for n in stamp('tekken2-stages').get('files', [])):
        log('Tekken 2 Remake stages: no Classic packs, skipped')
        return
    tekken3 = remake.tekken3_zip()
    storm = remake.storm_pack(ROOT)          # the lightning's flashes and thunder (TTT1 Stages' stage 4)
    code = hashlib.sha256()
    for f in ('tools/t2_remake_stages.py', 'tools/t2_remake_stage.py', 'tools/t2_remake_disc.py',
              'tools/data/t2_remake_stages.json'):
        code.update((ADDON / f).read_bytes())
    want = {'version': remake.VERSION, 'code': code.hexdigest()[:16],
            'pictures': stamp('tekken2-remake-pictures').get('stamp'), 'classic': stamp('tekken2-stages').get('stamp'),
            'tekken3': {'path': str(tekken3), **file_stamp(tekken3)} if tekken3 else None,
            # its content, not its time: each build copies the TTT packs again
            'storm': hashlib.sha256(storm.read_bytes()).hexdigest()[:16] if storm else None}
    have = stamp('tekken2-remake-stages')
    if have.get('stamp') == want and all((out / n).is_file() for n in have.get('files', [])):
        return
    if not tekken3:
        log('Tekken 2 Remake stages: no tekken3.zip (the Expanded setup’s), the Classic’s packs in their place')
    if tekken3 and not storm:
        log('Tekken 2 Remake stages: no TTT stage packs (TTT1 Stages), the lightning without flashes or thunder')
    log('Tekken 2 Remake stages: building them (a few minutes)')
    try:
        names = remake.convert(disc, out, tekken3, say=log, storm=storm)
    except ImportError as error:                    # numpy, Pillow: python_packages
        log(f'Tekken 2 Remake stages: {error}; left out')
        return
    except (DiscError, OSError, ValueError, SystemExit) as error:     # the generator says SystemExit
        log(f'Tekken 2 Remake stages: {error}; left out')
        return
    save_stamp('tekken2-remake-stages', {'stamp': want, 'files': names})


def ttt_ps2_music() -> None:
    """The Jukebox's TTT console music (tools/ttt_ps2_music.py), from the
    Expanded's PS2 stage music pack and the TTT PS2 disc it was made from."""
    sys.path.insert(0, str(ADDON / 'tools'))
    import ttt_ps2_music as music
    music_pack_step(music, 'ttt-ps2-music', 'TTT PS2 music', 'the TTT PS2 disc',
                    lambda pack, out, disc: music.convert(pack, out, disc, say=log))


def expanded_cc() -> str | None:
    """The Expanded's pinned clang on Windows (its setup gets it when missing,
    as for the build: dev/build.py), else the one installed."""
    sys.path.insert(0, str(ROOT / 'launcher'))
    try:
        import setup_backend
        toolchain = setup_backend.tools()
    except Exception as error:   # noqa: BLE001 - any failure: look for an installed compiler
        log(f'the Expanded setup gives no compiler ({error})')
        toolchain = None
    if sys.platform == 'win32' and toolchain and (Path(toolchain) / 'bin/clang.exe').is_file():
        return str(Path(toolchain) / 'bin/clang.exe')
    return None


def ttt_arcade_music() -> None:
    """The Jukebox's TTT arcade music (tools/ttt_arcade_music.py), rendered
    from the Expanded's arcade stage music pack with a C compiler."""
    sys.path.insert(0, str(ADDON / 'tools'))
    import ttt_arcade_music as music

    def convert(pack, out, disc):
        cc = music.compiler(expanded_cc())
        if not cc:
            log('TTT arcade music: no C compiler, skipped')
            return None
        return music.convert(pack, out, cc, say=log)
    music_pack_step(music, 'ttt-arcade-music', 'TTT arcade music', 'tektagt.zip', convert)


def tekken5dr_music() -> None:
    """The Jukebox's TEKKEN 5 DR music (tools/tekken5dr_music.py), from the
    Dark Resurrection ISO the Expanded's setup was given: again when the ISO or
    the tool's VERSION changed or a file is gone; nothing without the ISO."""
    sys.path.insert(0, str(ADDON / 'tools'))
    import tekken5dr_music as music
    iso = music.find_iso(ROOT)
    if not iso:
        log('Tekken 5 DR music: no Dark Resurrection ISO (the Expanded setup asks for it), skipped')
        return
    want = {'version': music.VERSION, 'iso': {'path': str(iso), **file_stamp(iso)}, 'skip': music.skipped()}
    have = stamp('tekken5dr-music')
    out = ROOT / music.OUT
    if have.get('stamp') == want and all((out / n).is_file() for n in have.get('files', [])):
        return
    log('Tekken 5 DR music: taking it from the ISO')
    try:
        names = music.convert(iso, out, say=log)
    except (music.IsoError, OSError) as error:
        log(f'Tekken 5 DR music: {error}; left out')
        return
    save_stamp('tekken5dr-music', {'stamp': want, 'files': names})


def tekken3_portraits() -> None:
    """Tournament Mode's portraits of the stock fighters (tools/t3_portraits.py),
    from the Tekken 3 disc the Expanded runs (game.toml), written over the
    ones the game kept: again when the disc or the tool's VERSION changed or
    a picture is gone. Without them (no disc, another region) the game keeps
    each one as its loading screen first shows it."""
    sys.path.insert(0, str(ADDON / 'tools'))
    from disc_image import DiscError
    import t3_portraits as portraits
    disc = portraits.find_disc(ROOT)
    if not disc:
        log('Tekken 3 portraits: no Tekken 3 disc in game.toml, skipped')
        return
    want = {'version': portraits.VERSION, 'disc': {'path': str(disc), **file_stamp(disc)}}
    have = stamp('tekken3-portraits')
    out = ROOT / portraits.OUT
    if have.get('stamp') == want and all((out / n).is_file() for n in have.get('files', [])):
        return
    log('Tekken 3 portraits: taking them from the disc')
    try:
        names = portraits.convert(disc, out, say=log)
    except (DiscError, OSError, ValueError) as error:
        log(f'Tekken 3 portraits: {error}; left out')
        return
    save_stamp('tekken3-portraits', {'stamp': want, 'files': names})


def jukebox_titles() -> None:
    """The Jukebox's title lists (tools/jukebox_titles.py): every run, since
    it adds lines for music that came since, and makes a list it wrote and
    nobody changed again when the add-on's (data/jukebox) changed."""
    sys.path.insert(0, str(ADDON / 'tools'))
    import jukebox_titles as titles
    titles.update(ROOT)


STEPS: list = [python_packages, tekken2_disc, tekken2_music, tekken2_stages, tekken2_remake_pictures,
               tekken2_remake_stages, ttt_ps2_music, ttt_arcade_music, tekken5dr_music,
               jukebox_titles,   # the music first: the titles list it
               tekken3_portraits]


def main() -> int:
    parser = argparse.ArgumentParser(description='TMST add-on setup (after the Expanded setup).')
    parser.add_argument('--interactive', action='store_true', help='ask for what is missing in a window')
    choice = parser.add_mutually_exclusive_group()
    choice.add_argument('--t2-disc', metavar='FILE', help='the Tekken 2 disc (.cue, .bin or .chd)')
    choice.add_argument('--no-t2-disc', action='store_true', help='no Tekken 2 disc')
    parser.add_argument('--no-build', action='store_true', help='the steps only, no build of the game')
    parser.parse_args(namespace=OPTIONS)
    if not (ROOT / 'game.toml').is_file():
        log(f'{ROOT} is not a Tekken 3 Expanded folder')
        return 1
    if not in_venv() and venv_python().is_file() and not os.environ.get('TMST_SETUP_AGAIN'):
        # The Expanded's Python (its numpy, its pip) for every step and the build.
        return subprocess.call([str(venv_python()), '-u', str(Path(__file__).resolve()), *sys.argv[1:]],
                               env=dict(os.environ, TMST_SETUP_AGAIN='1'))
    if PACKAGES.is_dir() and str(PACKAGES) not in sys.path:
        sys.path.insert(0, str(PACKAGES))
    try:
        for n, step in enumerate(STEPS, 1):
            print(f'TMST step {n}/{len(STEPS)}: {step.__name__}', flush=True)   # the setup window's progress
            step()
    except SetupFailed as error:
        log(str(error))
        return 1
    log('done' if STEPS else 'nothing to make yet')
    return 0 if OPTIONS.no_build else build()


if __name__ == '__main__':
    sys.exit(main())
