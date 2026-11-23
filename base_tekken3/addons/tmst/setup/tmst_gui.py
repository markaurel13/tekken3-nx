"""The TMST add-on's setup window (Setup TMST.cmd, .command, .sh): the Tekken 2 disc, then
the add-on's setup (setup/tmst_setup.py) and the game built again with it
(dev/build.py), with their progress; then the game.

    pythonw setup/tmst_gui.py [<Tekken 2 disc>]

The look is the Expanded's setup window's (launcher/easy_launcher.py). Both
steps run as processes of their own; their output goes to
addons/tmst/workspace/setup.log, and the window reads its progress from it:
the setup's "TMST step n/N" lines and the build's Ninja "[n/N]" lines.
"""
from __future__ import annotations
import ctypes, json, os, queue, re, subprocess, sys, threading
from pathlib import Path
import tmst_platform as platform

ADDON = Path(__file__).resolve().parents[1]
ROOT = ADDON.parents[1]                     # <expanded>/addons/tmst/setup
LOG = ADDON / 'workspace' / 'setup.log'
EXE = ROOT / 'build-release' / platform.EXE_NAME
sys.path.insert(0, str(ADDON / 'tools'))
sys.path.insert(0, str(ROOT / 'launcher'))

FONT = 'Segoe UI' if os.name == 'nt' else 'Helvetica Neue' if sys.platform == 'darwin' else 'DejaVu Sans'
MODS = 'Jukebox · Photo Mode · Tournament Mode · VS Mode+ · Tekken 2 Content'
# Where each part of the work sits on the progress bar.
SETUP_SHARE, CONFIGURE_SHARE = 0.30, 0.08
STEP_TEXT = {
    'python_packages': ('Checking the Python modules', 'numpy, from the Expanded’s Python (installed here when it has none).'),
    'tekken2_disc': ('Checking your Tekken 2 disc', ''),
    'tekken2_music': ('Making the Jukebox’s Tekken 1 and 2 music', 'From your Tekken 2 disc. The first time takes a minute or two.'),
    'tekken2_stages': ('Making the Tekken 2 stages', 'From your Tekken 2 disc: its backgrounds and floors. A few seconds.'),
    'tekken2_remake_pictures': ('Opening the Tekken 2 Remake backgrounds', 'With your Tekken 2 disc. A few seconds.'),
    'tekken2_remake_stages': ('Building the Tekken 2 Remake stages', 'From your Tekken 2 disc and tekken3.zip: the remade stages. A few minutes.'),
    'ttt_ps2_music': ('Making the Jukebox’s TTT console music', 'From the TTT PS2 music the Expanded’s setup imported. The first time takes a minute.'),
    'ttt_arcade_music': ('Making the Jukebox’s TTT arcade music', 'Rendered from tektagt.zip’s music with the Expanded’s compiler.'),
    'tekken5dr_music': ('Making the Jukebox’s Tekken 5 DR music', 'From the Dark Resurrection ISO the Expanded’s setup was given.'),
    'jukebox_titles': ('Updating the Jukebox’s title lists', ''),
    'tekken3_portraits': ('Taking the Tekken 3 portraits', 'Tournament Mode’s pictures of the Tekken 3 fighters, from your Tekken 3 disc.'),
}


def expanded_backend():
    """The Expanded's setup module (internal to it, docs/COMPAT.md): None if it cannot be loaded."""
    try:
        import setup_backend
        return setup_backend
    except Exception:   # noqa: BLE001
        return None


def expanded_problem(backend) -> str | None:
    """Why the add-on cannot be set up yet, or None."""
    if not (ROOT / 'game.toml').is_file():
        return ('This folder is not inside Tekken 3 Expanded. Put it at '
                + os.path.join('<Tekken 3 Expanded>', 'addons', 'tmst') + '.')
    if backend is None or not (ROOT / '.setup/ready.json').is_file() or not (ROOT / 'build-release/CMakeCache.txt').is_file():
        return (f'Tekken 3 Expanded has not been set up yet. Run {platform.script("Setup Tekken 3")} in '
                f'{ROOT} first and let it finish (the game starts), close the game, then open this again.')
    return None


def game_running() -> bool:
    return platform.game_running(EXE)


