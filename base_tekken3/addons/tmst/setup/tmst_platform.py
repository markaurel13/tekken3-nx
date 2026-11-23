"""What differs between Windows, macOS and Linux for the add-on's setup
(setup/tmst_gui.py, setup/tmst_setup.py): the names of the scripts the
player runs, whether the game is running, opening a file, and starting and
stopping a step's processes as a whole."""
from __future__ import annotations
import os, signal, subprocess, sys
from pathlib import Path

WINDOWS = os.name == 'nt'
MAC = sys.platform == 'darwin'
# The script's ending on this system: Setup TMST.cmd, .command or .sh.
SCRIPT = '.cmd' if WINDOWS else '.command' if MAC else '.sh'
# The Expanded's double-click to play, beside its setup (launcher/setup_backend.py).
PLAY = 'Tekken 3 Expanded.exe' if WINDOWS else 'Tekken 3 Expanded.app' if MAC else 'Tekken 3 Expanded.sh'
EXE_NAME = 'Tekken_3_Expanded.exe' if WINDOWS else 'Tekken_3_Expanded'


def script(name: str) -> str:
    """'Setup TMST' -> 'Setup TMST.cmd' (Windows), '.command' (macOS), '.sh' (Linux)."""
    return name + SCRIPT


def game_running(exe: Path) -> bool:
    """The game (build-release's) is running: a running game cannot be built again."""
    try:
        if WINDOWS:
            # tasklist writes in the console's (OEM) code page, in the system's
            # language: a German "ausgeführt" is not cp1252 (issue #7).
            out = subprocess.run(['tasklist', '/fi', f'imagename eq {EXE_NAME}', '/fo', 'csv', '/nh'],
                                 capture_output=True, text=True, encoding='oem', errors='replace',
                                 creationflags=subprocess.CREATE_NO_WINDOW).stdout or ''
            return EXE_NAME.lower() in out.lower()
        target = exe.resolve()
        if Path('/proc').is_dir():                      # Linux: each process's executable
            for entry in Path('/proc').iterdir():
                if entry.name.isdigit():
                    try:
                        if Path(os.readlink(entry / 'exe')) == target:
                            return True
                    except OSError:
                        pass
            return False
        # macOS: ps gives each process's executable path in full.
        out = subprocess.run(['ps', '-A', '-o', 'comm='], capture_output=True, text=True,
                             errors='replace').stdout or ''
        for line in out.splitlines():
            line = line.strip()
            if line == str(target) or (line.endswith('/' + EXE_NAME) and Path(line).resolve() == target):
                return True
    except OSError:
        pass
    return False


def open_file(path: Path) -> None:
    if WINDOWS:
        os.startfile(str(path))
    else:
        subprocess.Popen(['open' if MAC else 'xdg-open', str(path)])


def popen_options() -> dict:
    """A step's process: without a console window on Windows; elsewhere in a
    session of its own, so that stop() reaches the compilers it starts."""
    if WINDOWS:
        return {'creationflags': subprocess.CREATE_NO_WINDOW}
    return {'start_new_session': True}


def stop(process: subprocess.Popen) -> None:
    """A step's process and (macOS, Linux) every process it started. On
    Windows the Expanded's ProcessJob (setup/tmst_gui.py's Job) stops those."""
    if process.poll() is not None:
        return
    if not WINDOWS:
        try:
            os.killpg(process.pid, signal.SIGTERM)
            return
        except OSError:
            pass
    process.terminate()
