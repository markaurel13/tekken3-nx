#!/usr/bin/env python3
"""Tekken 2 Content's Stages, Remake: the user's backgrounds, kept as patches
keyed to the Tekken 2 disc.

    python tools/t2_remake_patch.py apply [--disc <Tekken 2 disc>]
    python tools/t2_remake_patch.py check
    python tools/t2_remake_patch.py make --export <folder of the *-full.zip> [--export ...] [--disc ...]   (developers)

The Remake's backgrounds are the user's own pictures, painted over Tekken 2's
panoramas (the old fork's stage zips, pictures/*.png). They are not in git
as pictures (the user's, 2026-11-05: "elég, ha lemez nélkül nem lehet
visszanyerni"): each is a patch, tools/data/t2-remake-patches/<name>.t2p,
the picture compressed without loss (WebP) and mixed with a key stream made
from the disc's own data for its stage. Without the disc a patch is noise;
with it, apply writes the pictures to workspace/t2-stages/pictures/ as they
were, their pixels checked by their SHA-256.

The key: SHA-256 of the stage's fighter's BNS group (its textures and its
record, entries GROUP0 + GROUP * fighter and the next, as the disc holds
them; tools/tekken2_stages.py). The key stream: SHAKE-256 of the content
key and the picture's name. A patch may open with more than one disc
version: each has its own key, which unwraps the same content key.

A patch:
  "T2RPAT01"; u16 stage, u8 mode (0 RGB, 1 L), u8 name's length, u32 width,
  height, the payload's length; u8 keys, 3 bytes 0; the name (UTF-8),
  padded to 4; per key 16 bytes of its check (SHA-256 of "check" and the
  disc's key) and 32 of the content key mixed with the disc's; 32 bytes the
  pixels' SHA-256; then the payload (the WebP's bytes, mixed).
"""
from __future__ import annotations

import argparse
import hashlib
import io
import json
import struct
import sys
import zipfile
import zlib
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ADDON / 'tools'))
import tekken2_stages as classic  # noqa: E402
from disc_image import DiscError, open_disc  # noqa: E402

# Increase whenever what apply writes changes: the setup writes them again.
VERSION = 1
MAGIC = b'T2RPAT01'
PATCHES = ADDON / 'tools/data/t2-remake-patches'
PICTURES = 'pictures'                       # in t2-stages/ (never staged)
RECIPES = ADDON / 'tools/data/t2_remake_stages.json'
MODES = ('RGB', 'L')
MAX_TOTAL = 24 * 1024 * 1024                # check_repo: all the patches together


def pictures_of(cfg) -> list[str]:
    """The picture files a recipe names ("panorama", a sign's "text", ...)."""
    if isinstance(cfg, dict):
        return [n for v in cfg.values() for n in pictures_of(v)]
    if isinstance(cfg, list):
        return [n for v in cfg for n in pictures_of(v)]
    return [cfg] if isinstance(cfg, str) and cfg.endswith('.png') else []


def wanted() -> dict:
    """{picture: Tekken 2 stage} of every picture the Remake's recipes name."""
    data = json.loads(RECIPES.read_text(encoding='utf-8'))
    out = {}
    for owner, cfg in data['arcade'].items():
        stage = cfg.get('stage', data['owners'].get(owner))
        for name in pictures_of(cfg):
            out[name] = stage
    return out


def disc_key(src: classic.Source, stage: int) -> bytes:
    """The key a disc gives a stage: its fighter's BNS group as the disc holds it."""
    group = classic.GROUP0 + classic.GROUP * classic.FIGHTER_OF[stage]
    h = hashlib.sha256(b'TMST t2p key')
    for i in (group, group + 1):
        h.update(hashlib.sha256(src.entry(i)).digest())
    return h.digest()


def check_of(key: bytes) -> bytes:
    return hashlib.sha256(b'TMST t2p check' + key).digest()[:16]


def mixed(data: bytes, key: bytes, name: str) -> bytes:
    """data XOR the key stream (both ways)."""
    import numpy as np
    stream = hashlib.shake_256(key + name.encode('utf-8')).digest(len(data))
    return (np.frombuffer(data, np.uint8) ^ np.frombuffer(stream, np.uint8)).tobytes()


def xor(a: bytes, b: bytes) -> bytes:
    return bytes(x ^ y for x, y in zip(a, b))


def pixels_digest(img) -> bytes:
    return hashlib.sha256(img.mode.encode() + struct.pack('<2I', *img.size) + img.tobytes()).digest()


