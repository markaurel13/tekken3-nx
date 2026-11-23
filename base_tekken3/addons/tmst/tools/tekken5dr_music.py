#!/usr/bin/env python3
"""The Jukebox's TEKKEN 5 DR music, from the player's Tekken: Dark Resurrection
PSP disc (USA, ULUS10139; the ISO the Expanded's setup was given, its
.setup/last-inputs.json's dr_iso) into addons/tmst/workspace/t5dr-music/*.bgm
(run by the add-on's setup, setup/tmst_setup.py).

    python tools/tekken5dr_music.py [--iso <Dark Resurrection ISO>] [--root <expanded folder>]

The disc: an ordinary ISO 9660 (2 048-byte sectors); the game's data is
PSP_GAME/USRDIR/TKPDATA1.BIN, files without names, each a number in the
EBOOT's table (the Expanded's tools/psp/dr_iso.py reads it whole; most of its
files are encrypted). The music is not: 49 plain NPSF files (Namco's stream),
found at the sectors of PIECES and checked by their SHA-256 (another dump or
version: that piece is left out, logged). The pieces --disable in
data/jukebox/tekken5-dr.txt are not made (skipped()).

NPSF: 'NPSF', u32 block (0x1000: both channels'), u32 bytes per channel, u32
channels (2), u32 data offset (0x800), s32 loop start in samples (-1: none),
u32 sample rate (44 100), u32 volume (1 000 = full), then the name (0x34). The
data is PS-ADPCM (16-byte blocks of 28 samples), half a block (0x800 bytes) of
the left channel, then 0x800 of the right, and so on; the last block is
shorter, its first half left, the rest right. (Read as 0x1000 a channel, the
channels mix every 0.16 s: the pieces stutter, their left and right do not
correlate; at 0x800 they do, 0.9 and more.)

.bgm: an ADPCM pack (tools/adpcm_pack.py, src/jukebox/tmst_adpcm.c), the
NPSF's PS-ADPCM as it is, decoded by the Jukebox as it plays; its loop the
NPSF's (none: 0, the beginning), its gain the NPSF's volume times one gain for
all (LEVEL), so the pieces keep the game's levels between them and sit near
Tekken 3's music loudness.

No ISO: nothing is made (and nothing removed). Half a minute or so.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import struct
import sys
from concurrent.futures import ProcessPoolExecutor
from pathlib import Path

ADDON = Path(__file__).resolve().parents[1]
EXPANDED = ADDON.parents[1]                 # <expanded>/addons/tmst
sys.path.insert(0, str(ADDON / 'tools'))
import adpcm_pack  # noqa: E402

OUT = 'addons/tmst/workspace/t5dr-music'
# Increase whenever the .bgm files this writes change: the setup makes them again.
VERSION = 3

SECTOR = 2048
ARCHIVE = '/PSP_GAME/USRDIR/TKPDATA1.BIN'
RATE, FULL_VOLUME = 44100, 1000
# The gain on top of the NPSF's volume: the pieces' RMS near Tekken 3's music (the Jukebox's other packs,
# tools/tekken2_music.py RMS: 5 147); measured, the stage pieces' RMS with their volume is 8 000-9 500.
LEVEL = 0.6
JOBS = max(1, min(6, os.cpu_count() or 2))

# file name, TKPDATA1 file number, first sector in TKPDATA1.BIN, size, SHA-256 (NPSF's own name).
PIECES = [
    ('t5dr-character-select.bgm', 6, 1464, 2865152, 'a2a835f6a9f731f05ea508e97728d852eef3587bcbe463ddf85ec04980a9ec1c'),  # tkdr_PSP_ch_sele
    ('t5dr-rannyu.bgm', 7, 2863, 186368, 'e071c272e15660925d1a24c470d3cd092cec98a72395cdd60fa1a1ce6563d4ed'),  # rannyu_re.wav
    ('t5dr-prologue.bgm', 8, 2954, 2693120, '47c19933f814a3b9d87653dde12ff8b4caff6adaa5b8f469c3fba81be4ef86ca'),  # Prologue_Master_
    ('t5dr-continue.bgm', 9, 4269, 3188736, 'f540d43dacd07f73cba6619a0dac92f4fc57bd7f3bc69932a25d7fc4290e83ec'),  # continu_lp32.wav
    ('t5dr-taki-se.bgm', 2843, 234295, 5339136, '72804ec2ab796910181e47b46c2a9d2d5fc46453cffb37eddbf429fe7baa79eb'),  # Taki_se.wav
    ('t5dr-jinpachi.bgm', 2844, 236902, 1136640, '0302315197306f0a2fe6f8e4774c9e9fe68e51efec4ac9c2ab8cee9f1d3d9fe0'),  # Jinpachi Toujoh_
    ('t5dr-haikyo-se.bgm', 2845, 237457, 6576128, '2af0e980e06a122f0bdefaeab45e136fb09b874bcf6ea63e7efca7924ab61956'),  # Haikyo2_PSP_mast
    ('t5dr-tora-se.bgm', 2846, 240668, 9635840, 'a46fa5fdbf00d67933baed8f99aadaf8b5a80fede43ae848ab32b03c6d1944e5'),  # Tora_Master_lp_s
    ('t5dr-koyo.bgm', 2847, 245373, 10897408, '38d46b80dcd70254c3b335281aa3ecb48ae4aa80d31a733003083309fea57b26'),  # Koyo_Master_lp.w
    ('t5dr-nagareboshi.bgm', 2848, 250694, 10131456, '07af6076bc95f4b5a6af2255234a3a003dba04c6f93beccfc0f8437eeef95431'),  # Nagareboshi_Mast
    ('t5dr-ring.bgm', 2849, 255641, 7161856, 'a3424e99b9f146da4ea7a9fc889b68463ee739374d562913c38ebf0c56a8db0a'),  # Ring2_Master_lp.
    ('t5dr-yakei.bgm', 2850, 259138, 7575552, '69d3f7ee47648c699149c8eb78ee0958af5d6d189eab02a63b530a38585200de'),  # Yakei_Master_lp.
    ('t5dr-sougen.bgm', 2851, 262837, 3405824, '3e289e7ecd82e4856c164bc005bc6179298c2e770adfad5b53cf0b21d3d8efaf'),  # Sougen_Master_te
    ('t5dr-kaizokusen-se.bgm', 2852, 264500, 9054208, 'a77d5f5653f04d85272976a6246a6074e21c5a9f13f7084c6c73ca73e26c28be'),  # Kaizokusen_Maste
    ('t5dr-halloween.bgm', 2853, 268921, 9267200, '32ba603c32b5613f998aca5dc203af95527c2e838a717a7c82a2569e351191de'),  # Hallowin_Master_
    ('t5dr-noumu.bgm', 2854, 273446, 6658048, 'e0c84a0068582e197d03b2849b2ba24b963813e054d5deef132a40675ccb9fba'),  # Noumu_Master_se_
    ('t5dr-matsuri-se.bgm', 2855, 276697, 6739968, '095a3d905249c59e232ce734d66ea0327fb58bcda83b6d24810342f13f67dd40'),  # Matsuri_Master_s
    ('t5dr-ohrora-se.bgm', 2856, 279988, 6223872, '6c0f367bde7848b15e1b0a1213ffe57405e4d676783387e1f8b1f04d1435129a'),  # Ohrora_Mstr_se_l
    ('t5dr-gokuraku.bgm', 2857, 283027, 6780928, '1a2c62385226227ba2dcce032f2833bdcb94cf1049245dfafd350332d926e234'),  # Gokuraku_Master_
    ('t5dr-colony.bgm', 2858, 286338, 5588992, '19d79a6e3165633c96fe5d79c6fd4a64a99dd5bcb8ac9eac7aea415838d04bfc'),  # Colony2_Master_l
    ('t5dr-yakai.bgm', 2859, 289067, 5072896, '07f02a71cd472eb808f0e757a6b0c1325161f0d92a757f5ee93f5081767becb1'),  # Yakai_Master_lp.
    ('t5dr-lovely.bgm', 2860, 291544, 6293504, '8707af5e2a27eaee4fac658526b46645cfdc1ba321d111296cd7f33e8b499500'),  # Lovely_Master_Lp
    ('t5dr-ohkami.bgm', 2861, 294617, 8030208, '64149e8490ad2279f908918850c425024fac6401817e08fb6d0c50e7e4b9723a'),  # Ohkami_Master_lp
    ('t5dr-koujou.bgm', 2862, 298538, 7006208, '862d47ed3e350ca357100f496923becd0969d4c4175146eac5560abc1e3eb78d'),  # koujou_looped.wa
    ('t5dr-yukinoshiro-se.bgm', 2863, 301959, 7587840, 'bb37bdf9d6d0fbde026fe7832621ac09be4b8dd7be8db2b1fd3429ac59e3fb6b'),  # Yukinoshi_Mastr_
    ('t5dr-boss-se.bgm', 2864, 305664, 7522304, 'fd5933f2c0fad9496f11201c9ca4de245a6c508d6f7f782fb536ccecfd9f198a'),  # Boss2_Master02_s
    ('t5dr-taki.bgm', 2865, 309337, 5339136, 'b4aaed67db5a4aaf549efc04e602ede0d344c75ab0e3e541fb3d2c3f9ced9064'),  # Taki.wav
    ('t5dr-haikyo.bgm', 2866, 311944, 6576128, '8aaf9d1540031c13d2381b8ec1d627f3fe3c3af333bac910feb51e0e2d70e402'),  # Haikyo2_PSP_mast
    ('t5dr-tora.bgm', 2867, 315155, 9639936, '1a4fd581af6a8e137252dfc3907e4188286db5c7a09b63e58fcb9e2704cf52f4'),  # Tora_Master_lp.w
    ('t5dr-kaizokusen.bgm', 2868, 319862, 9054208, 'c0dc913e6cfb8721ba923b2d1167b3a13a07614aac478933d7b86cf6f302e85b'),  # Kaizokusen_Maste
    ('t5dr-matsuri.bgm', 2869, 324283, 6719488, 'a51ae406fcacfbf379816095b28e97b1a2ce69adfdab642941ff542fad0fe6b2'),  # Matsuri_Master_l
    ('t5dr-ohrora.bgm', 2870, 327564, 6223872, '232998bc55ae927bf0b4295b5dcd5cff20c40df4259a01d13520e5305bb12dd2'),  # Ohrora_Master_lp
    ('t5dr-yukinoshiro.bgm', 2871, 330603, 7587840, '3d7fee9ca98b79e47d8beeec4523dfce97b9a68754e8916b8c5fa7cab9c6b0a0'),  # Yukinoshi_Master
    ('t5dr-boss.bgm', 2872, 334308, 7522304, '914634483ea2cf4a867770b9b1fa523dd8dc871914492aa9dd2a3512e6f7911a'),  # Boss2_Master02_l
    ('t5dr-name-entry.bgm', 2873, 337981, 4073472, '2a5a2f5fa7849ad6ebe7605d8508bfd189f04cc7add8f043440b4a60539ed7b5'),  # NameEntry_Master
    ('t5dr-game-over.bgm', 2874, 339970, 460800, 'bf94ba838631b4ccbf0fb3e8d33b499378aace472bfc619c9b990620a6bf9380'),  # Gameover_Master_
    ('t5dr-staff-roll.bgm', 2875, 340195, 11012096, 'becc7ef1e23ad629db8859b6e569a275ab3fdeec890f5e88391c0025248f95d1'),  # StaffRoll_DDR_PS
    ('t5dr-epilogue.bgm', 2876, 345572, 2021376, '7ee8fad89761c88ed12b5cef5bfa90982b7aae34ac721551fcf182ffaf757d84'),  # Epilogue_Master.
    ('t5dr-heat.bgm', 2877, 346559, 2713600, '3099f102265bec56e6c8a8a9175603790653f539bdf4e61947526bc3835e6f70'),  # 01_HEAT_MASTER.w
    ('t5dr-dark.bgm', 2878, 347884, 4401152, 'd49ff8c0ae82e41a47a8234602180ad7e0e933fbd1ae87cfe00c17793bf5cbc1'),  # 03_DARK_MASTER.w
    ('t5dr-brave.bgm', 2879, 350033, 3700736, '3cc2bd596a8ed9725cb5ad8b4f90a211335742ed7b7f88e4b4ef5b1d14fed62d'),  # 02_BRAVE_MASTER.
    ('t5dr-sorrow.bgm', 2880, 351840, 4241408, '101accdbc775547482c4c582aa20c76339cbfe9e163e2184d73aef4984f642e5'),  # 04_SORROW_MASTER
    ('t5dr-comical.bgm', 2881, 353911, 3545088, '6d18a62e02a07b244b512c19dfd687d88debc3b8e2835ca5e09408d06500369d'),  # 05_COMICAL_MASTE
    ('t5dr-practice.bgm', 2882, 355642, 11032576, 'b5b59c80111f2c0984589ed991a8526a714cad1d0c4b147ce36c33966eafb766'),  # Practice_DDR_PSP
    ('t5dr-tekken-bowl.bgm', 2883, 361029, 9320448, '02fb50cbb21a81c8869482c9b5f74aaaab38e94de655ca114bd55c5fdb319062'),  # Discobowl_DDR_Ma
    ('t5dr-tsunagi.bgm', 2885, 365580, 2209792, 'eeb23035774a274d8b98c94da14c5e8b07de18b4d1f767d048b3463b4759f601'),  # Tsunagi_DDR_PSP_
    ('t5dr-tournament.bgm', 2886, 366659, 4597760, 'e7d7e78b91bebd22daf468e90ba2bdc06797fbbd1df117220486e4f4183354ac'),  # Tornament_DDR_PS
    ('t5dr-map.bgm', 2887, 368904, 3954688, 'a0f3ea736cb4b2a9557b532abfb4adbde2edf6070a2543df9e2eccb17f87ac00'),  # Map_DDR_PSP_Mast
    ('t5dr-mode-select.bgm', 2888, 370835, 3164160, '63325a86091398dcae820f4c6ae63fc30ac3a82363fde0250ed5be479c2bff82'),  # ModeSelect_DDR_P
]


class IsoError(Exception):
    pass


def find_iso(root: Path) -> Path | None:
    """The Dark Resurrection ISO the Expanded's setup was last given, if it is still there."""
    try:
        iso = json.loads((root / '.setup/last-inputs.json').read_text(encoding='utf-8')).get('dr_iso')
    except (OSError, ValueError):
        return None
    return Path(iso) if iso and Path(iso).is_file() else None