def known_t2_disc() -> str:
    """The Tekken 2 disc the add-on's setup kept, else one found beside the Expanded's files."""
    try:
        state = json.loads((ADDON / 'workspace/tekken2-disc.json').read_text(encoding='utf-8'))
    except (OSError, ValueError):
        state = {}
    if state.get('disc') and Path(state['disc']['path']).is_file():
        return state['disc']['path']
    if state.get('declined'):
        return ''
    try:
        import tekken2_disc
        sys.path.insert(0, str(ADDON / 'setup'))
        import tmst_setup
        found = tekken2_disc.search(tmst_setup.expanded_source_folders())
    except Exception:   # noqa: BLE001 - nothing found is the same as nothing there
        return ''
    return str(found[0]) if found else ''


class Job:
    """Windows owns the worker tree, so Cancel also stops the compilers: the
    Expanded's own ProcessJob when it has one. Elsewhere the step runs in a
    session of its own, which Cancel stops whole (setup/tmst_platform.py)."""
    def __init__(self):
        self.job = None
        if not platform.WINDOWS:
            return
        try:
            from easy_launcher import ProcessJob
            self.job = ProcessJob()
        except Exception:   # noqa: BLE001 - without it Cancel stops the step's own process only
            self.job = None

    def assign(self, process):
        if self.job:
            try:
                self.job.assign(process)
            except OSError:
                pass

    def close(self):
        if self.job:
            self.job.close()
            self.job = None


