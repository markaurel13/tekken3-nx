"""Chains the TMST add-on's wrapper in front of the Expanded's, for symbols
the Expanded already wraps (GNU ld / lld cannot --wrap a symbol twice).

Run by the build right before the game is linked (PRE_LINK, dev/tmst_inject.cmake):
in the Expanded's compiled object that defines __wrap_<sym>, that name is
renamed to __tmst_chain_<sym>; the add-on's own object defines __wrap_<sym>
and calls __tmst_chain_<sym>. Only build products change, never a source.

    tmst_chain.py --objcopy X --nm Y --objdir DIR --state FILE sym...

The renamed objects are listed in FILE, so that a build without the add-on
(dev/build.py --without) deletes them and lets Ninja compile them again.
"""
from __future__ import annotations
import argparse, json, subprocess, sys
from pathlib import Path


def ours(path: Path, objdir: Path) -> bool:
    # The add-on's own objects (src/**/tmst_*.c): never renamed. Only the part
    # under the object folder counts (a build folder may sit under addons/tmst).
    return 'tmst' in path.relative_to(objdir).as_posix().lower()


def defines(nm: str, obj: Path, name: str) -> bool:
    # -P (POSIX: name, type, ...): llvm-nm and GNU nm of any age read it.
    out = subprocess.run([nm, '--defined-only', '-P', str(obj)], capture_output=True, text=True).stdout
    return name in (line.split()[0] for line in out.splitlines() if line.strip())


def candidates(objdir: Path, names: list[bytes], cached: list[str]):
    seen = set()
    for c in cached:
        p = Path(c)
        if p.is_file(): seen.add(p); yield p
    # .obj (Windows), .o (elsewhere, and the Android NDK's on any host)
    for p in objdir.rglob('*'):
        if p.suffix not in ('.o', '.obj') or p in seen or ours(p, objdir): continue
        try: data = p.read_bytes()
        except OSError: continue
        if any(n in data for n in names): yield p


def main() -> int:
    a = argparse.ArgumentParser()
    a.add_argument('--objcopy', required=True); a.add_argument('--nm', required=True)
    a.add_argument('--objdir', required=True, type=Path); a.add_argument('--state', required=True, type=Path)
    a.add_argument('symbols', nargs='*')
    args = a.parse_args()
    try: state = json.loads(args.state.read_text(encoding='utf-8'))
    except (OSError, ValueError): state = {}
    bad = 0
    for sym in args.symbols:
        wrap, chain = f'__wrap_{sym}', f'__tmst_chain_{sym}'
        done = []
        for obj in candidates(args.objdir, [wrap.encode(), chain.encode()], state.get(sym, [])):
            if defines(args.nm, obj, chain):
                done.append(str(obj))
            elif defines(args.nm, obj, wrap):
                subprocess.run([args.objcopy, f'--redefine-sym={wrap}={chain}', str(obj)], check=True)
                print(f'TMST chain: {sym}: {obj.name} renamed {wrap} -> {chain}')
                done.append(str(obj))
        if not done:
            print(f'TMST chain: no object of the Expanded defines {wrap} (docs/COMPAT.md)', file=sys.stderr)
            bad += 1
        state[sym] = done
    args.state.write_text(json.dumps(state, indent=1), encoding='utf-8')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