def encode(img, stage: int, name: str, keys: list[bytes]) -> bytes:
    """A patch of img (RGB or L), opened by any of keys (the first the content key)."""
    from PIL import Image
    if img.mode not in MODES:
        raise ValueError(f'{name}: a picture of mode {img.mode}')
    buf = io.BytesIO()
    img.convert('RGB').save(buf, 'WEBP', lossless=True, quality=100, method=6, exact=True)
    webp = buf.getvalue()
    back = Image.open(io.BytesIO(webp)).convert(img.mode)
    if back.tobytes() != img.tobytes():
        raise ValueError(f'{name}: WebP did not keep the picture')
    content = keys[0]
    payload = mixed(webp, content, name)
    raw = name.encode('utf-8')
    head = MAGIC + struct.pack('<HBB3IB3x', stage, MODES.index(img.mode), len(raw), *img.size, len(payload), len(keys))
    head += raw + bytes(-len(raw) % 4)
    for k in keys:
        head += check_of(k) + xor(content, k)
    return head + pixels_digest(img) + payload


def header(data: bytes) -> dict:
    """A patch's fields (no key needed)."""
    if data[:8] != MAGIC:
        raise ValueError('not a Remake picture patch')
    stage, mode, n, w, h, size, keys = struct.unpack_from('<HBB3IB3x', data, 8)
    o = 8 + struct.calcsize('<HBB3IB3x')
    name = data[o:o + n].decode('utf-8')
    o += n + (-n % 4)
    locks = [(data[o + 48 * i:o + 48 * i + 16], data[o + 48 * i + 16:o + 48 * i + 48]) for i in range(keys)]
    o += 48 * keys
    digest, payload = data[o:o + 32], data[o + 32:]
    if len(payload) != size or mode >= len(MODES):
        raise ValueError(f'{name}: a damaged patch')
    return dict(stage=stage, mode=MODES[mode], name=name, size=(w, h), locks=locks, digest=digest, payload=payload)


def decode(data: bytes, key: bytes):
    """The picture of a patch, opened with a disc's key (ValueError: not this disc's)."""
    from PIL import Image
    p = header(data)
    content = next((xor(wrap, key) for check, wrap in p['locks'] if check == check_of(key)), None)
    if content is None:
        raise ValueError(f'{p["name"]}: this disc does not open it (another version of Tekken 2?)')
    try:
        img = Image.open(io.BytesIO(mixed(p['payload'], content, p['name']))).convert(p['mode'])
    except OSError as error:
        raise ValueError(f'{p["name"]}: {error}') from None
    if img.size != p['size'] or pixels_digest(img) != p['digest']:
        raise ValueError(f'{p["name"]}: the picture is not the one patched')
    return img


def patch_files() -> list[Path]:
    return sorted(PATCHES.glob('*.t2p'))


def apply(disc: Path, out: Path, say=print) -> list[str]:
    """The pictures into out/pictures (names relative to out). A picture
    already there with the patch's pixels stays; one that does not open
    (another disc, a damaged patch) is left out, and said; nothing else
    stays in the folder."""
    from PIL import Image
    folder = out / PICTURES
    folder.mkdir(parents=True, exist_ok=True)
    names, keys = [], {}
    with open_disc(disc) as d:
        src = classic.Source(d)
        for path in patch_files():
            data = path.read_bytes()
            try:
                p = header(data)
                target = folder / p['name']
                try:
                    with Image.open(target) as have:
                        if pixels_digest(have.convert(p['mode'])) == p['digest']:
                            names.append(f'{PICTURES}/{p["name"]}')
                            continue
                except OSError:
                    pass
                if p['stage'] not in keys:
                    keys[p['stage']] = disc_key(src, p['stage'])
                img = decode(data, keys[p['stage']])
            except ValueError as error:
                say(f'Remake picture {path.name}: {error}')
                continue
            part = target.with_name(target.name + '.part')
            img.save(part, 'PNG')
            part.replace(target)
            names.append(f'{PICTURES}/{p["name"]}')
    kept = {n.split('/', 1)[1] for n in names}
    for f in folder.iterdir():                  # older pictures, another disc's: gone
        if f.is_file() and f.name not in kept:
            f.unlink()
    return names