def gui(t2_arg: str | None, test_hook=None):
    import tkinter as tk
    from tkinter import ttk, filedialog, messagebox
    backend = expanded_backend()

    class App:
        def __init__(self):
            self.window = tk.Tk()
            self.window.title('Tekken 3 Expanded — TMST add-on')
            self.window.configure(bg='#11131b')
            self.window.resizable(False, False)
            self.events = queue.Queue()
            self.process = None
            self.job = None
            self.busy = False
            self.epoch = 0
            s = ttk.Style(self.window)
            s.theme_use('clam')
            s.configure('.', font=(FONT, 10), background='#11131b', foreground='#ececf2')
            s.configure('TFrame', background='#11131b')
            s.configure('TLabel', background='#11131b', foreground='#ececf2')
            s.configure('Muted.TLabel', foreground='#a9adbd')
            s.configure('Title.TLabel', font=(FONT, 25, 'bold'))
            s.configure('TEntry', fieldbackground='#232735', foreground='#f4f4f7', padding=8)
            s.configure('TButton', background='#2d3242', foreground='#f4f4f7', padding=(14, 8), borderwidth=0)
            s.map('TButton', background=[('active', '#404758'), ('disabled', '#20232d')])
            s.configure('Play.TButton', background='#744bd7', font=(FONT, 12, 'bold'), padding=(20, 12))
            s.map('Play.TButton', background=[('active', '#8862e8'), ('disabled', '#393247')])
            s.configure('TProgressbar', background='#9775ea', troughcolor='#232735', borderwidth=0)
            root = ttk.Frame(self.window, padding=28)
            root.pack(fill='both', expand=True)
            ttk.Label(root, text='TMST Add-On for Tekken 3 Expanded', style='Title.TLabel').pack(anchor='w')
            ttk.Label(root, text='New features for Tekken 3 Expanded.',
                      style='Muted.TLabel').pack(anchor='w', pady=(4, 22))
            ttk.Label(root, text='Installs: ' + MODS).pack(anchor='w', pady=(0, 14))
            frame = ttk.Frame(root)
            frame.pack(fill='x', pady=(0, 9))
            ttk.Label(frame, text='Tekken 2 · USA or Europe PlayStation disc').pack(anchor='w', pady=(0, 5))
            row = ttk.Frame(frame)
            row.pack(fill='x')
            self.t2 = tk.StringVar(value=t2_arg or known_t2_disc())
            self.entry = ttk.Entry(row, textvariable=self.t2, width=58)
            self.entry.pack(side='left', fill='x', expand=True)
            self.browse_button = ttk.Button(row, text='Browse', command=self.browse)
            self.browse_button.pack(side='right', padx=(9, 0))
            ttk.Label(root, text='Use your own game files. Nothing is uploaded.\n'
                      'Uses the Expanded’s own Python and compilers; the first build takes a few minutes.\n'
                      'Switch the mods on in the game’s Mods tab, TMST group.',
                      style='Muted.TLabel', justify='left').pack(anchor='w', pady=(12, 20))
            self.problem = expanded_problem(backend)
            self.status = tk.StringVar(value='Tekken 3 Expanded is not ready' if self.problem else 'Ready to install')
            self.detail = tk.StringVar(value=self.problem or f'Tekken 3 Expanded: {ROOT}')
            ttk.Label(root, textvariable=self.status, font=(FONT, 11, 'bold'), wraplength=585).pack(anchor='w')
            ttk.Label(root, textvariable=self.detail, style='Muted.TLabel', wraplength=585, justify='left').pack(anchor='w', pady=(5, 10))
            self.progress = ttk.Progressbar(root, mode='determinate', maximum=1000, length=590)
            self.progress.pack(fill='x', pady=(0, 16))
            actions = ttk.Frame(root)
            actions.pack(fill='x')
            self.primary = ttk.Button(actions, text='Install & play', style='Play.TButton', command=self.start)
            self.primary.pack(side='right')
            self.cancel_button = ttk.Button(actions, text='Cancel', command=self.cancel, state='disabled')
            self.cancel_button.pack(side='right', padx=(0, 9))
            ttk.Button(actions, text='Open setup log', command=self.open_log).pack(side='left')
            if self.problem:
                for w in (self.primary, self.entry, self.browse_button):
                    w.configure(state='disabled')
            self.window.protocol('WM_DELETE_WINDOW', self.close)
            self.window.after(100, self.poll)
            self.window.update_idletasks()
            width, height = self.window.winfo_reqwidth(), self.window.winfo_reqheight()
            x = max(0, (self.window.winfo_screenwidth() - width) // 2)
            y = max(0, (self.window.winfo_screenheight() - height) // 2)
            self.window.geometry(f'+{x}+{y}')

        def browse(self):
            chosen = filedialog.askopenfilename(parent=self.window, title='Choose your Tekken 2 disc',
                                                filetypes=[('Disc image', '*.cue *.bin *.chd'), ('All files', '*.*')])
            if chosen:
                self.t2.set(chosen)

        def set_busy(self, value):
            self.busy = value
            for w in (self.entry, self.browse_button, self.primary):
                w.configure(state='disabled' if value else 'normal')
            self.cancel_button.configure(state='normal' if value else 'disabled')

        def set_progress(self, fraction, back=False):
            value = int(max(0.0, min(1.0, fraction)) * 1000)
            if back or value > self.progress['value']:      # forward only: Ninja runs more than once
                self.progress.configure(value=value)

        def t2_args(self) -> list[str] | None:
            """The setup's Tekken 2 arguments; None when the disc given cannot be used and
            the player goes back to choose another."""
            path = self.t2.get().strip().strip('"')
            if not path:
                return ['--no-t2-disc']
            try:
                import tekken2_disc
                info = tekken2_disc.identify(path)
            except Exception as error:   # noqa: BLE001 - DiscError, OSError: the reason is the message
                if messagebox.askyesno('Tekken 2 disc not usable',
                                       f'{error}\n\nContinue without the Tekken 2 disc?',
                                       icon='warning', default='no', parent=self.window):
                    return ['--no-t2-disc']
                self.status.set('Choose another Tekken 2 disc')
                self.detail.set('Click Browse, or clear the field to install without it.')
                return None
            self.detail.set(f'Tekken 2 disc: {info["region"]} ({info["serial"]})')
            return ['--t2-disc', path]

        def start(self):
            if game_running():
                self.status.set('Close the game first')
                self.detail.set('Tekken 3 Expanded is running: a running game cannot be built again.')
                return
            args = self.t2_args()
            if args is None:
                return
            self.epoch += 1
            self.set_busy(True)
            self.set_progress(0, back=True)
            self.status.set('Starting the add-on’s setup')
            LOG.parent.mkdir(parents=True, exist_ok=True)
            python = Path(sys.executable).with_name('python.exe') if os.name == 'nt' else Path(sys.executable)
            steps = [[str(python), '-u', str(ADDON / 'setup/tmst_setup.py'), '--no-build', *args],
                     [str(python), '-u', str(ADDON / 'dev/build.py')]]
            threading.Thread(target=self.work, args=(steps, self.epoch), daemon=True).start()

        def work(self, steps, epoch):
            env = dict(os.environ, PYTHONUTF8='1', PYTHONIOENCODING='utf-8')
            with LOG.open('w', encoding='utf-8') as log:
                for n, command in enumerate(steps):
                    if epoch != self.epoch:
                        return
                    try:
                        job = Job()
                        process = subprocess.Popen(command, cwd=ADDON, stdin=subprocess.DEVNULL, stdout=subprocess.PIPE,
                                                   stderr=subprocess.STDOUT, text=True, encoding='utf-8', errors='replace', env=env,
                                                   **platform.popen_options())
                        job.assign(process)
                    except OSError as error:
                        self.events.put((epoch, ('error', f'Could not start: {error}')))
                        return
                    self.process, self.job = process, job
                    for line in process.stdout:
                        log.write(line)
                        log.flush()
                        self.events.put((epoch, ('line', n, line.rstrip())))
                    code = process.wait()
                    job.close()
                    if code:
                        self.events.put((epoch, ('failed', n, code)))
                        return
                self.events.put((epoch, ('done',)))

        def on_line(self, part, line):
            if part == 0:                             # the add-on's setup
                m = re.match(r'TMST step (\d+)/(\d+): (\w+)', line)
                if m:
                    done, total = int(m[1]) - 1, int(m[2])
                    self.set_progress(SETUP_SHARE * done / total)
                    text = STEP_TEXT.get(m[3], (m[3].replace('_', ' ').capitalize(), ''))
                    self.status.set(text[0])
                    self.detail.set(text[1])
                elif line.startswith('TMST setup: ') and line != 'TMST setup: done':
                    self.detail.set(line[len('TMST setup: '):])
                return
            m = re.match(r'\[(\d+)/(\d+)\]', line)    # Ninja
            if m:
                self.status.set('Building your game with the add-on')
                share = SETUP_SHARE + CONFIGURE_SHARE
                self.set_progress(share + (1 - share) * int(m[1]) / max(1, int(m[2])))
                self.detail.set(f'Step {m[1]} of {m[2]}. Your mod settings and game data stay.')
            elif line.startswith('> ') and '--build' not in line:
                self.status.set('Preparing the game to build')
                self.detail.set('The Expanded’s compilers; the first time they may be downloaded.')
                self.set_progress(SETUP_SHARE)
            elif line.startswith('TMST build: '):
                self.detail.set(line[len('TMST build: '):])

        def poll(self):
            try:
                while True:
                    epoch, event = self.events.get_nowait()
                    if epoch != self.epoch:
                        continue
                    kind = event[0]
                    if kind == 'line':
                        self.on_line(event[1], event[2])
                    elif kind in ('failed', 'error'):
                        self.process = None
                        self.set_busy(False)
                        self.status.set('Setup needs attention')
                        if kind == 'error':
                            self.detail.set(event[1])
                        elif not self.detail.get():
                            self.detail.set('Open the setup log for details, then click Try again.')
                        else:
                            self.detail.set(self.detail.get() + '\nOpen the setup log for details, then click Try again.')
                        self.primary.configure(text='Try again')
                    elif kind == 'done':
                        self.process = None
                        self.set_busy(False)
                        self.set_progress(1)
                        self.status.set('The TMST add-on is installed')
                        self.detail.set('Switch on its mods in the Mods tab (TMST group), then restart the game.')
                        self.primary.configure(text='Play', command=self.play)
            except queue.Empty:
                pass
            self.window.after(100, self.poll)

        def play(self):
            try:
                if backend is None:
                    raise RuntimeError('the Expanded setup cannot be loaded')
                backend.launch_game(settings=True)     # its launcher: the Mods tab is there
                self.window.destroy()
            except Exception as error:   # noqa: BLE001
                self.status.set('Could not launch the game')
                self.detail.set(f'{error}. Start {platform.PLAY} yourself.')

        def cancel(self):
            self.epoch += 1
            if self.job:
                self.job.close()
                self.job = None
            if self.process:
                platform.stop(self.process)
            self.process = None
            self.set_busy(False)
            self.status.set('Setup cancelled')
            self.detail.set('What is done is kept. You can try again whenever you’re ready.')
            self.primary.configure(text='Try again')

        def close(self):
            if self.busy:
                self.cancel()
            self.window.destroy()

        def open_log(self):
            if LOG.is_file():
                platform.open_file(LOG)
            else:
                self.detail.set('The log appears when the setup starts.')

    app = App()
    if test_hook:
        test_hook(app)
    app.window.mainloop()


if __name__ == '__main__':
    if os.name == 'nt':
        try:
            ctypes.windll.shcore.SetProcessDpiAwareness(1)
        except (AttributeError, OSError):
            pass
    try:
        gui(sys.argv[1] if len(sys.argv) > 1 else None)
    except Exception as error:   # noqa: BLE001 - pythonw has no console: show it
        import tkinter as tk
        from tkinter import messagebox
        w = tk.Tk()
        w.withdraw()
        messagebox.showerror('TMST add-on', str(error))
        w.destroy()