def locate(iso, path: str) -> tuple[int, int] | None:
    """(first sector, size) of a file of an ISO 9660 image (an open file), or None."""
    def sectors(lba: int, size: int) -> bytes:
        iso.seek(lba * SECTOR)
        return iso.read(size)
    pvd = sectors(16, SECTOR)
    if pvd[1:6] != b'CD001':
        raise IsoError('not an ISO 9660 image')
    record = pvd[156:190]
    for part in path.strip('/').upper().split('/'):
        lba, size = struct.unpack_from('<I', record, 2)[0], struct.unpack_from('<I', record, 10)[0]
        data, at, record = sectors(lba, size), 0, None
        while at < len(data):
            n = data[at]
            if not n:                                   # the rest of this sector is padding
                at = (at // SECTOR + 1) * SECTOR
                continue
            name = data[at + 33:at + 33 + data[at + 32]].decode('ascii', 'replace').split(';')[0].upper()
            if name == part:
                record = data[at:at + n]
                break
            at += n
        if record is None:
            return None
    return struct.unpack_from('<I', record, 2)[0], struct.unpack_from('<I', record, 10)[0]


def header(data: bytes) -> tuple[int, int, int, int, int, int]:
    """(block, bytes per channel, channels, data offset, loop start or -1, volume) of an NPSF."""
    if data[:4] != b'NPSF':
        raise IsoError('not an NPSF stream')
    block, size, channels, offset, loop, rate, volume = struct.unpack_from('<IIIIiII', data, 4)
    if channels != 2 or rate != RATE or not block or block % 32:
        raise IsoError(f'an NPSF of {channels} channel(s) at {rate} Hz')
    return block, size, channels, offset, loop, volume


def channels(data: bytes) -> tuple[bytes, bytes, int, int]:
    """(left, right, loop start or -1, volume) of an NPSF: its two channels' ADPCM."""
    block, size, channel_count, offset, loop, volume = header(data)
    interleave = block // channel_count             # each channel's share of a block
    body = data[offset:]
    step = 2 * interleave
    whole = len(body) // step * step
    if whole // 2 >= size:                          # every block whole
        whole = (size + interleave - 1) // interleave * step
    # The last block is shorter: the rest, half left, half right.
    half = (len(body) - whole) // 2
    left = b''.join([body[at:at + interleave] for at in range(0, whole, step)] + [body[whole:whole + half]])[:size]
    right = b''.join([body[at + interleave:at + step] for at in range(0, whole, step)]
                     + [body[whole + half:whole + 2 * half]])[:size]
    if len(left) != size or len(right) != size:
        raise IsoError('an NPSF cut short')
    return left, right, loop, volume


def make(iso: str, base: int, piece: tuple, out: str) -> str | None:
    """One piece (in a worker process): read, checked, written as an ADPCM pack;
    None when its bytes are not the ones expected."""
    name, _, sector, size, sha = piece
    with open(iso, 'rb') as f:
        f.seek((base + sector) * SECTOR)
        data = f.read(size)
    if len(data) != size or hashlib.sha256(data).hexdigest() != sha:
        return None
    left, right, loop, volume = channels(data)
    adpcm_pack.write(out, left, right, loop if loop > 0 else 0, LEVEL * volume / FULL_VOLUME)
    return Path(out).name


def skipped() -> list[str]:
    """The pieces not made: --disable in the title list (tools/jukebox_titles.py's skipped())."""
    import jukebox_titles
    return jukebox_titles.skipped('t5dr-music')


def convert(iso: Path, out: Path, say=print, jobs: int = JOBS, skip=None) -> list[str]:
    """Every piece of PIECES as out/<file> but those of skip (default
    skipped(), their files removed); the names written. IsoError when the
    image has no TKPDATA1.BIN."""
    skip = set(skipped() if skip is None else skip)
    with open(iso, 'rb') as f:
        found = locate(f, ARCHIVE)
    if not found:
        raise IsoError(f'{iso.name} has no {ARCHIVE}')
    base = found[0]
    out.mkdir(parents=True, exist_ok=True)
    for name in skip:
        (out / name).unlink(missing_ok=True)
    order = sorted((p for p in PIECES if p[0] not in skip), key=lambda p: -p[3])     # the longest first, so the workers end together
    names = []
    with ProcessPoolExecutor(max_workers=max(1, jobs)) as pool:
        futures = [(p, pool.submit(make, str(iso), base, p, str(out / p[0]))) for p in order]
        for piece, future in futures:
            name = future.result()
            if name:
                names.append(name)
            else:
                say(f'Tekken 5 DR music: {piece[0]} is not the one known (another dump?), left out')
    say(f'Tekken 5 DR music: {len(names)} piece(s) in {out}')
    return sorted(names)


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--root', type=Path, default=EXPANDED, help='the Expanded folder (default: the one this add-on is in)')
    ap.add_argument('--iso', type=Path, help="the Dark Resurrection ISO (default: the Expanded setup's dr_iso)")
    ap.add_argument('--jobs', type=int, default=JOBS, help=f'pieces made at a time (default {JOBS})')
    args = ap.parse_args(argv)
    iso = args.iso or find_iso(args.root)
    if not iso:
        print('Tekken 5 DR music: no Dark Resurrection ISO (the Expanded setup asks for it)')
        return 0
    convert(iso, args.root / OUT, jobs=args.jobs)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