def check(paths: list[Path] | None = None) -> list[str]:
    """What check_repo asks of the patches, without a disc: each a patch of a
    picture the recipes name, its payload not a picture as it is (no WebP,
    PNG or JPEG signature, as random as mixing makes it), within MAX_TOTAL."""
    errors, total, want = [], 0, wanted()
    for path in patch_files() if paths is None else paths:
        data = path.read_bytes()
        total += len(data)
        try:
            p = header(data)
        except (ValueError, struct.error, UnicodeDecodeError) as error:
            errors.append(f'{path.name}: {error}')
            continue
        if want.get(p['name']) != p['stage']:
            errors.append(f'{path.name}: {p["name"]} is no picture of stage {p["stage"]} in the recipes')
        body = p['payload']
        if any(sig in body[:64] for sig in (b'RIFF', b'WEBP', b'\x89PNG', b'\xff\xd8\xff')):
            errors.append(f'{path.name}: the payload is a picture as it is')
        sample = body[:1 << 20]
        if sample and len(zlib.compress(sample, 9)) < 0.995 * len(sample):
            errors.append(f'{path.name}: the payload is not mixed')
    if total > MAX_TOTAL:
        errors.append(f'the patches: {total} bytes (limit {MAX_TOTAL})')
    return errors


def make(exports: list[Path], discs: list[Path], say=print) -> None:
    """(Developers) the patches of the pictures in the exports' *-full.zip
    (pictures/; a later export's picture over an earlier one's of the same
    name), keyed to the given discs (the first one's key the content key)."""
    from PIL import Image
    want = wanted()
    found = {}
    for z in [z for export in exports for z in sorted(export.glob('*-full.zip'))]:
        with zipfile.ZipFile(z) as f:
            for info in f.infolist():
                name = info.filename.rsplit('/', 1)[-1]
                if info.filename.startswith('pictures/') and name in want:
                    found[name] = f.read(info)
    missing = sorted(set(want) - set(found))
    if missing:
        raise SystemExit('no picture in the zips for: ' + ', '.join(missing))
    stage_keys: dict = {}
    for disc in discs:
        with open_disc(disc) as d:
            src = classic.Source(d)
            for stage in sorted(set(want.values())):
                stage_keys.setdefault(stage, [])
                k = disc_key(src, stage)
                if k not in stage_keys[stage]:
                    stage_keys[stage].append(k)
    PATCHES.mkdir(parents=True, exist_ok=True)
    for old in patch_files():
        old.unlink()
    for name, raw in sorted(found.items()):
        img = Image.open(io.BytesIO(raw))
        img = img.convert('L' if img.mode in ('L', 'LA', '1') else 'RGB')
        data = encode(img, want[name], name, stage_keys[want[name]])
        (PATCHES / (Path(name).stem + '.t2p')).write_bytes(data)
        say(f'{name}: stage {want[name]}, {img.size[0]} x {img.size[1]} {img.mode}, {len(data)} bytes, '
            f'{len(stage_keys[want[name]])} disc key(s)')


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    sub = ap.add_subparsers(dest='cmd', required=True)
    a = sub.add_parser('apply', help='the pictures from the patches, with your Tekken 2 disc')
    a.add_argument('--disc', type=Path, help='the Tekken 2 disc (default: the one the setup remembered)')
    a.add_argument('--root', type=Path, default=classic.EXPANDED, help='the Expanded folder')
    sub.add_parser('check', help='the patches as check_repo checks them')
    m = sub.add_parser('make', help='(developers) the patches from the stage zips')
    m.add_argument('--export', type=Path, action='append', required=True,
                   help='a folder of the *-full.zip (more: the later ones win)')
    m.add_argument('--disc', type=Path, action='append', default=[], help='a Tekken 2 disc (more: more versions)')
    args = ap.parse_args(argv)
    if args.cmd == 'check':
        errors = check()
        for e in errors:
            print('ERROR', e)
        print('t2 remake patches:', 'OK' if not errors else f'{len(errors)} problem(s)')
        return 1 if errors else 0
    discs = args.disc if args.cmd == 'make' else [args.disc] if args.disc else []
    if not discs:
        found = classic.find_disc(ADDON / 'workspace')
        if not found:
            print('no Tekken 2 disc (give --disc, or run the setup)')
            return 1
        discs = [found]
    try:
        if args.cmd == 'make':
            make(args.export, discs)
        else:
            names = apply(discs[0], args.root / classic.OUT)
            print(f'{len(names)} of {len(patch_files())} pictures')
    except DiscError as error:
        print(error)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
