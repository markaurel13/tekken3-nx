"""Where the symbols the TMST add-on wraps are defined, for the builds that
emulate -Wl,--wrap with the preprocessor (dev/tmst_wraps.cmake): macOS, whose
ld64 has no --wrap (the Expanded does the same for its own wraps:
tools/resolve_wrap_targets.py), and TMST_EMULATE_WRAPS=ON, which tries that
emulation on Windows or Linux.

    tmst_wraps.py --sources FILE --ours DIR sym...

FILE lists the game's sources (psx-runtime's, one path a line); those under
DIR (the add-on) are never a definition nor the Expanded's wrapper. Prints
one line a symbol:

    sym|<file defining sym>|<file defining __wrap_sym>

a field empty when no source defines it, or more than one does (said on
stderr); then one line per weak declaration in the add-on's sources, which
ld64 must be told may stay undefined (-U):

    weak|name
"""
from __future__ import annotations
import argparse, re, sys
from pathlib import Path

# A recompiled shard's routine, as the code generator spells every one.
GUEST = r'^void {name}\(CPUState\* cpu\)'
# A host definition: from column 0, maybe extern "C", not a declaration
# (the line does not end with ';'); one-line bodies count.
HOST = r'^(?:extern\s+"C"\s+)?(?!extern\b)(?!static\b)[A-Za-z_][A-Za-z0-9_ \t\*]*\b{name}\s*\('
WEAK = re.compile(r'\b([A-Za-z_]\w*)\s*\([^;{\n]*\)\s*__attribute__\(\(weak\)\)\s*;')


def defines(text: str, name: str) -> bool:
    if re.search(GUEST.format(name=re.escape(name)), text, re.M):
        return True
    head = re.compile(HOST.format(name=re.escape(name)))
    return any(head.match(line) and not line.rstrip().endswith(';') for line in text.splitlines())


def under(path: Path, folder: Path) -> bool:
    try:
        path.relative_to(folder)
        return True
    except ValueError:
        return False


def main() -> int:
    a = argparse.ArgumentParser()
    a.add_argument('--sources', type=Path, required=True)
    a.add_argument('--ours', type=Path, required=True)
    a.add_argument('symbols', nargs='*')
    args = a.parse_args()
    ours = args.ours.resolve()
    texts, mine = {}, {}
    for line in args.sources.read_text(encoding='utf-8').splitlines():
        if not line.strip():
            continue
        path = Path(line.strip())
        if path.suffix.lower() not in ('.c', '.cc', '.cpp', '.cxx', '.m', '.mm'):
            continue
        try:
            text = path.read_text(encoding='utf-8', errors='replace')
        except OSError:
            print(f'tmst_wraps: {path} not there yet (made by the build)', file=sys.stderr)
            continue
        (mine if under(path.resolve(), ours) else texts)[path] = text

    def one(name: str, what: str, sym: str) -> str:
        hits = sorted({str(p) for p, t in texts.items() if defines(t, name)})
        if len(hits) > 1:
            print(f'tmst_wraps: {sym}: {what} in {len(hits)} sources ({", ".join(hits)})', file=sys.stderr)
        return hits[0] if len(hits) == 1 else ''

    for sym in args.symbols:
        print(f'{sym}|{one(sym, "defined", sym)}|{one("__wrap_" + sym, "__wrap_ defined", sym)}')
    weak = sorted({m.group(1) for t in mine.values() for m in WEAK.finditer(t)})
    for name in weak:
        print(f'weak|{name}')
    return 0


if __name__ == '__main__':
    sys.exit(main())
