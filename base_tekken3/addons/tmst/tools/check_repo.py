"""Checks of the TMST add-on that need no disc (CI and before each commit):

- every package under mods/ is a tmst.* package, its folder matches its id
  and version, and addon.cmake lists exactly those packages;
- every source in addon.cmake exists, and every wrap names its file;
- no retail-derived data: no disc images, executables or game archives, no
  file over the size limit, nothing under folders that hold game data;
- the Remake's picture patches (tools/data/t2-remake-patches/*.t2p, over
  the size limit): only patches there, each mixed with its disc's key
  (tools/t2_remake_patch.py check), within their own total.

    python tools/check_repo.py
"""
from __future__ import annotations
import re, subprocess, sys, tomllib
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
FORBIDDEN_SUFFIXES = {'.bin', '.cue', '.chd', '.iso', '.img', '.mdf', '.exe', '.dll', '.zip', '.7z', '.rar',
                      '.ast', '.bgm', '.xa', '.tim', '.tmd', '.vab', '.t3pv', '.cap', '.rgba'}
MAX_BYTES = 512 * 1024
# paths (relative, posix) a reviewer has cleared: the Remake's generator,
# the old fork's tools/t2_arcade_stage.py ported (source, its 23 stages'
# recipes' code, no game data; 2026-11-07)
ALLOWED_BIG = {'tools/t2_remake_stage.py'}
PATCHES = 'tools/data/t2-remake-patches'    # the Remake's picture patches: checked apart


def cmake_list(text: str, name: str) -> list[str]:
    m = re.search(r'set\(\s*' + name + r'\b(.*?)\)', text, re.S)
    if not m: return []
    body = re.sub(r'#.*', '', m.group(1))
    return [t.strip('"') for t in body.split()]


def tracked_files() -> list[Path]:
    try:
        out = subprocess.run(['git', '-C', str(ADDON), 'ls-files', '--cached', '--others', '--exclude-standard'],
                             capture_output=True, text=True, check=True).stdout
        return [ADDON / p for p in out.splitlines() if p]
    except (OSError, subprocess.CalledProcessError):
        return [p for p in ADDON.rglob('*') if p.is_file() and '.git' not in p.parts and 'build' not in p.parts]


def main() -> int:
    errors: list[str] = []
    addon = (ADDON / 'addon.cmake').read_text(encoding='utf-8')

    for src in cmake_list(addon, 'TMST_SOURCES'):
        if not (ADDON / src).is_file(): errors.append(f'addon.cmake: missing source {src}')
    for wrap in cmake_list(addon, 'TMST_WRAPS'):
        sym, _, file = wrap.partition('|')
        file = file.partition('|')[0]           # "|NAME": a build feature it needs
        if not file or not (ADDON / file).is_file(): errors.append(f'addon.cmake: wrap {sym} without its file')
        elif f'__wrap_{sym}' not in (ADDON / file).read_text(encoding='utf-8', errors='replace'):
            errors.append(f'addon.cmake: {file} defines no __wrap_{sym}')

    for chain in cmake_list(addon, 'TMST_CHAINS'):
        sym, _, file = chain.partition('|')
        file = file.partition('|')[0]           # "|NAME": a build feature it needs
        text = (ADDON / file).read_text(encoding='utf-8', errors='replace') if file and (ADDON / file).is_file() else ''
        if f'__wrap_{sym}' not in text or f'__tmst_chain_{sym}' not in text:
            errors.append(f'addon.cmake: chain {sym}: {file or "?"} must define __wrap_{sym} and call __tmst_chain_{sym}')

    listed = set(cmake_list(addon, 'TMST_PACKAGES'))
    found = set()
    for manifest in sorted((ADDON / 'mods').glob('*/*/manifest.toml')):
        pkg, ver = manifest.parent.parent.name, manifest.parent.name
        found.add(pkg)
        try:
            data = tomllib.loads(manifest.read_text(encoding='utf-8'))
        except tomllib.TOMLDecodeError as error:
            errors.append(f'{manifest.relative_to(ADDON)}: {error}'); continue
        if data.get('id') != pkg: errors.append(f'{pkg}: id is {data.get("id")!r}')
        if data.get('version') != ver: errors.append(f'{pkg}: version is {data.get("version")!r}, folder {ver}')
        if not pkg.startswith('tmst.'): errors.append(f'{pkg}: not a tmst.* package')
        for plugin in data.get('plugin', []):
            if not str(plugin.get('id', '')).startswith('tmst.'): errors.append(f'{pkg}: plugin {plugin.get("id")} not tmst.*')
        features = {f.get('id') for f in data.get('feature', [])}
        for plugin in data.get('plugin', []):
            if plugin.get('feature') not in features: errors.append(f'{pkg}: plugin of unknown feature {plugin.get("feature")}')
        # A part requiring another feature: both known, and the defaults keep to it (else the Expanded refuses the package).
        default = {f.get('id'): f.get('default_enabled', False) for f in data.get('feature', [])}
        for c in data.get('constraint', []):
            if c.get('kind') != 'requires_feature': continue
            if c.get('feature') not in features or c.get('requires_feature') not in features:
                errors.append(f'{pkg}: constraint of unknown feature {c.get("feature")} / {c.get("requires_feature")}')
            elif default[c['feature']] and not default[c['requires_feature']]:
                errors.append(f'{pkg}: {c["feature"]} is on by default, {c["requires_feature"]} it requires is not')
    if listed != found:
        errors.append(f'addon.cmake TMST_PACKAGES {sorted(listed)} != mods/ {sorted(found)}')

    patches = []
    for path in tracked_files():
        rel = path.relative_to(ADDON).as_posix()
        if rel.startswith(('dev/build/', '.git/')): continue
        if rel.startswith(PATCHES + '/'):
            if path.suffix != '.t2p': errors.append(f'{rel}: not a picture patch')
            else: patches.append(path)
            continue
        if path.suffix.lower() in FORBIDDEN_SUFFIXES: errors.append(f'{rel}: game-data file type')
        if path.is_file() and path.stat().st_size > MAX_BYTES and rel not in ALLOWED_BIG:
            errors.append(f'{rel}: {path.stat().st_size} bytes (limit {MAX_BYTES}; game data?)')

    if patches:
        sys.path.insert(0, str(ADDON / 'tools'))
        try:
            import t2_remake_patch
            errors += [f'{PATCHES}/{e}' for e in t2_remake_patch.check(patches)]
        except ImportError as error:    # numpy (the disc reader's)
            errors.append(f'{PATCHES}: cannot check ({error})')

    for e in errors: print('ERROR', e)
    print('check_repo:', 'OK' if not errors else f'{len(errors)} problem(s)')
    return 1 if errors else 0


if __name__ == '__main__':
    sys.exit(main())
