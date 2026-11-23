#!/usr/bin/env python3
"""Tekken 2 Content's Stages, Remake: a Tekken 2 stage drawn by the Tekken 3
arcade-stage engine (the Expanded's src/tekken3_arcade_stages.c and the
add-on's src/tekken2/tmst_tekken2_stages.c).

    python tools/t2_remake_stage.py --owner bruce --pictures DIR --out DIR [--tekken3 tekken3.zip]

The old fork's tools/t2_arcade_stage.py (the user's, the version its stage
zips were made by), ported to the add-on: its stage data from the add-on's
disc reader (tools/t2_remake_disc.py), the Expanded's own
tools/arcade_stages (decode, pack, scene), the recipes in
tools/data/t2_remake_stages.json, the pictures where the caller says
(PICTURES). The rest as it was; what follows is its own description.

The stage as if Tekken 3 had it (tools/data/t2_stages.json "arcade"): an
arcade pack (tools/arcade_stages/pack.py, format pack.FORMAT) for the
owner's arena, which src/tekken3_ttt1_stage.c hands to the arcade engine
(mods/ttt1/arcade-<owner>.ast; the music stays the Tekken 2 fighter's, and
without the Arcade Stages feature the stage of tools/t2_stage.py is drawn).

- the far background: Tekken 3's strip of 64 x 64 cells round the fight, of
  16 colours each, from the stage's panorama: a picture of the player's own
  ("panorama": a file in tools/data/t2_hd or workspace/t2-import/hd, hd(), its ends faded into each
  other over "seam" pixels) or Tekken 2's (its 64 x 32 cells, each with its
  own palette, twice as tall: Tekken 2 drew them at 240 lines, Tekken 3's
  arcade draws 480); "degrees" of the turn per picture, "yoff", "pitch";
  "crop" cuts the player's picture, "erase" takes off of it what the stage
  builds in 3D (erase());
- the floor: Tekken 3's grid, fog and tables (the base arcade stage's,
  "base"), its 10 x 10 cells Tekken 2's 10 x 10 floor tiles with their own
  palettes (turned as Tekken 2 turns them), the near-camera halves each a
  quarter of its tile, magnified twice; "floor_tufts" draws dry grass on
  some of them;
- the tiles ("scenery"): the stage's own landscape, built here (rim(),
  butte(), boulder(), mushroom() and its "stacks", tufts()), textured with "materials" cut from the panorama
  picture (material(): strips of rock whose pictures run on into each
  other) and lit at their corners by a low sun and the sky;
- "grass": dry tufts round the fight, as objects (grass_sections());
- "dust": puffs the runtime raises where a fighter hits the ground (DUST);
- "eagle": four models and its perches on the buttes' tops (OBJS, ROTT,
  OBJA 200, EAGL): the runtime flies it round the canyon and lands it;
- "ship" (OBJA 202, SHIP), "gulls" (OBJA 203, GULL): a ship passing on the
  bay, gulls circling over the beach;
- "blocks", "rubble", "gears", "phone_box": London's ruins in the sand.
- "crates", "barrels", "spools", "cones", "beacons", "steams": the launch pad's
  hangar floor; the beacons' lamps turn, their light sweeps the floor, steam
  rises (ANIM: beacon_frames()); "drones" fly round as the gulls do.
- "night": no panorama, a black room (Mirror Darkness); "mirror" (MIRR): its
  mirror round the camera as tools/ttt1_stage.py mirror() (drawn by
  src/tekken3_ttt1_stage.c), its frame and the glow at its foot; "fogs":
  sheets of mist drifting low round the fight (ANIM: fog_frames());
  "feathers" (FTHR): white feathers floating down round the fight, rocking
  and turning over (drawn by src/tekken3_arcade_stages.c feathers()).
- the beach: "sea" (sea_frames(), flame_frames(): one ANIM of the panorama's
  sea row and the torches' flame), "lifeguard" (its tower), "tikis",
  "coconuts", "driftwood", "surfboard", "life_ring", "bits" (shells and
  starfish); "gulls" "crabs" (GULL: crabs on the sand after the birds).
- the Korean temple: "seokdeungs" (stone lanterns, their windows the lamp
  picture's flicker), "lotus_lines" (lotus lanterns on ropes),
  "jangseungs" (guardian posts), "cairns", "seoktap" (a stone pagoda),
  "onggis" (earthenware jars); "magpies" fly as the gulls do.
- the Taj Mahal: "pool" (pool_frames(), mughal_frames(): one ANIM of the
  panorama's water row, its reflection rippling and the moon glittering on
  it, and the lamps' flicker), "mughal_lamps" (brass lamps on posts),
  "diya_lines" (oil lamps on the ground), "cypresses" (in marble pots),
  "jalis" (pierced marble screens), "kalashas" (brass pots with marigolds),
  "fountains" (lotus basins, their spray); "bats" fly as the gulls do,
  fluttering; "fireflies" (FFLY) glow and fade low round the fight.
- Szechwan Fields: "clouds" (cloud_frames(), szechwan_frames(),
  smoke_frames(): one ANIM of the panorama's cloud row billowing, the
  prayer flags fluttering and the incense smoke rising), "sky" (the
  picture's sky lengthened up to that blue), "bamboos" (living clumps),
  "tea_rows" (clipped hedges), "pailou" (a painted gate), "censer" (a
  bronze ding, its smoke), "flag_lines" (prayer flags), "wine_jars",
  "sheaves", "baskets", "mossy_stones"; "cranes" fly as the gulls do, in
  a V; "fireflies" "kind" "butterfly" (FFLY): butterflies by day.
- Morning Fields: "clouds" as Szechwan's (the flags' red a scarecrow's rag),
  "grade" a morning haze on the horizon, "bales" (round hay bales),
  "walls" (dry-stone), "rail_fences" (with a gate), "cross" (a wayside
  cross), "cart" (a hay cart), "scarecrow", "skeps" (beehives on a bench),
  "sunflowers", "flowers" (wild flower clumps, as the tufts); "doves" fly
  as the gulls do, "rabbits" hop round the fight after them (GULL kind 1),
  "feathers" "petal" white: dandelion fluff.
- Serene Forest: "wrap" (one turn of a picture that repeats, its start
  crossfaded from past its end), "foot" (the trunks going into the dark
  under them), "grass" with ferns round the fighters, "forest"
  (forest_pictures(): the bugs' glow, the fern); built but left out for
  now: "seam_repeat", "clouds" "extras" false (a row swaying alone in the
  ANIM), sun_pulse() (ANI4: "sun_patches"' light and beams brightening
  and fading), "fogs" as ANI2, "logs", "stumps", "mushrooms";
  "fireflies" "kind" "bugs" (FFLY kind 2: glowing bugs of four colours),
  "feathers" "leaves" (FTHR shape 4: autumn leaves strewn, kicked up, one
  or two falling from the trees).
- the Great Wall: "squeeze" (the sky pressed down: its thin clouds over the
  hills, where the camera sees them), "clouds" "picture" "great-wall"
  (great_wall_frames(): war banners, the beacon's fire, a dragonfly, a puff
  of dust in the flags' place; smoke_frames() "plume"), "battlements" (the
  parapet in ruins), "rubble_bricks", "beacon_towers" (fire and smoke on
  top), "war_banners"; "crows" fly in a flock; "fireflies" "kind"
  "dragonfly" (FFLY kind 3: hovering and darting), "feathers" "dust" (FTHR
  shape 5: wisps the wind drives along the ground).

Game data and pictures made from it stay in workspace/: written to
workspace/t2-import/stages/arcade-<owner>.ast and copied into the staged
catalogue and each build's mods/ttt1.
"""
from __future__ import annotations

import argparse
import copy
import dataclasses
import json
import os
import struct
import sys
from pathlib import Path

import numpy as np
from PIL import Image

ADDON = Path(__file__).resolve().parents[1]
ROOT = ADDON.parents[1]                  # the Expanded: its tools/arcade_stages
sys.path[:0] = [str(ADDON / 'tools'), str(ROOT / 'tools/arcade_stages')]
import decode  # noqa: E402
import pack  # noqa: E402
import scene  # noqa: E402
import t2_remake_disc as t2_stage  # noqa: E402
from t2_glyphs import FONT3, FONT5, GLYPHS, neon_words, stamp, text as glyph_text  # noqa: E402

DATA = json.loads((ADDON / 'tools/data/t2_remake_stages.json').read_text(encoding='utf-8'))
# Where the pictures ("panorama", a material's "source", the neon sign's
# "text") are looked for, and the lightning's "like" pack: the caller's.
PICTURES: list = []
LIKE: dict = {}
# The high half's rectangles no picture goes on (the add-on's: Unknown's
# wolf, TTT Hitspark's fixed cells; empty: the old fork's packs to the byte).
# With it too, the writing on the stages is real words in the country's
# language (tools/t2_glyphs.py), where the old fork drew made-up strokes.
RESERVED: list = []


def hd(name: str) -> Path:
    """A picture of PICTURES: the first folder that has it."""
    for folder in PICTURES:
        if (Path(folder) / name).is_file(): return Path(folder) / name
    raise SystemExit(f'{name}: no such picture in ' + ' or '.join(str(d) for d in PICTURES))


def like_pack(name: str) -> Path:
    """The lightning's "like" pack (a TTT1 stage's LGHT, THND): where the caller says."""
    return Path(LIKE.get(name, name))
PACK_STAGE = 200                     # a pack's stage number: 200 + Tekken 2 stage (no Tekken 3 stage's special cases)
CLUT_ROW0 = 100                      # palettes laid out from this row of the low half (pack.Relocation moves them up)
# Palettes a low half row (pack.Relocation moves a row as one run); the
# add-on's tools/t2_remake_stages.py makes it shorter for a stage whose runs
# find no room in a full high half otherwise (16: any slot's width)
CLUT_RUN = 64
# (the add-on's) the Pagoda lights' palettes (pagoda_lights(): eyes or rain,
# candles) in runs of this many, each its own animation; None: one run each
PG_CHUNK = None
# (the add-on's) palette_frames() past the VRAM (tools/t2_remake_stages.py:
# the old fork's last five only, the other stages' packs as they were)
PALETTE_RAM = False
RAM_SLOT = 64 * 64                   # (the add-on's) Vram.ram()'s first slot: far past the VRAM (y 4608)


class Vram:
    """System 12 VRAM for the pack: 4-bit 64 x 64 pictures in the high half
    (16 x 64 halfwords, in columns), palettes in the low half."""

    def __init__(self):
        self.v = np.zeros((1024, 1024), np.uint16)
        self.rects, self.slot, self.cluts = [], 0, 0
        self.holes = []                  # slots skipped by reserve(), taken first
        self.clut_run = CLUT_RUN         # palettes a row (pack.Relocation moves each row as one run)
        self.blocked = set()             # slots another's textures hold (King's helicopter: block())
        # (the add-on's) an animation's frames kept out of the VRAM: in ram(),
        # pictures go to slots from RAM_SLOT on (y 1024 on, past the VRAM),
        # which tools/t2_remake_stages.py takes out of the pack's VRAM into T2AN
        self.in_ram, self.ram_slot = False, RAM_SLOT
        self.same = {}                   # (the add-on's) pictures by their texels and colours

    def ram(self):
        """With the add-on's RESERVED: what is made in it goes past the VRAM."""
        import contextlib
        vr = self

        @contextlib.contextmanager
        def frames():
            was, vr.in_ram = vr.in_ram, bool(RESERVED)
            try:
                yield
            finally:
                vr.in_ram = was
        return frames()

    def _ram_run(self, n: int) -> int:
        if self.ram_slot % 64 + n > 64:
            self.ram_slot = (self.ram_slot // 64 + 1) * 64
        at, self.ram_slot = self.ram_slot, self.ram_slot + n
        rows = 512 + (self.ram_slot // 64 + 1) * 64
        if rows > self.v.shape[0]:
            self.v = np.concatenate([self.v, np.zeros((rows - self.v.shape[0], 1024), np.uint16)])
        return at

    def frame_run(self, n: int) -> int:
        """reserve(n) for an animation's frame (past the VRAM in the add-on's packs)."""
        with self.ram():
            return self.reserve(n)

    def frame(self, *args, **kw):
        """picture() for an animation's frame (past the VRAM in the add-on's packs)."""
        with self.ram():
            return self.picture(*args, **kw)

    def block(self, x: int, y: int, w: int, h: int) -> None:
        """The slots under the rectangle (x, y, w, h) of the high half kept
        free (textures put there at their own place)."""
        for sy in range((y - 512) // 64, (y - 512 + h + 63) // 64):
            for sx in range(x // 16, (x + w + 15) // 16):
                self.blocked.add(64 * sy + sx)

    def reserve(self, n: int) -> int:
        """n slots side by side in one row of the VRAM (an animation's
        rectangle); the row's rest skipped goes to the next pictures."""
        if self.in_ram:
            return self._ram_run(n)
        while True:
            if self.slot % 64 + n > 64:
                nxt = (self.slot // 64 + 1) * 64
                self.holes += [s for s in range(self.slot, nxt) if s not in self.blocked]
                self.slot = nxt
            hit = [s for s in range(self.slot, self.slot + n) if s in self.blocked]
            if not hit:
                break
            self.holes += [s for s in range(self.slot, hit[-1]) if s not in self.blocked]
            self.slot = hit[-1] + 1
        at, self.slot = self.slot, self.slot + n
        return at

    def picture(self, idx: np.ndarray, colours: list[int] | None, at: int | None = None,
                clut: tuple[int, int] | None = None) -> tuple[int, int, tuple[int, int]]:
        """(x, y) of the 64 x 64 indices, and (x, y) of their palette; at:
        a reserved slot; clut: the palette of another picture (colours
        None), an animation's frame."""
        reuse = None
        if at is None and RESERVED and not self.in_ram:
            # (the add-on's) a picture already here, the same to the texel and
            # colour, taken again (the Classic's repeated places, the Church's bays)
            reuse = (idx.astype(np.uint8).tobytes(), tuple(int(c) for c in colours) if colours is not None else clut)
            if reuse in self.same:
                return self.same[reuse]
        if at is None and self.in_ram:
            at = self._ram_run(1)
        if at is None:
            if self.holes:
                at = self.holes.pop(0)
            else:
                while self.slot in self.blocked:
                    self.slot += 1
                at, self.slot = self.slot, self.slot + 1
        x, y = (at % 64) * 16, 512 + (at // 64) * 64
        if y + 64 > 1024 and at < RAM_SLOT:
            raise SystemExit('the pictures do not fit the high half of the VRAM')
        idx = idx.astype(np.uint16)
        if reuse is not None:
            self.same[reuse] = (x, y, clut)
        self.v[y:y + 64, x:x + 16] = idx[:, 0::4] | idx[:, 1::4] << 4 | idx[:, 2::4] << 8 | idx[:, 3::4] << 12
        self.rects.append(('image', x, y, 16, 64, 4))
        if clut is not None:
            return x, y, clut
        cx, cy = (self.cluts % self.clut_run) * 16, CLUT_ROW0 + self.cluts // self.clut_run
        self.cluts += 1
        self.v[cy, cx:cx + 16] = colours
        self.rects.append(('clut', cx, cy, 16, 1, 4))
        if reuse is not None:
            self.same[reuse] = (x, y, (cx, cy))
        return x, y, (cx, cy)

    def palette(self, colours: list[int]) -> tuple[int, int]:
        """(x, y) of a palette alone (another for a picture already here:
        the lightning's lit sky)."""
        cx, cy = (self.cluts % self.clut_run) * 16, CLUT_ROW0 + self.cluts // self.clut_run
        self.cluts += 1
        self.v[cy, cx:cx + 16] = colours
        self.rects.append(('clut', cx, cy, 16, 1, 4))
        return cx, cy


def bgr555(rgb: np.ndarray) -> np.ndarray:
    c = np.clip(np.round(rgb / 8), 0, 31).astype(np.uint16)
    return c[..., 0] | c[..., 1] << 5 | c[..., 2] << 10


def rgb_of(c: np.ndarray) -> np.ndarray:
    return np.stack([(c & 31) << 3, (c >> 5 & 31) << 3, (c >> 10 & 31) << 3], -1).astype(float)


def sixteen(img: np.ndarray) -> tuple[list[int], np.ndarray]:
    """16 colours for a 64 x 64 picture (median cut, then moved to the means
    of the texels they take) and its indices. Colour 0 shows nothing on the
    PlayStation: black is 1, 1, 1."""
    q = Image.fromarray(img.astype(np.uint8)).quantize(16, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    pal = np.array((q.getpalette() + [0] * 48)[:48], float).reshape(16, 3)
    x = img.reshape(-1, 3).astype(float)
    for _ in range(12):
        near = ((x[:, None] - pal[None]) ** 2).sum(2).argmin(1)
        for k in range(16):
            if (near == k).any(): pal[k] = x[near == k].mean(0)
    colours = bgr555(pal)
    colours[colours == 0] = 0x0421
    shown = rgb_of(colours)
    idx = ((x[:, None] - shown[None]) ** 2).sum(2).argmin(1).reshape(img.shape[:2])
    return [int(c) for c in colours], idx


def cell_desc(x: int, y: int) -> int:
    """A far-background cell word (scene.FarBg) for the 64 x 64 picture at
    VRAM (x, y), y in 512..1023."""
    return (x // 64) << 8 | (y // 256 & 3) << 12 | (x % 64) // 16 | (y % 256) // 64 << 2


def panorama_picture(cfg: dict, vram: np.ndarray, rec: dict) -> np.ndarray:
    """The far background's picture, rows x 64 tall, a whole number of 64
    pixel columns wide, its ends joining."""
    if cfg.get('night'):
        # a black room: nothing round it (the engine's black band under it)
        return np.zeros((64 * cfg.get('rows', 2), 64 * cfg.get('columns', 8), 3))
    if cfg.get('_picture') is not None:
        # the add-on's, in the Classic's frame (classic_cfg())
        return cfg['_picture'].astype(float)
    if cfg.get('panorama'):
        img = np.asarray(Image.open(hd(cfg['panorama'])).convert('RGB')).astype(float)
        if cfg.get('erase'):
            img = erase(img, cfg['erase'])
        if cfg.get('webs'):
            img = painted_webs(img, cfg['webs'])
        if cfg.get('squeeze'):
            # "squeeze" [y0, y1, y2]: the sky's rows y0..y1 pressed into
            # y2..y1, all above them moved down as much, the top its first
            # row on (the Great Wall's thin clouds brought down over the
            # hills, where the camera sees them)
            y0, y1, y2 = cfg['squeeze']
            band = np.asarray(Image.fromarray(img[y0:y1].astype(np.uint8)).resize((img.shape[1], y1 - y2), Image.LANCZOS))
            top = img[:y0]
            img = np.concatenate([np.repeat(img[:1], y2 - y0, 0), top, band.astype(float), img[y1:]])
        if cfg.get('crop'):
            x0, y0, x1, y1 = cfg['crop']
            if y1 > img.shape[0]:
                # past the picture's foot: its last row on (the beach's sand,
                # under the floor; a cell row then holds the whole sea)
                img = np.concatenate([img, np.repeat(img[-1:], y1 - img.shape[0], 0)])
            if y0 < 0:
                # above its top: its first row on (the night sky; the Taj
                # Mahal's pool edge then falls at a cell row's top), with
                # "sky" going to that colour at the top (Szechwan's blue)
                top = np.repeat(img[:1], -y0, 0)
                if cfg.get('sky'):
                    # the first row smoothed along (no streaks up the sky)
                    k = np.ones(61) / 61
                    first = np.stack([np.convolve(np.pad(img[0, :, ch], 30, mode='wrap'), k, 'valid') for ch in range(3)], -1)
                    near = np.clip((np.arange(-y0) - (-y0 - 16)) / 16, 0, 1)[:, None, None]
                    top = np.repeat(first[None], -y0, 0) * (1 - near) + img[:1] * near
                    t = (1 - np.arange(-y0) / -y0)[:, None, None] ** 1.3
                    top = top * (1 - t) + np.array(cfg['sky'], float)[None, None] * t
                img = np.concatenate([top, img])
                y1, y0 = y1 - y0, 0
            img = img[y0:y1, x0:x1]
        if cfg.get('wrap'):
            # "wrap" pixels: the crop runs that far past one whole turn of a
            # picture that repeats (Serene Forest's, its period half its
            # width); its start crossfaded from that run, which then goes
            n = cfg['wrap']
            t = np.linspace(0, 1, n)[None, :, None]
            img = img.copy()
            img[:, :n] = img[:, -n:] * (1 - t) + img[:, :n] * t
            img = img[:, :-n]
        if cfg.get('grade'):
            img = grade(img, cfg['grade'])
        if cfg.get('foot'):
            # "foot" {"height" pixels, "colour"}: the picture's foot going
            # into that dark (Serene Forest's trunks into the night under
            # them: the band under the picture the same at any pitch)
            h = cfg['foot']['height']
            t = np.clip(np.arange(img.shape[0]) - (img.shape[0] - h), 0, None)[:, None, None] / h
            t = t * t * (3 - 2 * t)
            img = img * (1 - t) + np.array(cfg['foot'].get('colour', [0, 0, 0]), float)[None, None] * t
        rows = cfg.get('rows', round(img.shape[0] / 64))
        cols = cfg.get('columns', round(img.shape[1] * rows * 64 / img.shape[0] / 64))
        img = np.asarray(Image.fromarray(img.astype(np.uint8)).resize((cols * 64, rows * 64), Image.LANCZOS)).astype(float)
        if cfg.get('bank'):
            # "bank" {"height" pixels, "colour"}: the near shore along the
            # picture's foot, grass to the floor's (Cambodia's lake beyond
            # the fight), its top ragged with tufts and reeds
            h, rnd = cfg['bank']['height'], np.random.default_rng(cfg['bank'].get('seed', 7))
            w = img.shape[1]
            top = h + 1.5 * np.sin(np.arange(w) * 0.21) + rnd.uniform(0, 2.5, w)
            reeds = rnd.uniform(0, 1, w) < 0.12
            top = top + reeds * rnd.uniform(3, 8, w)
            depth = img.shape[0] - np.arange(img.shape[0])[:, None]          # rows up from the foot
            on = depth <= top[None]
            grass = np.array(cfg['bank'].get('colour', [44, 98, 32]), float)
            shade = (0.75 + 0.35 * rnd.uniform(0, 1, img.shape[:2])) * (0.85 + 0.25 * np.clip((top[None] - depth) / max(1, h), 0, 1))
            img = np.where(on[..., None], grass[None, None] * shade[..., None], img)
        seam = cfg.get('seam', 0)
        if seam:
            # The right end fades into the left's beginning, mirrored: the
            # strip goes round with no edge (the picture's ends differ).
            t = np.linspace(0, 1, seam)[None, :, None]
            if cfg.get('seam_repeat'):
                # the picture twice the same (Serene Forest's): its end into
                # what comes before its middle, as the first half runs on
                half = img.shape[1] // 2
                img[:, -seam:] = img[:, -seam:] * (1 - t) + img[:, half - seam:half] * t
            else:
                img[:, -seam:] = img[:, -seam:] * (1 - t) + img[:, seam - 1::-1] * t
        if cfg.get('roll'):
            # "roll" pixels of the (cropped) picture: what the camera sees
            # when the fight starts (the launch pad's shuttle behind them)
            width = (cfg['crop'][2] - cfg['crop'][0]) if cfg.get('crop') else img.shape[1]
            img = np.roll(img, int(round(cfg['roll'] * img.shape[1] / width)), axis=1)
        if cfg.get('twice'):
            # "twice": the picture side by side with itself, a whole turn of
            # the ring (no repeat): what is drawn after (the moon) is there once
            img = np.concatenate([img, img], axis=1)
        if cfg.get('moon'):
            # the moon drawn on last (the cloud row's frames draw it again
            # over their billowing clouds: moon_over())
            cfg['_moonless'] = img.copy()
            img = moon_over(img, cfg['moon'], 0.0)
        return img
    pano = t2_stage.panorama(vram, rec).astype(float)
    return np.repeat(pano, 2, axis=0)


def web_strands(cx: float, cy: float, radius: float, corner: str | None, seed: int) -> tuple[list, list]:
    """A spider's web as strands, (radials, spiral): each a list of
    polylines ((x, y) points), centre (cx, cy), "radius" pixels; corner
    "tl", "tr", "bl", "br": a corner web, its centre in that corner, only
    the quarter into the room (its strands sagging), else a whole orb web.
    The spiral's polylines in the order a spider lays them (outward in)."""
    rnd = np.random.default_rng(seed)
    if corner:
        a0 = {'tl': 0.0, 'tr': 90.0, 'br': 180.0, 'bl': 270.0}[corner]
        angles = np.radians(a0 + np.linspace(4, 86, 7) + rnd.uniform(-3, 3, 7))
    else:
        n = 13
        angles = np.radians(np.arange(n) * 360 / n + rnd.uniform(-7, 7, n))
    # screen: x right, y down; angle 0 is right, 90 down
    lens = radius * rnd.uniform(0.8, 1.05, len(angles))
    radials = [[(cx, cy), (cx + l * np.cos(a), cy + l * np.sin(a))] for a, l in zip(angles, lens)]
    spiral = []
    rings = np.linspace(0.95, 0.18, 9 if corner else 11)
    for k, f in enumerate(rings):
        pts = [(cx + f * l * np.cos(a), cy + f * l * np.sin(a)) for a, l in zip(angles, lens)]
        if not corner:
            pts.append(pts[0])
        # each span sagging a little between the radials (down)
        line = []
        for (x0, y0), (x1, y1) in zip(pts, pts[1:]):
            for t in np.linspace(0, 1, 5)[:-1]:
                sag = 0.06 * f * radius * 4 * t * (1 - t) * (1.0 if corner else 0.4)
                line.append((x0 + (x1 - x0) * t, y0 + (y1 - y0) * t + sag))
        line.append(pts[-1])
        spiral.append(line)
    return radials, spiral


def draw_strands(size: tuple, lines: list, colour, alpha: float, ss: int = 4, width: float = 1.0) -> np.ndarray:
    """The strands drawn thin and soft: RGBA (float, alpha 0..1) of size
    (w, h), supersampled ss times."""
    from PIL import ImageDraw
    w, h = size
    im = Image.new('L', (w * ss, h * ss), 0)
    d = ImageDraw.Draw(im)
    for line in lines:
        d.line([(x * ss, y * ss) for x, y in line], fill=255, width=max(1, int(round(width * ss * 0.6))))
    a = np.asarray(im.resize((w, h), Image.BOX), float) / 255 * alpha
    out = np.zeros((h, w, 4))
    out[..., :3] = np.array(colour, float)
    out[..., 3] = np.clip(a, 0, 1)
    return out


def painted_webs(img: np.ndarray, spec: dict) -> np.ndarray:
    """"webs" {"at": [[x, y, radius, corner], ...] pixels of the player's
    picture (corner "tl", "tr", "bl", "br", or "" an orb web), "colour",
    "alpha"}: cobwebs in the corners of Eternal Darkness's hall, strung on
    the picture."""
    out = img.copy()
    colour, alpha = spec.get('colour', [150, 150, 160]), spec.get('alpha', 0.4)
    for k, (x, y, r, corner) in enumerate(spec['at']):
        x0, y0 = int(max(0, x - r - 4)), int(max(0, y - r - 4))
        x1, y1 = int(min(img.shape[1], x + r + 4)), int(min(img.shape[0], y + r + 4))
        rad, spi = web_strands(x - x0, y - y0, r, corner or None, 300 + k)
        layer = draw_strands((x1 - x0, y1 - y0), rad + spi, colour, alpha, width=1.4)
        a = layer[..., 3:]
        out[y0:y1, x0:x1] = out[y0:y1, x0:x1] * (1 - a) + layer[..., :3] * a
    return out


def weave_frames(pic: np.ndarray, spec: dict) -> tuple[tuple, list[np.ndarray], list[int]]:
    """"weave" {"cell": [row, column] of the far background, "radius",
    "centre" [x, y] in it, "steps", "hold", "gone", "colour", "alpha"}: a
    spider spinning its orb web in one cell of the picture: the frame
    threads first, then the spiral round and round, the spider (a dark
    speck with legs) where it is spinning; the web held a while, then
    torn away and gone a while. Returns the cell, its (colours, indices)
    as shown first, and per frame of the loop its indices (in the same
    16 colours)."""
    row, col = spec['cell']
    base = pic[64 * row:64 * row + 64, 64 * col:64 * col + 64].astype(float)
    cx, cy = spec.get('centre', [32, 32])
    rad, spi = web_strands(cx, cy, spec.get('radius', 26), None, spec.get('seed', 77))
    colour, alpha = spec.get('colour', [160, 160, 172]), spec.get('alpha', 0.55)
    # the spinning in steps: each radial, then the spiral a span at a time
    pieces = [[r] for r in rad]
    for line in spi:
        for k in range(0, len(line) - 1, 4):
            pieces.append([line[k:k + 5]])
    steps = spec.get('steps', 40)
    cut = np.linspace(0, len(pieces), steps + 1).round().astype(int)[1:]
    def frame(n_pieces: int, spider_at=None, torn: float = 0.0) -> np.ndarray:
        lines = [l for p in pieces[:n_pieces] for l in p]
        if torn:
            rnd = np.random.default_rng(int(torn * 100))
            lines = [l for l in lines if rnd.uniform() > torn]
        img = base.copy()
        if lines:
            layer = draw_strands((64, 64), lines, colour, alpha, ss=4, width=1.0)
            a = layer[..., 3:]
            img = img * (1 - a) + layer[..., :3] * a
        if spider_at is not None:
            sx, sy = spider_at
            yy, xx = np.mgrid[0:64, 0:64]
            body = np.hypot((xx - sx) / 1.3, yy - sy) < 1.6
            legs = (np.abs(yy - sy) < 0.7) & (np.abs(xx - sx) < 3.2) | (np.abs(xx - sx) < 0.7) & (np.abs(yy - sy) < 2.4)
            img[legs] = img[legs] * 0.3 + np.array([20, 16, 14]) * 0.7
            img[body] = [14, 10, 10]
        return img
    last = lambda n: pieces[n - 1][-1][-1] if n else (cx, cy)
    frames = [frame(n, last(n)) for n in cut]
    full = frame(len(pieces))
    hold = [full] * spec.get('hold', 30)
    torn = [frame(len(pieces), None, t) for t in (0.3, 0.6, 0.85)]
    gone = [frame(0)] * spec.get('gone', 12)
    loop = gone + frames + hold + torn
    colours, _ = sixteen(np.concatenate([full, frame(len(pieces), (cx, cy))]))
    shown = rgb_of(np.array(colours, np.uint16))
    idx = [((f.reshape(-1, 1, 3) - shown[None]) ** 2).sum(2).argmin(1).reshape(64, 64) for f in loop]
    return (row, col), colours, idx


def moon_over(img: np.ndarray, spec: dict, t: float, behind=None) -> np.ndarray:
    """The moon on the night sky ("moon": "x", "y" its middle in the
    panorama picture's pixels, "radius", "halo" the glow's radius, "colour",
    "halo_colour", "lining" the clouds' silver near it, "breath" the
    glow's waver over the frames): a pale disc, its seas faintly darker,
    the clouds over it hiding it as thick as they are (their light against
    the sky's, "sky" its brightness), lit at their edges near it; the glow
    added round it, t the frame's phase (0: the shown picture)."""
    h, w = img.shape[:2]
    cx, cy, r = spec['x'], spec['y'], spec.get('radius', 9)
    yy, xx = np.mgrid[0:h, 0:w].astype(float)
    dx = (xx - cx + w / 2) % w - w / 2
    d = np.hypot(dx, yy - cy)
    lum = img @ np.array([0.3, 0.5, 0.2])
    cover = np.clip((lum - spec.get('sky', 18)) / spec.get('thick', 45), 0, 1)
    rnd = np.random.default_rng(spec.get('seed', 3))
    seas = np.zeros((h, w))
    for _ in range(5):
        sx, sy, sr = rnd.uniform(-0.5, 0.5) * r, rnd.uniform(-0.5, 0.5) * r, rnd.uniform(0.2, 0.45) * r
        seas = np.maximum(seas, np.clip(1 - np.hypot(dx - sx, yy - cy - sy) / sr, 0, 1))
    disc = np.clip(r + 0.5 - d, 0, 1)
    moon = np.array(spec.get('colour', [238, 234, 214]), float)[None, None] * (1 - 0.14 * seas)[..., None]
    a = (disc * (1 - 0.85 * cover))[..., None]
    out = img * (1 - a) + moon * a
    breath = 1 + spec.get('breath', 0.1) * np.sin(t)
    if spec.get('breath_rows'):
        # breathing only inside the animated row, none at its edges
        y0, y1 = spec['breath_rows']
        breath = 1 + (breath - 1) * (np.sin(np.pi * np.clip((yy - y0) / (y1 - y0), 0, 1)) ** 1.5)[..., None]
    R = spec.get('halo', 5 * r)
    # the glow and the lining cut off far from it (nothing changed there:
    # "twice" lays those cells once)
    glow = np.exp(-(np.maximum(d - r, 0) / R) ** 2) * (d > r - 1) * (d < r + 2.5 * R)
    breath = np.broadcast_to(np.asarray(breath, float), (h, w, 1)) if np.ndim(breath) else breath
    out = out + glow[..., None] * np.array(spec.get('halo_colour', [40, 44, 60]), float) * breath
    lining = cover * np.exp(-(np.maximum(d - r, 0) / (2.2 * R)) ** 2) * (d < r + 4 * R)
    out = out + lining[..., None] * np.array(spec.get('lining', [70, 74, 86]), float) * breath
    return np.clip(out, 0, 255)


def grade(img: np.ndarray, spec: dict) -> np.ndarray:
    """The panorama's light at another hour ("grade"): each row times a
    colour going from "top" to "horizon" (at "horizon_row", a part of the
    picture's height) to "ground", and a "glow" added along the horizon,
    "glow_width" rows (part of the height) each way."""
    h = img.shape[0]
    t = np.arange(h) / max(1, h - 1)
    hr = spec['horizon_row']
    top, hor, gnd = (np.array(spec[k], float) for k in ('top', 'horizon', 'ground'))
    k = np.where(t[:, None] < hr, top + (hor - top) * (t[:, None] / hr), hor + (gnd - hor) * ((t[:, None] - hr) / (1 - hr)))
    out = img * k[:, None, :]
    if spec.get('glow'):
        g = np.exp(-((t - hr) / spec.get('glow_width', 0.12)) ** 2)
        out = out + g[:, None, None] * np.array(spec['glow'], float)[None, None]
    return np.clip(out, 0, 255)


def erase(img: np.ndarray, spec: dict) -> np.ndarray:
    """Something taken off the panorama ("erase": what the stage builds in
    3D instead, not to stand there twice): inside "polygon" (pixels of the
    picture) the sky and the far land, each row going from the median of
    the "left" columns [x0, x1] to that of the "right" columns, the left's
    rows from "left_until" on the right's (rock under it there); its edge
    softened, a little grain on it."""
    from PIL import ImageDraw, ImageFilter
    h, w = img.shape[:2]
    m = Image.new('L', (w, h), 0)
    ImageDraw.Draw(m).polygon([tuple(p) for p in spec['polygon']], fill=255)
    m = m.filter(ImageFilter.MaxFilter(9))
    right = np.median(img[:, slice(*spec['right'])], axis=1)
    left = np.median(img[:, slice(*spec['left'])], axis=1)
    left[spec.get('left_until', h):] = right[spec.get('left_until', h):]
    xs = np.where(np.asarray(m).any(0))[0]
    x0, x1 = xs.min(), xs.max() + 1
    fill = img.copy()
    t = np.linspace(0, 1, x1 - x0)[None, :, None]
    fill[:, x0:x1] = left[:, None] * (1 - t) + right[:, None] * t
    rnd = np.random.default_rng(3)
    grain = Image.fromarray(np.clip(128 + 40 * rnd.normal(0, 1, (h, w)), 0, 255).astype(np.uint8))
    fill += (np.asarray(grain.filter(ImageFilter.GaussianBlur(1.5))).astype(float)[..., None] - 128) / 8
    soft = np.asarray(m.filter(ImageFilter.GaussianBlur(4))).astype(float)[..., None] / 255
    return img * (1 - soft) + fill * soft


def far_background(cfg: dict, base: scene.FarBg, pic: np.ndarray, vr: Vram, sea: list | None = None,
                   own: dict | None = None) -> scene.FarBg:
    """sea: a list the "sea" row's cells go into ((indices, colours,
    palette) each), that row laid last, side by side in the VRAM with a
    slot after it (sea_frames(): the beach's animation). own: {(row,
    column): (colours, indices)} cells drawn so (city_lights(): New York's
    windows)."""
    rows, cols = pic.shape[0] // 64, pic.shape[1] // 64
    desc, cluts = [[0] * cols for _ in range(rows)], [[0] * cols for _ in range(rows)]
    sea_row = (cfg.get('sea') or cfg.get('pool') or cfg.get('clouds') or cfg.get('water'))['row'] if sea is not None else -1
    # "clouds" "water": Cambodia's lake row animated as well, laid after
    # the cloud row (the two one rectangle, its picture after them)
    lake_row = cfg.get('clouds', {}).get('water', {}).get('row', -1) if sea is not None else -1
    order = [r for r in range(rows) if r not in (sea_row, lake_row)] + ([sea_row] if sea is not None else []) +         ([lake_row] if lake_row >= 0 else [])
    # "twice" (the picture side by side with itself): a cell of the second
    # half the same as the first half's is that one (only where the moon
    # is do they differ); the sea row's cells laid are then the first
    # half's and the second's that differ ("_sea_cols": their columns)
    half = cols // 2 if cfg.get('twice') else 0
    same = lambda r, c: half and c >= half and np.array_equal(pic[64 * r:64 * r + 64, 64 * c:64 * c + 64],
                                                             pic[64 * r:64 * r + 64, 64 * (c - half):64 * (c - half) + 64])
    laid = {r: [c for c in range(cols) if not same(r, c)] for r in range(rows)}
    if half:
        cfg['_sea_cols'] = laid[sea_row] if sea_row >= 0 else None
    start = None
    for r in order:
        if r == sea_row:
            start = vr.reserve(len(laid[r]) * (2 if lake_row >= 0 else 1) + after_sea(cfg))
        elif r == lake_row:
            start += cols
        else:
            start = None
        for c in range(cols):
            if c not in laid[r]:
                desc[r][c], cluts[r][c] = desc[r][c - half], cluts[r][c - half]
                continue
            if cfg.get('night'):        # nothing drawn (colour 0): the black band under it
                colours, idx = [0] * 16, np.zeros((64, 64), np.uint16)
            elif own and (r, c) in own:
                colours, idx = own[r, c]
            else:
                colours, idx = sixteen(pic[64 * r:64 * r + 64, 64 * c:64 * c + 64])
            x, y, (cx, cy) = vr.picture(idx, colours, at=None if start is None else start + laid[r].index(c))
            if r in (sea_row, lake_row):
                sea.append((idx, colours, (cx, cy), (x, y)))
            desc[r][c], cluts[r][c] = cell_desc(x, y), (cy << 6 | cx >> 4)
    fb = copy.deepcopy(base)
    fb.cols, fb.rows, fb.desc, fb.cluts = cols, rows, desc, cluts
    # degrees of the turn the picture takes: the turn holds 0x2D000 /
    # degrees pixels (scene.farbg_commands), the kind map a cell each
    fb.degrees = cfg.get('degrees', 0x2D000 // (cols * 64))
    fb.ring = scene.tdiv(scene.tdiv(0x2D000, fb.degrees), 64)
    fb.kinds = [[0x31] * fb.ring for _ in range(rows)]
    fb.yoff, fb.pitch = cfg.get('yoff', base.yoff), cfg.get('pitch', base.pitch)
    top, bottom = pic[:2].reshape(-1, 3).mean(0), pic[-2:].reshape(-1, 3).mean(0)
    fb.top_rgb, fb.top_on = tuple(int(v) for v in top), 1
    fb.bottom_rgb, fb.bottom_on = tuple(int(v) for v in bottom), 1
    fb.cell_rgb = fb.top_rgb
    return fb


def after_sea(cfg: dict) -> int:
    """Slots after the sea (pool) row in its animation: the flame (the
    lamps), and with "pool" "spray" the fountains' water; after the
    "clouds" row the flags' picture and the smoke; after New York's
    "water" its picture and the steam."""
    if cfg.get('clouds') and not cfg['clouds'].get('extras', True):
        return 0                     # Serene Forest's leaves alone
    if cfg.get('clouds', {}).get('picture') in ('cambodia', 'athens', 'desert', 'hills'):
        return 1                     # Cambodia's (Athens', Nowhere Desert's) picture, no smoke
    return 2 if cfg.get('pool', {}).get('spray') or cfg.get('clouds') or cfg.get('water') else 1


def bolt_skies(cfg: dict, pic: np.ndarray, fb, vr: Vram) -> list:
    """The far background as the lightning lights it ("lightning": "bolts"
    [x of the panorama picture, its foot's row], "variants" which bolt each
    of the flashes shows): every cell its own picture with a brighter
    palette, the clouds lit most ("lit" [top, horizon] gain, the "horizon"
    row), the cells a bolt crosses drawn anew with it (a jagged white
    stroke, forked, a glow round it). Returns (desc, cluts) per variant, as
    FARV wants them."""
    spec = cfg['lightning']
    rows, cols = pic.shape[0] // 64, pic.shape[1] // 64
    top, low = spec.get('lit', [2.1, 1.35])
    hz = spec.get('horizon', 360)
    gain = np.interp(np.arange(pic.shape[0]), [0, hz, pic.shape[0]], [top, low, 1.2])[:, None, None]
    lit = np.clip(pic * gain + np.array(spec.get('tint', [14, 16, 26]), float), 0, 255)
    lit_clut = [[None] * cols for _ in range(rows)]
    for r in range(rows):
        for c in range(cols):
            colours, _ = sixteen(pic[64 * r:64 * r + 64, 64 * c:64 * c + 64])
            rgb = rgb_of(np.array(colours, np.uint16))
            g = np.interp(64 * r + 32, [0, hz, pic.shape[0]], [top, low, 1.2])
            new = bgr555(np.clip(rgb * g + np.array(spec.get('tint', [14, 16, 26])), 0, 255))
            new[new == 0] = 0x0421
            lit_clut[r][c] = vr.palette([int(v) for v in new])
    rnd = np.random.default_rng(spec.get('seed', 21))
    bolts = []
    yy, xx = np.mgrid[0:pic.shape[0], 0:pic.shape[1]]
    for bx, foot in spec['bolts']:
        # its way down: steps sideways at random, a fork off it half way
        path, x = [], float(bx)
        for y in range(0, foot):
            if y % 9 == 0: x += rnd.normal(0, 4.5)
            path.append((x, y))
        fork, fx = [], path[foot // 2][0]
        for y in range(foot // 2, int(foot * 0.8)):
            if y % 7 == 0: fx += rnd.normal(5, 3)
            fork.append((fx, y))
        img = lit.copy()
        glow = np.zeros(pic.shape[:2])
        for pts, w in ((path, 1.0), (fork, 0.6)):
            for x, y in pts:
                x0 = int(round(x))
                for d in range(-14, 15):
                    xi = (x0 + d) % pic.shape[1]
                    glow[y, xi] = max(glow[y, xi], w * np.exp(-(d / 6.0) ** 2) * 0.5)
                for d in (-1, 0, 1) if w == 1.0 else (0,):
                    glow[y, (x0 + d) % pic.shape[1]] = 1.6
        img = np.clip(img + glow[..., None] * np.array([200, 210, 255]), 0, 255)
        cells = {}
        for r in range(rows):
            for c in range(cols):
                if glow[64 * r:64 * r + 64, 64 * c:64 * c + 64].max() > 0.2:
                    colours, idx = sixteen(img[64 * r:64 * r + 64, 64 * c:64 * c + 64])
                    x, y, clut = vr.picture(idx, colours)
                    cells[r, c] = (cell_desc(x, y), clut)
        bolts.append(cells)
    out = []
    for v in spec.get('variants', range(len(bolts))):
        desc = [[cells_desc for cells_desc in row] for row in fb.desc]
        cluts = [[cy << 6 | cx >> 4 for cx, cy in row] for row in lit_clut]
        for (r, c), (d, (cx, cy)) in bolts[v].items():
            desc[r][c], cluts[r][c] = d, cy << 6 | cx >> 4
        out.append((desc, cluts))
    return out


def sea_frames(cells: list, spec: dict) -> list:
    """The sea row's cells as the surf washes in and out and the sun
    glitters on the water ("sea": "frames" of them, played forth and back;
    each cell's own 16 colours, only its indices change). Under the foam's
    lowest row in each column (the bright pixels from "foam_from" down) the
    picture is drawn down by up to "wash" pixels (the wave running up the
    sand) and back, fading to nothing "reach" rows away (the row's foot
    meets the static row under it); on the water above it ("water_to")
    "glitter" of the blue pixels take the cell's brightest colour now and
    then. Returns per frame the cells' indices."""
    n, wash, reach = spec.get('frames', 8), spec.get('wash', 4), spec.get('reach', 10)
    rnd = np.random.default_rng(spec.get('seed', 17))
    out = [[] for _ in range(n)]
    cols = len(cells)
    phase = rnd.uniform(0, 2 * np.pi, 3)
    for c, (idx, colours, *_) in enumerate(cells):
        rgb = rgb_of(np.array(colours, np.uint16))
        lum = rgb @ np.array([0.3, 0.59, 0.11])
        bright = int(lum.argmax())
        px = rgb[idx]
        foam = (lum[idx] > spec.get('foam_lum', 170)) & (px[..., 2] >= px[..., 0])
        water = (px[..., 2] > px[..., 0] + 40) & (lum[idx] < 170)
        foam[:spec.get('foam_from', 36)] = False
        yy = np.arange(64)[:, None]
        edge = np.where(foam.any(0), 63 - np.argmax(foam[::-1], 0), -100)       # each column's lowest foam row
        x = np.arange(64) + 64 * c
        # the waves along the shore: not all of it at once
        along = 0.65 + 0.35 * np.sin(2 * np.pi * x / (64 * cols) * 5 + phase[0]) * np.sin(2 * np.pi * x / (64 * cols) * 2 + phase[1])
        twinkle = rnd.uniform(0, 1, (64, 64))
        for f in range(n):
            t = f / max(1, n - 1)
            t = t * t * (3 - 2 * t)
            d = wash * t * along                                                # pixels drawn down in each column
            w = np.clip(1 - np.abs(yy - edge[None]) / reach, 0, 1) * (edge[None] >= 0)
            src = np.clip(np.round(yy - d[None] * w), 0, 63).astype(int)
            frame = idx[src, np.arange(64)[None].repeat(64, 0)]
            on = water & (np.abs(twinkle - (f + 0.5) / n) < spec.get('glitter', 0.006) / 2)
            on[spec.get('water_to', 40):] = False
            frame = np.where(on, bright, frame)
            out[f].append(frame.astype(np.uint16))
    return out


def flame_frames(n: int) -> tuple[list[int], list]:
    """A torch's flame as it flickers, n frames (played forth and back): a
    tongue rising from the picture's foot, white yellow at its root to deep
    orange at its licking tip, swaying. Colour 0 nothing, the others solid
    (in daylight an added flame is lost on the bright sky)."""
    ramp = np.array([(0, 0, 0), (70, 16, 2), (110, 28, 4), (150, 44, 6), (186, 62, 10), (214, 84, 14), (236, 110, 22),
                     (248, 138, 36), (255, 166, 56), (255, 192, 84), (255, 214, 120), (255, 232, 160), (255, 244, 200),
                     (255, 250, 228), (255, 252, 240), (255, 255, 250)], float)
    colours = [0] + [int(c) for c in bgr555(ramp[1:])]
    rnd = np.random.default_rng(81)
    ph = rnd.uniform(0, 2 * np.pi, 4)
    yy, xx = np.mgrid[0:64, 0:64]
    v = 1 - yy / 63.0                                      # 0 the foot, 1 the top
    frames = []
    for f in range(n):
        t = 2 * np.pi * f / n
        sway = 5 * np.sin(t + ph[0]) * v ** 1.5 + 2 * np.sin(3 * t + ph[1]) * v ** 3
        tall = 0.85 + 0.12 * np.sin(2 * t + ph[2])
        width = 15 * np.clip(1 - v / tall, 0, 1) ** 0.7 * np.clip(v * 5 + 0.25, 0, 1)
        dx = np.abs(xx - 31.5 - sway)
        k = np.clip(1 - dx / np.maximum(width, 1e-3), 0, 1) * (v < tall)
        heat = np.clip(k ** 0.6 * (1.15 - 0.85 * v / tall), 0, 1)
        frames.append(np.where(heat < 0.07, 0, np.clip(np.round(heat * 15), 1, 15)).astype(np.uint16))
    return colours, frames


def beach_animation(cfg: dict, sea: list, vr: Vram) -> tuple:
    """The beach's one animation: the sea row (sea_frames()) and, in the
    slot after it, the torches' flame (flame_frames()), each frame of them
    side by side as the shown ones. Returns the shown flame's picture, the
    rectangle's place and the frames' places in play order."""
    spec = cfg['sea']
    n = spec.get('frames', 8)
    waves = sea_frames(sea, spec)
    fcolours, fire = flame_frames(n)
    x0, y0 = sea[0][3]
    at = (y0 - 512) // 64 * 64 + x0 // 16
    shown = vr.picture(fire[0], fcolours, at=at + len(sea))
    places = []
    for f in range(n):
        g = vr.frame_run(len(sea) + 1)
        for c, cell in enumerate(sea):
            vr.picture(waves[f][c], None, at=g + c, clut=cell[2])
        vr.picture(fire[f], None, at=g + len(sea), clut=shown[2])
        places.append(((g % 64) * 16, 512 + (g // 64) * 64))
    order = list(range(n)) + list(range(n - 2, 0, -1))
    return shown, (x0, y0), [places[k] for k in order]


def pool_frames(cells: list, spec: dict) -> list:
    """The pool row's cells as the reflection ripples and the moon glitters
    ("pool": "frames" of them, played round and round; each cell's own 16
    colours, only its indices change). From row "water_from" of the cells
    down each row of the picture is drawn aside by up to "ripple" pixels,
    more toward the foot (the near water), a slow wave going down it; now
    and then "glitter" of the water's texels take the cell's brightest
    colour for a frame. Returns per frame the cells' indices."""
    n, amp, w0 = spec.get('frames', 8), spec.get('ripple', 2.0), spec.get('water_from', 31)
    rings = spec.get('rain_rings', 0)
    rnd = np.random.default_rng(spec.get('seed', 16))
    out = [[] for _ in range(n)]
    yy = np.arange(64)
    depth = np.clip((yy - w0) / max(1, 63 - w0), 0, 1)
    for c, (idx, colours, *_) in enumerate(cells):
        rgb = rgb_of(np.array(colours, np.uint16))
        lum = rgb @ np.array([0.3, 0.59, 0.11])
        bright = int(lum.argmax())
        phase = rnd.uniform(0, 2 * np.pi, 64)
        when = rnd.integers(0, n, (64, 64))
        spark = rnd.uniform(0, 1, (64, 64)) < spec.get('glitter', 0.004)
        spark[:w0 + 2] = False
        # "rain_rings" drops a cell: each a thin ring widening on the water
        # for four frames from its own, flat (seen from the side), smaller
        # far off; not drawn in a colour of its own but the water under it
        # a shade lighter (lighter), fading as it widens
        # the palette colour nearest 26 brighter, 12..50 brighter; none such:
        # the same (a ring never shows as a white line on dark water)
        step = lum[None] - lum[:, None]
        want = np.abs(step - 26) + 0.02 * ((rgb[None] - rgb[:, None]) ** 2).sum(2) ** 0.5
        want[(step < 12) | (step > 50)] = np.inf
        lighter = np.where(np.isfinite(want.min(1)), want.argmin(1), np.arange(16))
        drops = [(rnd.uniform(4, 60), rnd.uniform(w0 + 3, 62), int(rnd.integers(n))) for _ in range(rings)]
        fade = rnd.uniform(0, 1, (64, 64))
        for f in range(n):
            t = 2 * np.pi * f / n
            # each row its own phase a little off its neighbours': the
            # reflection breaks into wavering bands
            d = np.round(amp * (0.35 + 0.65 * depth) * np.sin(t + yy * 0.7 + 0.25 * phase)).astype(int)
            d[:w0] = 0
            src = np.clip(np.arange(64)[None] - d[:, None], 0, 63)
            frame = idx[yy[:, None], src]
            frame = np.where(spark & (when == f), bright, frame).astype(np.int64)
            for dx, dy, born in drops:
                age = (f - born) % n
                if age > 3: continue
                near = 0.5 + 0.7 * (dy - w0) / (64 - w0)
                if age == 0:
                    # the drop's splash: a pixel or two, the lightest
                    on = (np.abs(np.arange(64)[None] - dx) < 0.5 + near) & (np.abs(yy[:, None] - dy) < 0.5)
                    frame = np.where(on, lighter[lighter[frame]], frame)
                    continue
                r = 0.9 + 1.5 * age
                e = np.hypot((np.arange(64)[None] - dx) / (r * 1.9 * near), (yy[:, None] - dy) / (r * 0.6 * near))
                on = (np.abs(e - 1) < 0.2 + 0.1 / age) & (yy[:, None] > w0) & (fade < 1.15 - 0.3 * age)
                frame = np.where(on, lighter[frame], frame)
            out[f].append(frame.astype(np.uint16))
    return out


DIYA_ROWS, GLOW_COLS = (16, 31), (32, 47)    # the Mughal lamp picture: a diya's flame (cols 0..31), a firefly's glow
RAIN_COLS = (48, 63)                          # and a raindrop


def mughal_frames(n: int) -> tuple[list[int], list]:
    """The Taj Mahal's lamps as their flames flicker: n frames (played round
    and round), one palette. Rows 0..15 a brass lantern's pierced side: the
    brass, its star-shaped holes lit from inside; 16..31 a diya's flame
    (columns 0..31, a tongue on nothing) and a firefly's glow (columns
    32..47, round, the same in every frame) and a raindrop (columns
    48..63); 32..63 the light the lantern
    throws on the floor (seen from above), round, added. Colour 0 nothing;
    1..11 dark red to yellow white; 12..14 brass; 15 dark."""
    ramp = np.array([(0, 0, 0), (52, 16, 4), (92, 30, 6), (132, 46, 8), (170, 66, 12), (204, 90, 18), (228, 118, 28),
                     (244, 148, 44), (252, 178, 70), (255, 206, 110), (255, 230, 160), (255, 246, 214),
                     (96, 66, 24), (146, 104, 40), (196, 150, 66), (22, 16, 10)], float)
    colours = [0] + [0x8000 | int(c) for c in bgr555(ramp[1:])]
    rnd = np.random.default_rng(73)
    ph = rnd.uniform(0, 2 * np.pi, 4)
    t = 2 * np.pi * np.arange(n) / n
    level = 0.84 + 0.09 * np.sin(t + ph[0]) + 0.05 * np.sin(2 * t + ph[1]) + 0.03 * np.sin(3 * t + ph[2])
    yy, xx = np.mgrid[0:64, 0:64]
    frames = []
    for f in range(n):
        idx = np.zeros((64, 64), np.uint16)
        # the lantern's side: brass, two rows of stars pierced through it
        side = np.full((16, 64), 13, np.uint16)
        side[0] = side[15] = 12
        side[:, ::21] = 14
        dx, dy = (xx[:16] % 11) - 5, (yy[:16] % 8) - 3.5
        hole = ((np.abs(dx) + np.abs(dy) < 3.6) | (np.maximum(np.abs(dx), np.abs(dy)) < 2.2)) & (yy[:16] > 0) & (yy[:16] < 15)
        glow = level[f] * (0.75 + 0.25 * np.sin(xx[:16] * 0.4 + t[f] * 2 + ph[3]))
        side[hole] = np.clip(np.round(4 + glow[hole] * 7), 4, 11)
        idx[:16] = side
        # the diya's flame: a teardrop rising from the wick, swaying
        fy, fx = yy[16:32, :32], xx[16:32, :32]
        v = 1 - (fy - 16) / 15.0                      # 0 the wick, 1 the tip
        sway = 1.6 * np.sin(2 * t[f] + ph[1]) * v ** 1.5
        tall = 0.8 + 0.18 * level[f]
        width = 6.5 * np.clip(1 - v / tall, 0, 1) ** 0.6 * np.clip(v * 4 + 0.3, 0, 1)
        k = np.clip(1 - np.abs(fx - 15.5 - sway) / np.maximum(width, 1e-3), 0, 1) * (v < tall)
        heat = np.clip(k ** 0.5 * (1.2 - 0.8 * v / tall), 0, 1)
        idx[16:32, :32] = np.where(heat < 0.1, 0, np.clip(np.round(heat * 11), 1, 11))
        # a firefly's glow: a bright core, a soft halo (tinted when drawn)
        d = np.hypot(xx[16:32, 32:48] - 39.5, yy[16:32, 32:48] - 23.5) / 8
        g = np.clip(1 - d, 0, 1) ** 2
        idx[16:32, 32:48] = np.where(g < 0.04, 0, np.clip(np.round(1 + g * 10), 1, 11))
        # a raindrop (columns 48..63, the runtime stretches it along its
        # fall): a round bright head at the foot, a thin tail fading up it,
        # the palest colours only (tinted blue white when drawn)
        dy, dx = yy[16:32, 48:64] - 16, xx[16:32, 48:64] - 55.5
        head = np.hypot(dx / 2.6, (dy - 12.5) / 2.6) < 1
        tail = (np.abs(dx) < 0.8 + 1.6 * dy / 15) & (dy >= 2) & (dy < 13)
        drop = np.where(head, 11, np.where(tail, np.where(dy > 8, 11, 10), 0))
        drop[(dy < 6) & tail & ((dy + xx[16:32, 48:64]) % 2 == 0)] = 0      # the tail's end thinning out
        idx[16:32, 48:64] = drop
        # the light on the floor
        gx, gz = np.meshgrid((np.arange(64) - 31.5) / 32, (np.arange(32) - 15.5) / 16)
        light = level[f] * np.clip(1 - np.hypot(gx, gz), 0, 1) ** 1.6
        idx[32:] = np.where(light < 0.05, 0, np.clip(1 + np.floor(light * 7), 1, 7))
        frames.append(idx)
    return colours, frames


SPRAY_COLS, BASIN_COLS = (0, 47), (48, 63)     # the fountain picture: its spray seen from the side, its basin's water


def fountain_frames(n: int) -> tuple[list[int], list]:
    """A fountain's water as it plays: n frames (round and round), one
    palette. Columns 0..47 the spray seen from the side: jets rising from
    the foot's middle and falling outward in arcs, drops running along
    them (colour 0 nothing; added, see-through); columns 48..63 the
    basin's water from its rim (row 0) to its middle (row 63): dark,
    rings running out from the middle where the spray falls. Colours 1..8
    dark blue to white."""
    ramp = np.array([(20, 30, 46), (32, 48, 70), (48, 68, 96), (70, 94, 124), (100, 126, 156), (136, 162, 190),
                     (176, 198, 220), (226, 236, 246)], float)
    colours = [0] + [0x8000 | int(c) for c in bgr555(ramp)] + [0] * 7
    jets = ((21, 58), (13, 52), (23, 36), (6, 62))
    def put(idx, x, y, v, wide=2):
        for dx in range(wide):
            ix, iy = int(round(x)) + dx - wide // 2, int(round(y))
            for yy in (iy, iy + 1) if wide > 1 else (iy,):
                if 0 <= ix <= 47 and 0 <= yy < 64:
                    idx[yy, ix] = max(idx[yy, ix], v)
    frames = []
    for f in range(n):
        idx = np.zeros((64, 64), np.uint16)
        for k, (reach, height) in enumerate(jets):
            arc = lambda tt, side: (23.5 + side * reach * tt, 63 - height * 4 * tt * (1 - tt) - 4 * tt)
            for side in (-1, 1):
                # the jet itself: a stream, brighter where it turns over
                for tt in np.linspace(0, 1, 90):
                    put(idx, *arc(tt, side), 3 + int(2 * np.exp(-((tt - 0.5) / 0.2) ** 2)))
                # bright drops running along it, a twelfth of the way each frame
                for drop in range(12):
                    t0 = ((drop + f / n + 0.29 * k + (0.5 if side > 0 else 0)) / 12) % 1
                    put(idx, *arc(t0, side), 8)
                    if t0 > 0.03:
                        put(idx, *arc(t0 - 0.03, side), 6)
        idx[56:, 21:27] = np.maximum(idx[56:, 21:27], 6)        # the spout's column of water
        # the basin: rings running out from the middle (row 63) to the rim
        yy = np.arange(64)[:, None].repeat(16, 1)
        wave = np.sin(2 * np.pi * ((63 - yy) / 16 - f / n))
        idx[:, 48:] = np.clip(np.round(2.5 + 1.5 * wave + (wave > 0.85) * 2), 1, 6)
        frames.append(idx)
    return colours, frames


def pool_animation(cfg: dict, pool: list, vr: Vram) -> tuple:
    """The Taj Mahal's one animation: the pool row (pool_frames()) and, in
    the slot after it, the lamps' picture (mughal_frames()), each frame of
    them side by side as the shown ones (as beach_animation()), played
    round and round. Returns the lamps' shown picture, the rectangle's
    place and the frames' places."""
    spec = cfg['pool']
    n = spec.get('frames', 8)
    waves = pool_frames(pool, spec)
    lcolours, lamps = mughal_frames(n)
    x0, y0 = pool[0][3]
    at = (y0 - 512) // 64 * 64 + x0 // 16
    shown = vr.picture(lamps[0], lcolours, at=at + len(pool))
    water = None
    if spec.get('spray'):
        wcolours, plays = fountain_frames(n)
        water = vr.picture(plays[0], wcolours, at=at + len(pool) + 1)
    places = []
    for f in range(n):
        g = vr.frame_run(len(pool) + after_sea(cfg))
        for c, cell in enumerate(pool):
            vr.picture(waves[f][c], None, at=g + c, clut=cell[2])
        vr.picture(lamps[f], None, at=g + len(pool), clut=shown[2])
        if water:
            vr.picture(plays[f], None, at=g + len(pool) + 1, clut=water[2])
        places.append(((g % 64) * 16, 512 + (g // 64) * 64))
    return shown, (x0, y0), places, water


def cloud_frames(cells: list, row: np.ndarray, spec: dict) -> list:
    """The cloud row's cells as the clouds billow ("clouds": "frames" of
    them, played round and round; each cell's own 16 colours, only its
    indices change): the row's picture (row, RGB) drawn aside and up by a
    slow wave of "billow" pixels running along it, nothing at the row's
    top and foot (the rows over and under it stand still), each cell then
    taken back to its own palette; "foot" pixels: from that row down it
    stands still (Cambodia's tree line under the clouds). Returns per frame
    the cells' indices."""
    n, amp, length = spec.get('frames', 6), spec.get('billow', 1.6), spec.get('wave', 110)
    h, w = row.shape[:2]
    yy, xx = np.mgrid[0:h, 0:w].astype(float)
    taper = np.sin(np.pi * (yy + 0.5) / h) ** 1.5
    if spec.get('foot'):
        taper = np.sin(np.pi * np.clip((yy + 0.5) / spec['foot'], 0, 1)) ** 1.5
    out = [[] for _ in range(n)]
    for f in range(n):
        t = 2 * np.pi * f / n
        dx = amp * taper * np.sin(t - 2 * np.pi * xx / length + 0.09 * yy)
        dy = 0.5 * amp * taper * np.sin(t - 2 * np.pi * xx / (0.7 * length) + 1.3)
        sx, sy = (xx - dx) % w, np.clip(yy - dy, 0, h - 1)
        x0, y0 = np.floor(sx).astype(int) % w, np.floor(sy).astype(int)
        fx, fy = (sx - x0)[..., None], (sy - y0)[..., None]
        x1, y1 = (x0 + 1) % w, np.minimum(y0 + 1, h - 1)
        img = (row[y0, x0] * (1 - fx) * (1 - fy) + row[y0, x1] * fx * (1 - fy) + row[y1, x0] * (1 - fx) * fy
               + row[y1, x1] * fx * fy)
        if spec.get('moon'):
            # the moon still, the clouds billowing over it, its halo breathing
            full = np.zeros((64 * (spec['moon_row'] + 1), w, 3))
            full[-64:] = img
            img = moon_over(full, spec['moon'], t, behind=row)[-64:]
        for c, (idx, colours, *_) in enumerate(cells):
            shown = rgb_of(np.array(colours, np.uint16))
            cc = spec['cols'][c] if spec.get('cols') else c          # "twice": the cell's column in the row
            part = img[:, 64 * cc:64 * cc + 64].reshape(-1, 3)
            out[f].append(((part[:, None] - shown[None]) ** 2).sum(2).argmin(1).reshape(64, 64).astype(np.uint16))
    return out


FLAG_ROWS = (0, 15)                           # the Szechwan picture: five prayer flags, then a butterfly (GLOW_COLS, DIYA_ROWS)
FLAG_COLOURS = ((36, 92, 184), (236, 236, 228), (204, 42, 34), (42, 142, 64), (242, 202, 44))


def szechwan_frames(n: int) -> tuple[list[int], list]:
    """Szechwan's picture as the wind plays: n frames (round and round), one
    palette. Rows 0..15 five prayer flags side by side (blue, white, red,
    green, yellow, columns 12 k + 1 .. 12 k + 11), a wave of shade running
    across each and its loose foot corner lifting and falling, a wind horse
    printed on it; rows 16..31 columns 32..47 a butterfly seen from above
    (white, the runtime tints it and beats its wings: FFLY). Colour 0
    nothing; 1..10 the flags' colours, light and shaded; 11 black; 12, 13
    the butterfly's wing."""
    pal = [(0, 0, 0)]
    for rgb in FLAG_COLOURS:
        pal += [rgb, tuple(int(v * 0.68) for v in rgb)]
    pal += [(24, 22, 20), (252, 250, 240), (214, 206, 184), (0, 0, 0), (0, 0, 0)]
    colours = [0] + [0x8000 | int(c) for c in bgr555(np.array(pal[1:], float))]
    yy, xx = np.mgrid[0:64, 0:64]
    rnd = np.random.default_rng(91)
    horse = rnd.uniform(0, 1, (6, 6)) > 0.55                    # the printed wind horse, a blot of ink
    frames = []
    for f in range(n):
        t = 2 * np.pi * f / n
        idx = np.zeros((64, 64), np.uint16)
        for k in range(5):
            x0 = 12 * k + 1
            fx, fy = xx[:16, x0:x0 + 11] - x0, yy[:16, x0:x0 + 11]
            wave = np.sin(t + k * 1.3 - fx * 0.55 + fy * 0.12)
            flag = np.where(wave < -0.35, 2 * k + 2, 2 * k + 1)
            # the foot's free corner, lifting with the wind
            foot = 15 - 2.5 * (0.5 + 0.5 * np.sin(t + k * 1.3)) * (fx / 10)
            flag = np.where(fy > foot, 0, flag)
            ink = np.zeros_like(fx, bool)
            if RESERVED:                    # the add-on's: a horse's shape
                ink[5:11, 1:10] = GLYPHS['horse']
            else:
                ink[5:11, 2:8] = horse
            flag = np.where(ink & (flag > 0), 2 * k + 2, flag)
            idx[:16, x0:x0 + 11] = flag
        # the butterfly: two wings each side of a black body, dark edges and a spot
        by, bx = yy[16:32, 32:48] - 23.5, xx[16:32, 32:48] - 39.5
        fore = ((np.abs(bx) - 3.8) / 4.2) ** 2 + ((by + 2.2) / 4.6) ** 2 < 1
        hind = ((np.abs(bx) - 3.0) / 3.2) ** 2 + ((by - 3.6) / 3.4) ** 2 < 1
        wing = (fore | hind) & (np.abs(bx) > 0.6)
        edge = wing & ~(((np.abs(bx) - 3.8) / 3.2) ** 2 + ((by + 2.2) / 3.6) ** 2 < 1) & ~(
            ((np.abs(bx) - 3.0) / 2.3) ** 2 + ((by - 3.6) / 2.5) ** 2 < 1)
        fly = np.where(wing, np.where(np.abs(bx) > 4.5, 13, 12), 0)
        fly = np.where(edge, 11, fly)
        fly = np.where(wing & (np.hypot(np.abs(bx) - 4.5, by + 2.5) < 1.1), 11, fly)
        fly = np.where((np.abs(bx) < 0.8) & (np.abs(by) < 6), 11, fly)
        idx[16:32, 32:48] = fly
        frames.append(idx)
    return colours, frames


def smoke_frames(n: int, plume: bool = False) -> tuple[list[int], list]:
    """Incense smoke as it rises: n frames of 64 x 64 (round and round), its
    own palette, added. Two thin strands from the foot's middle, swaying
    more and spreading wider as they climb, a slow wave going up them,
    fading out at the top. Colour 0 nothing, 1..15 grey, dim to light.
    plume: a beacon fire's smoke instead (the Great Wall's), one thick
    column billowing out as it climbs, puffs rolling up it."""
    colours = [0] + [0x8000 | int(bgr555(np.array([150, 150, 156], float) * (k / 15) ** 1.3)) for k in range(1, 16)]
    yy, xx = np.mgrid[0:64, 0:64].astype(float)
    up = 1 - yy / 63                                           # 0 the foot, 1 the top
    frames = []
    for f in range(n):
        t = 2 * np.pi * f / n
        k = np.zeros((64, 64))
        if plume:
            centre = 31.5 + 7 * up ** 1.5 * np.sin(2 * np.pi * up * 0.8 - t)
            width = 5 + 20 * up ** 1.1
            puffs = 0.7 + 0.3 * np.sin(2 * np.pi * up * 3 - t) * np.cos((xx - centre) * 0.25 + t)
            k = np.exp(-((xx - centre) / width) ** 2 * 1.6) * puffs * (1 - up) ** 0.9
            k = np.clip(k * 1.2 * np.clip(up * 10, 0, 1), 0, 1)
            frames.append(np.where(k < 0.07, 0, np.clip(np.round(k * 15), 1, 15)).astype(np.uint16))
            continue
        for s, ph in ((0, 0.0), (1, 2.2)):
            centre = 31.5 + (2 + 14 * up ** 1.4) * np.sin(2 * np.pi * up * 1.6 - t + ph) + (s - 0.5) * 3 * up
            width = 0.9 + 6 * up ** 1.6
            k += np.exp(-((xx - centre) / width) ** 2) * (1 - up) ** 0.8 * (0.9 - 0.3 * s)
        k = np.clip(k * np.clip(up * 12, 0, 1), 0, 1)
        frames.append(np.where(k < 0.07, 0, np.clip(np.round(k * 15), 1, 15)).astype(np.uint16))
    return colours, frames


WALL_FLAME = ((32, 63), (0, 31))             # the Great Wall picture: the beacon's fire (rows, cols)
WALL_DUST = (48, 16, 16, 16)                 # and a puff of dust: u, v, width, height (FTHR shape 5)


def great_wall_frames(n: int) -> tuple[list[int], list]:
    """The Great Wall's picture as the wind plays (Szechwan's place:
    szechwan_frames()): n frames (round and round), one palette. Rows
    0..15 five war banners (columns 12 k + 1 .. 12 k + 11: red bordered
    yellow, yellow bordered red, black bordered red, red bordered black,
    white bordered red), the border's teeth along the free sides, a round
    seal in the middle, a wave of shade running down each, the free foot
    lifting; rows 16..31 columns 32..47 a dragonfly seen from above (white,
    the runtime tints it: FFLY kind 3), columns 48..63 a puff of dust (grey,
    added: FTHR shape 5); rows 32..63 columns 0..31 the beacon's fire,
    flickering (added). Colour 0 nothing; 1..5 red, dark red, yellow, dark
    yellow, black; 6..8 the dragonfly; 9..11 the dust; 12..15 the fire."""
    pal = [(0, 0, 0), (200, 40, 30), (132, 24, 20), (238, 192, 52), (168, 128, 30), (26, 22, 22), (236, 232, 220),
           (128, 130, 134), (186, 202, 214), (40, 38, 34), (80, 76, 68), (132, 126, 112), (150, 30, 8), (232, 92, 20),
           (252, 182, 52), (255, 240, 172)]
    colours = [0] + [0x8000 | int(c) for c in bgr555(np.array(pal[1:], float))]
    yy, xx = np.mgrid[0:64, 0:64]
    # field, border, its shade: the five banners
    kinds = ((1, 3), (3, 1), (5, 1), (1, 5), (6, 1))
    shade = {1: 2, 3: 4, 5: 5, 6: 7}
    rnd = np.random.default_rng(93)
    noise = rnd.uniform(0, 1, (64, 64))
    frames = []
    for f in range(n):
        t = 2 * np.pi * f / n
        idx = np.zeros((64, 64), np.uint16)
        for k, (field, border) in enumerate(kinds):
            x0 = 12 * k + 1
            fx, fy = xx[:16, x0:x0 + 11] - x0, yy[:16, x0:x0 + 11]
            wave = np.sin(t + k * 1.1 - fy * 0.5 + fx * 0.15)
            flag = np.where(wave < -0.4, shade[field], field)
            # the border: the free side (right) and the foot, toothed
            edge = (fx >= 9) | ((fy >= 14) & ((fx % 3) < 2)) | ((fx == 8) & ((fy % 3) < 1.5))
            flag = np.where(edge, np.where(wave < -0.4, shade[border], border), flag)
            # its character: three strokes across, one down (the old fork's,
            # read as 玉); the add-on's 明 (Ming), the middle one 令 (command)
            if RESERVED:
                g = GLYPHS['令' if k == 2 else '明']
                glyph = np.zeros(fx.shape, bool)
                glyph[2:2 + g.shape[0], 1:1 + g.shape[1]] = g
            else:
                glyph = ((fx >= 2) & (fx <= 6) & np.isin(fy, (3, 7, 11))) | ((fx == 4) & (fy >= 3) & (fy <= 11)) |                 ((fx == 6) & (fy >= 8) & (fy <= 9))
            flag = np.where(glyph, border, flag)
            # the foot lifting with the wind, its free corner most
            foot = 15.5 - 2.0 * (0.5 + 0.5 * np.sin(t + k * 1.1)) * (fx / 10)
            flag = np.where(fy > foot, 0, flag)
            idx[:16, x0:x0 + 11] = flag
        # the dragonfly: a long body down the middle, the head at the top,
        # two pairs of narrow wings across
        by, bx = yy[16:32, 32:48] - 23.5, xx[16:32, 32:48] - 39.5
        fore = (((np.abs(bx) - 3.6) / 3.6) ** 2 + ((by + 1.6) / 1.05) ** 2 < 1) & (np.abs(bx) > 0.6)
        hind = (((np.abs(bx) - 3.4) / 3.4) ** 2 + ((by + 0.1) / 1.0) ** 2 < 1) & (np.abs(bx) > 0.6)
        fly = np.where(fore | hind, 8, 0)
        fly = np.where((fore | hind) & (np.abs(bx) > 6.2), 7, fly)
        body = (np.abs(bx) < 0.7) & (by > -5.5) & (by < 7.5)
        fly = np.where(body, np.where((by > 0) & ((by.astype(int) % 2) == 0), 7, 6), fly)
        fly = np.where(np.hypot(bx, by + 6.2) < 1.3, 6, fly)
        idx[16:32, 32:48] = fly
        # the dust: a soft wisp, lighter in its heart
        dy, dx = yy[16:32, 48:64] - 23.5, xx[16:32, 48:64] - 55.5
        k = np.exp(-(dx / 5.2) ** 2 - (dy / 4.6) ** 2) * (0.75 + 0.25 * noise[16:32, 48:64])
        idx[16:32, 48:64] = np.where(k < 0.18, 0, np.where(k < 0.4, 9, np.where(k < 0.65, 10, 11)))
        # the fire: tongues licking up from a wide foot, flickering
        fy, fx = (yy[32:64, 0:32] - 32) / 31.0, (xx[32:64, 0:32] - 15.5) / 15.5
        up = 1 - fy                                            # 0 the foot, 1 the top
        sway = 0.12 * np.sin(t * 2 + up * 5) * up
        tongues = 0.5 + 0.5 * np.sin(fx * 7.5 + np.sin(t + up * 3) * 1.5 + t * 3)
        width = (1 - up ** 0.8) * (0.75 + 0.25 * tongues)
        heat = np.clip(1 - np.abs(fx - sway) / np.maximum(width, 0.01), 0, 1) * (1 - up) ** 0.3
        heat *= 0.8 + 0.4 * noise[32:64, 0:32] * np.sin(t * 3 + noise[32:64, 0:32] * 6) ** 2
        idx[32:64, 0:32] = np.where(heat < 0.12, 0, np.where(heat < 0.35, 12, np.where(heat < 0.6, 13,
                                                                                       np.where(heat < 0.82, 14, 15))))
        frames.append(idx)
    return colours, frames


PALM_FROND = ((0, 31), (0, 31))              # the Cambodia picture: a sugar palm's fan (rows, cols)
LOTUS_FLOWER = ((0, 15), (32, 47))           # a lotus flower seen from the side, a leaf on its stalk (cols 48..63)
LOTUS_LEAF = ((0, 15), (48, 63))
LOTUS_POND = ((32, 63), (0, 63))             # a patch of lotus pond seen from above


def cambodia_frames(n: int) -> tuple[list[int], list]:
    """Cambodia's picture as the wind plays (Szechwan's place:
    szechwan_frames()): n frames, one palette. Rows 0..31 columns 0..31 a
    sugar palm's fan leaf, its leaflets spreading from the stalk at the
    foot's middle, their split tips stirring in the wind; rows 0..15
    columns 32..47 a lotus flower (pink, its petals cupped round the
    yellow heart) and 48..63 a lotus leaf held up on its stalk; rows 16..31
    columns 48..63 a raindrop (RAIN_COLS, DIYA_ROWS: as the Taj Mahal's);
    rows 32..63 a patch of lotus pond seen from above, dark water, round
    leaves and a few flowers, its edge ragged (on nothing). Colour 0
    nothing; 1..4 the fan's greens, 5 its stalk; 6, 7 the lotus leaves; 8..10
    pink; 11 yellow; 12, 13 the raindrop; 14, 15 the pond's water."""
    pal = [(0, 0, 0), (36, 70, 30), (58, 98, 40), (86, 124, 50), (128, 150, 70), (104, 86, 52), (52, 100, 46),
           (96, 146, 70), (176, 70, 112), (226, 124, 164), (246, 200, 218), (238, 196, 62), (236, 242, 250),
           (176, 190, 210), (24, 42, 46), (44, 70, 76)]
    colours = [0] + [0x8000 | int(c) for c in bgr555(np.array(pal[1:], float))]
    yy, xx = np.mgrid[0:64, 0:64].astype(float)
    rnd = np.random.default_rng(95)
    # the pond: still, the same in every frame
    pond = np.zeros((32, 64), np.uint16)
    py, px = yy[32:] - 47.5, xx[32:] - 31.5
    edge = np.hypot(px / 31, py / 15.5) + 0.06 * np.sin(np.arctan2(py, px) * 7) + 0.03 * rnd.normal(0, 1, (32, 64))
    water = edge < 0.98
    pond[water] = np.where((xx[32:] + 2 * yy[32:]).astype(int)[water] % 9 == 0, 15, 14)
    for _ in range(14):
        cx, cy, r = rnd.uniform(6, 58), rnd.uniform(4, 28), rnd.uniform(3, 5.5)
        d = np.hypot(xx[32:] - cx, yy[32:] - 32 - cy)
        notch = np.abs(np.arctan2(yy[32:] - 32 - cy, xx[32:] - cx) - rnd.uniform(-3, 3)) < 0.25
        leaf = (d < r) & ~notch & water
        pond[leaf] = np.where(d[leaf] < r * 0.55, 7, 6)
    for _ in range(4):
        cx, cy = rnd.uniform(10, 54), rnd.uniform(6, 26)
        d = np.hypot(xx[32:] - cx, yy[32:] - 32 - cy)
        pond[d < 2.4] = 9
        pond[d < 1.2] = 11
    # the lotus flower: petals cupped up round the heart
    fy, fx = yy[:16, 32:48] - 15, xx[:16, 32:48] - 39.5
    flower = np.zeros((16, 16), np.uint16)
    for a, rx in ((0.0, 2.6), (-0.55, 2.4), (0.55, 2.4), (-1.0, 2.0), (1.0, 2.0)):
        u = fx * np.cos(a) + (fy + 5) * np.sin(a)
        v = -fx * np.sin(a) + (fy + 5) * np.cos(a)
        petal = (u / rx) ** 2 + (v / 6.2) ** 2 < 1
        flower[petal] = np.where(v[petal] < -2, 10, np.where(np.abs(u[petal]) > rx * 0.6, 8, 9))
    flower[(np.abs(fx) < 0.7) & (fy > -1)] = 6
    flower[np.hypot(fx, fy + 6.5) < 1.2] = 11
    # the leaf on its stalk: a wide shallow cup seen from the side
    ly, lx = yy[:16, 48:64] - 15, xx[:16, 48:64] - 55.5
    leaf = np.zeros((16, 16), np.uint16)
    cup = (np.abs(lx) < 7.5) & (ly > -12 + 0.06 * lx ** 2) & (ly < -9 + 0.04 * lx ** 2)
    leaf[cup] = np.where(ly[cup] < -10.5 + 0.05 * lx[cup] ** 2, 7, 6)
    leaf[(np.abs(lx - 0.5) < 0.7) & (ly > -9)] = 6
    # the raindrop: as mughal_frames()'s
    dy, dx = yy[16:32, 48:64] - 16, xx[16:32, 48:64] - 55.5
    head = np.hypot(dx / 2.6, (dy - 12.5) / 2.6) < 1
    tail = (np.abs(dx) < 0.8 + 1.6 * dy / 15) & (dy >= 2) & (dy < 13)
    drop = np.where(head, 12, np.where(tail, np.where(dy > 8, 12, 13), 0)).astype(np.uint16)
    drop[(dy < 6) & tail & ((dy + xx[16:32, 48:64]) % 2 == 0)] = 0
    angles = np.linspace(-1.35, 1.35, 23)
    lengths = 27 + 3 * rnd.uniform(-1, 1, len(angles))
    phases = rnd.uniform(0, 2 * np.pi, len(angles))
    frames = []
    for f in range(n):
        t = 2 * np.pi * f / n
        idx = np.zeros((64, 64), np.uint16)
        # the fan: leaflets from the stalk's end (15.5, 29), each a narrow
        # pleat, its tip split and stirring, the pleats light and dark in turn
        oy, ox = yy[:32, :32] - 29, xx[:32, :32] - 15.5
        r, a = np.hypot(ox, oy), np.arctan2(ox, -oy)
        fan = np.zeros((32, 32), np.uint16)
        for k, (a0, ln, ph) in enumerate(zip(angles, lengths, phases)):
            stir = 0.05 * np.sin(t + ph) * np.clip((r - 14) / 12, 0, 1)
            near = np.abs(a - a0 - stir) < 0.055 + 0.01 * (r < 8)
            tip = r > ln - 4
            split = tip & (np.abs(a - a0 - stir) < 0.012)
            on = near & (r > 2.5) & (r < ln) & ~split
            fan[on] = (1 + k % 2) + (r[on] > 0.5 * ln) * (1 + (k % 3 == 0))
        fan[(np.abs(ox) < 1.2) & (oy > -2) & (oy < 3)] = 5
        idx[:32, :32] = fan
        idx[:16, 32:48] = flower
        idx[:16, 48:64] = leaf
        idx[16:32, 48:64] = drop
        idx[32:] = pond
        frames.append(idx)
    return colours, frames


OLIVE_SPRAY = ((0, 31), (0, 31))             # the Athens picture: an olive spray (rows, cols)
OLIVE_BOUGH = ((32, 63), (0, 63))            # a wider bough of olive leaves
GREEK_FLAG = ((0, 20), (32, 63))             # the Greek flag, waving


def athens_frames(n: int) -> tuple[list[int], list]:
    """Athens' picture as the wind plays (cambodia_frames()'s place): n
    frames, one palette. Rows 0..31 columns 0..31 a clump of olive leaves
    on its twig, dense, lit from above, shaded deep inside, the leaves
    stirring and turning their silver undersides up now and then; rows
    32..63 a wider bough of such clumps; rows 0..20 columns 32..63 the Greek flag
    (nine blue and white stripes, the white cross on blue in its corner),
    waving from the pole at its left, the wave growing toward its free
    end, its folds light and dark. Colour 0 nothing; 1..4 the olive
    leaves (shaded, mid, lit, silver), 5 the twigs, 6 and 13 the shade deep
    in the clumps, 12 a silver leaf's light midrib; 7..9 the flag's blue
    (shown, in a fold, lit), 10, 11 its white."""
    pal = [(0, 0, 0), (58, 74, 50), (86, 102, 66), (118, 132, 92), (164, 174, 150), (88, 78, 62), (38, 52, 36),
           (16, 92, 176), (10, 62, 128), (60, 132, 210), (242, 242, 238), (186, 192, 204), (200, 208, 190), (70, 88, 56),
           (0, 0, 0), (0, 0, 0)]
    colours = [0] + [0x8000 | int(c) for c in bgr555(np.array(pal[1:], float))]
    rnd = np.random.default_rng(111)

    def clump(h: int, w: int, blobs: list, count: int):
        # blobs: [(cx, cy, rx, ry)] the clumps' outlines; the leaves inside
        # them, each pointing out from its clump's middle, lit from above
        out = []
        while len(out) < count:
            cx, cy, rx, ry = blobs[int(rnd.integers(len(blobs)))]
            r, a = np.sqrt(rnd.uniform(0, 1)), rnd.uniform(0, 2 * np.pi)
            px, py = cx + rx * r * np.cos(a), cy + ry * r * np.sin(a)
            if not (1 <= px < w - 1 and 1 <= py < h - 1): continue
            d = np.arctan2(py - cy, px - cx) + rnd.normal(0, 0.6)
            lit = np.clip(0.5 - (py - cy) / (2 * ry) + rnd.normal(0, 0.18), 0, 1)
            out.append((px, py, d, rnd.uniform(3.6, 5.6), rnd.uniform(1.0, 1.5), rnd.uniform(0, 2 * np.pi), lit, r))
        return sorted(out, key=lambda l: -l[7])              # the outer leaves first, the inner over them

    def draw(h: int, w: int, blobs: list, leaves: list, stalk, t: float) -> np.ndarray:
        yy, xx = np.mgrid[0:h, 0:w].astype(float)
        idx = np.zeros((h, w), np.uint16)
        for cx, cy, rx, ry in blobs:                         # the shade deep in the clumps
            inner = np.hypot((xx - cx) / (rx * 0.8), (yy - cy) / (ry * 0.8)) < 1
            low = yy > cy - 1
            idx[inner] = np.where(low[inner], np.where((xx[inner] + 3 * yy[inner]) % 7 < 3, 13, 6), 2)
        x0, y0, x1, y1 = stalk
        for s in np.linspace(0, 1, 30):
            idx[np.hypot(xx - x0 - (x1 - x0) * s, yy - y0 - (y1 - y0) * s) < 1.4 - 0.7 * s] = 5
        for px, py, d, ln, wd, ph, lit, _ in leaves:
            a = d + 0.25 * np.sin(t + ph)
            u = (xx - px) * np.cos(a) + (yy - py) * np.sin(a)
            v = -(xx - px) * np.sin(a) + (yy - py) * np.cos(a)
            leaf = (u > -0.5) & (u < ln) & (np.abs(v) < wd * np.sin(np.pi * np.clip((u + 0.5) / (ln + 0.5), 0, 1)) ** 0.6)
            if np.sin(t + ph) > 0.82:                        # turned over in the wind: silver
                idx[leaf] = 4 if lit > 0.45 else 3
                idx[leaf & (np.abs(v) < 0.3)] = 12
            else:
                idx[leaf] = 1 + int(round(lit * 2))
        return idx

    blobs_a = [(16, 14, 14, 11), (10, 19, 8, 7), (23, 19, 8, 7)]
    blobs_b = [(17, 16, 15, 12), (45, 14, 16, 11), (31, 19, 13, 9), (56, 20, 7, 7)]
    leaves_a, leaves_b = clump(32, 32, blobs_a, 190), clump(32, 64, blobs_b, 380)
    # the flag: 32 x 21, its stripes and the canton as Greece's
    fh, fw = 21, 32
    fy, fx = np.mgrid[0:fh, 0:fw].astype(float)
    frames = []
    for f in range(n):
        t = 2 * np.pi * f / n
        idx = np.zeros((64, 64), np.uint16)
        idx[:32, :32] = draw(32, 32, blobs_a, leaves_a, (16, 31, 15, 20), t)
        idx[32:, :] = draw(32, 64, blobs_b, leaves_b, (30, 31, 32, 20), t)
        u = fx / (fw - 1)
        phase = 2 * np.pi * 1.3 * u - t
        sy = fy - 2.2 * u * np.sin(phase)
        sx = fx - 0.8 * u * (1 - np.cos(phase))
        on = (sy >= 0) & (sy < fh) & (sx >= 0)
        stripe = np.clip((sy * 9 / fh).astype(int), 0, 8)
        canton = (sy < 5 * fh / 9) & (sx < 5 * fh / 9)
        cy, cx = 5 * fh / 18, 5 * fh / 18
        cross = canton & ((np.abs(sy - cy) < fh / 18) | (np.abs(sx - cx) < fh / 18))
        white = np.where(canton, cross, stripe % 2 == 1)
        fold = np.cos(phase) * u
        flag = np.where(white, np.where(fold < -0.35, 11, 10), np.where(fold < -0.35, 8, np.where(fold > 0.45, 9, 7)))
        idx[:fh, 32:32 + fw] = np.where(on, flag, 0).astype(np.uint16)
        frames.append(idx)
    return colours, frames


TUMBLEWEED = (0, 0, 32, 32)                  # the Nowhere Desert picture: a tumbleweed (u, v, width, height)
DESERT_PUFF = (32, 0, 16, 16)                # and a puff of dust (the dust devil's: FTHR shape 6)


def desert_frames(n: int) -> tuple[list[int], list]:
    """Nowhere Desert's picture (athens_frames()'s place): n frames, all
    the same (the runtime turns and moves what is in it). Rows 0..31
    columns 0..31 a tumbleweed, a ball of dry tangled twigs, its outer
    shell thicker, lit from above, on nothing; rows 0..15 columns 32..47 a
    soft puff of dust (grey, added: the dust devil). Colour 0 nothing; 1..4
    the twigs, dark to light; 5..7 the dust."""
    pal = [(0, 0, 0), (84, 66, 46), (124, 98, 66), (162, 134, 92), (198, 172, 124), (40, 38, 36), (84, 80, 74),
           (140, 132, 120)] + [(0, 0, 0)] * 8
    colours = [0] + [0x8000 | int(c) for c in bgr555(np.array(pal[1:], float))]
    rnd = np.random.default_rng(121)
    idx = np.zeros((64, 64), np.uint16)
    weed = np.zeros((32, 32), np.uint16)
    yy, xx = np.mgrid[0:32, 0:32].astype(float)
    weed[np.hypot(xx - 15.5, yy - 15.5) < 11.5] = 1          # the dark tangle deep inside
    weed[(np.hypot(xx - 15.5, yy - 15.5) < 11.5) & (rnd.uniform(0, 1, (32, 32)) < 0.25)] = 0
    for _ in range(420):
        r = 14.5 * np.sqrt(rnd.uniform(0.0, 1.0)) ** 0.5
        a = rnd.uniform(0, 2 * np.pi)
        x, y = 15.5 + r * np.cos(a), 15.5 + r * np.sin(a)
        d, bend = rnd.uniform(0, 2 * np.pi), rnd.normal(0, 0.25)
        shade = int(np.clip(round(2.5 - 1.6 * (y - 15.5) / 15 + rnd.normal(0, 0.6)), 1, 4))
        for _ in range(int(rnd.integers(4, 10))):
            ix, iy = int(round(x)), int(round(y))
            if np.hypot(x - 15.5, y - 15.5) < 15.5 and 0 <= ix < 32 and 0 <= iy < 32:
                weed[iy, ix] = shade
            d += bend
            x, y = x + np.cos(d), y + np.sin(d)
    idx[:32, :32] = weed
    yy, xx = np.mgrid[0:16, 0:16].astype(float)
    k = np.exp(-((xx - 7.5) / 5.4) ** 2 - ((yy - 7.5) / 5.4) ** 2) * (0.75 + 0.25 * rnd.uniform(0, 1, (16, 16)))
    idx[:16, 32:48] = np.where(k < 0.16, 0, np.where(k < 0.38, 5, np.where(k < 0.64, 6, 7)))
    return colours, [idx.copy() for _ in range(n)]


HILLS_FLUFF = (0, 0, 16, 16)                 # the Hills picture: a dandelion's seed (FTHR shape 7)


def hills_frames(n: int) -> tuple[list[int], list]:
    """The Hills' picture (athens_frames()'s place): n frames, all the
    same. Rows 0..15 columns 0..15 a dandelion's seed: its white parachute
    of fine hairs spread from the top of a thin beak, the brown seed at
    the foot; nothing round it."""
    S = 64
    yy, xx = np.mgrid[0:S, 0:S].astype(float) / 4
    rgba = np.zeros((S, S, 4))
    cx, cy = 7.5, 5.0
    r, a = np.hypot(xx - cx, yy - cy), np.arctan2(yy - cy, xx - cx)
    hairs = (np.abs(np.sin(a * 11)) < 0.22) & (r < 5.2) & (yy < cy + 1.2)
    hairs |= (r < 1.1)
    tips = (np.abs(r - 4.9) < 0.6) & (yy < cy + 1.5)
    rgba[hairs | tips] = (244, 244, 236, 255)
    beak = (np.abs(xx - cx) < 0.35) & (yy > cy) & (yy < 12.5)
    rgba[beak] = (210, 206, 190, 255)
    seed = np.hypot((xx - cx) / 0.8, (yy - 13.4) / 1.6) < 1
    rgba[seed] = (120, 86, 52, 255)
    small = np.asarray(Image.fromarray(rgba.astype(np.uint8)).resize((16, 16), Image.LANCZOS), float)
    pic = np.zeros((64, 64, 4))
    pic[:16, :16] = small
    pic[..., 3] = np.where(pic[..., 3] >= 90, 255, 0)
    colours, idx = see_through(pic)
    return colours, [idx.astype(np.uint16) for _ in range(n)]


def meadow_frames(cells: list, row: np.ndarray, spec: dict) -> list:
    """The Hills' grass row as the wind runs over it ("clouds" "water" with
    "kind" "meadow"): only the grass sways (green or straw coloured, not
    the grey rock nor the sky), from "from" down, more toward the row's
    foot; gusts ("gust" pixels apart) sweep along it, where one is the
    blades bent further and paler ("sheen"). Each cell's own 16 colours,
    only its indices change. Returns per frame the cells' indices."""
    n, amp, y0 = spec.get('frames', 6), spec.get('ripple', 1.5), spec.get('from', 20)
    h, w = row.shape[:2]
    yy, xx = np.mgrid[0:h, 0:w].astype(float)
    grow = np.clip((yy - y0 + 1) / max(1, h - y0), 0, 1) * (yy >= y0)
    mx, mn = row.max(2), row.min(2)
    grass = (mx - mn > 28) & (row[..., 1] > row[..., 2] * 1.15)
    out = [[] for _ in range(n)]
    for f in range(n):
        t = 2 * np.pi * f / n
        gust = 0.5 + 0.5 * np.cos(2 * np.pi * xx / spec.get('gust', 300) - t)
        dx = amp * (0.3 + grow) * grow * (0.35 + 0.65 * gust) * np.sin(2 * t - 2 * np.pi * xx / spec.get('wave', 46) + 0.5 * yy)
        sx = (xx - dx) % w
        x0 = np.floor(sx).astype(int) % w
        fx = (sx - np.floor(sx))[..., None]
        yi = yy.astype(int)
        img = row[yi, x0] * (1 - fx) + row[yi, (x0 + 1) % w] * fx
        img = img * (1 + spec.get('sheen', 0.1) * gust * grow)[..., None]
        moved = grass & grass[yi, x0] & (grow > 0)
        for c, (idx, colours, *_) in enumerate(cells):
            shown = rgb_of(np.array(colours, np.uint16))
            part = img[:, 64 * c:64 * c + 64].reshape(-1, 3)
            new = ((part[:, None] - shown[None]) ** 2).sum(2).argmin(1).reshape(64, 64)
            out[f].append(np.where(moved[:, 64 * c:64 * c + 64], new, idx).astype(np.uint16))
    return out


def cloud_animation(cfg: dict, cloud: list, vr: Vram, pic: np.ndarray) -> tuple:
    """Szechwan's one animation: the cloud row (cloud_frames()) and, in the
    two slots after it, the flags' and butterfly's picture
    (szechwan_frames()) and the incense smoke (smoke_frames()), each frame
    of them side by side as the shown ones (as pool_animation()), played
    round and round. Returns the flags' shown picture, the rectangle's
    place, the frames' places and the smoke's shown picture."""
    spec = cfg['clouds']
    n = spec.get('frames', 6)
    r = spec['row']
    waves = cloud_frames(cloud, pic[64 * r:64 * r + 64], spec) if spec.get('picture') not in ('cambodia', 'athens', 'desert', 'hills') else None
    x0, y0 = cloud[0][3]
    if not spec.get('extras', True):
        # the row alone (Serene Forest's crowns swaying)
        places = []
        for f in range(n):
            g = vr.frame_run(len(cloud))
            for c, cell in enumerate(cloud):
                vr.picture(waves[f][c], None, at=g + c, clut=cell[2])
            places.append(((g % 64) * 16, 512 + (g // 64) * 64))
        return None, (x0, y0), places, None
    if spec.get('picture') in ('cambodia', 'athens', 'desert', 'hills'):
        # Cambodia's: the cloud row and the lake row ("water", water_frames())
        # side by side, its picture (cambodia_frames()) after them, no
        # smoke; "bounce": the frames played forth and back (fewer of them);
        # Athens': the cloud row alone, its picture athens_frames();
        # Nowhere Desert's: the cloud row, the moon over it (cloud_frames()
        # draws it again on each frame), its picture desert_frames()
        lake = spec.get('water')
        cols = len(cloud) // 2 if lake else len(cloud)
        row = cfg['_moonless'][64 * r:64 * r + 64] if cfg.get('moon') else pic[64 * r:64 * r + 64]
        waves = cloud_frames(cloud[:cols], row, dict(spec, moon=cfg.get('moon'), moon_row=r, cols=cfg.get('_sea_cols')))
        ripples = (meadow_frames if lake and lake.get('kind') == 'meadow' else water_frames)(
            cloud[cols:], pic[64 * lake['row']:64 * lake['row'] + 64], dict(lake, frames=n)) if lake else [[] for _ in range(n)]
        fcolours, flags = {'cambodia': cambodia_frames, 'desert': desert_frames, 'hills': hills_frames}.get(spec['picture'], athens_frames)(n)
        x0, y0 = cloud[0][3]
        at = (y0 - 512) // 64 * 64 + x0 // 16
        shown = vr.picture(flags[0], fcolours, at=at + len(cloud))
        places = []
        for f in range(n):
            g = vr.frame_run(len(cloud) + 1)        # (the add-on's: past the VRAM, T2AN)
            for c, cell in enumerate(cloud):
                vr.picture((waves[f] + ripples[f])[c], None, at=g + c, clut=cell[2])
            vr.picture(flags[f], None, at=g + len(cloud), clut=shown[2])
            places.append(((g % 64) * 16, 512 + (g // 64) * 64))
        if spec.get('bounce'):
            places += places[-2:0:-1]
        return shown, (x0, y0), places, None
    # the Great Wall's banners, fire, dragonfly and dust in the flags' place
    wall = spec.get('picture') == 'great-wall'
    fcolours, flags = (great_wall_frames if wall else szechwan_frames)(n)
    scolours, smoke = smoke_frames(n, plume=wall)
    x0, y0 = cloud[0][3]
    at = (y0 - 512) // 64 * 64 + x0 // 16
    shown = vr.picture(flags[0], fcolours, at=at + len(cloud))
    puff = vr.picture(smoke[0], scolours, at=at + len(cloud) + 1)
    places = []
    for f in range(n):
        g = vr.frame_run(len(cloud) + 2)
        for c, cell in enumerate(cloud):
            vr.picture(waves[f][c], None, at=g + c, clut=cell[2])
        vr.picture(flags[f], None, at=g + len(cloud), clut=shown[2])
        vr.picture(smoke[f], None, at=g + len(cloud) + 1, clut=puff[2])
        places.append(((g % 64) * 16, 512 + (g // 64) * 64))
    return shown, (x0, y0), places, puff


def recolour(rgb: np.ndarray, spec: dict, mean: float) -> np.ndarray:
    """The floor's colours as another ground's ("floor_recolour"): each
    colour's brightness against the tiles' mean (mean) brings "colour", its
    contrast times "contrast", nothing darker than "floor" of the mean (the
    cracks of Tekken 2's dry earth only a trace in the sand)."""
    lum = rgb @ np.array([0.3, 0.59, 0.11]) / mean
    lo = spec.get('floor', 0.0)
    lum = np.where(lum < lo + 0.1, lo + 0.1 * np.exp((lum - lo - 0.1) / 0.1), lum)
    k = 1 + spec.get('contrast', 1.0) * (lum - 1)
    out = np.clip(np.array(spec['colour'], float) * k[..., None], 0, 255)
    if spec.get('keep_paint'):
        # "keep_paint": painted marks (strong colour: the launch pad's yellow
        # lines, its red ring) stay, times it, only the grey goes over
        sat = (rgb.max(-1) - rgb.min(-1)) / np.maximum(1, rgb.max(-1))
        w = np.clip((sat - 0.3) / 0.2, 0, 1)[..., None]
        out = out * (1 - w) + np.clip(rgb * spec['keep_paint'], 0, 255) * w
    return out


def floor_indices(vram: np.ndarray, entry: tuple) -> np.ndarray:
    """A floor tile's own 4-bit indices, turned as t2_stage.floor_tile()."""
    uv, page, _, clut = entry
    tx, ty = (page & 15) * 64, (page >> 4 & 1) * 256
    u, v = np.meshgrid((uv & 255) + np.arange(64), (uv >> 8) + np.arange(64))
    index = vram[(ty + v) % 512, (tx + u // 4) % 1024] >> ((u & 3) * 4) & 15
    return np.rot90(index, (0, 3, 2, 1)[page >> 14 & 3])


def floor(base: scene.Floor, vram: np.ndarray, rec: dict, vr: Vram, tufts: dict | None = None,
          tint=None, recolouring: dict | None = None, flowers: dict | None = None, chase: dict | None = None,
          puddles: dict | None = None, roll=None, grains: dict | None = None, blend: dict | None = None) -> scene.Floor:
    """tufts ("floor_tufts"): "count" cells of the floor, picked at random
    ("seed"), each with a dry tuft of grass drawn on it (seen from above:
    the arcade engine drops scenery round the fight, the floor is all there
    is there); each a cell of its own (40 at most). tint: the floor's
    colours times it (the panorama's "grade" "floor": its hour's light).
    flowers ("floor_flowers"): "count" grass cells, most of them ("middle")
    round the fight, each with a few small wild flowers strewn on it
    (flowers_from_above()): the meadow's flowers on to the fighters' feet;
    "pictures" of them at most (cells of the same tile share them).
    chase ("floor_chase"): Hong Kong's running lights, Tekken 2's own: the
    tiles of the palettes "cluts" (Tekken 2's) keep their indices, the
    "dots" colours of them turned round one place a frame as Tekken 2 turns
    them (issue #12: colour i takes i + 1's; tools/tekken2_stages.py
    LAYOUTS); those palettes a run of their own (CHASE_ROW), the frames'
    colours in fl.chase (T2CH, stepped as Tekken 2 steps them: "period",
    "add").
    blend ("floor_blend"): Hong Kong's floor fading from palette "clut"
    towards "to" and back, as Tekken 2 does (issue #12): every picture of
    those tiles (the cell, its quarters, a puddle's) gets a second palette,
    the same picture drawn from "to" (the same tint, puddle, enlarging),
    each colour the mean of its texels there (faded()); fl.blend those
    palettes' places and both colours (T2BL, colours "first".."last").
    puddles ("floor_puddles"): "count" cells (not the chase's), each with a
    rain puddle on it (puddle_from_above()).
    roll ("floor_roll" [rows, columns]): Tekken 2's 10 x 10 cells moved
    round so (the Pagoda's carved words where the fighters start).
    grains ("floor_grains"): every tile strewn with "count" short green
    blades and dark specks (grass_grains(), each tile its own), drawn in
    16 colours of its own (Serene Forest's leaf litter grown through)."""
    tint = None if tint is None else np.array(tint, float)
    fl = copy.deepcopy(base)
    cells = list(rec['floor'])
    if roll:
        order = np.roll(np.arange(100).reshape(10, 10), roll, axis=(0, 1)).ravel()
        cells = [cells[k] for k in order]
    mean = None
    if recolouring:
        every = np.concatenate([t2_stage.floor_tile(vram, e).reshape(-1, 3) for e in dict.fromkeys(cells)]).astype(float)
        mean = float((every @ np.array([0.3, 0.59, 0.11])).mean())
    painted, art = {}, {}               # cell -> its picture's tag, tag -> (kind, seed, at)
    if tufts:
        rnd = np.random.default_rng(tufts.get('seed', 11))
        for k in rnd.choice(100, tufts['count'], replace=False):
            painted[int(k)] = int(k)
            art[int(k)] = ('tuft', int(rnd.integers(1 << 30)), rnd.uniform(16, 112, 2))
    if flowers:
        rnd = np.random.default_rng(flowers.get('seed', 12))
        grass = [k for k, g in enumerate(grass_cells(vram, rec)) if g and k not in painted]
        middle = [k for k in grass if 2 <= k // 10 <= 7 and 2 <= k % 10 <= 7]
        n_mid = min(len(middle), round(flowers['count'] * flowers.get('middle', 0.7)))
        order = list(rnd.permutation(middle))[:n_mid] + list(rnd.permutation([k for k in grass if k not in middle]))
        seeds = [int(v) for v in rnd.integers(1 << 30, size=flowers.get("kinds", 4))]
        made, n = set(), 0
        for k in order:
            if n == flowers['count']: break
            # a picture this tile has already, else a new one while there is room
            have = [sd for t, sd in made if t == cells[k]]
            if not have and len(made) >= flowers.get('pictures', 10): continue
            sd = have[0] if have else seeds[len(made) % len(seeds)]
            made.add((cells[k], sd))
            painted[int(k)] = ('f', sd)
            art[('f', sd)] = ('flowers', sd, None)
            n += 1
    if puddles:
        rnd = np.random.default_rng(puddles.get('seed', 13))
        free = [k for k in range(100) if k not in painted and not (chase and cells[k][3] in chase['cluts'])
                and not (4 <= k // 10 <= 5 and 4 <= k % 10 <= 5) and 1 <= k // 10 <= 8 and 1 <= k % 10 <= 8
                and cells[k][3] not in puddles.get('not', [])]
        for k in rnd.choice(free, puddles['count'], replace=False):
            painted[int(k)] = ('p', int(k))
            art[('p', int(k))] = ('puddle', int(rnd.integers(1 << 30)), None)
    chase_at = {}
    if chase:
        # the chase's palettes as drawn, one run: shown, then a frame a row
        dots = chase['dots']
        run = []
        for c in chase['cluts']:
            pal = vram[c >> 6 & 511, (c & 63) * 16:(c & 63) * 16 + 16].copy()
            if tint is not None:
                pal = bgr555(np.clip(rgb_of(pal) * tint, 0, 255))
            pal[pal == 0] = 0x0421
            run.append(pal)
            chase_at[c] = (16 * (len(run) - 1), CHASE_ROW)
        # the disc's colours turned round, frame f f places (frame 0 the disc's)
        frames = []
        for f in range(len(dots)):
            frames.append([[int(pal[dots[(dots.index(j) + f) % len(dots)]]) if j in dots else int(c)
                            for j, c in enumerate(pal)] for pal in run])
        palette_run(vr, CHASE_ROW, frames[0])
        fl_chase = frames
    words, quarters, fades = {}, {}, {}
    for key in dict.fromkeys(cells[k] if k not in painted else (cells[k], painted[k]) for k in range(100)):
        e = key[0] if isinstance(key[0], tuple) else key
        tile = t2_stage.floor_tile(vram, e)
        if tint is not None:
            tile = np.clip(tile * tint, 0, 255).astype(np.uint8)
        if recolouring:
            tile = recolour(tile.astype(float), recolouring, mean).astype(np.uint8)
        big = np.asarray(Image.fromarray(tile).resize((128, 128), Image.LANCZOS)).astype(float)
        tile_b = big_b = None
        if blend and e[3] == blend['clut']:
            # the same tile drawn from the palette it fades to
            tile_b = t2_stage.floor_tile(vram, (*e[:3], blend['to']))
            if tint is not None:
                tile_b = np.clip(tile_b * tint, 0, 255).astype(np.uint8)
            if recolouring:
                tile_b = recolour(tile_b.astype(float), recolouring, mean).astype(np.uint8)
            big_b = np.asarray(Image.fromarray(tile_b).resize((128, 128), Image.LANCZOS)).astype(float)
        if grains:
            big = over(big, grass_grains(grains, hash(key) & 0xFFFFFF), (64, 64))
            if e is key:
                x, y, cl = vr.picture(*sixteen(np.asarray(Image.fromarray(big.astype(np.uint8)).resize((64, 64), Image.LANCZOS)))[::-1])
                words[key] = pack_floor_words(x, y, cl)
                for hr in range(2):
                    for hc in range(2):
                        colours, q = sixteen(big[64 * hr:64 * hr + 64, 64 * hc:64 * hc + 64])
                        qx, qy, qc = vr.picture(q, colours)
                        quarters[key, hr, hc] = pack_floor_words(qx, qy, qc)
                continue
        if e is key and e[3] in chase_at:
            # the chase's: its own indices, its run's palette; the quarters
            # the same palette, the dots exactly where they were
            cl = chase_at[e[3]]
            idx = floor_indices(vram, e)
            x, y, _ = vr.picture(idx, None, clut=cl)
            words[key] = pack_floor_words(x, y, cl)
            shown = rgb_of(vr.v[cl[1], cl[0]:cl[0] + 16])
            dist = ((big.reshape(-1, 1, 3) - shown[None]) ** 2).sum(2)
            dist[:, chase['dots']] = np.inf
            q_all = dist.argmin(1).reshape(128, 128)
            fine = np.kron(idx, np.ones((2, 2), int))
            q_all = np.where(np.isin(fine, chase['dots']), fine, q_all)
            for hr in range(2):
                for hc in range(2):
                    qx, qy, _ = vr.picture(q_all[64 * hr:64 * hr + 64, 64 * hc:64 * hc + 64], None, clut=cl)
                    quarters[key, hr, hc] = pack_floor_words(qx, qy, cl)
            continue
        if e is key:
            # its own 16 colours, exactly (the turn changes no colour)
            uv, page, _, clut = e
            pal = vram[clut >> 6 & 511, (clut & 63) * 16:(clut & 63) * 16 + 16].copy()
            if tint is not None:
                pal = bgr555(np.clip(rgb_of(pal) * tint, 0, 255))
            if recolouring:
                pal = bgr555(recolour(rgb_of(pal), recolouring, mean))
            pal[pal == 0] = 0x0421
            shown = rgb_of(pal)
            idx = ((tile.reshape(-1, 1, 3).astype(float) - shown[None]) ** 2).sum(2).argmin(1).reshape(64, 64)
            x, y, cl = vr.picture(idx, [int(c) for c in pal])
            if tile_b is not None:
                fades[cl] = ([int(c) for c in pal], faded(tile_b, idx, pal))
        else:
            kind, seed, at = art[key[1]]
            lay = (lambda img: over(img, tuft_from_above(seed), at)) if kind == 'tuft' else \
                (lambda img: over(img, puddle_from_above(seed), (64, 64))) if kind == 'puddle' else \
                (lambda img: over(img, flowers_from_above(seed), (64, 64)))
            big = lay(big)
            colours, idx = sixteen(np.asarray(Image.fromarray(big.astype(np.uint8)).resize((64, 64), Image.LANCZOS)))
            x, y, cl = vr.picture(idx, colours)
            if big_b is not None:
                big_b = lay(big_b)
                small_b = np.asarray(Image.fromarray(np.clip(big_b, 0, 255).astype(np.uint8)).resize((64, 64), Image.LANCZOS))
                fades[cl] = (colours, faded(small_b, idx, colours))
        words[key] = pack_floor_words(x, y, cl)
        for hr in range(2):
            for hc in range(2):
                colours, q = sixteen(big[64 * hr:64 * hr + 64, 64 * hc:64 * hc + 64])
                qx, qy, qc = vr.picture(q, colours)
                quarters[key, hr, hc] = pack_floor_words(qx, qy, qc)
                if big_b is not None:
                    fades[qc] = (colours, faded(big_b[64 * hr:64 * hr + 64, 64 * hc:64 * hc + 64], q, colours))
    keys = [cells[k] if k not in painted else (cells[k], painted[k]) for k in range(100)]
    fl.cells = [[words[keys[10 * r + c]] for c in range(10)] for r in range(10)]
    fl.halves = [[quarters[keys[10 * (r // 2) + c // 2], r % 2, c % 2] for c in range(20)] for r in range(20)]
    seen = list(dict.fromkeys(w0 & 0x39FFFFFF for row in fl.cells for w0, _ in row))
    if len(seen) > 40:
        raise SystemExit(f'{len(seen)} distinct floor cells: the game gives up over 40')
    fl.unique = [[seen.index(fl.cells[r][c][0] & 0x39FFFFFF) for c in range(10)] for r in range(10)]
    fl.chase = (fl_chase, [chase_at[c] for c in chase['cluts']]) if chase else None
    fl.blend = [(at, a, b) for at, (a, b) in fades.items()] if blend else None
    if fl.blend and len(fl.blend) > BLEND_MAX:
        raise SystemExit(f'{len(fl.blend)} palettes fade: the runtime takes {BLEND_MAX}')
    return fl


BLEND_MAX = 64                       # src/tekken2/tmst_tekken2_stages.c BLEND_MAX


def faded(img_b: np.ndarray, idx: np.ndarray, colours) -> list[int]:
    """floor()'s "floor_blend": the palette a picture of indices idx (its
    colours the shown ones) fades to, img_b the same picture drawn from the
    other palette: each colour the mean of img_b's texels it takes (a colour
    no texel takes stays as it is; never colour 0)."""
    out = [int(c) for c in colours]
    flat, ix = np.asarray(img_b, float).reshape(-1, 3), np.asarray(idx).reshape(-1)
    for k in range(len(out)):
        taken = ix == k
        if taken.any():
            out[k] = int(bgr555(flat[taken].mean(0))) or 0x0421
    return out


def grass_grains(spec: dict, seed: int) -> np.ndarray:
    """Grass through a tile seen from above, RGBA 128 x 128 (floor()'s
    "floor_grains"): "count" short blades, greens light and dark, each a
    stroke or two, and dark specks of earth between."""
    from PIL import ImageDraw
    rnd = np.random.default_rng(seed)
    s = 4 * 128
    im = Image.new('RGBA', (s, s), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    greens = spec.get('colours', [[92, 130, 40], [120, 150, 52], [70, 100, 30], [140, 160, 70]])
    for _ in range(spec.get('specks', 60)):
        x, y, r = rnd.uniform(0, s), rnd.uniform(0, s), rnd.uniform(3, 7)
        d.ellipse((x - r, y - r * 0.7, x + r, y + r * 0.7), fill=(30, 24, 14, 200))
    for _ in range(spec.get('count', 160)):
        x, y = rnd.uniform(0, s), rnd.uniform(0, s)
        a, ln = rnd.uniform(0, 2 * np.pi), rnd.uniform(8, 18)
        g = np.array(greens[int(rnd.integers(len(greens)))], float) * rnd.uniform(0.85, 1.15)
        d.line([(x, y), (x + np.cos(a) * ln, y + np.sin(a) * ln * 0.8)], fill=tuple(int(v) for v in np.clip(g, 0, 255)) + (255,),
               width=int(rnd.integers(2, 4)))
    return np.asarray(im.resize((128, 128), Image.LANCZOS)).astype(float)


# --- dry grass ----------------------------------------------------------------
# Tufts of sun-dried desert grass, drawn four times finer and brought down:
# straw and pale khaki blades, a little grey-green left at the root.

STRAW = [(206, 182, 128), (222, 202, 150), (184, 156, 102), (160, 132, 86), (196, 176, 120), (148, 140, 96)]
# a summer meadow's blades ("tuft_blades": "green"): the Korean temple's grass
GREEN = [(96, 122, 44), (114, 136, 50), (80, 104, 36), (126, 140, 58), (100, 126, 40), (70, 94, 30)]


def blade_colour(rnd, depth: float, blades=STRAW) -> tuple:
    c = np.array(blades[rnd.integers(len(blades))], float) * rnd.uniform(0.85, 1.08)
    return tuple(int(v) for v in np.clip(c * (0.62 + 0.38 * depth), 0, 255)) + (255,)


def tuft_side(seed: int, blades=STRAW) -> np.ndarray:
    """A tuft seen from the side, RGBA 64 x 64: blades from the root (the
    picture's bottom middle) fanning up and out, the outer ones bowing over."""
    from PIL import ImageDraw
    rnd = np.random.default_rng(seed)
    s = 4 * 64
    im = Image.new('RGBA', (s, s), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    for k in range(int(rnd.integers(26, 38))):
        lean = rnd.uniform(-1.15, 1.15)
        length = s * rnd.uniform(0.45, 0.95) * (1 - 0.3 * abs(lean))
        x0 = s / 2 + rnd.uniform(-0.07, 0.07) * s
        pts = []
        for t in np.linspace(0, 1, 9):
            a = lean * (0.6 + 0.6 * t)                 # bows out further up
            pts.append((x0 + np.sin(a) * length * t, s - 1 - np.cos(a) * length * t + abs(lean) * 0.25 * length * t * t))
        width = rnd.uniform(6, 10)
        for i in range(len(pts) - 1):
            d.line([pts[i], pts[i + 1]], fill=blade_colour(rnd, 0.35 + 0.65 * i / 8, blades),
                   width=max(1, int(width * (1 - 0.6 * i / 8))))
    return np.asarray(im.resize((64, 64), Image.LANCZOS)).astype(float)


def tuft_from_above(seed: int) -> np.ndarray:
    """A tuft lying on the floor seen from above, RGBA 48 x 48 (the floor's
    halves: about 2 cm a pixel): blades from its middle all round, its
    shadow on the ground away from the low sun."""
    from PIL import ImageDraw, ImageFilter
    rnd = np.random.default_rng(seed)
    s = 4 * 48
    shadow = Image.new('L', (s, s), 0)
    ImageDraw.Draw(shadow).ellipse((s * 0.28, s * 0.36, s * 0.86, s * 0.78), fill=110)
    shadow = shadow.filter(ImageFilter.GaussianBlur(s * 0.05))
    im = Image.new('RGBA', (s, s), (40, 22, 12, 0))
    im.putalpha(shadow)
    d = ImageDraw.Draw(im)
    c = s / 2
    for k in range(int(rnd.integers(30, 44))):
        a = rnd.uniform(0, 2 * np.pi)
        length = s * rnd.uniform(0.18, 0.44)
        curl = rnd.uniform(-0.5, 0.5)
        pts = [(c + np.cos(a + curl * t) * length * t, c + np.sin(a + curl * t) * length * t * 0.8) for t in np.linspace(0, 1, 7)]
        width = rnd.uniform(3, 6)
        for i in range(len(pts) - 1):
            d.line([pts[i], pts[i + 1]], fill=blade_colour(rnd, 0.5 + 0.5 * i / 6), width=max(1, int(width * (1 - 0.5 * i / 6))))
    return np.asarray(im.resize((48, 48), Image.LANCZOS)).astype(float)


FLOWER_HEADS = (((214, 34, 26), (30, 26, 30)), ((64, 96, 210), (44, 70, 170)), ((246, 244, 236), (232, 196, 40)),
                ((240, 206, 40), (196, 150, 20)), ((126, 80, 186), (96, 58, 150)))


def flowers_from_above(seed: int) -> np.ndarray:
    """Small wild flowers on the floor seen from above, RGBA 128 x 128 (a
    floor cell at the halves' scale, about 2 cm a pixel): two or three
    patches of a few flowers each (poppies, cornflowers, daisies,
    buttercups, lavender), a rosette of leaves under each, a soft shadow."""
    from PIL import ImageDraw, ImageFilter
    rnd = np.random.default_rng(seed)
    s = 4 * 128
    shadow = Image.new('L', (s, s), 0)
    im = Image.new('RGBA', (s, s), (30, 40, 14, 0))
    heads = []
    for _ in range(int(rnd.integers(3, 5))):
        cx, cy = rnd.uniform(0.12, 0.88, 2) * s
        kind = int(rnd.integers(len(FLOWER_HEADS)))
        for _ in range(int(rnd.integers(4, 9))):
            heads.append((cx + rnd.normal(0, s * 0.07), cy + rnd.normal(0, s * 0.07), kind if rnd.uniform() < 0.8 else
                           int(rnd.integers(len(FLOWER_HEADS)))))
    sd = ImageDraw.Draw(shadow)
    for x, y, _ in heads:
        sd.ellipse((x - 14, y - 6, x + 26, y + 22), fill=90)
    im.putalpha(shadow.filter(ImageFilter.GaussianBlur(10)))
    d = ImageDraw.Draw(im)
    for x, y, _ in heads:
        for k in range(5):
            a = rnd.uniform(0, 2 * np.pi)
            g = tuple(int(v) for v in np.array(GREEN[int(rnd.integers(len(GREEN)))]) * 0.9) + (255,)
            d.line([(x, y), (x + np.cos(a) * 34, y + np.sin(a) * 26)], fill=g, width=9)
    for x, y, kind in heads:
        petal, eye = FLOWER_HEADS[kind]
        r = rnd.uniform(17, 23)
        # five petals round the eye, each a little darker at its foot
        for k in range(5):
            a = 2 * np.pi * k / 5 + rnd.uniform(0, 1.2)
            px, py = x + np.cos(a) * r * 0.55, y + np.sin(a) * r * 0.5
            d.ellipse((px - r * 0.55, py - r * 0.5, px + r * 0.55, py + r * 0.5), fill=tuple(petal) + (255,))
        d.ellipse((x - r * 0.36, y - r * 0.32, x + r * 0.36, y + r * 0.32), fill=tuple(eye) + (255,))
    return np.asarray(im.resize((128, 128), Image.LANCZOS)).astype(float)


def over(img: np.ndarray, rgba: np.ndarray, at) -> np.ndarray:
    """rgba laid over img (RGB), its middle at (x, y)."""
    out = img.copy()
    h, w = rgba.shape[:2]
    x0, y0 = int(at[0] - w / 2), int(at[1] - h / 2)
    ys, xs = slice(max(0, y0), min(img.shape[0], y0 + h)), slice(max(0, x0), min(img.shape[1], x0 + w))
    part = rgba[ys.start - y0:ys.stop - y0, xs.start - x0:xs.stop - x0]
    a = part[..., 3:] / 255
    out[ys, xs] = out[ys, xs] * (1 - a) + part[..., :3] * a
    return out


def see_through(rgba: np.ndarray) -> tuple[list[int], np.ndarray]:
    """16 colours for a picture with holes: colour 0 is nothing (the
    PlayStation draws no texel of 0), the 15 others its opaque texels'."""
    solid = rgba[..., 3] >= 128
    px = rgba[..., :3][solid]
    q = Image.fromarray(px.reshape(-1, 1, 3).astype(np.uint8)).quantize(15, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE)
    pal = np.array((q.getpalette() + [0] * 45)[:45], float).reshape(15, 3)
    colours = bgr555(pal)
    colours[colours == 0] = 0x0421
    idx = np.zeros(rgba.shape[:2], np.uint8)
    idx[solid] = ((px[:, None] - rgb_of(colours)[None]) ** 2).sum(2).argmin(1) + 1
    return [0] + [int(c) for c in colours], idx


def pack_floor_words(x: int, y: int, clut: tuple[int, int]) -> tuple[int, int]:
    tpage = (x >> 6) | (y & 0x100) >> 4 | (y & 0x200) << 2
    return tpage << 16 | (y & 0xFF) << 8 | (x & 63) * 4, (clut[1] << 6 | clut[0] >> 4) << 16


# --- scenery: the tiles -------------------------------------------------------
# Built here, in centimetres (the tiles' unit: world / 10), y down, the fight's
# middle at the origin (the arcade engine draws the tiles round the fighters'
# midpoint). A face is a quad in the GPU's order (a b / c d), textured with a
# whole 64 x 64 picture, lit per corner (Gouraud colours, 0x80 = as stored).

GT4 = 0x3D010A0C


def material(src: Image.Image, spec: dict) -> list[np.ndarray]:
    """A strip of rock from the panorama picture ("box" [x0, y0, x1, y1]) as
    "tiles" 64 x 64 pictures that go on one into the next and from the last
    into the first (its end faded into its beginning): neighbouring faces
    take them in turn, so the rock runs on instead of repeating one picture.
    "lift" brightens its shadows (the picture's are deep: lit from far
    away), "soften" takes the harshest speckle and contrast off."""
    from PIL import ImageFilter
    n, blend = spec.get('tiles', 4), 16
    strip = src.crop(tuple(spec['box'])).resize((64 * n + blend, 64), Image.LANCZOS)
    soft = np.asarray(strip.filter(ImageFilter.GaussianBlur(1.2))).astype(float)
    img = np.asarray(strip).astype(float)
    k = spec.get('soften', 0.3)
    img = img * (1 - k) + soft * k
    mean = img.reshape(-1, 3).mean(0)
    img = mean + (img - mean) * (1 - 0.6 * k)
    img = 255 * (np.clip(img, 0, 255) / 255) ** (1 - spec.get('lift', 0.3))
    out = img[:, blend:].copy()
    t = np.linspace(0, 1, blend)[None, :, None]
    out[:, -blend:] = out[:, -blend:] * (1 - t) + img[:, :blend] * t
    return [out[:, 64 * i:64 * i + 64] for i in range(n)]


_ARCADE_VRAM = {}


def arcade_vram(arc, stage: int) -> np.ndarray:
    if stage not in _ARCADE_VRAM:
        _ARCADE_VRAM[stage] = scene.Stage(arc, stage).vram
    return _ARCADE_VRAM[stage]


def borrowed(spec: dict, arc, vr: Vram) -> list:
    """Pictures of a Tekken 3 arcade stage's own ("borrow": "stage", its
    4-bit texture page "tpage", palette "clut", the top left "uv" and
    "pictures" 64 x 64 ones one under the other), their indices and colours
    as they are: Paul's oil drums on another quay."""
    v, tpage, clut = arcade_vram(arc, spec['stage']), spec['tpage'], spec['clut']
    px, py = (tpage & 15) * 64, (tpage & 0x10) * 16 + ((tpage & 0x800) >> 2)
    colours = [int(c) for c in v[clut >> 6, (clut & 63) * 16:(clut & 63) * 16 + 16]]
    u0, v0 = spec['uv']
    out = []
    for k in range(spec.get('pictures', 1)):
        idx = np.zeros((64, 64), np.uint16)
        for y in range(64):
            for x in range(64):
                u = u0 + x
                idx[y, x] = v[py + v0 + 64 * k + y, px + u // 4] >> ((u & 3) * 4) & 15
        out.append(vr.picture(idx, colours))
    return out


# --- Hills: Alex's, a bone bed of dinosaurs in the grass -----------------------
# The painted pictures (painted() "trex", "raptor", "ammonite", "fossil"):
# drawn big, brought down; the skulls and the ammonite with holes (colour 0).

from PIL import ImageDraw, ImageFilter  # noqa: E402

FOSSIL = (204, 186, 146)


def soft_noise(rnd, h, w, passes=2):
    n = rnd.normal(0, 1, (h, w))
    for _ in range(passes):
        n = (n + np.roll(n, 1, 0) + np.roll(n, 1, 1) + np.roll(n, -1, 0) + np.roll(n, -1, 1)) / 5
    return (n - n.mean()) / n.std()


def skull_rgba(size: int, seed: int, kind: str = 'trex', grass: bool = True) -> np.ndarray:
    """A dinosaur's skull seen from the side, its nose to the left, its jaw
    open (drawn 4 times bigger, then brought down): the bone fossil tan,
    lit from above, its openings (the eye's, the big one before it, the
    nostril, the one behind the eye, the jaw's) holes; teeth along both
    jaws; "grass": its foot sunk in grass blades."""
    S = 4 * size
    k = S / 128.0
    P = lambda pts: [(x * k, y * k) for x, y in pts]
    rnd = np.random.default_rng(seed)
    if kind == 'trex':
        upper = [(4, 62), (5, 50), (12, 42), (30, 36), (50, 30), (70, 24), (86, 18), (100, 17), (112, 21), (121, 30),
                 (126, 43), (125, 56), (119, 65), (106, 63), (84, 66), (60, 67), (36, 67), (18, 66), (8, 64)]
        lower = [(115, 60), (123, 68), (120, 78), (106, 85), (84, 94), (58, 101), (34, 106), (20, 108), (17, 103),
                 (30, 96), (52, 88), (76, 79), (98, 70)]
        holes = [('e', 93, 35, 6, 6.5, 0), ('e', 66, 46, 12, 6.5, -0.3), ('e', 13, 50, 4, 2.6, -0.2),
                 ('e', 110, 44, 5.5, 9, 0.2), ('e', 92, 78, 8, 3, -0.35)]
        teeth_u = [(x, 66 + 0.02 * (x - 8)) for x in range(11, 90, 8)]
        teeth_l = [(x, 107 - (x - 20) * 0.27) for x in range(25, 92, 8)]
        tooth = 11
    else:                                            # a raptor's: long and low
        upper = [(4, 62), (6, 56), (20, 50), (44, 44), (70, 38), (92, 36), (108, 38), (120, 46), (122, 56),
                 (116, 64), (96, 63), (64, 64), (34, 65), (12, 64)]
        lower = [(112, 61), (120, 68), (114, 76), (90, 82), (60, 88), (30, 92), (12, 93), (10, 88), (30, 84),
                 (60, 77), (90, 70)]
        holes = [('e', 98, 46, 7.5, 7, 0), ('e', 66, 52, 10, 5, -0.15), ('e', 14, 56, 3.5, 2.2, 0),
                 ('e', 115, 52, 3.5, 6, 0)]
        teeth_u = [(x, 64.5) for x in range(10, 90, 7)]
        teeth_l = [(x, 92 - (x - 12) * 0.28) for x in range(16, 88, 7)]
        tooth = 6
    a = Image.new('L', (S, S), 0)
    d = ImageDraw.Draw(a)
    d.polygon(P(upper), fill=255)
    d.polygon(P(lower), fill=255)
    for x, y in teeth_u:
        t = tooth * rnd.uniform(0.7, 1.1)
        d.polygon(P([(x - 2.8, y - 1), (x + 2.8, y - 1), (x + 0.8, y + t)]), fill=255)
    for x, y in teeth_l:
        t = tooth * rnd.uniform(0.6, 1.0)
        d.polygon(P([(x - 2, y + 1), (x + 2, y + 1), (x - 0.4, y - t)]), fill=255)
    holemask = Image.new('L', (S, S), 0)
    hd = ImageDraw.Draw(holemask)
    yy, xx = np.mgrid[0:S, 0:S] / k
    hole = np.zeros((S, S), bool)
    for _, cx, cy, rx, ry, rot in holes:
        u = (xx - cx) * np.cos(rot) + (yy - cy) * np.sin(rot)
        v = -(xx - cx) * np.sin(rot) + (yy - cy) * np.cos(rot)
        hole |= (u / rx) ** 2 + (v / ry) ** 2 < 1
    alpha = np.asarray(a, float) / 255 * ~hole
    # the bone: tan, darker low down and round its holes, cracks, earth in it
    noise = soft_noise(rnd, S, S, 3)
    fine = soft_noise(rnd, S, S, 1)
    base = np.array(FOSSIL, float)
    img = base[None, None] * (1 + 0.07 * noise[..., None] + 0.05 * fine[..., None])
    img *= (1.12 - 0.35 * np.clip((yy - 20) / 90, 0, 1))[..., None]         # lit from above
    blur = np.asarray(Image.fromarray((alpha * 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(2.2 * k)), float) / 255
    img *= (0.55 + 0.45 * np.clip(blur * 1.6 - 0.3, 0, 1))[..., None]        # its edges and the holes' rims rounded off
    # its bulk: a height from how deep inside it each point is, lit from
    # the upper left (the cheek and the snout's ridge stand out)
    height = np.asarray(Image.fromarray((alpha * 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(5 * k)), float) / 255
    height += 0.012 * noise * alpha
    gy, gx = np.gradient(height * 220 / k)
    nrm = np.dstack([-gx, -gy, np.ones_like(gx)])
    nrm /= np.linalg.norm(nrm, axis=2, keepdims=True)
    lit = nrm @ (np.array([-0.5, -0.7, 0.6]) / np.linalg.norm([-0.5, -0.7, 0.6]))
    img *= np.clip(0.35 + 0.85 * lit, 0.3, 1.3)[..., None]
    earth = np.clip(noise - 1.0, 0, 1)[..., None]
    img = img * (1 - 0.6 * earth) + np.array([110, 84, 56], float) * 0.6 * earth
    cracks = np.abs(np.sin((xx * 0.7 + yy * 1.3 + 9 * noise) * 2 * np.pi / 31)) < 0.025
    img[cracks] *= 0.6
    # the teeth paler, browner at the root
    teethmask = np.zeros((S, S), bool)
    tm = Image.new('L', (S, S), 0); td = ImageDraw.Draw(tm)
    rnd2 = np.random.default_rng(seed)
    for x, y in teeth_u:
        t = tooth * rnd2.uniform(0.7, 1.1)
        td.polygon(P([(x - 2.8, y + 0.5), (x + 2.8, y + 0.5), (x + 0.8, y + t)]), fill=255)
    for x, y in teeth_l:
        t = tooth * rnd2.uniform(0.6, 1.0)
        td.polygon(P([(x - 2, y - 0.5), (x + 2, y - 0.5), (x - 0.4, y - t)]), fill=255)
    teethmask = np.asarray(tm) > 0
    img[teethmask] = img[teethmask] * 0.3 + np.array([226, 214, 170], float) * 0.7
    out = np.zeros((S, S, 4))
    out[..., :3], out[..., 3] = img, alpha * 255
    small = np.asarray(Image.fromarray(np.clip(out, 0, 255).astype(np.uint8)).resize((size, size), Image.LANCZOS), float)
    # a dark line round it (it stands out from the grass behind)
    al = small[..., 3] >= 128
    edge = al & ~(np.roll(al, 1, 0) & np.roll(al, -1, 0) & np.roll(al, 1, 1) & np.roll(al, -1, 1))
    small[edge, :3] *= 0.6
    small[..., 3] = np.where(al, 255, 0)
    if grass:
        g = np.random.default_rng(seed + 1)
        h = size
        yy2, xx2 = np.mgrid[0:h, 0:size].astype(float)
        for _ in range(int(size * 2.2)):
            x0, top = g.uniform(0, size), g.uniform(h * 0.74, h * 0.9)
            lean = g.uniform(-0.35, 0.35)
            t = np.clip((yy2 - top) / max(1, h - top), 0, 1)
            centre = x0 + lean * (h - yy2)
            blade = (yy2 >= top) & (np.abs(xx2 - centre) < 0.45 + 0.9 * t)
            green = np.array([[64, 104, 38], [86, 128, 46], [110, 140, 54], [52, 86, 32], [140, 150, 70]][int(g.integers(5))], float)
            small[blade, :3] = green * (0.8 + 0.3 * (1 - t[blade]))[:, None]
            small[blade, 3] = 255
        small[int(h * 0.93):, 3] = np.where(small[int(h * 0.93):, 3] > 0, 255, 0)
    return small


def ammonite_rgba(seed: int) -> np.ndarray:
    """An ammonite in its stone: the coiled shell's ribs running out from
    the middle, the coil's seam dark, a lip of the rock round it."""
    rnd = np.random.default_rng(seed)
    S = 256
    yy, xx = np.mgrid[0:S, 0:S].astype(float)
    x, y = (xx - 128) / 4, (yy - 128) / 4
    r, a = np.hypot(x, y), np.arctan2(y, x)
    noise = soft_noise(rnd, S, S, 3)
    b = 0.17                                     # the spiral's growth: r = r0 e^(b a)
    turns = (np.log(np.maximum(r, 0.5) / 2.0) / b - a) / (2 * np.pi)
    whorl = turns - np.floor(turns)
    rock = r < 30 + 1.2 * noise
    shell = r < 27
    out = np.zeros((S, S, 4))
    stone = np.array([136, 120, 96], float)[None, None] * (1 + 0.08 * noise[..., None])
    out[..., :3] = stone
    shellc = np.array([196, 160, 112], float)[None, None] * (1 + 0.06 * noise[..., None])
    ribs = 0.5 + 0.5 * np.cos(a * 22 + 3 * np.log(np.maximum(r, 1)))
    shade = 0.78 + 0.22 * ribs
    shade *= 0.72 + 0.28 * np.sin(np.pi * whorl)      # each whorl rounded
    img = shellc * shade[..., None]
    img[whorl < 0.07] *= 0.45                          # the coil's seam
    img *= (1.1 - 0.2 * (y + 30) / 60)[..., None]      # lit from above
    out[shell, :3] = img[shell]
    out[..., 3] = rock * 255
    small = np.asarray(Image.fromarray(np.clip(out, 0, 255).astype(np.uint8)).resize((64, 64), Image.LANCZOS), float)
    small[..., 3] = np.where(small[..., 3] >= 128, 255, 0)
    return small


def dandelion_rgba(seed: int) -> np.ndarray:
    """A clump of dandelions (a tuft's picture, on nothing): the jagged
    leaves' rosette low down, thin pale stalks up from it, on each a yellow
    flower or a white ball of seeds (the seeds the wind carries off)."""
    rnd = np.random.default_rng(seed)
    S = 256
    yy, xx = np.mgrid[0:S, 0:S].astype(float)
    out = np.zeros((S, S, 4))
    for _ in range(9):                                   # the leaves, toothed, spread low
        x0, a = rnd.uniform(90, 166), rnd.uniform(-1.2, 1.2)
        length, width = rnd.uniform(60, 95), rnd.uniform(12, 18)
        u = (xx - x0) * np.sin(a) - (yy - 250) * np.cos(a)
        v = (xx - x0) * np.cos(a) + (yy - 250) * np.sin(a)
        t = np.clip(u / length, 0, 1)
        edge = width * (1 - t) * (0.6 + 0.4 * np.abs(np.sin(u * 0.22)))
        leaf = (u > 0) & (u < length) & (np.abs(v) < edge)
        g = np.array([[58, 104, 40], [72, 120, 46], [50, 92, 34]][int(rnd.integers(3))], float)
        out[leaf, :3] = g * (0.85 + 0.25 * (1 - np.abs(v[leaf]) / np.maximum(edge[leaf], 1)))[:, None]
        out[leaf & (np.abs(v) < 1.5), :3] = g * 1.3
        out[leaf, 3] = 255
    for _ in range(int(rnd.integers(3, 5))):             # the stalks and their heads
        x0, top, lean = rnd.uniform(70, 186), rnd.uniform(26, 120), rnd.uniform(-0.25, 0.25)
        xt = x0 + lean * (240 - top)
        cx = x0 + lean * (240 - yy)
        stalk = (yy > top) & (yy < 244) & (np.abs(xx - cx) < 2.6)
        out[stalk, :3] = (150, 170, 96); out[stalk, 3] = 255
        if rnd.uniform() < 0.55:                        # a ball of seeds: white, soft edged, its hairs radiating
            r, a = np.hypot(xx - xt, yy - top), np.arctan2(yy - top, xx - xt)
            ball = r < 26 * (0.92 + 0.08 * np.abs(np.sin(a * 13)))
            out[ball, :3] = np.array([238, 238, 230], float) * (0.86 + 0.14 * np.cos(a[ball] + 2.2))[:, None]
            out[ball & (r < 4), :3] = (160, 140, 100)
            out[ball, 3] = 255
        else:                                           # a flower: yellow, many narrow petals
            r, a = np.hypot(xx - xt, (yy - top) * 1.5), np.arctan2(yy - top, xx - xt)
            flower = r < 22 * (0.8 + 0.2 * np.abs(np.cos(a * 11)))
            out[flower, :3] = np.array([246, 204, 30], float) * (0.85 + 0.2 * (1 - r[flower] / 22))[:, None]
            out[flower, 3] = 255
    small = np.asarray(Image.fromarray(out.astype(np.uint8)).resize((64, 64), Image.LANCZOS), float)
    small[..., 3] = np.where(small[..., 3] >= 110, 255, 0)
    return small


def fossil_rgb(seed: int) -> np.ndarray:
    """Fossil bone: tan, streaked along, cracked across, earth in its pits."""
    rnd = np.random.default_rng(seed)
    yy, xx = np.mgrid[0:64, 0:64].astype(float)
    noise = soft_noise(rnd, 64, 64, 5)
    img = np.array(FOSSIL, float)[None, None] * (1 + 0.05 * noise[..., None])
    img *= (0.92 + 0.08 * np.sin((yy + 3 * noise) * 2 * np.pi / 9))[..., None]
    img[np.abs(np.sin((xx + 4 * noise) * 2 * np.pi / 21)) < 0.035] *= 0.62
    earth = np.clip(-noise - 1.2, 0, 1)[..., None]
    img = img * (1 - 0.5 * earth) + np.array([104, 80, 54], float) * 0.5 * earth
    return np.clip(img, 0, 255)


def painted(kind: str, vr: Vram) -> list:
    """The quay's own pictures, drawn here: "iron" (a bollard's cast iron,
    black paint worn to rust), "rope" (a mooring line's twisted strands),
    "yellow" (a railing's safety paint, scratched), "lid" (a drum's top,
    rust)."""
    if kind == 'dandelion':
        return [vr.picture(*see_through(dandelion_rgba(170 + k))[::-1]) for k in range(2)]
    if kind in ('raptor', 'ammonite'):
        rgba = skull_rgba(64, 162, 'raptor', grass=False) if kind == 'raptor' else ammonite_rgba(163)
        return [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    if kind == 'fossil':
        return [vr.picture(*sixteen(fossil_rgb(154 + k))[::-1]) for k in range(2)]
    if kind == 'tooth':
        return [vr.picture(*sixteen(np.clip(fossil_rgb(156) * 0.5 + np.array([236, 224, 184]) * 0.5, 0, 255))[::-1])]
    rnd = np.random.default_rng({'iron': 31, 'rope': 32, 'yellow': 33, 'lid': 34, 'stone': 35, 'carved': 36, 'rust': 37,
                                 'phone': 38, 'phone_roof': 39, 'crate': 40, 'crate_top': 41, 'container': 42,
                                 'barrel_blue': 43, 'barrel_red': 44, 'barrel_lid': 45, 'spool': 46, 'spool_rim': 47,
                                 'cable': 48, 'cone': 49, 'pole': 50, 'granite': 51, 'slab': 52, 'torii': 53,
                                 'torii_black': 54, 'bamboo': 55, 'wood': 56, 'thatch': 57, 'fringe': 58, 'plank': 59,
                                 'driftwood': 60, 'coconut': 61, 'board': 62, 'ring_red': 63, 'ring_white': 64,
                                 'flag': 65, 'shells': 66, 'lotus': 67, 'jangseung_m': 68, 'jangseung_f': 69,
                                 'jangseung_body': 70, 'onggi': 71, 'marble': 72, 'brass': 73, 'clay': 74,
                                 'cypress': 75, 'jali': 76, 'marigold': 77, 'bamboo_green': 78, 'bamboo_leaves': 79,
                                 'tea': 80, 'tile_green': 81, 'plaque': 82, 'xuanzi': 83, 'bronze': 84, 'wine_jar': 85,
                                 'sheaf': 86, 'wicker': 87, 'mossy': 88, 'hay': 89, 'hay_end': 90, 'drystone': 91,
                                 'burlap': 92, 'shirt': 93, 'skep': 94, 'wheel': 95, 'poppies': 96, 'cornflowers': 97,
                                 'lavender': 98, 'daisies': 99, 'sunflower': 100, 'leaf_green': 101, 'balusters': 102, 'slat': 103,
                                 'hydrant': 104, 'steel': 105, 'cart_side': 106, 'canvas_blue': 107, 'canvas_yellow': 108,
                                 'tyre': 109, 'usps': 110, 'usps_side': 111, 'usps_top': 112, 'wire': 113,
                                 'manhole': 114, 'jangseung_body_f': 125, 'pew': 115, 'pew_end': 116, 'marble_dark': 117, 'frontal': 118,
                                 'linen': 119, 'wax': 120, 'chain': 121, 'carpet': 122, 'concrete': 123, 'wall_brick': 124,
                                 'sandstone': 125, 'naga': 126, 'laterite': 127, 'apsara': 128, 'fig_root': 129,
                                 'palm_trunk': 130, 'matting': 131, 'boat': 132, 'fig_leaves': 133, 'net': 134, 'reeds': 135,
                                 'pentelic': 136, 'flute': 137, 'doric_frieze': 138, 'ionic': 139, 'architrave': 140,
                                 'caryatid': 141, 'inscription': 142, 'odeon': 143, 'olive_bark': 144,
                                 'redrock': 145, 'redrock_dark': 146, 'redrock_top': 147, 'petroglyph': 148, 'bone': 149,
                                 'cholla': 150, 'pear_pad': 151, 'barbwire': 152, 'skull': 153}[kind])
    noise = rnd.normal(0, 1, (64, 64))
    for _ in range(2):
        noise = (noise + np.roll(noise, 1, 0) + np.roll(noise, 1, 1)) / 3
    noise = (noise - noise.mean()) / noise.std()
    yy, xx = np.mgrid[0:64, 0:64]
    if kind == 'iron':
        img = np.array([46, 44, 48], float)[None, None] * (1 + 0.12 * noise[..., None])
        rust = np.clip(noise - 0.9, 0, None)[..., None]
        img = img * (1 - np.clip(rust, 0, 1)) + np.array([112, 58, 32], float) * np.clip(rust, 0, 1)
    elif kind == 'rope':
        twist = (np.sin((xx + yy * 1.0) * 2 * np.pi / 8) + 1) / 2
        img = np.array([168, 146, 104], float)[None, None] * (0.62 + 0.38 * twist[..., None]) * (1 + 0.06 * noise[..., None])
    elif kind == 'yellow':
        img = np.array([206, 168, 40], float)[None, None] * (1 + 0.08 * noise[..., None])
        scratch = (rnd.uniform(0, 1, (64, 64)) > 0.985) | (noise < -1.6)
        img[scratch] = (70, 58, 40)
    elif kind in ('stone', 'carved'):
        # the tower's limestone, sand-scoured, grime run down its joints
        img = np.array([150, 134, 108], float)[None, None] * (1 + 0.06 * noise[..., None])
        img *= (1 - 0.18 * np.clip(rnd.normal(0, 1, (1, 64)).repeat(64, 0) * (yy / 63), 0, 1))[..., None]
        if kind == 'stone':
            img[30:32] *= 0.62                          # a bed joint
            img[:30, 40:42] *= 0.66; img[32:, 14:16] *= 0.66
        else:
            # a Gothic panel: two lancets, their pointed heads, a moulding
            for x0 in (6, 34):
                arch = (xx >= x0) & (xx < x0 + 24) & (yy >= 14 + 6 * np.abs(xx - x0 - 11.5) / 11.5)
                img[arch & ((xx == x0) | (xx == x0 + 23) | (yy == 58))] *= 0.55
                img[arch & (xx > x0) & (xx < x0 + 23) & (yy < 58)] *= 0.8
            img[4:7] *= 0.6; img[61:] *= 0.65
        img[noise < -1.7] *= 0.7
    elif kind == 'rust':
        img = np.array([108, 58, 34], float)[None, None] * (1 + 0.16 * noise[..., None])
        flake = np.clip(noise - 0.8, 0, 1)[..., None]
        img = img * (1 - flake) + np.array([150, 92, 50], float) * flake
        img[noise < -1.4] = (52, 36, 30)
    elif kind == 'phone':
        # a K6 kiosk's side, the red sun-faded: the crown's dark sign band,
        # eight rows of three panes over a solid foot panel
        img = np.array([150, 34, 28], float)[None, None] * (1 + 0.1 * noise[..., None])
        if RESERVED:                        # the add-on's: its word, as on the K6's crown
            img[1:8, 6:58] = (36, 30, 30)
            word = glyph_text('TELEPHONE', FONT3)
            stamp(img, word, 2, (64 - word.shape[1]) // 2, (200, 196, 180))
        else:
            img[2:7, 6:58] = (36, 30, 30)
            img[3:6, 14:50] = (200, 196, 180)
        glass = np.array([70, 82, 88], float)
        for r in range(8):
            for c in range(3):
                y0, x0 = 10 + 5 * r, 10 + 15 * c
                pane = glass * rnd.uniform(0.8, 1.25)
                if rnd.uniform() < 0.22: pane = np.array([24, 22, 24], float)     # broken
                img[y0:y0 + 4, x0:x0 + 13] = pane
        img[52:54, 6:58] *= 0.7
        img[56:] = img[56:] * 0.6 + np.array([150, 128, 100], float) * 0.4       # sand blown up its foot
    elif kind == 'phone_roof':
        img = np.array([140, 32, 26], float)[None, None] * (1 + 0.1 * noise[..., None])
        img[(np.abs(xx - 31.5) < 3) | (np.abs(yy - 31.5) < 3)] *= 0.75
    elif kind in ('crate', 'crate_top'):
        # a flight case of the hangar: olive drab, steel edges, rivets; its
        # side a stencilled block and a hazard band
        img = np.array([86, 92, 70], float)[None, None] * (1 + 0.07 * noise[..., None])
        edge = (xx < 4) | (xx > 59) | (yy < 4) | (yy > 59)
        img[edge] = np.array([128, 130, 132], float) * (1 + 0.08 * noise[edge][:, None])
        img[edge & (((xx - 2) % 12 == 0) & ((yy < 3) | (yy > 60)) | ((yy - 2) % 12 == 0) & ((xx < 3) | (xx > 60)))] = (60, 60, 62)
        if kind == 'crate':
            if RESERVED:                                          # the add-on's: a real word
                img[14:25, 9:55] = (196, 192, 170)
                word = glyph_text('FRAGILE', FONT5)
                stamp(img, word, 16, (64 - word.shape[1]) // 2, (40, 42, 36))
            else:
                img[14:24, 12:52] = (196, 192, 170)                  # the stencil's plate
                for x0 in range(15, 49, 5):                           # its "letters"
                    img[16:22, x0:x0 + 3] = (40, 42, 36)
            band = (yy >= 44) & (yy < 52) & (xx >= 4) & (xx < 60)
            img[band] = np.where((((xx + yy) // 4) % 2 == 0)[band][:, None], (204, 168, 30), (30, 28, 26))
        else:
            img[(np.abs(xx - yy) < 2) | (np.abs(xx + yy - 63) < 2)] *= 0.8
        img[noise < -1.8] *= 0.75                                 # scuffs
    elif kind == 'container':
        # a small blue container's corrugated side, rust at the foot
        rib = (np.sin(xx * 2 * np.pi / 8) + 1) / 2
        img = np.array([40, 74, 122], float)[None, None] * (0.78 + 0.3 * rib[..., None]) * (1 + 0.06 * noise[..., None])
        img[:, :3] = img[:, 61:] = (52, 58, 66)
        img[:3] = img[61:] = (52, 58, 66)
        rust = np.clip(noise + (yy - 50) / 8 - 0.6, 0, 1)[..., None] * (yy > 44)[..., None]
        img = img * (1 - rust) + np.array([104, 62, 38], float) * rust
    elif kind in ('barrel_blue', 'barrel_red'):
        # a steel drum, painted, two rolling hoops, the paint worn off them
        paint = (36, 70, 140) if kind == 'barrel_blue' else (150, 36, 30)
        img = np.array(paint, float)[None, None] * (1 + 0.05 * noise[..., None])
        for y0 in (20, 42):
            img[y0:y0 + 3] = img[y0:y0 + 3] * 0.6 + np.array([120, 120, 124], float) * 0.4
        img[:2] = img[62:] = (70, 70, 74)
        worn = noise < -2.1
        img[worn] = img[worn] * 0.5 + np.array([96, 96, 100], float) * 0.5
        img[28:36, 22:42] = (200, 200, 190)                     # its label
        img[30:34, 25:39] = (60, 60, 60)
    elif kind == 'barrel_lid':
        r = np.hypot(xx - 31.5, yy - 31.5)
        img = np.array([96, 98, 104], float)[None, None] * (1 + 0.08 * noise[..., None])
        img[(r > 24) & (r < 28)] *= 0.6
        img[np.hypot(xx - 44, yy - 22) < 4] = (36, 36, 40)
        img[np.hypot(xx - 20, yy - 40) < 3] = (36, 36, 40)
    elif kind == 'spool':
        # a cable drum's flange: plywood, its grain, bolts round the hub
        r = np.hypot(xx - 31.5, yy - 31.5)
        grain = (np.sin((yy + 3 * noise) * 2 * np.pi / 7) + 1) / 2
        img = np.array([150, 116, 76], float)[None, None] * (0.85 + 0.2 * grain[..., None])
        for a in np.radians(np.arange(0, 360, 60)):
            img[np.hypot(xx - 31.5 - 12 * np.cos(a), yy - 31.5 - 12 * np.sin(a)) < 2] = (50, 46, 42)
        img[r < 6] = (30, 26, 22)
        img[(r > 29)] *= 0.7
    elif kind == 'spool_rim':
        img = np.array([132, 100, 64], float)[None, None] * (1 + 0.1 * noise[..., None])
        img[:, ::16] *= 0.6
    elif kind == 'cable':
        # black cable wound round the drum's core
        wind = (np.sin(xx * 2 * np.pi / 6) + 1) / 2
        img = np.array([34, 34, 38], float)[None, None] * (0.7 + 0.6 * wind[..., None]) * (1 + 0.05 * noise[..., None])
    elif kind == 'cone':
        # a traffic cone: orange, two reflective white bands
        img = np.array([224, 96, 24], float)[None, None] * (1 + 0.06 * noise[..., None])
        img[(yy >= 14) & (yy < 22)] = (230, 230, 222)
        img[(yy >= 34) & (yy < 40)] = (230, 230, 222)
        img[56:] = (40, 40, 42)                                   # its black foot
    elif kind == 'pole':
        img = np.array([110, 112, 118], float)[None, None] * (1 + 0.1 * noise[..., None])
        img[:, ::16] *= 0.8
    elif kind in ('granite', 'slab'):
        # a stone lantern's weathered granite, lichen and grime in it; the
        # slabs of the steps paler, worn smooth
        base = (128, 122, 112) if kind == 'granite' else (150, 140, 124)
        img = np.array(base, float)[None, None] * (1 + 0.1 * noise[..., None])
        speck = rnd.uniform(0, 1, (64, 64))
        img[speck > 0.93] *= 0.75
        img[speck < 0.04] *= 1.15
        moss = np.clip(noise - 1.1, 0, 1)[..., None]
        img = img * (1 - 0.6 * moss) + np.array([78, 88, 54], float) * 0.6 * moss
        if kind == 'slab':
            img[:2] *= 0.7; img[62:] *= 0.7
    elif kind == 'torii':
        # vermilion lacquer, worn and dusty, the grain showing through
        grain = (np.sin((xx + 2.5 * noise) * 2 * np.pi / 9) + 1) / 2
        img = np.array([196, 62, 30], float)[None, None] * (0.9 + 0.12 * grain[..., None]) * (1 + 0.05 * noise[..., None])
        img[noise < -1.7] = img[noise < -1.7] * 0.6 + np.array([90, 60, 44], float) * 0.4
    elif kind == 'torii_black':
        img = np.array([34, 30, 30], float)[None, None] * (1 + 0.12 * noise[..., None])
    elif kind == 'bamboo':
        # dry bamboo: straw yellow, a node every 21 rows, darker toward them
        node = np.abs(((yy + 4) % 21) - 10) / 10
        img = np.array([176, 150, 86], float)[None, None] * (0.72 + 0.28 * node[..., None]) * (1 + 0.05 * noise[..., None])
        img[(yy + 4) % 21 == 0] = (98, 84, 46)
        img[:, ::16] *= 0.85
    elif kind == 'wood':
        # an old cedar post, grey brown, its grain along it
        grain = (np.sin((xx * 1.0 + 3 * noise) * 2 * np.pi / 6) + 1) / 2
        img = np.array([88, 66, 50], float)[None, None] * (0.8 + 0.25 * grain[..., None]) * (1 + 0.05 * noise[..., None])
    elif kind in ('thatch', 'fringe'):
        # dried palm fronds laid in courses, straw to grey brown, the
        # fronds' ribs running down; the fringe's foot ragged and dark
        rib = (np.sin((xx + 1.5 * noise) * 2 * np.pi / 3.2) + 1) / 2
        img = np.array([170, 142, 88], float)[None, None] * (0.7 + 0.32 * rib[..., None]) * (1 + 0.07 * noise[..., None])
        for y0 in range(0, 64, 16):                             # the courses' lower edges, shadowed
            img[y0 + 13:y0 + 16] *= 0.68
        if kind == 'fringe':
            ends = 50 + (8 * np.abs(np.sin(xx * 1.7) * np.cos(xx * 0.53))).astype(int)
            img[yy >= ends] = img[yy >= ends] * 0.45
    elif kind == 'plank':
        # sun-greyed boards, their seams across
        grain = (np.sin((yy * 1.0 + 3 * noise) * 2 * np.pi / 7) + 1) / 2
        img = np.array([150, 128, 100], float)[None, None] * (0.82 + 0.2 * grain[..., None]) * (1 + 0.05 * noise[..., None])
        img[:, ::16] *= 0.6
    elif kind == 'driftwood':
        grain = (np.sin((xx * 1.0 + 2.5 * noise) * 2 * np.pi / 5) + 1) / 2
        img = np.array([168, 160, 148], float)[None, None] * (0.75 + 0.3 * grain[..., None]) * (1 + 0.06 * noise[..., None])
    elif kind == 'coconut':
        # the husk's brown fibre
        fibre = (np.sin((yy + 2 * noise) * 2 * np.pi / 4) + 1) / 2
        img = np.array([104, 70, 40], float)[None, None] * (0.75 + 0.35 * fibre[..., None]) * (1 + 0.08 * noise[..., None])
    elif kind == 'board':
        # a surfboard's deck: white, a red stripe down its middle, wax
        img = np.array([236, 234, 226], float)[None, None] * (1 + 0.03 * noise[..., None])
        img[:, 28:36] = (206, 52, 40)
        img[:, 26:28] = img[:, 36:38] = (40, 60, 120)
    elif kind in ('ring_red', 'ring_white'):
        img = np.array((214, 54, 36) if kind == 'ring_red' else (238, 236, 228), float)[None, None] * (1 + 0.04 * noise[..., None])
    elif kind == 'flag':
        # the lifeguards' red over yellow
        img = np.where((yy < 32)[..., None], np.array([214, 40, 32], float), np.array([246, 204, 40], float)) \
            * (1 + 0.04 * noise[..., None])
    elif kind == 'shells':
        # seen from above, on nothing (colour 0): a starfish (top left), a
        # scallop (top right), a cowrie (bottom left), a coconut's broken
        # half shell (bottom right)
        rgba = np.zeros((64, 64, 4))
        cy, cx = yy[:32, :32] - 15.5, xx[:32, :32] - 15.5
        a, r = np.arctan2(cy, cx), np.hypot(cx, cy)
        star = r < 4 + 10 * np.abs(np.cos(2.5 * a)) ** 3
        rgba[:32, :32][star] = (226, 116, 60, 255)
        rgba[:32, :32][star & (r < 4)] = (196, 90, 48, 255)
        cy, cx = yy[:32, 32:] - 18, xx[:32, 32:] - 47.5
        a, r = np.arctan2(cx, -cy), np.hypot(cx, cy)
        fan = (r < 13) & (np.abs(a) < 1.25)
        rgba[:32, 32:][fan] = np.where(((a * 8 % 1) < 0.5)[fan][:, None], (238, 214, 196, 255), (206, 168, 150, 255))
        rgba[:32, 32:][(np.abs(cx) < 4) & (cy > 0) & (cy < 4)] = (206, 168, 150, 255)
        cy, cx = yy[32:, :32] - 47.5, xx[32:, :32] - 15.5
        cow = (cx / 10) ** 2 + (cy / 7) ** 2 < 1
        rgba[32:, :32][cow] = (236, 222, 190, 255)
        rgba[32:, :32][cow & ((cx / 10) ** 2 + (cy / 7) ** 2 < 0.4)] = (190, 150, 100, 255)
        cy, cx = yy[32:, 32:] - 47.5, xx[32:, 32:] - 47.5
        r = np.hypot(cx / 1.1, cy)
        rgba[32:, 32:][r < 12] = (84, 56, 34, 255)
        rgba[32:, 32:][r < 9.5] = (238, 230, 212, 255)
        return [vr.picture(*see_through(rgba)[::-1])]
    elif kind == 'lotus':
        # a lotus lantern's paper petals, four colours a band of 16 rows each
        # (pink, red, yellow, green): two tiers of petals, their tips up,
        # each petal paler toward its tip, a dark line round it
        img = np.zeros((64, 64, 3))
        for b, rgb in enumerate(((240, 118, 164), (218, 48, 52), (250, 204, 58), (96, 176, 84))):
            r = yy[:16] % 8
            t = (((xx[:16] + 4 * (yy[:16] // 8)) % 8) - 3.5) / 3.5
            tip = 1 + 6 * t ** 2
            shade = np.where(r < tip, 0.55, 0.8 + 0.3 * (1 - (r - tip) / 8))
            band = np.array(rgb, float)[None, None] * shade[..., None]
            band[np.abs(r - tip) < 0.7] = np.array(rgb, float) * 0.45
            img[16 * b:16 * b + 16] = band * (1 + 0.03 * noise[:16, :, None])
    elif kind in ('jangseung_m', 'jangseung_f', 'jangseung_body', 'jangseung_body_f'):
        # a village guardian post's carved head (its face round column 32,
        # the side toward the fight) or its body, the guardian's name in
        # black down its front; old pine, grey brown, its grain upright
        grain = (np.sin((xx * 1.0 + 3 * noise) * 2 * np.pi / 7) + 1) / 2
        img = np.array([138, 112, 84], float)[None, None] * (0.82 + 0.22 * grain[..., None]) * (1 + 0.05 * noise[..., None])
        black = np.array([28, 22, 20], float)
        if kind in ('jangseung_body', 'jangseung_body_f'):
            for g in range(5):
                y0 = 5 + 11 * g
                strokes = rnd.uniform(0, 1, (9, 9)) > 0.55
                strokes[:, 4] |= g % 2 == 0
                strokes[4, :] |= g % 2 == 1
                if RESERVED:                # the add-on's: his 천하대장군, her 지하여장군
                    strokes = GLYPHS[('지하여장군' if kind == 'jangseung_body_f' else '천하대장군')[g]]
                img[y0:y0 + 9, 28:37][strokes] = black
        else:
            m = kind == 'jangseung_m'
            if m:
                img = img * 0.7 + np.array([176, 70, 50], float) * 0.3      # his face painted red
            for ex in (23, 41):
                brow = (np.abs(yy - (16 - 0.12 * (xx - ex) ** 2 * (1 if m else 0.5))) < (2.2 if m else 1.3)) &                        (np.abs(xx - ex) < 7)
                img[brow] = black
                d = np.hypot(xx - ex, yy - 25)
                img[d < (6 if m else 4.5)] = (232, 228, 214)
                img[d < (2.8 if m else 2.2)] = black
                img[np.abs(d - (6 if m else 4.5)) < 0.7] = black
            nose = (yy >= 28) & (yy <= 40) & (np.abs(xx - 32) < 3 + (yy - 28) * 0.35)
            img[nose] *= 0.72
            img[nose & ((yy == 40) | (np.abs(np.abs(xx - 32) - (3 + (yy - 28) * 0.35)) < 0.8))] = black
            if m:
                mouth = (yy >= 45) & (yy <= 54) & (np.abs(xx - 32) < 12)
                img[mouth] = (236, 230, 210)
                img[mouth & (((xx - 32) % 4 == 0) | (yy == 49) | (yy == 45) | (yy == 54))] = black
                img[(np.abs(yy - (43 - 0.04 * (xx - 32) ** 2)) < 1.4) & (np.abs(xx - 32) < 15)] = black   # moustache
                for bx in range(22, 43, 4):
                    img[56:63, bx:bx + 1] = black                                                   # beard
            else:
                img[np.hypot(xx - 19, yy - 36) < 4] = img[np.hypot(xx - 19, yy - 36) < 4] * 0.5 + np.array([220, 70, 70]) * 0.5
                img[np.hypot(xx - 45, yy - 36) < 4] = img[np.hypot(xx - 45, yy - 36) < 4] * 0.5 + np.array([220, 70, 70]) * 0.5
                lips = (np.abs(yy - 48) < 2.5 - 0.02 * (xx - 32) ** 2) & (np.abs(xx - 32) < 9)
                img[lips] = (196, 40, 40)
                img[:6] = black                                                                     # her hair
    elif kind == 'onggi':
        # an earthenware jar's dark brown glaze, a finger-drawn wave round
        # its shoulder, runs of the glaze down it
        img = np.array([86, 48, 30], float)[None, None] * (1 + 0.08 * noise[..., None])
        wave = np.abs(yy - (22 + 4 * np.sin(xx * 2 * np.pi / 16))) < 1.2
        img[wave] = (138, 96, 62)
        leaf = (np.abs(yy - 30 - 3 * np.sin(xx * 2 * np.pi / 32)) < 0.9) & (xx % 32 < 18)
        img[leaf] = (128, 88, 56)
        run = (np.abs(np.sin(xx * 0.7) * np.cos(xx * 0.23)) > 0.9) & (yy > 36)
        img[run] *= 1.25
        img[:3] *= 0.7
    elif kind == 'marble':
        # Makrana marble: white with a faint warm cast, grey veins wandering
        # across it
        img = np.array([226, 226, 222], float)[None, None] * (1 + 0.025 * noise[..., None])
        vein = np.abs(np.sin((xx * 0.55 + yy * 0.3 + 7 * noise) * 2 * np.pi / 40))
        img[vein < 0.06] *= 0.8
        img[(vein >= 0.06) & (vein < 0.14)] *= 0.92
    elif kind == 'brass':
        # polished brass, darkened in its hollows, a highlight down it
        shine = np.exp(-((xx % 32 - 10) / 5.0) ** 2)
        img = np.array([150, 108, 44], float)[None, None] * (0.85 + 0.45 * shine[..., None]) * (1 + 0.06 * noise[..., None])
        img[::16] *= 0.7
    elif kind == 'clay':
        # a diya's fired clay, its oil dark in the bowl
        img = np.array([156, 82, 46], float)[None, None] * (1 + 0.1 * noise[..., None])
        img[:4] = (60, 34, 22)
    elif kind == 'cypress':
        # a cypress's dense dark foliage in upright tufts
        tuft = (np.sin((xx + 2 * noise) * 2 * np.pi / 6) * np.sin((yy * 0.5 + 3 * noise) * 2 * np.pi / 8) + 1) / 2
        img = np.array([30, 58, 34], float)[None, None] * (0.6 + 0.6 * tuft[..., None]) * (1 + 0.08 * noise[..., None])
    elif kind == 'jali':
        # a pierced marble screen: a frame, inside it eight-pointed stars
        # cut through on a square lattice (colour 0: seen through)
        rgba = np.zeros((64, 64, 4))
        vein = np.abs(np.sin((xx * 0.5 + yy * 0.25 + 6 * noise) * 2 * np.pi / 40))
        rgba[..., :3] = np.array([224, 224, 220], float)[None, None] * (1 + 0.025 * noise[..., None])
        rgba[vein < 0.08, :3] *= 0.85
        rgba[..., 3] = 255
        dx, dy = (xx - 3) % 10 - 4.5, (yy - 3) % 10 - 4.5
        star = (np.maximum(np.abs(dx), np.abs(dy)) < 3.2) | (np.abs(dx) + np.abs(dy) < 4.6)
        inside = (xx >= 4) & (xx <= 59) & (yy >= 4) & (yy <= 59)
        rgba[star & inside, 3] = 0
        rgba[(xx < 3) | (xx > 60) | (yy < 3) | (yy > 60), :3] *= 0.88
        return [vr.picture(*see_through(rgba)[::-1])]
    elif kind == 'marigold':
        # a garland of marigolds: round heads, orange and yellow by turns
        cell = ((xx // 8) + (yy // 8)) % 2
        d = np.hypot(xx % 8 - 3.5, yy % 8 - 3.5)
        base = np.where(cell[..., None] == 0, np.array([238, 128, 20], float), np.array([248, 188, 36], float))
        img = base * (1.05 - 0.45 * (d / 5.0)[..., None]) * (1 + 0.08 * noise[..., None])
        img[d > 4.6] = (60, 70, 24)
    elif kind == 'bamboo_green':
        # a living culm: fresh green, a node every 21 rows (a ridge, the
        # white bloom under it), faint yellow streaks along it
        node = np.abs(((yy + 4) % 21) - 10) / 10
        streak = np.clip(rnd.normal(0, 1, (1, 64)).repeat(64, 0) - 1.2, 0, 1)
        img = np.array([86, 128, 46], float)[None, None] * (0.8 + 0.22 * node[..., None]) * (1 + 0.05 * noise[..., None])
        img = img * (1 - 0.4 * streak[..., None]) + np.array([170, 168, 78], float) * 0.4 * streak[..., None]
        img[(yy + 4) % 21 == 0] = (58, 84, 30)
        img[(yy + 4) % 21 == 1] = (176, 190, 150)
        img[:, ::16] *= 0.85
    elif kind == 'bamboo_leaves':
        # a spray of bamboo leaves on nothing (colour 0): long narrow
        # blades hanging from thin twigs, two greens, a pale midrib
        rgba = np.zeros((64, 64, 4))
        for _ in range(16):
            x0, y0 = rnd.uniform(14, 50), rnd.uniform(4, 30)
            a = rnd.uniform(0.35, 2.8) if rnd.uniform() < 0.8 else rnd.uniform(-0.3, 0.3)
            length, width = rnd.uniform(18, 30), rnd.uniform(2.2, 3.4)
            u = (xx - x0) * np.cos(a) + (yy - y0) * np.sin(a)
            v = -(xx - x0) * np.sin(a) + (yy - y0) * np.cos(a)
            t = u / length
            blade = (t > 0) & (t < 1) & (np.abs(v) < width * np.sin(np.pi * np.clip(t, 0, 1) ** 0.7))
            green = np.array([58, 112, 40] if rnd.uniform() < 0.5 else [92, 140, 52], float) * rnd.uniform(0.85, 1.1)
            rgba[blade, :3] = green * (1 + 0.06 * noise[blade][:, None])
            rgba[blade & (np.abs(v) < 0.5), :3] = green * 1.25
            rgba[blade, 3] = 255
        return [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    elif kind == 'tea':
        # a clipped tea bush: small dark glossy leaves, the new shoots on
        # top light green
        leaf = (np.sin((xx + 2 * noise) * 2 * np.pi / 5) * np.sin((yy + 2 * noise) * 2 * np.pi / 5) + 1) / 2
        img = np.array([40, 82, 36], float)[None, None] * (0.65 + 0.55 * leaf[..., None]) * (1 + 0.08 * noise[..., None])
        shoot = (rnd.uniform(0, 1, (64, 64)) > 0.9) & (yy < 22)
        img[shoot] = (128, 170, 66)
        img[(leaf > 0.92)] *= 1.25
    elif kind == 'tile_green':
        # glazed green roof tiles: rows of round tiles down the slope, the
        # channels between them dark, the glaze shining on each roll
        roll = (np.cos((xx % 8 - 3.5) / 3.5 * np.pi / 2) ** 2)
        img = np.array([40, 110, 70], float)[None, None] * (0.55 + 0.6 * roll[..., None]) * (1 + 0.05 * noise[..., None])
        img[yy % 16 == 15] *= 0.7
        img[60:] = (196, 160, 60)                           # the eave's golden drip tiles
    elif kind == 'plaque':
        # the gate's name board: dark blue, a gold frame, four gold words
        img = np.array([28, 40, 88], float)[None, None] * (1 + 0.05 * noise[..., None])
        gold = np.array([214, 172, 60], float)
        img[(xx < 4) | (xx > 59) | (yy < 4) | (yy > 59)] = gold
        img[(xx == 5) | (xx == 58) | (yy == 5) | (yy == 58)] = (120, 90, 30)
        for k in range(4):
            x0 = 10 + 12 * k
            strokes = rnd.uniform(0, 1, (9, 9)) > 0.5
            strokes[4, :] |= k % 2 == 0
            strokes[:, 4] |= k % 2 == 1
            if RESERVED:                    # the add-on's: 天府之國, right to left
                strokes = GLYPHS['國之府天'[k]]
            img[27:36, x0:x0 + 9][strokes] = gold
    elif kind == 'xuanzi':
        # a beam's painting: bands of blue and green, white and gold lines
        # between, a whorl in the middle of each panel
        band = (yy * 6 // 64) % 3
        img = np.where((band == 1)[..., None], np.array([34, 108, 96], float), np.array([36, 62, 128], float))
        img = img * (1 + 0.05 * noise[..., None])
        img[(yy % 21 == 0) | (yy % 21 == 1)] = (226, 222, 206)
        img[np.abs(np.hypot((xx % 32) - 15.5, (yy % 21) - 10.5) - 6) < 1.2] = (214, 172, 60)
        img[(xx % 32 == 0)] = (160, 40, 30)
    elif kind == 'bronze':
        # an old bronze censer: dark brown green, a turquoise patina run
        # down from its rim, a band of key pattern round its top
        img = np.array([70, 62, 42], float)[None, None] * (1 + 0.1 * noise[..., None])
        patina = np.clip(noise * 0.6 + (1 - yy / 63) * 0.5 - 0.4, 0, 1)[..., None]
        img = img * (1 - 0.7 * patina) + np.array([70, 140, 118], float) * 0.7 * patina
        key = (yy >= 6) & (yy < 18)
        mk = (((xx % 12) < 2) | ((yy - 6) % 12 < 2) | (((xx % 12) >= 6) & ((xx % 12) < 8) & ((yy - 6) % 12 < 8)))
        img[key & mk] = (130, 110, 60)
        img[(yy == 4) | (yy == 19)] = (40, 36, 26)
    elif kind == 'wine_jar':
        # a rice wine jar: brown glaze, a red paper diamond on its belly
        # with a black word, the glaze running at its foot
        img = np.array([104, 60, 32], float)[None, None] * (1 + 0.08 * noise[..., None])
        img *= (0.85 + 0.25 * np.exp(-((xx % 32 - 9) / 6.0) ** 2))[..., None]
        dia = np.abs(xx % 32 - 15.5) + np.abs(yy - 30) < 11
        img[dia] = (200, 40, 32)
        img[dia & (np.abs(xx % 32 - 15.5) < 2.5) & (np.abs(yy - 30) < 6)] = (24, 18, 16)
        img[dia & (np.abs(yy - 30) < 1.2) & (np.abs(xx % 32 - 15.5) < 5)] = (24, 18, 16)
        img[56:] *= 0.75
    elif kind == 'sheaf':
        # a rice sheaf: golden straw upright, a twisted band round its waist
        stalk = (np.sin((xx + 1.2 * noise) * 2 * np.pi / 2.6) + 1) / 2
        img = np.array([206, 170, 88], float)[None, None] * (0.72 + 0.32 * stalk[..., None]) * (1 + 0.06 * noise[..., None])
        img[(yy >= 30) & (yy < 36)] = np.array([150, 116, 60], float) * (0.8 + 0.3 * stalk[(yy >= 30) & (yy < 36)][:, None])
        img[:6] = img[:6] * 0.8 + np.array([170, 150, 70], float) * 0.2         # the ears
    elif kind == 'wicker':
        # a bamboo basket's weave: light strips over and under
        over = ((xx // 4 + yy // 4) % 2 == 0)
        img = np.where(over[..., None], np.array([190, 160, 100], float), np.array([150, 120, 70], float))
        img = img * (1 + 0.06 * noise[..., None])
        img[(xx % 4 == 0) | (yy % 4 == 0)] *= 0.75
        img[:3] = img[61:] = (120, 92, 52)
    elif kind == 'mossy':
        # a field stone half grown over with moss
        img = np.array([120, 116, 104], float)[None, None] * (1 + 0.1 * noise[..., None])
        moss = np.clip(noise * 0.8 + (yy / 63 - 0.3) * -1.2 + 0.4, 0, 1)[..., None]
        img = img * (1 - 0.8 * moss) + np.array([70, 100, 42], float) * (1 + 0.1 * noise[..., None]) * 0.8 * moss
    elif kind == 'hay':
        # a round bale's rolled hay: pale gold, fine stalks across it, the
        # net wrap's faint lines along
        stalk = (np.sin((yy + 1.5 * noise) * 2 * np.pi / 2.4) + 1) / 2
        img = np.array([204, 180, 112], float)[None, None] * (0.74 + 0.3 * stalk[..., None]) * (1 + 0.07 * noise[..., None])
        img[noise < -1.3] *= 0.78
        img[:, ::11] = img[:, ::11] * 0.82 + np.array([236, 230, 210], float) * 0.18
    elif kind == 'hay_end':
        # a bale's end: the hay rolled up in a spiral, darker toward its core
        r, a = np.hypot(xx - 31.5, yy - 31.5), np.arctan2(yy - 31.5, xx - 31.5)
        coil = (np.sin(r * 2 * np.pi / 4.2 - a + 0.8 * noise) + 1) / 2
        img = np.array([196, 170, 100], float)[None, None] * (0.7 + 0.32 * coil[..., None]) * (1 + 0.06 * noise[..., None])
        img *= (0.8 + 0.2 * np.clip(r / 31.5, 0, 1))[..., None]
    elif kind == 'drystone':
        # a dry-stone wall: flat field stones laid in rough courses, dark
        # chinks between, lichen and moss on some
        img = np.zeros((64, 64, 3))
        y = 0
        while y < 64:
            h = int(rnd.integers(7, 12))
            x = -int(rnd.integers(0, 12))
            while x < 64:
                w = int(rnd.integers(10, 22))
                tone = np.array(rnd.choice([[150, 142, 126], [134, 126, 110], [162, 152, 130], [120, 114, 102]]), float)
                tone = tone * rnd.uniform(0.9, 1.08)
                img[y:y + h, max(0, x):x + w] = tone
                if rnd.uniform() < 0.3:
                    img[y:y + 2, max(0, x):x + w] = tone * 0.6 + np.array([90, 110, 50]) * 0.4
                img[y:y + h, max(0, x):max(0, x) + 1] = (40, 38, 34)
                x += w
            img[y + h - 1:y + h] = (44, 40, 36)
            y += h
        img *= (1 + 0.08 * noise[..., None])
    elif kind == 'wall_brick':
        # the Great Wall's grey bricks: long courses, every other one half a
        # brick on, pale lime mortar, each brick its own tone, rain streaks
        # down from the top, moss and soil at the foot
        img = np.zeros((64, 64, 3))
        for row in range(8):
            off = 8 if row % 2 else 0
            for b in range(-1, 4):
                tone = np.array([128, 124, 110], float) * rnd.uniform(0.82, 1.1) + rnd.uniform(-6, 6, 3)
                x0 = b * 16 + off
                img[8 * row:8 * row + 8, max(0, x0):max(0, min(64, x0 + 16))] = tone
        img[::8] = (158, 152, 134)
        for row in range(8):
            off = 8 if row % 2 else 0
            img[8 * row + 1:8 * row + 8, off::16] = (152, 146, 128)
        img *= (1 + 0.07 * noise[..., None])
        streak = np.clip(np.sin(xx * 0.9 + 3 * noise) - 0.6, 0, 1) * (1 - yy / 63) ** 2
        img *= (1 - 0.3 * streak)[..., None]
        moss = np.clip(noise * 0.6 + (yy / 63 - 0.8) * 3, 0, 1)[..., None]
        img = img * (1 - 0.6 * moss) + np.array([82, 92, 54], float) * 0.6 * moss
    elif kind == 'burlap':
        # a scarecrow's sack head: coarse weave, two stitched eyes, a grin
        weave = ((xx % 3 == 0) | (yy % 3 == 0))
        img = np.array([176, 146, 96], float)[None, None] * (1 + 0.07 * noise[..., None])
        img[weave] *= 0.86
        for ex in (22, 42):
            img[(np.abs(xx - ex) < 4) & (np.abs(yy - 26) < 4)] = (34, 28, 24)
        smile = (np.abs(yy - (46 - 0.012 * (xx - 32) ** 2)) < 1.4) & (np.abs(xx - 32) < 13)
        img[smile] = (44, 30, 26)
        img[(np.abs(yy - 46 + 0.012 * (xx - 32) ** 2) < 3.5) & (np.abs(xx - 32) < 12) & (xx % 4 == 0)] = (60, 44, 34)
    elif kind == 'shirt':
        # a worn flannel shirt: red check, a darker cross weave, a patch
        check = ((xx // 8) % 2) + ((yy // 8) % 2)
        img = np.array([168, 40, 34], float)[None, None] * np.where(check[..., None] == 2, 0.55, np.where(check[..., None] == 1, 0.8, 1.0))
        img = img * (1 + 0.06 * noise[..., None])
        img[(xx % 8 == 3) | (yy % 8 == 3)] = img[(xx % 8 == 3) | (yy % 8 == 3)] * 0.7 + np.array([30, 24, 22]) * 0.3
        img[36:50, 10:24] = np.array([70, 84, 120], float) * (1 + 0.06 * noise[36:50, 10:24, None])   # the patch
        img[36, 10:24] = img[49, 10:24] = (230, 220, 190)
    elif kind == 'skep':
        # a straw beehive: thick coils of straw, bound with split bramble
        coil = (np.cos((yy % 8 - 3.5) / 3.5 * np.pi / 2) ** 1.5)
        img = np.array([196, 160, 92], float)[None, None] * (0.55 + 0.5 * coil[..., None]) * (1 + 0.06 * noise[..., None])
        bind = ((xx + 5 * (yy // 8)) % 10 < 2)
        img[bind] = np.array([120, 92, 56], float) * (0.6 + 0.5 * coil[bind][:, None])
        img[56:] = (40, 30, 22)                             # the bees' door, at the foot
        img[56:, :20] = img[56:, 44:] = np.array([170, 140, 84], float)
    elif kind == 'leaf_green':
        # sunflower leaves and stems: a fresh mid green, veins
        img = np.array([78, 120, 40], float)[None, None] * (1 + 0.08 * noise[..., None])
        img[(xx + yy) % 9 == 0] *= 0.8
    elif kind == 'wheel':
        # a cart wheel on nothing (colour 0): iron tyre, wooden felloes,
        # twelve spokes, the hub
        rgba = np.zeros((64, 64, 4))
        r, a = np.hypot(xx - 31.5, yy - 31.5), np.arctan2(yy - 31.5, xx - 31.5)
        wood = np.array([128, 92, 56], float) * (1 + 0.08 * noise[..., None])
        rim = (r > 25) & (r < 31.5)
        off = np.abs(((a * 12 / (2 * np.pi)) + 0.5) % 1 - 0.5)          # 0 on a spoke, 0.5 between two
        spoke = (r < 26) & (off * 2 * np.pi * r / 12 < 1.4)
        hub = r < 6
        for m, rgb in ((rim, wood), (spoke, wood * 0.9), (hub, wood * 0.75)):
            rgba[m, :3] = rgb[m]
            rgba[m, 3] = 255
        rgba[(r > 29.5) & (r < 31.5), :3] = (58, 54, 52)
        return [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    elif kind in ('poppies', 'cornflowers', 'lavender', 'daisies'):
        # a clump of wild flowers on nothing (colour 0): thin green stems
        # and leaves from the foot, the flowers at their tops
        rgba = np.zeros((64, 64, 4))
        greens = (np.array([70, 112, 40], float), np.array([96, 132, 52], float))
        heads = {'poppies': ((214, 34, 26), (176, 22, 20)), 'cornflowers': ((64, 96, 210), (44, 70, 170)),
                 'lavender': ((126, 80, 186), (96, 58, 150)), 'daisies': ((246, 244, 236), (226, 222, 210))}[kind]
        for _ in range(26 if kind == 'lavender' else 16):
            x0 = rnd.uniform(6, 58)
            top = rnd.uniform(6, 34)
            lean = rnd.uniform(-0.25, 0.25)
            for y in range(int(top), 64):
                x = int(round(x0 + lean * (64 - y)))
                if 0 <= x < 63:
                    rgba[y, x:x + 2, :3] = greens[int(rnd.integers(2))]; rgba[y, x:x + 2, 3] = 255
            hx, hy = x0 + lean * (64 - top), top
            if kind == 'lavender':
                spike = (np.abs(xx - hx) < 1.6) & (yy > hy - 2) & (yy < hy + 9)
                rgba[spike, :3] = np.array(heads[int(rnd.integers(2))], float); rgba[spike, 3] = 255
            else:
                rad = rnd.uniform(2.4, 3.6)
                d = np.hypot(xx - hx, (yy - hy) * 1.3)
                head = d < rad
                rgba[head, :3] = np.array(heads[0], float) * (1.05 - 0.25 * (d[head] / rad))[:, None]
                rgba[head, 3] = 255
                eye = d < (1.0 if kind != 'daisies' else 1.3)
                rgba[eye, :3] = (30, 26, 30) if kind == 'poppies' else (232, 196, 40) if kind == 'daisies' else heads[1]
        # a few blades of grass at the foot
        for _ in range(10):
            x0, y0 = rnd.uniform(4, 60), rnd.uniform(44, 54)
            for y in range(int(y0), 64):
                x = int(round(x0 + (64 - y) * rnd.uniform(-0.2, 0.2)))
                if 0 <= x < 64:
                    rgba[y, x, :3] = greens[0] * 0.9; rgba[y, x, 3] = 255
        return [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    elif kind == 'sunflower':
        # a sunflower's head on nothing (colour 0): golden petals round a
        # dark brown seed disc
        rgba = np.zeros((64, 64, 4))
        r, a = np.hypot(xx - 31.5, yy - 31.5), np.arctan2(yy - 31.5, xx - 31.5)
        petal = (r < 30 * (0.78 + 0.22 * np.abs(np.cos(a * 11)))) & (r > 12)
        rgba[petal, :3] = np.array([246, 192, 30], float) * (1.05 - 0.2 * (r[petal] / 30))[:, None]
        rgba[petal, 3] = 255
        disc = r <= 13
        seeds = (np.sin(xx * 1.7) * np.sin(yy * 1.7) > 0)
        rgba[disc, :3] = np.where(seeds[disc][:, None], np.array([70, 44, 22], float), np.array([96, 64, 30], float))
        rgba[disc, 3] = 255
        return [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    elif kind == 'balusters':
        # a cast iron railing's bays on nothing (colour 0): thin black
        # bars, a rail at the foot, a ring of scrolls under the top rail
        rgba = np.zeros((64, 64, 4))
        iron = np.array([40, 38, 42], float) * (1 + 0.1 * noise[..., None])
        bars = (xx % 8 < 2)
        rail = (yy > 59) | (yy < 3)
        scroll = (np.abs(np.hypot((xx % 8) - 4, yy - 9) - 3.2) < 0.9) & (yy < 14)
        m = bars | rail | scroll
        rgba[m, :3] = iron[m]
        rgba[m, 3] = 255
        return [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    elif kind == 'slat':
        # a park bench's slat: wood painted the parks' dark green, worn
        img = np.array([46, 84, 52], float)[None, None] * (1 + 0.1 * noise[..., None])
        worn = np.clip(noise - 1.1, 0, 1)[..., None]
        img = img * (1 - worn) + np.array([120, 96, 66], float) * worn
        img[:, ::21] *= 0.75
    elif kind == 'hydrant':
        # a hydrant's paint: fire red, chipped, the bonnet's silver at the top
        img = np.array([176, 34, 30], float)[None, None] * (1 + 0.1 * noise[..., None])
        img[:10] = np.array([168, 168, 160], float) * (1 + 0.08 * noise[:10, :, None])
        img[noise < -1.6] = (70, 36, 30)
    elif kind in ('steel', 'cart_side'):
        # brushed stainless steel; the cart's side its sign: a red band,
        # "HOT DOGS" in white blocks, a yellow stripe
        img = np.array([176, 178, 182], float)[None, None] * (1 + 0.05 * rnd.normal(0, 1, (1, 64)).repeat(64, 0)[..., None])
        img *= (1 + 0.04 * noise[..., None])
        if kind == 'cart_side':
            img[8:30] = (186, 30, 34)
            if RESERVED:                    # the add-on's: the words themselves
                word = glyph_text('HOT DOGS', FONT5)
                stamp(img, word, 15, (64 - word.shape[1]) // 2, (244, 240, 232))
            else:
                for k, x0 in enumerate(range(6, 58, 6)):
                    if k != 3:
                        img[13:25, x0:x0 + 4] = (244, 240, 232)
            img[32:36] = (240, 196, 40)
    elif kind in ('canvas_blue', 'canvas_yellow'):
        # an umbrella's panel: blue or yellow canvas, its seams darker
        base = (32, 74, 170) if kind == 'canvas_blue' else (244, 200, 46)
        img = np.array(base, float)[None, None] * (1 + 0.06 * noise[..., None])
        img[:, :2] *= 0.75; img[:, 62:] *= 0.75
    elif kind == 'tyre':
        # a cart's wheel on nothing (colour 0): black tyre, grey hub
        rgba = np.zeros((64, 64, 4))
        r = np.hypot(xx - 31.5, yy - 31.5)
        tyre, hub = (r > 22) & (r < 31.5), r < 10
        rgba[tyre, :3] = np.array([30, 30, 32], float) * (1 + 0.1 * noise[tyre][:, None])
        rgba[(r >= 10) & (r <= 22), :3] = (120, 122, 126)
        rgba[hub, :3] = (168, 170, 174)
        rgba[r < 31.5, 3] = 255
        return [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    elif kind in ('usps', 'usps_side', 'usps_top'):
        # a mailbox's navy blue; its face the chute's flap and a white
        # eagle's head in a blue square, the lettering
        img = np.array([34, 52, 112], float)[None, None] * (1 + 0.07 * noise[..., None])
        if kind == 'usps':
            img[6:16, 10:54] = (70, 84, 136)                    # the flap
            img[6:8, 10:54] = (20, 30, 70)
            img[26:46, 22:42] = (230, 230, 232)                 # the badge
            img[30:42, 26:38] = (34, 52, 112)
            if RESERVED:                                        # the add-on's: U.S. MAIL
                word = glyph_text('U.S. MAIL', FONT3)
                stamp(img, word, 49, (64 - word.shape[1]) // 2, (220, 220, 224))
            else:
                img[50:54, 14:50] = (220, 220, 224)             # lettering
        if kind == 'usps_top':
            img[:, ::16] *= 0.8
    elif kind == 'wire':
        # a litter basket's mesh: dark green wire, the inside dark through it
        img = np.full((64, 64, 3), 26.0) * (1 + 0.1 * noise[..., None])
        mesh = ((xx + yy) % 8 < 2) | ((xx - yy) % 8 < 2)
        img[mesh] = np.array([46, 82, 50], float) * (1 + 0.1 * noise[mesh][:, None])
        img[:4] = img[60:] = (46, 82, 50)
    elif kind == 'manhole':
        # a cast iron manhole cover from above: a raised grid, a ring
        r = np.hypot(xx - 31.5, yy - 31.5)
        img = np.array([62, 58, 56], float)[None, None] * (1 + 0.1 * noise[..., None])
        img[((xx % 6 < 2) | (yy % 6 < 2)) & (r < 24)] *= 1.35
        img[(r > 25) & (r < 28)] *= 1.4
        img[r > 29] *= 0.6
    elif kind in ('pew', 'pew_end'):
        # a pew's dark oak, waxed: the grain along it, a plank's edge; its
        # end a raised panel, a pointed arch cut in it
        grain = (np.sin((yy * 0.9 + 4 * noise + 0.15 * xx) * 2 * np.pi / 9) + 1) / 2
        img = np.array([78, 48, 28], float)[None, None] * (0.8 + 0.3 * grain[..., None]) * (1 + 0.05 * noise[..., None])
        if kind == 'pew':
            img[::21] *= 0.6
        else:
            img[(xx < 5) | (xx > 58) | (yy < 5) | (yy > 58)] *= 1.2
            arch = (np.abs(xx - 31.5) < 13) & (yy > 14 + 9 * (np.abs(xx - 31.5) / 13) ** 2) & (yy < 54)
            edge = arch & ~((np.abs(xx - 31.5) < 11) & (yy > 17 + 9 * (np.abs(xx - 31.5) / 11) ** 2) & (yy < 52))
            img[arch] *= 0.72
            img[edge] *= 0.55
    elif kind == 'marble_dark':
        # a column's dark grey marble, polished, pale veins across it
        img = np.array([70, 72, 80], float)[None, None] * (1 + 0.05 * noise[..., None])
        vein = np.abs(np.sin((xx * 0.35 + yy * 0.6 + 8 * noise) * 2 * np.pi / 36))
        img[vein < 0.05] = img[vein < 0.05] * 0.4 + np.array([170, 168, 160], float) * 0.6
        img[(vein >= 0.05) & (vein < 0.12)] *= 1.18
        img *= (0.8 + 0.35 * np.exp(-((xx - 22) / 7.0) ** 2))[..., None]
    elif kind == 'frontal':
        # the altar's frontal: crimson silk, a gold border, a gold cross
        img = np.array([128, 20, 26], float)[None, None] * (1 + 0.06 * noise[..., None])
        img *= (0.9 + 0.1 * np.sin(xx * 2 * np.pi / 6))[..., None]
        gold = np.array([204, 160, 64], float)
        img[(xx < 4) | (xx > 59) | (yy < 5) | (yy > 58)] = gold
        img[((xx < 6) | (xx > 57) | (yy < 7) | (yy > 56)) & ~((xx < 4) | (xx > 59) | (yy < 5) | (yy > 58))] = (64, 10, 12)
        img[(np.abs(xx - 31.5) < 3) & (yy > 14) & (yy < 50)] = gold
        img[(np.abs(yy - 26) < 3) & (np.abs(xx - 31.5) < 12)] = gold
        img *= (1 + 0.05 * noise[..., None])
    elif kind == 'linen':
        # the altar cloth: white linen, a lace edge of small holes
        img = np.array([224, 220, 206], float)[None, None] * (1 + 0.03 * noise[..., None])
        img[yy > 56] *= np.where(((xx + (yy % 2) * 2) % 4 < 2), 0.7, 1.0)[yy > 56][:, None]
        img[(yy > 50) & (yy < 53)] *= 0.86
    elif kind == 'wax':
        # a church candle's ivory wax, drips run down from its top
        img = np.array([228, 218, 192], float)[None, None] * (1 + 0.025 * noise[..., None])
        drip = np.zeros(64)
        for x0 in rnd.uniform(0, 64, 5):
            drip = np.maximum(drip, rnd.uniform(8, 30) * np.exp(-((np.arange(64) - x0) / 2.2) ** 2))
        img[yy < drip[None]] *= 1.06
        img[np.abs(yy - drip[None]) < 1.2] *= 0.9
        img[:3] *= 0.85
    elif kind == 'chain':
        # an iron chain's links seen from the side, dark against the dark
        img = np.full((64, 64, 3), 30.0) * (1 + 0.1 * noise[..., None])
        link = np.abs(np.hypot((xx - 31.5) / 2.2, ((yy % 16) - 7.5) / 1.0) - 6) < 1.4
        img[link] = np.array([92, 88, 84], float) * (1 + 0.1 * noise[link][:, None])
    elif kind == 'carpet':
        # the altar steps' runner: deep red wool, a gold edge
        img = np.array([112, 18, 22], float)[None, None] * (1 + 0.08 * noise[..., None])
        img[(xx < 5) | (xx > 58)] = (176, 132, 50)
        img[((xx > 6) & (xx < 9)) | ((xx > 54) & (xx < 57))] = (176, 132, 50)
    elif kind == 'concrete':
        # a rooftop parapet's cast concrete: grey, rain streaks down it, a
        # darker joint and the stains of the city's dirt
        img = np.array([104, 104, 108], float)[None, None] * (1 + 0.1 * noise[..., None])
        streak = np.clip(rnd.normal(0, 1, (1, 64)).repeat(64, 0) * (yy / 63) - 0.6, 0, 1)
        img *= (1 - 0.3 * streak)[..., None]
        img[:, 62:] *= 0.6
        img *= (1 - 0.25 * np.clip(noise - 1.0, 0, 1))[..., None]
    elif kind in ('sandstone', 'naga'):
        # Angkor's grey sandstone: a cool grey with an ochre cast, black
        # lichen in blotches, rain streaks down it, green moss low; the
        # naga's body its scales, rows of arcs along it
        img = np.array([146, 138, 116], float)[None, None] * (1 + 0.09 * noise[..., None])
        lichen = np.clip(noise - 0.9, 0, 1)[..., None]
        img = img * (1 - 0.55 * lichen) + np.array([44, 44, 40], float) * 0.55 * lichen
        streak = np.clip(rnd.normal(0, 1, (1, 64)).repeat(64, 0) - 0.9, 0, 1) * (1 - yy / 63)
        img *= (1 - 0.25 * streak)[..., None]
        moss = np.clip(noise * 0.6 + (yy / 63 - 0.75) * 3, 0, 1)[..., None]
        img = img * (1 - 0.5 * moss) + np.array([84, 96, 52], float) * 0.5 * moss
        if kind == 'naga':
            arc = np.hypot((xx % 8) - 4, ((yy + 4 * ((xx // 8) % 2)) % 8) - 8)
            img[np.abs(arc - 4) < 0.8] *= 0.62
            img[np.abs(arc - 3) < 0.6] *= 1.12
    elif kind == 'laterite':
        # laterite blocks: rusty red brown, full of small dark pits
        img = np.array([146, 86, 56], float)[None, None] * (1 + 0.1 * noise[..., None])
        pits = rnd.uniform(0, 1, (64, 64))
        img[pits > 0.9] *= 0.55
        img[pits < 0.05] *= 1.18
        img[(yy % 32) < 2] *= 0.6
        img[((xx + 16 * ((yy // 32) % 2)) % 32) < 2] *= 0.6
        moss = np.clip(noise - 1.0, 0, 1)[..., None]
        img = img * (1 - 0.5 * moss) + np.array([76, 92, 46], float) * 0.5 * moss
    elif kind == 'apsara':
        # an apsara carved in the sandstone: a celestial dancer in low relief
        # (a tall crown, the face, a bare body, the arms up and bent, the
        # skirt flaring, one knee bent), a frame round her; lit from the
        # top left: the relief's upper edges light, its lower ones dark
        cx, h = 32.0, np.zeros((64, 64))

        def blob(x0, y0, rx, ry, a=0.0):
            u = (xx - x0) * np.cos(a) + (yy - y0) * np.sin(a)
            v = -(xx - x0) * np.sin(a) + (yy - y0) * np.cos(a)
            return (u / rx) ** 2 + (v / ry) ** 2 < 1
        fig = blob(cx, 13, 3.6, 4.2) | blob(cx, 6, 2.2, 4.0) | blob(cx, 2.6, 1.0, 2.6)       # face, crown, its spire
        fig |= blob(cx, 22, 4.2, 6.0) | blob(cx, 31, 5.0, 3.6)                               # the body, the hips
        fig |= blob(cx - 1, 39, 9.0, 4.0) | blob(cx + 2, 44, 7.5, 3.4)                       # the flaring skirt
        fig |= blob(cx - 5, 52, 2.0, 8.0, 0.25) | blob(cx + 6, 49, 2.0, 6.0, -0.9) | blob(cx + 3, 56, 2.0, 5.0, 0.5)
        for side in (-1, 1):                                                             # the arms
            fig |= blob(cx + side * 8, 18, 4.5, 1.4, side * -0.6) | blob(cx + side * 11, 12, 1.4, 4.2, side * 0.2)
            fig |= blob(cx + side * 12, 7.5, 2.2, 1.6)                                   # the hands, bent back
        h[fig] = 1.0
        h[(xx < 3) | (xx > 60) | (yy < 1) | (yy > 61)] = 1.0                             # the frame
        h = (h + np.roll(h, 1, 0) + np.roll(h, 1, 1) + np.roll(h, -1, 0) + np.roll(h, -1, 1)) / 5
        shade = (h - np.roll(np.roll(h, 1, 0), 1, 1)) * 1.4
        img = np.array([146, 138, 116], float)[None, None] * (0.86 + 0.14 * h[..., None]) * (1 + 0.07 * noise[..., None])
        img *= (1 + 0.45 * shade)[..., None]
        lichen = np.clip(noise - 1.2, 0, 1)[..., None]
        img = img * (1 - 0.5 * lichen) + np.array([40, 40, 36], float) * 0.5 * lichen
    elif kind == 'fig_root':
        # a strangler fig's roots: smooth grey brown bark (darker than the
        # sandstone they cling to), long streaks along
        fibre = rnd.normal(0, 1, (64, 64))
        for _ in range(5):
            fibre = (fibre + np.roll(fibre, 1, 0) + np.roll(fibre, -1, 0)) / 3
        fibre = (fibre - fibre.mean()) / fibre.std()
        img = np.array([112, 96, 76], float)[None, None] * (1 + 0.1 * fibre[..., None]) * (1 + 0.04 * noise[..., None])
        img[fibre < -1.5] *= 0.7
    elif kind == 'palm_trunk':
        # a sugar palm's trunk: dark grey, the old leaf bases' rings round it
        img = np.array([84, 78, 70], float)[None, None] * (1 + 0.08 * noise[..., None])
        ring = (yy + 2 * noise) % 9
        img[ring < 1.5] *= 0.62
        img[(ring > 1.5) & (ring < 3)] *= 1.15
    elif kind == 'matting':
        # a stilt house's walls: woven palm and bamboo matting, pale straw,
        # the strips over and under on the slant
        over = (((xx + yy) // 5 + (xx - yy) // 5) % 2 == 0)
        img = np.where(over[..., None], np.array([196, 168, 112], float), np.array([156, 128, 80], float))
        img = img * (1 + 0.06 * noise[..., None])
        img[((xx + yy) % 5 == 0) | ((xx - yy) % 5 == 0)] *= 0.8
    elif kind == 'boat':
        # a wooden longtail boat's side: dark planks along, a painted band
        # (blue, red, white) along its top
        grain = (np.sin((yy + 2 * noise) * 2 * np.pi / 6) + 1) / 2
        img = np.array([96, 64, 40], float)[None, None] * (0.8 + 0.25 * grain[..., None]) * (1 + 0.05 * noise[..., None])
        img[::13] *= 0.6
        img[:6] = (40, 76, 150)
        img[6:9] = (186, 40, 34)
        img[9:11] = (226, 222, 210)
        img[:11] *= (1 + 0.05 * noise[:11, :, None])
    elif kind in ('pentelic', 'flute', 'doric_frieze', 'ionic', 'architrave', 'caryatid', 'inscription'):
        # Athens' Pentelic marble: white weathered to honey, a golden
        # patina in blotches, grime run down from the top, small pits
        base = np.array([212, 198, 172], float)
        img = base[None, None] * (1 + 0.035 * noise[..., None])
        patina = np.clip(noise * 0.8 + 0.2, 0, 1.4)[..., None] * 0.35
        img = img * (1 - patina) + np.array([200, 166, 118], float) * patina
        grime = np.clip(rnd.normal(0, 1, (1, 64)).repeat(64, 0) - 0.8, 0, 1) * (1 - yy / 80)
        img *= (1 - 0.22 * grime)[..., None]
        img[rnd.uniform(0, 1, (64, 64)) > 0.985] *= 0.7
        if kind == 'flute':
            # a Doric shaft's flutes: five across, shallow, meeting in sharp arrises
            s = (xx + 0.5) % 12.8 / 12.8
            img *= (0.8 + 0.28 * np.sin(np.pi * s) ** 0.6 - 0.1 * s)[..., None]
            img[(s < 0.06) | (s > 0.96)] *= 1.12
        elif kind == 'doric_frieze':
            # a triglyph (its two grooves and half grooves, the guttae
            # under it) and a weathered metope beside it
            img[:7] *= 1.08                                     # the taenia's ledge
            img[7:9] *= 0.62
            for g, w in ((0.5, 1.6), (6.5, 2.4), (12.5, 2.4), (18.5, 1.6)):
                img[11:58][np.abs(xx[11:58] - g) < w / 2] *= 0.55
            img[9:, 20:22] *= 0.6                               # the metope set back
            img[9:, 22:] *= 0.9
            img[58:60, :20] *= 0.6
            for gx in range(2, 20, 4):
                img[60:63, gx:gx + 2] *= 0.72                   # the guttae
        elif kind == 'ionic':
            # an Ionic capital's face: the two volutes rolled up at its
            # ends, the egg and dart between, the thin abacus over them;
            # lit from the top left (relief)
            h = np.zeros((64, 64))
            for cx, way in ((11.5, 1), (51.5, -1)):
                r, a = np.hypot(xx - cx, yy - 34), np.arctan2(yy - 34, xx - cx)
                coil = (r - way * 2.2 * (a + np.pi) / np.pi) % 4.4
                h[(r < 13) & (coil < 2.2)] = 1.0
                h[r < 2.2] = 1.2                                # the eye
            h[4:12, :] = 0.8                                    # the channel joining them
            h[:4, :] = 1.2                                      # the abacus
            egg = (np.hypot(((xx - 32) % 9 - 4.5) / 3.2, (yy - 22) / 6) < 1) & (yy > 15) & (np.abs(xx - 31.5) < 16)
            h[egg] = 1.0
            h = (h + np.roll(h, 1, 0) + np.roll(h, 1, 1)) / 3
            shade = (h - np.roll(np.roll(h, 1, 0), 1, 1)) * 1.3
            img *= (0.82 + 0.18 * h[..., None])
            img *= (1 + 0.45 * shade)[..., None]
        elif kind == 'architrave':
            # an Ionic architrave's three fasciae, each over the one below
            # it, a moulding of beads along its top
            img[:6] *= 1.06
            img[6:8] *= 0.6
            img[1:5][(xx[1:5] % 4) < 2] *= 0.8
            for top in (8, 26, 44):
                img[top:top + 2] *= 1.12
                img[top + 16:top + 18] *= 0.7
        elif kind == 'caryatid':
            # a caryatid's peplos: deep vertical folds down the skirt, a
            # slanting fold across the breast, the braided hair on top
            folds = np.sin(xx * 2 * np.pi / 6.4 + 0.6 * np.sin(yy / 9)) * np.clip((yy - 16) / 20, 0, 1)
            img *= (1 + 0.16 * folds)[..., None]
            img[10:18] *= (1 + 0.1 * np.sin((xx[10:18] + 2 * yy[10:18]) * 2 * np.pi / 7))[..., None]
            braid = (yy < 7) & (((xx + 2 * yy) % 5) < 2)
            img[braid] *= 0.8
            img[:2] *= 0.85
        elif kind == 'inscription':
            # a decree cut in the stone in Attic capitals (the Council and
            # the People decided...), a moulding along its top
            img[:5] *= 1.08
            img[5:7] *= 0.62
            strokes = {
                'E': [(0, 0, 0, 1), (0, 0, 1, 0), (0, .5, .8, .5), (0, 1, 1, 1)], 'D': [(0, 1, .5, 0), (.5, 0, 1, 1), (0, 1, 1, 1)],
                'O': 'ring', 'X': [(0, 0, 1, 0), (.15, .5, .85, .5), (0, 1, 1, 1)], 'N': [(0, 1, 0, 0), (0, 0, 1, 1), (1, 1, 1, 0)],
                'T': [(0, 0, 1, 0), (.5, 0, .5, 1)], 'H': [(0, 0, 0, 1), (1, 0, 1, 1), (0, .5, 1, .5)], 'I': [(.5, 0, .5, 1)],
                'B': [(0, 0, 0, 1), (0, 0, .8, .25), (.8, .25, 0, .5), (0, .5, 1, .75), (1, .75, 0, 1)],
                'Y': [(0, 0, .5, .5), (1, 0, .5, .5), (.5, .5, .5, 1)], 'L': [(0, 1, .5, 0), (.5, 0, 1, 1)],
                'K': [(0, 0, 0, 1), (1, 0, 0, .5), (0, .5, 1, 1)], 'A': [(0, 1, .5, 0), (.5, 0, 1, 1), (.2, .6, .8, .6)],
                'W': [(0, 1, .3, 1), (.7, 1, 1, 1), (.3, 1, 0, .4), (0, .4, .5, 0), (.5, 0, 1, .4), (1, .4, .7, 1)],
                'M': [(0, 1, 0, 0), (0, 0, .5, .6), (.5, .6, 1, 0), (1, 0, 1, 1)],
                'S': [(1, 0, 0, 0), (0, 0, .5, .5), (.5, .5, 0, 1), (0, 1, 1, 1)],
                'P': [(0, 1, 0, 0), (0, 0, 1, 0), (1, 0, 1, 1)], 'Q': 'theta', 'R': [(0, 1, 0, 0), (0, 0, 1, .25), (1, .25, 0, .5)]}
            # Greek capitals by Latin stand-ins: D delta, X xi, L lambda,
            # W omega, P pi, Q theta, R rho, Y upsilon, H eta
            lines = ['EDOXENTHI', 'BOYLHIKAI', 'TWIDHMWI', 'AQHNAIWN', 'PRYTANES', 'EPESTATE', 'SWKRATHS']
            if RESERVED:                    # the add-on's: the verb whole, ἐπεστάτει ("presided")
                lines[5] = 'EPESTATEI'
            cut = np.zeros((64, 64), bool)
            for row, text in enumerate(lines):
                y0 = 10 + 7.6 * row
                for col, ch in enumerate(text):
                    x0 = 3.5 + 6.5 * col
                    st = strokes[ch]
                    if st in ('ring', 'theta'):
                        r = np.hypot((xx - x0 - 2) / 2.2, (yy - y0 - 2.5) / 2.7)
                        cut |= np.abs(r - 1) < 0.28
                        if st == 'theta':
                            cut |= (np.abs(yy - y0 - 2.5) < 0.5) & (np.abs(xx - x0 - 2) < 0.8)
                        continue
                    for ax, ay, bx, by in st:
                        p0, p1 = np.array([x0 + 4 * ax, y0 + 5 * ay]), np.array([x0 + 4 * bx, y0 + 5 * by])
                        d = p1 - p0
                        s = np.clip(((xx - p0[0]) * d[0] + (yy - p0[1]) * d[1]) / max(1e-6, d @ d), 0, 1)
                        cut |= np.hypot(xx - p0[0] - s * d[0], yy - p0[1] - s * d[1]) < 0.55
            img[cut] *= 0.5
            img[np.roll(cut, 1, 0) & ~cut] *= 1.1
    elif kind == 'odeon':
        # the Odeon's wall: brown poros blocks in courses, two tiers of
        # round-headed arches, their openings dark
        img = np.array([170, 132, 96], float)[None, None] * (1 + 0.08 * noise[..., None])
        img[(yy % 8) < 1] *= 0.72
        img[((xx + 8 * ((yy // 8) % 2)) % 16) < 1] *= 0.78
        for y0, y1 in ((4, 30), (36, 62)):
            for cx in (16, 48):
                w = 7.5
                opening = (np.abs(xx - cx) < w) & (yy < y1) & ((yy > y0 + w) | (np.hypot(xx - cx, yy - y0 - w) < w))
                ring = (np.abs(xx - cx) < w + 2.5) & (yy < y1) & ((yy > y0 + w) | (np.hypot(xx - cx, yy - y0 - w) < w + 2.5))
                img[ring & ~opening] *= 1.12
                img[opening] = np.array([46, 34, 28], float) * (1 + 0.1 * noise[opening][:, None])
                img[opening & (xx < cx - w + 2)] *= 0.7
        img[34:36] *= 1.15                                      # the cornice between the tiers
        img[36:37] *= 0.7
    elif kind == 'olive_bark':
        # an old olive's bark: grey brown, twisted, deep fissures spiralling
        # up it, pale lichen in spots
        twist = np.sin((xx + 0.45 * yy + 4 * noise) * 2 * np.pi / 13)
        img = np.array([112, 102, 86], float)[None, None] * (0.85 + 0.2 * twist[..., None]) * (1 + 0.06 * noise[..., None])
        img[twist < -0.75] *= 0.55
        img[rnd.uniform(0, 1, (64, 64)) > 0.975] = (170, 168, 140)
    elif kind in ('redrock', 'redrock_dark', 'redrock_top', 'petroglyph'):
        # Nowhere Desert's Entrada sandstone: brick red to orange, its
        # bedding in faint bands across, cross-bedding slanting through
        # them, black desert varnish run down from the top in streaks, small
        # pits; the top weathered paler, sandy; "petroglyph" a varnished face
        # with figures pecked through to the paler rock (two bighorn sheep, a
        # man with his arms up, a spiral, a snake, rings, a hand)
        base = np.array([176, 88, 54], float) if kind != 'redrock_top' else np.array([194, 112, 70], float)
        bed = (np.sin((yy + 2.5 * noise) * 2 * np.pi / 15) * 0.06
               + np.sin((yy * 0.8 + xx * 0.35 + 3 * noise) * 2 * np.pi / 7) * 0.03)
        img = base[None, None] * (1 + bed[..., None] + 0.06 * noise[..., None])
        if kind != 'redrock_top':
            streak = np.clip(rnd.normal(0, 1, (1, 64)).repeat(64, 0) * 1.2 - 0.5, 0, 1) * np.clip(1.1 - yy / 64, 0, 1)
            img = img * (1 - 0.5 * streak[..., None]) + np.array([52, 30, 26], float) * 0.5 * streak[..., None]
            img[(yy % 15 == 0) & (noise > -0.3)] *= 0.84
        else:
            img *= (1 + 0.08 * rnd.normal(0, 1, (64, 64)))[..., None]
        img[rnd.uniform(0, 1, (64, 64)) > 0.98] *= 0.72
        if kind == 'redrock_dark':
            img *= 0.74
        if kind == 'petroglyph':
            varnish = np.clip(0.75 + 0.2 * noise, 0, 1)[..., None]
            edge = np.clip(np.minimum.reduce([xx, yy, 63 - xx, 63 - yy]) / 5, 0, 1)[..., None]
            img = img * (1 - 0.7 * varnish * edge) + np.array([66, 40, 34], float) * 0.7 * varnish * edge
            cut = np.zeros((64, 64), bool)

            def line(x0, y0, x1, y1, w=0.8):
                d = np.array([x1 - x0, y1 - y0], float)
                t = np.clip(((xx - x0) * d[0] + (yy - y0) * d[1]) / max(1e-6, d @ d), 0, 1)
                return np.hypot(xx - x0 - t * d[0], yy - y0 - t * d[1]) < w
            for bx, by in ((8, 12), (24, 9)):                 # two bighorn sheep, their horns curled back
                cut |= (np.abs(xx - bx - 4) < 4.5) & (np.abs(yy - by - 2) < 1.6)
                for lx in (bx + 0.5, bx + 2, bx + 6, bx + 7.5):
                    cut |= line(lx, by + 3, lx, by + 7, 0.6)
                cut |= line(bx + 8.5, by + 1, bx + 10, by - 2, 0.7)
                cut |= np.abs(np.hypot(xx - bx - 8, yy - by + 2) - 2.2) < 0.6
            # a man, his arms up, his fingers spread
            for seg in ((44, 8, 44, 20, 0.9), (44, 20, 41, 26), (44, 20, 47, 26), (44, 12, 39, 10), (39, 10, 38, 6),
                        (44, 12, 49, 10), (49, 10, 50, 6)):
                cut |= line(*seg)
            cut |= np.hypot(xx - 44, yy - 6) < 1.8
            for fx in (-1, 0, 1):
                cut |= line(38, 6, 38 + 1.2 * fx, 3.5, 0.45) | line(50, 6, 50 + 1.2 * fx, 3.5, 0.45)
            # a spiral, a snake, rings, a hand
            r, a = np.hypot(xx - 14, yy - 40), np.arctan2(yy - 40, xx - 14)
            cut |= (r < 10) & (np.abs(((r - 1.4 * (a + np.pi)) % 4.4) - 2.2) < 0.7)
            for k in range(28):
                cut |= np.hypot(xx - 30 - k * 1.1, yy - 36 - 3 * np.sin(k * 0.55)) < 0.8
            cut |= np.hypot(xx - 61, yy - 36) < 1.3
            for rr in (2.5, 5.0):
                cut |= np.abs(np.hypot(xx - 52, yy - 52) - rr) < 0.6
            cut |= np.hypot(xx - 52, yy - 52) < 0.9
            cut |= np.hypot((xx - 31) / 2.6, (yy - 54) / 3.0) < 1
            for fa in np.linspace(-0.9, 0.9, 4):
                cut |= line(31, 52, 31 + 5.5 * np.sin(fa), 52 - 5.5 * np.cos(fa), 0.7)
            cut |= line(31, 55, 34.5, 52, 0.7)
            peck = cut & (rnd.uniform(0, 1, (64, 64)) > 0.12)
            img[peck] = np.array([196, 132, 92], float) * (1 + 0.08 * noise[peck][:, None])
    elif kind == 'bone':
        # bleached bone: chalky white, faint yellow, hairline cracks, brown
        # in its hollows
        img = np.array([214, 206, 184], float)[None, None] * (1 + 0.05 * noise[..., None])
        img[np.abs(np.sin((xx * 0.6 + yy + 6 * noise) * 2 * np.pi / 23)) < 0.04] *= 0.7
        img *= (1 - 0.18 * np.clip(-noise - 0.8, 0, 1))[..., None]
    elif kind == 'cholla':
        # a cholla's joints: grey green stems thick with pale spines, the
        # joints' necks every 16 rows
        img = np.array([104, 128, 84], float)[None, None] * (1 + 0.08 * noise[..., None])
        img *= (0.75 + 0.3 * np.sin(np.pi * ((yy % 16) + 0.5) / 16) ** 0.5)[..., None]
        spines = rnd.uniform(0, 1, (64, 64)) > 0.7
        img[spines] = np.array([214, 206, 168], float) * (1 + 0.08 * noise[spines][:, None])
        img[(yy % 16) == 0] *= 0.6
    elif kind in ('pear_pad', 'barbwire', 'skull'):
        out = []
        for var in range(2 if kind == 'pear_pad' else 1):
            rgba = np.zeros((64, 64, 4))
            if kind == 'pear_pad':
                # a prickly pear's pad: a flat green oval, darker at its
                # rim, the areoles in rows with their tufts of yellow
                # spines; the second with yellow flowers and a red fruit on
                # its top edge
                top = 14 if var else 3
                ry, rx = (63 - top) / 2, 20.0
                cy = top + ry
                d = np.hypot((xx - 31.5) / rx, (yy - cy) / ry)
                pad = d < 1
                g = np.array([70, 118, 58], float) * (1.05 - 0.25 * d[..., None]) * (1 + 0.06 * noise[..., None])
                rgba[pad, :3] = g[pad]
                rgba[pad & (d > 0.88), :3] *= 0.75
                rgba[pad, 3] = 255
                areole = pad & ((((xx - 31.5) + 3 * ((yy // 7) % 2)) % 6) < 1.2) & ((yy % 7) < 1.2)
                rgba[areole, :3] = (216, 200, 120)
                if var:
                    for fx in (18, 31, 44):
                        fy = cy - ry * np.sqrt(max(0.0, 1 - ((fx - 31.5) / rx) ** 2))
                        cup = np.hypot((xx - fx) / 5.5, (yy - fy + 4) / 5) < 1
                        if fx == 31:
                            rgba[cup, :3] = np.array([176, 40, 66], float) * (1 + 0.08 * noise[cup][:, None])
                        else:
                            rgba[cup, :3] = np.array([236, 204, 70], float) * (1 + 0.08 * noise[cup][:, None])
                            rgba[cup & (np.hypot(xx - fx, yy - fy + 4) < 1.6), :3] = (150, 120, 40)
                        rgba[cup, 3] = 255
            elif kind == 'barbwire':
                # three strands of barbed wire, each two wires twisted, a
                # barb every 12 columns, sagging a little between the posts
                for y0 in (10, 32, 54):
                    yc = y0 + 2.5 * np.sin(np.pi * xx / 63)
                    for ph in (0, np.pi):
                        rgba[np.abs(yy - yc - 0.9 * np.sin(xx * 0.9 + ph)) < 0.7] = (92, 90, 96, 255)
                    for bx in range(5, 64, 12):
                        barb = (np.abs(np.abs(xx - bx) - np.abs(yy - yc)) < 0.7) & (np.abs(xx - bx) < 2.6)
                        rgba[barb] = (110, 106, 110, 255)
            else:
                # a steer's skull seen from above and in front: the long
                # face to the nose, the eye sockets dark, the horns curving
                # out and up from the poll
                face = (np.abs(xx - 31.5) < 9 - 4 * np.clip((yy - 20) / 40, 0, 1)) & (yy > 14) & (yy < 60)
                face |= np.hypot((xx - 31.5) / 11, (yy - 18) / 7) < 1
                rgba[face, :3] = np.array([214, 206, 184], float) * (1 + 0.05 * noise[face][:, None])
                rgba[face, 3] = 255
                for sx in (-1, 1):
                    rgba[np.hypot((xx - 31.5 - sx * 7) / 3.2, (yy - 26) / 2.6) < 1] = (40, 30, 26, 255)
                    t = np.linspace(0, 1, 40)
                    for x, y, r in zip(31.5 + sx * (9 + 18 * t), 15 - 10 * t ** 2, 2.8 - 1.8 * t):
                        rgba[np.hypot(xx - x, yy - y) < r] = (198, 184, 150, 255)
                rgba[(np.abs(xx - 31.5) < 3) & (yy > 52) & (yy < 58)] = (60, 46, 38, 255)
            out.append(vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1]))
        return out
    elif kind in ('fig_leaves', 'net', 'reeds'):
        rgba = np.zeros((64, 64, 4))
        if kind == 'fig_leaves':
            # a fig's crown: many dark glossy oval leaves on nothing
            for _ in range(170):
                x0, y0 = rnd.uniform(4, 60), rnd.uniform(4, 60)
                a, length, width = rnd.uniform(0, np.pi), rnd.uniform(3.5, 5.5), rnd.uniform(1.8, 2.6)
                if np.hypot(x0 - 32, y0 - 32) > 28 + 3 * np.sin(5 * np.arctan2(y0 - 32, x0 - 32)): continue
                u = (xx - x0) * np.cos(a) + (yy - y0) * np.sin(a)
                v = -(xx - x0) * np.sin(a) + (yy - y0) * np.cos(a)
                leaf = (u / length) ** 2 + (v / width) ** 2 < 1
                green = np.array([[40, 78, 32], [52, 96, 38], [70, 112, 44], [34, 64, 28]][int(rnd.integers(4))], float)
                rgba[leaf, :3] = green * (1 + 0.15 * (v[leaf] / width))[:, None]
                rgba[leaf & (np.abs(v) < 0.5), :3] = green * 1.3
                rgba[leaf, 3] = 255
        elif kind == 'net':
            # a fishing net: knotted diamond mesh, dark twine, floats on top
            mesh = (((xx + yy) % 8) < 1) | (((xx - yy) % 8) < 1)
            rgba[mesh] = (70, 66, 56, 255)
            rgba[:3] = (60, 54, 44, 255)
            for x0 in range(4, 64, 12):
                rgba[1:5, x0:x0 + 4] = (220, 196, 80, 255)
        else:
            # reeds at the water's edge: tall narrow blades, green, yellowing
            # toward their tips, a few feathery heads
            for _ in range(26):
                x0, top, lean = rnd.uniform(4, 60), rnd.uniform(2, 26), rnd.uniform(-0.25, 0.25)
                t = np.clip((yy - top) / max(1, 63 - top), 0, 1)
                centre = x0 + lean * (63 - yy)
                blade = (yy >= top) & (np.abs(xx - centre) < 0.5 + 1.1 * t)
                green = np.array([[70, 118, 44], [92, 134, 52], [112, 140, 60], [60, 100, 40]][int(rnd.integers(4))], float)
                dry = (1 - t[blade])[:, None] * 0.6
                rgba[blade, :3] = green[None] * (1 - dry) + np.array([196, 180, 108]) * dry
                rgba[blade, 3] = 255
                if rnd.uniform() < 0.3:
                    head = np.hypot((xx - (x0 + lean * (63 - top))) / 2.2, (yy - top + 3) / 4.5) < 1
                    rgba[head] = (176, 150, 120, 255)
        return [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    else:
        r = np.hypot(xx - 31.5, yy - 31.5)
        img = np.array([92, 56, 36], float)[None, None] * (1 + 0.15 * noise[..., None])
        img[(r > 24) & (r < 28)] *= 0.6                 # the rolled rim
        img[np.hypot(xx - 44, yy - 22) < 4] = (40, 30, 26)   # the bung
    return [vr.picture(*sixteen(np.clip(img, 0, 255))[::-1])]


def textures(cfg: dict, vram: np.ndarray, rec: dict, vr: Vram, arc=None) -> dict:
    """The scenery's pictures by name: "materials" (material()) from the
    panorama picture (hd()), "borrow" (borrowed()) from a
    Tekken 3 stage, "painted" (painted()) and "sand", Tekken 2's first
    floor tile. Returns {name: [(x, y, (palette x, y)), ...]}, a material's
    pictures in turn."""
    out = {}
    src = Image.fromarray(np.clip(cfg['_picture'], 0, 255).astype(np.uint8)) if cfg.get('_picture') is not None         else Image.open(hd(cfg['panorama'])).convert('RGB') if cfg.get('panorama') else None
    for name, spec in cfg.get('materials', {}).items():
        own = Image.open(hd(spec['source'])).convert('RGB') if spec.get('source') else src
        out[name] = [vr.picture(*sixteen(p)[::-1]) for p in material(own, spec)]
    for name, spec in cfg.get('borrow', {}).items():
        out[name] = borrowed(spec, arc, vr)
    for name in cfg.get('painted', []):
        out[name] = painted(name, vr)
    if RESERVED and 'jangseung_body' in out:        # the add-on's: her post's name is her own
        out['jangseung_body_f'] = painted('jangseung_body_f', vr)
    colours, idx = sixteen(t2_stage.floor_tile(vram, rec['floor'][0]).astype(float))
    out['sand'] = [vr.picture(idx, colours)]
    if cfg.get('scenery', {}).get('tree') or cfg.get('scenery', {}).get('junipers'):
        out['bark'] = bark(vr)
    for name, other in cfg.get('alias', {}).items():
        # "alias": a picture under another name (boulder() takes "cliff")
        out[name] = out[other]
    if cfg.get('scenery', {}).get('tufts') or cfg.get('grass'):
        blades = {'straw': STRAW, 'green': GREEN}[cfg.get('tuft_blades', 'straw')]
        out['tuft'] = [vr.picture(*see_through(tuft_side(70 + k, blades))[::-1]) for k in range(cfg.get('tuft_pictures', 3))]
    return out


def lit(corners: list, light: dict, inside, height_dark: float = 0.0) -> list:
    """Corner colours of a face: an ambient light and the low sun (its
    "azimuth", degrees from +z, and "elevation"), each a colour of 0..2
    multipliers, the face turned away from the point inside (what it is
    the outside of); corners low down darker by height_dark (the canyon's
    shade); "haze" {"start", "end" cm from the fight, "colour", "amount"}:
    the far corners tinted toward the sky (they stand in the background);
    "points" [[x, y, z, reach, [r, g, b]], ...]: candles, each lighting
    what is within reach of it (cm), the more the nearer and the more the
    face turns to it (Church Sanctuary: only they light the dark church)."""
    p = np.array(corners, float)
    n = np.cross(p[1] - p[0], p[2] - p[0])
    if np.linalg.norm(n) < 1e-6: n = np.cross(p[3] - p[1], p[2] - p[1])
    n = n / max(1e-9, np.linalg.norm(n))
    if n @ (p.mean(0) - np.array(inside, float)) < 0: n = -n
    az, el = np.radians(light['azimuth']), np.radians(light['elevation'])
    to_sun = np.array([np.sin(az) * np.cos(el), -np.sin(el), np.cos(az) * np.cos(el)])
    facing = max(0.0, float(n @ to_sun))
    out = []
    for c in p:
        rgb = np.array(light['ambient']) + np.array(light['sun']) * facing
        if height_dark and c[1] > 0:
            rgb = rgb * max(0.35, 1 - height_dark * c[1] / 1000)
        if light.get('haze'):
            # the air between: far rock takes the sky's tint
            hz = light['haze']
            t = np.clip((np.hypot(c[0], c[2]) - hz['start']) / (hz['end'] - hz['start']), 0, 1) * hz.get('amount', 1.0)
            rgb = rgb * (1 - t) + rgb * np.array(hz['colour']) * t
        for x, y, z, reach, colour in light.get('points', ()):
            d = np.array([x, y, z], float) - c
            dist = max(1.0, float(np.linalg.norm(d)))
            if dist < reach:
                towards = max(0.0, float(n @ d) / dist)
                rgb = rgb + np.array(colour) * (1 - dist / reach) ** 2 * (0.3 + 0.7 * towards)
        out.append(tuple(int(v) for v in np.clip(np.round(rgb * 0x80), 0, 0xFF)))
    return out


def face(corners, tex, light, inside, rows=(0, 63), dark=0.0, var=0, cols=(0, 63)) -> dict:
    """var: which of the material's pictures (in turn round a ring, along
    a wall: the rock runs on); rows, cols: the part of it."""
    return dict(corners=[tuple(int(round(v)) for v in c) for c in corners], tex=tex, rows=rows, cols=cols, var=var,
                colors=lit(corners, light, inside, dark))


def grid_faces(a, b, c, d, tex, light, inside, size=260, dark=0.0, var=0) -> list:
    """The quad a b / c d cut into pieces of about size, each the whole
    picture (the GPU does not repeat a texture), the material's pictures in
    turn across from var."""
    a, b, c, d = map(np.array, (a, b, c, d))
    n = max(1, round(max(np.linalg.norm(b - a), np.linalg.norm(d - c)) / size))
    m = max(1, round(max(np.linalg.norm(c - a), np.linalg.norm(d - b)) / size))
    at = lambda s, t: (a + (b - a) * s) * (1 - t) + (c + (d - c) * s) * t
    return [face([at(i / n, j / m), at((i + 1) / n, j / m), at(i / n, (j + 1) / m), at((i + 1) / n, (j + 1) / m)],
                 tex, light, inside, dark=dark, var=var + i) for i in range(n) for j in range(m)]


def ring_point(r, y, a):
    return (r * np.sin(a), y, r * np.cos(a))


def rim(spec: dict, light: dict) -> list:
    """The mesa's edge round the fight: a low lip of rock from the ground,
    its top leaning back, then the drop (its face looks away)."""
    rnd = np.random.default_rng(spec.get('seed', 7))
    k_n, r0 = spec.get('segments', 72), spec['radius']
    a = 2 * np.pi * np.arange(k_n) / k_n
    def swell(waves):
        s = sum(np.sin(w * a + rnd.uniform(0, 2 * np.pi)) / (1 + i) for i, w in enumerate(waves))
        return (s - s.min()) / max(1e-9, s.max() - s.min())
    lo, hi = spec['height']
    h_top = lo + (hi - lo) * (0.75 * swell((3, 7, 12)) + 0.25 * rnd.uniform(0, 1, k_n))
    h_lip = h_top * (0.4 + 0.25 * swell((5, 9)))
    r_in = r0 + spec.get('jitter', 60) * (2 * swell((4, 11)) - 1)
    out = []
    for k in range(k_n):
        j = (k + 1) % k_n
        ak, aj = a[k], a[j] if j else 2 * np.pi
        prof = lambda i: [(r_in[i], 0), (r_in[i] + 50, -h_lip[i]), (r_in[i] + 160, -h_top[i]),
                          (r_in[i] + 300, -h_top[i] * 0.5), (r_in[i] + 360, spec.get('drop', 150))]
        pk, pj = prof(k), prof(j)
        texs = spec['faces']
        for s in range(4):
            top = [ring_point(pk[s + 1][0], pk[s + 1][1], ak), ring_point(pj[s + 1][0], pj[s + 1][1], aj)]
            foot = [ring_point(pk[s][0], pk[s][1], ak), ring_point(pj[s][0], pj[s][1], aj)]
            # the rim's faces look at the fight: their outside is the middle's side
            far = ring_point(r0 * 3, -h_top[k] * 3, ak)
            out += grid_faces(top[0], top[1], foot[0], foot[1], texs[s], light, far, size=spec.get('piece', 200), var=k)
    return out


def butte(spec: dict, light: dict, rnd) -> list:
    """A butte rising from the canyon: a many-sided stack of rock bands,
    narrowing, with a ledge, its flat top in sand."""
    ang, dist, height, radius = spec['at']
    a0 = np.radians(ang)
    cx, cz = dist * np.sin(a0), dist * np.cos(a0)
    sides = spec.get('sides', 9)
    levels = spec.get('levels', [[150, 1.08], [0, 1.0], [-0.45, 0.92], [-0.5, 0.8], [-0.9, 0.78], [-1.0, 0.7]])
    turn = rnd.uniform(0, 2 * np.pi)
    wob = rnd.uniform(0.85, 1.15, sides)
    def at(level, s):
        y, scale = levels[level]
        y = y * height if -2 < y <= 0 else y
        q = turn + 2 * np.pi * s / sides
        r = radius * scale * wob[s % sides]
        return (cx + r * np.sin(q), y, cz + r * np.cos(q))
    out = []
    for lv in range(len(levels) - 1):
        tex = spec['bands'][min(lv, len(spec['bands']) - 1)]
        for s in range(sides):
            top, foot = [at(lv + 1, s), at(lv + 1, s + 1)], [at(lv, s), at(lv, s + 1)]
            out += grid_faces(top[1], top[0], foot[1], foot[0], tex, light, (cx, top[0][1], cz),
                              size=spec.get('piece', 400), dark=light.get('foot_dark', 0.2), var=2 * s)
    cap = [at(len(levels) - 1, s) for s in range(sides)]
    centre = (cx, levels[-1][0] * height - 20, cz)
    for s in range(sides):
        out.append(face([cap[s], cap[(s + 1) % sides], centre, centre], 'sand', light, (cx, 1e6, cz)))
    return out


def boulder(spec, light, rnd) -> list:
    """[degrees, distance, size] and, optional, the part of it sunk into
    the ground (no straight foot on show, nothing standing on air)."""
    ang, dist, size = spec[:3]
    sink = spec[3] * size if len(spec) > 3 else 0
    a0 = np.radians(ang)
    cx, cz = dist * np.sin(a0), dist * np.cos(a0)
    sides = 5
    turn = rnd.uniform(0, 2 * np.pi)
    base = [(cx + size * rnd.uniform(0.8, 1.2) * np.sin(turn + 2 * np.pi * s / sides), 0,
             cz + size * rnd.uniform(0.8, 1.2) * np.cos(turn + 2 * np.pi * s / sides)) for s in range(sides)]
    mid = [(cx + 0.7 * (x - cx), -size * rnd.uniform(0.5, 0.8), cz + 0.7 * (z - cz)) for x, _, z in base]
    top = (cx, -size * rnd.uniform(0.9, 1.2), cz)
    if sink:
        base = [(x, y + sink, z) for x, y, z in base]
        mid = [(x, y + sink, z) for x, y, z in mid]
        top = (top[0], top[1] + sink, top[2])
    out = []
    for s in range(sides):
        t = (s + 1) % sides
        out.append(face([mid[t], mid[s], base[t], base[s]], 'cliff_dark', light, (cx, -size * 0.4, cz), var=s))
        out.append(face([top, top, mid[t], mid[s]], 'cliff', light, (cx, -size * 0.4, cz), var=s))
    return out


def mushroom(spec: dict, light: dict, rnd) -> list:
    """A mushroom rock ("mushroom": at [degrees, distance], "height", the
    cap's "radius"): a worn neck of rock, the cap flaring over it in
    stacked slabs, each ring of it its own ragged outline; "profile" [y as
    a part of the height (below the ground in cm when over 0), radius as a
    part of the cap's, material] from the foot up. The hoodoos and ledges
    ("stacks") are built so too: rock in ragged layers."""
    ang, dist = spec['at']
    h, rc = spec['height'], spec['radius']
    a0 = np.radians(ang)
    cx, cz = dist * np.sin(a0), dist * np.cos(a0)
    sides = spec.get('sides', 14)
    prof = spec['profile']
    turn = rnd.uniform(0, 2 * np.pi)
    rings = []
    for y, r, _ in prof:
        y = y * h if -2 < y <= 0 else y
        wob = rnd.uniform(0.86, 1.12, sides)
        rings.append([(cx + rc * r * wob[s] * np.sin(turn + 2 * np.pi * s / sides), y,
                       cz + rc * r * wob[s] * np.cos(turn + 2 * np.pi * s / sides)) for s in range(sides)])
    out = []
    for lv in range(len(rings) - 1):
        tex = prof[lv + 1][2]
        mid_y = (rings[lv][0][1] + rings[lv + 1][0][1]) / 2
        for s in range(sides):
            t = (s + 1) % sides
            top, foot = [rings[lv + 1][s], rings[lv + 1][t]], [rings[lv][s], rings[lv][t]]
            out += grid_faces(top[1], top[0], foot[1], foot[0], tex, light, (cx, mid_y, cz),
                              size=spec.get('piece', 320), dark=light.get('foot_dark', 0.2), var=s + lv)
    centre = (cx, rings[-1][0][1] - spec.get('dome', 40), cz)
    for s in range(sides):
        out.append(face([rings[-1][s], rings[-1][(s + 1) % sides], centre, centre], prof[-1][2], light, (cx, 1e6, cz), var=s))
    return out


def tufts(spec: dict, rnd) -> list:
    """Dry tufts of grass strewn on the ground ("tufts": "count" of them
    between "near" and "far" cm, outside the square round the fight the
    engine leaves out, "keep_out" circles [degrees, distance, radius] where
    rock stands; "on_grass": on the floor only where its cell is grass,
    the "grass" cells build() finds): two pictures crossed, seen from
    anywhere, a little smaller or bigger each; lit flat, darker at the
    root; "tex" other pictures than the tufts' (Morning Fields' "flowers":
    a clump each, its kind at random)."""
    out, light = [], spec.get('colour', [1.0, 0.92, 0.8])
    clear, n = spec.get('clear', 860), 0
    texs = spec.get('tex', 'tuft')
    texs = [texs] if isinstance(texs, str) else texs
    while n < spec['count']:
        r = spec['near'] + (spec['far'] - spec['near']) * rnd.uniform(0, 1) ** 1.6
        a = rnd.uniform(0, 2 * np.pi)
        x, z = r * np.sin(a), r * np.cos(a)
        if max(abs(x), abs(z)) < clear: continue
        if any(np.hypot(x - d * np.sin(np.radians(g)), z - d * np.cos(np.radians(g))) < k for g, d, k in spec.get('keep_out', [])):
            continue
        if spec.get('on_grass') and max(abs(x), abs(z)) < FLOOR_HALF:
            c, r = int((x + FLOOR_HALF) // (FLOOR_HALF / 5)), int((z + FLOOR_HALF) // (FLOOR_HALF / 5))
            if not spec['grass'][10 * r + c]:
                continue
        n += 1
        size = spec.get('size', 70) * rnd.uniform(0.7, 1.3)
        spin, var = rnd.uniform(0, np.pi), int(rnd.integers(8))
        tex = texs[int(rnd.integers(len(texs)))]
        for k in range(2):
            q = spin + k * np.pi / 2
            dx, dz = np.cos(q) * size / 2, np.sin(q) * size / 2
            corners = [(x - dx, -size, z - dz), (x + dx, -size, z + dz), (x - dx, 0, z - dz), (x + dx, 0, z + dz)]
            top = tuple(int(min(255, 0x80 * v)) for v in light)
            root = tuple(int(min(255, 0x80 * v * 0.7)) for v in light)
            for c in (corners, [corners[1], corners[0], corners[3], corners[2]]):
                out.append(dict(corners=[tuple(int(round(v)) for v in p) for p in c], tex=tex, rows=(0, 63),
                                var=var, colors=[top, top, root, root], filler=(id(spec), n)))
    return out


FLOOR_HALF = 1250                    # cm from the middle to the floor's edge (10 x 10 cells of 2.5 m)


def grass_cells(vram: np.ndarray, rec: dict) -> list[bool]:
    """Which of the floor's 100 cells are grass (green more than red and
    blue): "tufts" "on_grass" grow only on them, not on the paving."""
    out = []
    for e in rec['floor']:
        r, g, b = t2_stage.floor_tile(vram, e).reshape(-1, 3).astype(float).mean(0)
        out.append(bool(g > 1.1 * r and g > b))
    return out


def ragged(rnd, sides: int, rough: float) -> np.ndarray:
    """A ragged outline: radius multipliers round a ring, two waves and
    some noise (rock, not a turned shape)."""
    q = 2 * np.pi * np.arange(sides) / sides
    w = (0.08 * np.sin(2 * q + rnd.uniform(0, 7)) + 0.06 * np.sin(3 * q + rnd.uniform(0, 7))
         + 0.04 * np.sin(5 * q + rnd.uniform(0, 7)))
    return 1 + w * rough / 0.18 + rnd.uniform(-rough, rough, sides) * 0.5


def layered_rock(spec: dict, light: dict, rnd) -> list:
    """Rock of stacked slabs, as the wind leaves sandstone ("rocks": at
    [degrees, distance], "height" and "radius" in cm, the rest parts of
    them): a "stem" of ragged rings [y up, radius, dx, dz] from the
    ground ("stem_shade": in the shadow of what is over it), then "slabs" [bottom y up, thickness, radius across, radius
    along, dx, dz, side material, top material], each a lens of its own
    outline, turned and tipped at random (a 9th: its sides), the pictures running on round
    it and its side one picture tall from foot to top."""
    ang, dist = spec['at']
    h, rc = spec['height'], spec['radius']
    a0 = np.radians(ang)
    cx, cz = dist * np.sin(a0), dist * np.cos(a0)
    sides = spec.get('sides', 16)
    rough = spec.get('rough', 0.12)
    q = 2 * np.pi * np.arange(sides) / sides
    out = []

    def band(lo, hi, tex, inside, rows, var0, shade=1.0):
        for s in range(len(lo)):
            t = (s + 1) % len(lo)
            f = face([hi[t], hi[s], lo[t], lo[s]], tex, light, inside, rows=rows,
                     dark=light.get('foot_dark', 0.2), var=var0 + s)
            f['colors'] = [tuple(int(v * shade) for v in c) for c in f['colors']]
            out.append(f)

    stem = spec.get('stem', [])
    rings = []
    for y, r, dx, dz in stem:
        k = ragged(rnd, sides, rough)
        yy = -y * h if y < 2 else y
        rings.append([(cx + dx * rc + r * rc * k[s] * np.sin(q[s]), yy, cz + dz * rc + r * rc * k[s] * np.cos(q[s]))
                      for s in range(sides)])
    for i in range(len(rings) - 1):
        n = len(rings) - 1
        band(rings[i], rings[i + 1], spec.get('stem_material', 'stem'), (cx, rings[i][0][1], cz),
             (int(63 * i / n), int(63 * (i + 1) / n)), 3 * i, spec.get('stem_shade', 1.0))
    for j, slab in enumerate(spec['slabs']):
        yb, th, rx, rz, dx, dz, side, top = slab[:8]
        n = slab[8] if len(slab) > 8 else sides
        q = 2 * np.pi * np.arange(n) / n
        turn = rnd.uniform(0, 2 * np.pi)
        tip = rnd.uniform(-1, 1, 2) * spec.get('tip', 0.05)
        k = ragged(rnd, n, rough)
        ox, oz = cx + dx * rc, cz + dz * rc
        prof = [(0.0, 0.84), (0.35, 1.0), (0.7, 0.95), (1.0, 0.78)]
        lens = []
        for f, sc in prof:
            ring = []
            for s in range(n):
                lx = rx * rc * sc * k[s] * np.sin(q[s])
                lz = rz * rc * sc * k[s] * np.cos(q[s])
                x = ox + lx * np.cos(turn) + lz * np.sin(turn)
                z = oz - lx * np.sin(turn) + lz * np.cos(turn)
                y = -(yb + th * f) * h + (tip[0] * (x - ox) + tip[1] * (z - oz))
                ring.append((x, y, z))
            lens.append(ring)
        mid = (ox, -(yb + th / 2) * h, oz)
        for i in range(3):
            band(lens[i], lens[i + 1], side, mid, (21 * i, 21 * i + 21), 5 * j)
        under = (ox, -yb * h + 10, oz)
        cap = (ox, -(yb + th) * h - spec.get('dome', 0.02) * h, oz)
        for s in range(n):
            t = (s + 1) % n
            out.append(face([lens[0][t], lens[0][s], under, under], side, light, (ox, -1e6, oz), var=s))
            out.append(face([lens[3][s], lens[3][t], cap, cap], top, light, (ox, 1e6, oz), var=s))
    return out


def grass_model(spec: dict, pictures: list, reloc) -> bytes:
    """One square of "grass" (see grass_sections()) as an OBJS model."""
    rnd = np.random.default_rng(spec.get('seed', 21))
    colour = spec.get('colour', [1.0, 0.92, 0.8])
    top = tuple(int(min(255, 0x80 * v)) for v in colour)
    root = tuple(int(min(255, 0x80 * v * 0.7)) for v in colour)
    period = spec['period']
    if spec['count'] > 32:
        raise SystemExit(f'{spec["count"]} tufts: 32 at most in a square')
    verts, prims = [], []
    for _ in range(spec['count']):
        x, z = rnd.uniform(0, period, 2)
        size = spec.get('size', 50) * rnd.uniform(0.7, 1.3)
        spin = rnd.uniform(0, np.pi)
        px, py, (cx, cy) = pictures[int(rnd.integers(len(pictures)))]
        u0, v0 = (px & 63) * 4, py & 0xFF
        tpage = (px >> 6) | (py & 0x100) >> 4 | (py & 0x200) << 2
        for k in range(2):
            q = spin + k * np.pi / 2
            dx, dz = np.cos(q) * size / 2, np.sin(q) * size / 2
            corners = [(x - dx, -size, z - dz), (x + dx, -size, z + dz), (x - dx, 0, z - dz), (x + dx, 0, z + dz)]
            ids = []
            for c in corners:
                verts.append(tuple(int(round(v)) for v in c)); ids.append(len(verts) - 1)
            uv = [(u0, v0), (u0 + 63, v0), (u0, v0 + 63), (u0 + 63, v0 + 63)]
            cols = (top, top, root, root)
            prims.append(decode.Prim(GT4, 'GT4', tuple(ids), tuple(uv), cy << 6 | cx >> 4, tpage, cols))
            prims.append(decode.Prim(GT4, 'GT4', (ids[1], ids[0], ids[3], ids[2]), (uv[1], uv[0], uv[3], uv[2]),
                                     cy << 6 | cx >> 4, tpage, cols))
    model = struct.pack('<2H', len(verts), len(prims))
    model += b''.join(struct.pack('<4h', *v, 0) for v in verts)
    return model + b''.join(pack.prim_bytes(reloc.prim(p)) for p in prims)


def grass_sections(spec: dict, pictures: list, reloc, arc) -> list:
    """Dry tufts round the fight ("grass"), where the tiles leave nothing,
    fixed on the ground: one square of "period" cm with "count" tufts (32
    at most: a model's 256 vertices) of "size" cm, two pictures crossed,
    both ways round, built in the world's frame from the square's corner;
    the same model four times (OBJS, ROTT), OBJA 201 and GRAS (s16 period,
    world units): the runtime lays the four on the squares of the world's
    grid round the fighters (grass_place()). Drawn into tables at main
    rank "rank" (980: over the floor's 1021..982, under the fighters)."""
    period = spec['period']
    model = grass_model(spec, pictures, reloc)
    objs = struct.pack('<I', 4) + model * 4
    objs += struct.pack('<I', 4) + b''.join(struct.pack('<2B3H', m, 0, spec.get('rank', 980), 128, 0) for m in range(4))
    return [(b'OBJS', objs), (b'ROTT', struct.pack('<4096I', *arc.word(scene.ROT_TABLE, 4096, 'I'))),
            (b'OBJA', struct.pack('<2H', GRASS_STAGE, 0)), (b'GRAS', struct.pack('<2h', period * 10, 0))]


def bark(vr: Vram):
    """A dead tree's bark: sun-bleached grey-brown wood in long twisted
    fibres, darker cracks between them."""
    rnd = np.random.default_rng(12)
    fibre = rnd.normal(0, 1, (64, 64))
    for _ in range(6):                          # long along v, a little slanted
        fibre = (fibre + np.roll(fibre, 1, 0) + np.roll(np.roll(fibre, 1, 0), 1, 1) + np.roll(fibre, -1, 0)) / 4
    fibre = (fibre - fibre.mean()) / fibre.std()
    crack = np.clip(-fibre - 1.1, 0, None)
    base = np.array([122, 104, 86], float)
    img = base[None, None] * (1 + 0.16 * fibre[..., None]) - 55 * crack[..., None]
    colours, idx = sixteen(np.clip(img, 0, 255))
    return [vr.picture(idx, colours)]


def tube(path: list, radii: list, sides: int, tex: str, light: dict, turn: float = 0.0) -> list:
    """A branch (or the trunk): rings of sides corners round the points of
    path, radius radii, each quad both ways round (a twig is seen from
    anywhere)."""
    path = [np.array(p, float) for p in path]
    rings = []
    for k, (c, r) in enumerate(zip(path, radii)):
        d = path[min(k + 1, len(path) - 1)] - path[max(k - 1, 0)]
        d = d / max(1e-9, np.linalg.norm(d))
        u = np.cross(d, [0, 0, 1.0] if abs(d[2]) < 0.9 else [1.0, 0, 0]); u /= np.linalg.norm(u)
        w = np.cross(d, u)
        rings.append([c + r * (np.cos(turn * k + 2 * np.pi * s / sides) * u + np.sin(turn * k + 2 * np.pi * s / sides) * w)
                      for s in range(sides)])
    out = []
    for k in range(len(rings) - 1):
        mid = (path[k] + path[k + 1]) / 2
        for s in range(sides):
            t = (s + 1) % sides
            q = [rings[k + 1][s], rings[k + 1][t], rings[k][s], rings[k][t]]
            out.append(face(q, tex, light, mid, var=s))
            out.append(face([q[1], q[0], q[3], q[2]], tex, light, mid, var=s))
    return out


def dead_tree(spec: dict, light: dict) -> tuple[list, tuple]:
    """A dead juniper on the mesa ("tree": at [degrees, distance], height of
    its fork): a short trunk twisting in an S on spread roots, bare
    branches leaning out and forking, one rising higher than the rest: the
    eagle's perch, against the sky. Returns its faces and that perch (cm)."""
    rnd = np.random.default_rng(spec.get('seed', 4))
    ang, dist = spec['at']
    h = spec.get('height', 230)
    a0 = np.radians(ang)
    cx, cz = dist * np.sin(a0), dist * np.cos(a0)
    levels = 8
    trunk = [(cx + 34 * np.sin(1.6 * np.pi * t) * t, -h * t, cz + 22 * np.sin(2.6 * t)) for t in np.linspace(0, 1, levels)]
    radii = [27 * (1 - t) + 14 * t for t in np.linspace(0, 1, levels)]
    radii[0] = 36
    out = tube(trunk, radii, 5, 'bark', light, turn=0.35)
    for k in range(3):                          # roots spreading on the ground
        q = a0 + 2 * np.pi * k / 3 + rnd.uniform(-0.4, 0.4)
        reach = rnd.uniform(80, 120)
        root = [(cx + 8 * np.sin(q), -38, cz + 8 * np.cos(q)), (cx + reach * 0.5 * np.sin(q), -12, cz + reach * 0.5 * np.cos(q)),
                (cx + reach * np.sin(q), -2, cz + reach * np.cos(q))]
        out += tube(root, [15, 8, 2], 3, 'bark', light)
    top = np.array(trunk[-1])
    def branch(start, azimuth, rise, length, radius, subs):
        pts, rs, p = [start], [radius], np.array(start, float)
        for seg in range(3):
            azimuth += rnd.uniform(-0.3, 0.3); rise += rnd.uniform(-0.12, 0.12)
            d = np.array([np.cos(rise) * np.sin(azimuth), -np.sin(rise), np.cos(rise) * np.cos(azimuth)])
            p = p + d * length * rnd.uniform(0.85, 1.15)
            pts.append(tuple(p)); rs.append(radius * (1 - 0.28 * (seg + 1)))
        faces = tube(pts, rs, 3, 'bark', light)
        for k in range(1, 1 + subs):
            faces += branch(pts[k], azimuth + (1 if k % 2 else -1) * rnd.uniform(0.7, 1.1), rise + 0.1,
                            length * 0.6, rs[k] * 0.55, 0)[0]
        return faces, pts
    n = spec.get('branches', 4)
    for k in range(n):
        az = a0 + 2 * np.pi * (k + 0.5) / n + rnd.uniform(-0.3, 0.3)
        faces, _ = branch(tuple(top), az, rnd.uniform(0.3, 0.6), spec.get('reach', 85), 9, 2)
        out += faces
    # the high one, across the camera's view (the camera stands on the
    # stage's -z side, looking at +z, the fight along x): the eagle on it
    # shows against the sky with the branch under it
    faces, pts = branch(tuple(top), np.radians(spec.get('perch_azimuth', 90)), spec.get('perch_rise', 0.75), 75, 11, 0)
    out += faces
    perch = np.array(pts[2])
    return out, (perch[0], perch[1], perch[2])


# --- the quay: bollards, oil drums, a railing ------------------------------------

def polar(at) -> tuple[float, float]:
    """[degrees, distance] (from +z, cm) to (x, z)."""
    a = np.radians(at[0])
    return at[1] * np.sin(a), at[1] * np.cos(a)


def turned(profile: list, sides: int, cx: float, cz: float, tex: str, light: dict, turn: float = 0.0,
           cap: bool = True, wrap: int = 1, var: int = 0) -> list:
    """A turned shape: rings [y, radius, picture rows (v0, v1)] from the foot
    up, sides faces round each band, the picture going round wrap times
    (each face its part of the columns); a cap of wedges on the top ring."""
    q = turn + 2 * np.pi * np.arange(sides + 1) / sides
    rings = [[(cx + r * np.sin(a), y, cz + r * np.cos(a)) for a in q] for y, r, _ in profile]
    out, per = [], sides // wrap
    for lv in range(len(rings) - 1):
        rows = profile[lv + 1][2]
        name = tex if isinstance(tex, str) else tex[lv]
        mid = (cx, (rings[lv][0][1] + rings[lv + 1][0][1]) / 2, cz)
        for s in range(sides):
            c0 = round(63 * (s % per) / per)
            c1 = round(63 * (s % per + 1) / per)
            out.append(face([rings[lv + 1][s + 1], rings[lv + 1][s], rings[lv][s + 1], rings[lv][s]], name, light,
                            mid, rows=rows, cols=(c0, c1), var=var))
    if cap:
        top, centre = rings[-1], (cx, rings[-1][0][1], cz)
        for s in range(sides):
            out.append(face([top[s], top[s + 1], centre, centre], cap if isinstance(cap, str) else
                            (tex if isinstance(tex, str) else tex[-1]), light, (cx, 1e6, cz)))
    return out


def bollard(spec: dict, at, light: dict, rnd) -> list:
    """A mooring bollard ("bollard": "height", "radius" in cm): a base
    plate, a waisted iron post, its mushroom head; with "rope" a line
    wound round its neck, running off over the quay's edge (away from the
    fight) and down to the water."""
    cx, cz = polar(at)
    h, r = spec.get('height', 65), spec.get('radius', 20)
    prof = [(0, 1.5 * r, (0, 7)), (-0.08 * h, 1.5 * r, (0, 7)), (-0.08 * h, r, (8, 15)), (-0.5 * h, 0.85 * r, (16, 39)),
            (-0.82 * h, r, (40, 55)), (-0.86 * h, 1.4 * r, (56, 59)), (-h, 1.35 * r, (60, 63))]
    out = turned(prof, 8, cx, cz, 'iron', light, turn=rnd.uniform(0, np.pi), cap='iron')
    if len(at) > 2:                 # the rope's way off, degrees from +z
        neck = -0.62 * h
        loop = [(cx + 1.05 * r * np.sin(a), neck + 4 * np.sin(2 * a), cz + 1.05 * r * np.cos(a))
                for a in np.linspace(0, 2 * np.pi, 7)]
        out += tube(loop, [3] * len(loop), 3, 'rope', light)
        a = np.radians(at[0] + at[2])
        far = (cx + 260 * np.sin(a), 60, cz + 260 * np.cos(a))
        line = [loop[0], (cx + 90 * np.sin(a), neck + 18, cz + 90 * np.cos(a)),
                (cx + 180 * np.sin(a), -2, cz + 180 * np.cos(a)), far]
        out += tube(line, [3, 3, 3, 3], 3, 'rope', light)
    return out


def drum(spec: dict, at, light: dict, rnd) -> list:
    """An oil drum ("drum": "height", "radius"): Paul's stage's ribbed rust
    round it twice, the upper picture above the lower; at [degrees,
    distance, level]: level 1 stands on another."""
    cx, cz = polar(at)
    h, r = spec.get('height', 110), spec.get('radius', 36)
    y0 = -h * (at[2] if len(at) > 2 else 0)
    prof = [(y0, r, (0, 63)), (y0 - h / 2, r, (0, 63)), (y0 - h, r, (0, 63))]
    out = turned(prof, 8, cx, cz, ['drum_low', 'drum_high'], light, turn=rnd.uniform(0, np.pi), cap='lid', wrap=2)
    return out


def railing(spec: dict, run, light: dict) -> list:
    """A low railing ("railing": "height", "post" spacing) along run [x0,
    z0, x1, z1] (cm): posts, a top rail and a middle one, round tubes."""
    h, step = spec.get('height', 100), spec.get('post', 220)
    a, b = np.array(run[:2], float), np.array(run[2:], float)
    n = max(1, round(np.linalg.norm(b - a) / step))
    out = []
    pts = [a + (b - a) * k / n for k in range(n + 1)]
    for p in pts:
        out += tube([(p[0], 0, p[1]), (p[0], -h - 3, p[1])], [4, 4], 4, 'yellow', light)
    for y in (-h, -0.55 * h):
        out += tube([(p[0], y, p[1]) for p in pts], [3] * len(pts), 3, 'yellow', light)
    return out


# --- London's ruins: the tower's stones, its clockwork, a phone box -------------

def wound(q: list, inside) -> list:
    """A tile's quad turned to show its side away from inside (the tiles
    show the side cross(b - a, c - a) points away from)."""
    p = [np.array(c, float) for c in q]
    n = np.cross(p[1] - p[0], p[2] - p[0])
    if np.linalg.norm(n) < 1e-6: n = np.cross(p[3] - p[1], p[2] - p[1])
    return q if n @ (np.mean(p, 0) - np.array(inside, float)) < 0 else [q[1], q[0], q[3], q[2]]


def posed(points, lean: float, yaw: float, at, y0: float = 0.0) -> list:
    """Points built round their foot (y down) leant by lean degrees (about
    x), turned yaw degrees, set at [degrees, distance] y0 cm down."""
    a, b = np.radians(lean), np.radians(yaw)
    cx, cz = polar(at)
    out = []
    for x, y, z in points:
        y, z = y * np.cos(a) - z * np.sin(a), y * np.sin(a) + z * np.cos(a)
        x, z = x * np.cos(b) + z * np.sin(b), -x * np.sin(b) + z * np.cos(b)
        out.append((cx + x, y + y0, cz + z))
    return out


def box_faces(c: list, texs, light: dict) -> list:
    """The six faces of a box of corners c (0..3 its foot, 4..7 its top, the
    same way round): texs [sides..., top, bottom], the sides in turn."""
    inside = np.mean(np.array(c, float), 0)
    sides = texs[:-2]
    out = []
    for k in range(4):
        j = (k + 1) % 4
        out.append(face(wound([c[4 + k], c[4 + j], c[k], c[j]], inside), sides[k % len(sides)], light, inside, var=k))
    out.append(face(wound([c[4], c[5], c[7], c[6]], inside), texs[-2], light, inside))
    out.append(face(wound([c[0], c[1], c[3], c[2]], inside), texs[-1], light, inside))
    return out


def above_ground(faces: list) -> list:
    """Faces cut at the ground (y 0), the part under it left out: the
    engine has no depth test, what is under the sand would show on it. A
    cut face keeps its picture where it was (its corners' own "uv"), a
    triangle a quad of its last corner twice (the engine culls by the
    first three), five corners a quad and a triangle."""
    out = []
    for f in faces:
        q = f['corners']
        if q[0] == q[1]:
            # a triangle of its first corner twice (boulder()'s caps): the
            # engine would cull it always; its corners round, the last twice
            r0, r1 = f['rows']; c0, c1 = f.get('cols', (0, 63))
            uv = f.get('uv', [(c0, r0), (c1, r0), (c0, r1), (c1, r1)])
            f = dict(f, corners=[q[0], q[3], q[2], q[2]], uv=[uv[0], uv[3], uv[2], uv[2]],
                     colors=[f['colors'][i] for i in (0, 3, 2, 2)])
            q = f['corners']
        if all(c[1] <= 0 for c in q):
            out.append(f); continue
        if all(c[1] >= 0 for c in q):
            continue
        r0, r1 = f['rows']; c0, c1 = f.get('cols', (0, 63))
        uv = f.get('uv', [(c0, r0), (c1, r0), (c0, r1), (c1, r1)])
        ring = [(np.array(q[i], float), np.array(uv[i], float), np.array(f['colors'][i], float)) for i in (0, 1, 3, 2)]
        kept = []
        for i in range(4):
            (p, t, c), (p2, t2, c2) = ring[i], ring[(i + 1) % 4]
            if p[1] <= 0: kept.append((p, t, c))
            if (p[1] <= 0) != (p2[1] <= 0):
                k = p[1] / (p[1] - p2[1])
                kept.append((p + (p2 - p) * k, t + (t2 - t) * k, c + (c2 - c) * k))
        polys = [kept[:4], [kept[0], kept[3], kept[4]]] if len(kept) == 5 else [kept]
        for poly in polys:
            if len(poly) == 3: poly = [poly[0], poly[1], poly[2], poly[2]]
            if len(poly) != 4: continue
            a, b, d, c = poly                    # round the ring: a b d c
            g = dict(f)
            g['corners'] = [tuple(int(round(v)) for v in x[0]) for x in (a, b, c, d)]
            g['uv'] = [tuple(int(round(np.clip(v, 0, 63))) for v in x[1]) for x in (a, b, c, d)]
            g['colors'] = [tuple(int(round(v)) for v in x[2]) for x in (a, b, c, d)]
            out.append(g)
    return out


def strewn(spec: dict, rnd) -> list:
    """[degrees, distance, size] of "count" pieces between "near" and "far"
    cm, "size" [min, max], out of the "keep_out" circles [degrees, distance,
    radius]."""
    out = []
    while len(out) < spec['count']:
        at = [rnd.uniform(0, 360), rnd.uniform(spec['near'], spec['far'])]
        x, z = polar(at)
        if any(np.hypot(x - polar(k)[0], z - polar(k)[1]) < k[2] for k in spec.get('keep_out', [])):
            continue
        out.append(at + [rnd.uniform(*spec['size'])])
    return out


def block(at, size: float, light: dict, rnd, rock: bool = False) -> list:
    """A fallen stone of the tower ("blocks"): a dressed limestone block,
    half again as long as tall, tipped over and sunk in the sand; rock: a
    chunk of the cliff's red rock instead ("rubble")."""
    w, d, h = size * rnd.uniform(1.1, 1.7), size * rnd.uniform(0.7, 1.1), size * rnd.uniform(0.6, 0.9)
    foot = [(-w / 2, 0, -d / 2), (w / 2, 0, -d / 2), (w / 2, 0, d / 2), (-w / 2, 0, d / 2)]
    c = foot + [(x, -h, z) for x, _, z in foot]
    c = posed(c, rnd.uniform(-22, 22), rnd.uniform(0, 360), at, h * rnd.uniform(0.2, 0.45))
    if rock:
        return above_ground(box_faces(c, ['cliff_dark', 'cliff', 'cliff_dark', 'cliff', 'cliff', 'cliff'], light))
    sides = ['carved', 'stone', 'stone', 'stone'] if rnd.uniform() < 0.5 else ['stone']
    return above_ground(box_faces(c, sides + ['stone', 'stone'], light))


def gear(spec: dict, light: dict) -> list:
    """A wheel of the clockwork ("gears": at, "radius", "teeth", "lean" from
    upright, "yaw", "sink" a part of it under the sand): a toothed iron disc
    gone to rust, its hub standing out each side."""
    r, n, t = spec['radius'], spec['teeth'], spec.get('thick', 0.16 * spec['radius'])
    outline = []
    for k in range(n):
        for f, rr in ((0.0, 0.84), (0.12, 1.0), (0.38, 1.0), (0.5, 0.84), (0.75, 0.84)):
            a = 2 * np.pi * (k + f) / n
            outline.append((rr * r * np.sin(a), -rr * r * np.cos(a)))
    lean = spec.get('lean', 0)
    drop = r * (1 - 2 * spec.get('sink', 0.3))
    def put(pts):
        return posed([(x, y - drop, z) for x, y, z in pts], lean, spec.get('yaw', 0), spec['at'])
    front = put([(x, y, -t / 2) for x, y in outline])
    back = put([(x, y, t / 2) for x, y in outline])
    centre_f, centre_b = put([(0, 0, -t / 2), (0, 0, t / 2)])
    middle = np.mean([centre_f, centre_b], 0)
    out = []
    m = len(outline)
    for k in range(m):
        j = (k + 1) % m
        out.append(face(wound([front[k], front[j], back[k], back[j]], middle), 'rust', light, middle, var=k))
        for ring, c in ((front, centre_f), (back, centre_b)):
            out.append(face(wound([ring[k], ring[j], c, c], middle), 'rust', light, middle))
    for side in (-1, 1):
        rings = [[(rr * np.sin(2 * np.pi * s / 6), -rr * np.cos(2 * np.pi * s / 6), side * z) for s in range(6)]
                 for z, rr in ((t / 2, 0.28 * r), (t / 2 + 0.3 * t, 0.24 * r))]
        a_, b_ = put(rings[0]), put(rings[1])
        tip = put([(0, 0, side * (t / 2 + 0.3 * t + 2))])[0]
        for s in range(6):
            q = (s + 1) % 6
            out.append(face(wound([b_[s], b_[q], a_[s], a_[q]], middle), 'iron', light, middle))
            out.append(face(wound([b_[s], b_[q], tip, tip], middle), 'iron', light, middle))
    return above_ground(out)


def phone_box(spec: dict, light: dict) -> list:
    """A red telephone kiosk ("phone_box": at, "lean", "yaw", "sink" cm),
    92 cm square and 250 tall, its roof a low hip, half buried and leaning."""
    w, h, roof = 46, 250, 22
    foot = [(-w, 0, -w), (w, 0, -w), (w, 0, w), (-w, 0, w)]
    c = foot + [(x, -h, z) for x, _, z in foot]
    c = posed(c, spec.get('lean', 0), spec.get('yaw', 0), spec['at'], spec.get('sink', 0))
    out = box_faces(c, ['phone', 'phone', 'phone', 'phone', 'phone_roof', 'phone_roof'], light)
    out.pop(4)                                          # the top: the roof stands on it
    tip = posed([(0, -h - roof, 0)], spec.get('lean', 0), spec.get('yaw', 0), spec['at'], spec.get('sink', 0))[0]
    inside = np.mean(np.array(c, float), 0)
    for k in range(4):
        j = (k + 1) % 4
        out.append(face(wound([c[4 + k], c[4 + j], tip, tip], inside), 'phone_roof', light, inside))
    return above_ground(out)


# --- the launch pad: the hangar floor's kit -------------------------------------

def crate(spec, light: dict) -> list:
    """A flight case or a small container ("crates": [degrees, distance,
    width, depth, height, yaw, kind, lift]): an upright box, kind "crate"
    or "container" its sides' picture; lift cm: it stands on another."""
    at, (w, d, h), yaw = spec[:2], spec[2:5], spec[5] if len(spec) > 5 else 0
    kind = spec[6] if len(spec) > 6 else 'crate'
    foot = [(-w / 2, 0, -d / 2), (w / 2, 0, -d / 2), (w / 2, 0, d / 2), (-w / 2, 0, d / 2)]
    c = posed(foot + [(x, -h, z) for x, _, z in foot], 0, yaw, at, -(spec[7] if len(spec) > 7 else 0))
    top = 'crate_top' if kind == 'crate' else 'container'
    out = box_faces(c, [kind, top, top], light)
    out.pop()                                           # its bottom: never seen
    return out


def barrel(spec: dict, at, light: dict, rnd) -> list:
    """A steel drum standing ("barrels": [degrees, distance, "blue" | "red"],
    "barrel": "height", "radius")."""
    cx, cz = polar(at)
    h, r = spec.get('height', 88), spec.get('radius', 29)
    paint = 'barrel_' + (at[2] if len(at) > 2 else 'blue')
    prof = [(0, r, (0, 31)), (-h / 2, r, (0, 31)), (-h, r, (32, 63))]
    return turned(prof, 10, cx, cz, paint, light, turn=rnd.uniform(0, np.pi), cap='barrel_lid')


def spool(spec: dict, light: dict) -> list:
    """A wooden cable drum ("spools": at, "radius", "width", "core", "lean"
    90 lying on a flange, 0 standing on its rims, "yaw"): two plywood
    flanges, black cable wound round the core between them."""
    R, W, rc, n = spec['radius'], spec.get('width', 0.9 * spec['radius']), spec.get('core', 0.62 * spec['radius']), 14
    t = 0.08 * R
    lean = spec.get('lean', 0)
    drop = R if lean < 45 else 0                      # its foot: a rim on the floor, or its lower flange
    def put(pts):
        return posed([(x, y - drop, z + (W / 2 + t if lean >= 45 else 0)) for x, y, z in pts], lean, spec.get('yaw', 0), spec['at'])
    ring = lambda rr, z: put([(rr * np.sin(2 * np.pi * k / n), -rr * np.cos(2 * np.pi * k / n), z) for k in range(n)])
    out = []
    for z0 in (-W / 2 - t, W / 2):                    # the flanges
        a, b = ring(R, z0), ring(R, z0 + t)
        ca, cb = put([(0, 0, z0), (0, 0, z0 + t)])
        mid = np.mean([ca, cb], 0)
        for k in range(n):
            j = (k + 1) % n
            out.append(face(wound([a[k], a[j], b[k], b[j]], mid), 'spool_rim', light, mid, var=k))
            for rg, c in ((a, ca), (b, cb)):
                u = lambda p: (int(31.5 + 31 * np.sin(2 * np.pi * p / n)), int(31.5 - 31 * np.cos(2 * np.pi * p / n)))
                f = face(wound([rg[k], rg[j], c, c], mid), 'spool', light, mid)
                f['uv'] = [u(k), u(j), (32, 32), (32, 32)] if f['corners'][0] == tuple(int(round(v)) for v in rg[k]) \
                    else [u(j), u(k), (32, 32), (32, 32)]
                out.append(f)
    a, b = ring(rc, -W / 2), ring(rc, W / 2)
    axis = np.mean(put([(0, 0, 0)]), 0)
    for k in range(n):
        j = (k + 1) % n
        out.append(face(wound([a[k], a[j], b[k], b[j]], axis), 'cable', light, axis, var=k))
    return above_ground(out)


def cone(at, light: dict, rnd) -> list:
    """A traffic cone ("cones": [degrees, distance]), 70 cm."""
    cx, cz = polar(at)
    prof = [(0, 21, (56, 63)), (-3, 21, (56, 63)), (-3, 13, (52, 55)), (-70, 3, (0, 51))]
    return turned(prof, 8, cx, cz, 'cone', light, turn=rnd.uniform(0, np.pi), cap='cone')


GLOW = {'azimuth': 0, 'elevation': 90, 'ambient': [1.25, 1.25, 1.25], 'sun': [0, 0, 0]}
LAMP_ROWS = 8                        # the beacon's picture: its lamp's rows, the rest its light on the floor
POOL = 800                           # cm across the floor the light picture covers


@dataclasses.dataclass
class AddedPrim(decode.Prim):
    """A GT4 laid on what is under it, added (semi-transparent, the texture
    page's blending mode 1: B + F): the beacon's light on the floor."""

    @property
    def code(self):
        return 0x3E


def beacon(spec: dict, at, light: dict) -> list:
    """A warning beacon ("beacons": [degrees, distance], "beacon":
    "height"): a grey post, a squat housing, the turning orange lamp on it
    and its light swept round the floor (the picture's two parts, animated:
    beacon_frames()), the lamp lit by itself."""
    cx, cz = polar(at)
    h = spec.get('height', 230)
    out = turned([(0, 14, (0, 63)), (-6, 14, (0, 63)), (-6, 5, (0, 63)), (-h, 5, (0, 63)),
                  (-h, 18, (0, 63)), (-h - 10, 18, (0, 63))], 8, cx, cz, 'pole', light, cap=False)
    # the lamp: face s round it takes columns 8s..8s+7, the sweep's column c
    # is the side at 2 pi c / 64 from +z (turned(), no turn)
    out += turned([(-h - 10, 16, (0, LAMP_ROWS - 1)), (-h - 44, 15, (0, LAMP_ROWS - 1)), (-h - 50, 9, (0, 1))], 8, cx, cz,
                  'beacon', GLOW, cap='pole')
    # its light: a square on the floor round the foot, u along x, v along z
    x0, x1, z0, z1 = cx - POOL / 2, cx + POOL / 2, cz - POOL / 2, cz + POOL / 2
    corners = [(x0, -1, z0), (x1, -1, z0), (x0, -1, z1), (x1, -1, z1)]
    uv = [(0, LAMP_ROWS), (63, LAMP_ROWS), (0, 63), (63, 63)]
    if wound(corners, (cx, 1000, cz)) is not corners:
        corners, uv = [corners[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
    out.append(dict(corners=[tuple(int(round(v)) for v in c) for c in corners], tex='beacon', rows=(LAMP_ROWS, 63),
                    var=0, uv=uv, colors=[(spec.get('pool', 0x80),) * 3] * 4, added=True, under=True))
    return out


def steam(spec: dict, at) -> list:
    """A plume of steam from a vent in the floor ("steams": [degrees,
    distance], "steam": "width", "height"): two upright squares crossed,
    each seen from both sides, the animated steam picture (steam_picture())
    added on what is behind them."""
    cx, cz = polar(at)
    w, h = spec.get('width', 170), spec.get('height', 300)
    out = []
    for a in (np.radians(at[0] + 45), np.radians(at[0] - 45)):
        dx, dz = w / 2 * np.cos(a), -w / 2 * np.sin(a)
        q = [(cx - dx, -h, cz - dz), (cx + dx, -h, cz + dz), (cx - dx, 0, cz - dz), (cx + dx, 0, cz + dz)]
        uv = [(0, 0), (63, 0), (0, 63), (63, 63)]
        for order in ((0, 1, 2, 3), (1, 0, 3, 2)):
            out.append(dict(corners=[tuple(int(round(v)) for v in q[i]) for i in order], tex='steam', rows=(0, 63),
                            var=0, uv=[uv[i] for i in order], colors=[(spec.get('level', 0x80),) * 3] * 4, added=True))
    return out


def steam_picture(f: int) -> np.ndarray:
    """Frame f of 16 of the steam: soft puffs rising through a plume narrow
    at the vent (the bottom row) and spreading, thinning as it climbs; the
    puffs' field repeats every 32 rows and climbs 2 rows a frame, so the 16
    frames go round. Indices 0 (nothing) .. 15."""
    rnd = np.random.default_rng(61)
    field = np.zeros((32, 64))
    yy, xx = np.mgrid[0:32, 0:64]
    for _ in range(9):                      # puffs, wrapping round the field
        y, x, r = rnd.uniform(0, 32), rnd.uniform(10, 54), rnd.uniform(6, 11)
        for dy in (-32, 0, 32):
            field += np.exp(-(((yy - y - dy) / r) ** 2 + ((xx - x) / (1.3 * r)) ** 2))
    field = (field / field.max()) ** 1.6
    img = field[(np.arange(64) + 2 * f) % 32]
    v = np.arange(64)[:, None] / 63.0                # 0 the top, 1 the vent
    x = (np.arange(64)[None] - 31.5) / 32
    spread = 0.3 + 0.75 * (1 - v)
    plume = np.exp(-(x / spread) ** 2) * np.clip(1.1 - (1 - v), 0, 1) ** 1.5 * np.clip(v * 6, 0, 1)
    k = np.clip(img * plume * 1.2, 0, 1)
    return np.where(k < 0.06, 0, np.clip(np.round(k * 15), 1, 15)).astype(np.uint16)


def beacon_frames(vr: Vram, with_steam: bool = False) -> tuple[tuple, list, tuple | None]:
    """The beacon's picture as it turns: 16 frames (the runtime copies the
    next onto the shown one every ANIM_PERIOD frames: ANIM), all on one
    palette. Its top LAMP_ROWS rows the lamp, a bright orange sweep going
    round once; under them its beam on the floor (seen from above, x to the
    right, z down the picture), pointing where the sweep is. Colour 0 is
    nothing, the others carry the semi-transparency bit (only the floor's
    light is drawn semi-transparent: added). with_steam: the steam's frames
    too (steam_picture()), each beside the lamp's. Returns the shown
    picture's place, the frames' and the shown steam picture's."""
    ramp = np.array([(0, 0, 0), (60, 18, 2), (100, 32, 4), (140, 48, 8), (180, 66, 10), (215, 88, 14), (238, 114, 24),
                     (250, 142, 40), (255, 170, 64), (255, 196, 96), (255, 220, 150), (255, 242, 210),
                     (60, 60, 64), (100, 100, 106), (30, 30, 34), (1, 1, 1)], float)
    colours = [0] + [0x8000 | int(c) for c in bgr555(ramp[1:])]
    xx = np.arange(64)
    gh = 64 - LAMP_ROWS
    gx, gz = np.meshgrid((np.arange(64) - 31.5) / 32, (np.arange(gh) - (gh - 1) / 2) / (gh / 2))
    r, ang = np.hypot(gx, gz), np.arctan2(gx, gz)
    frames = []
    for f in range(16):
        phase = 64 * f / 16
        d = np.minimum(np.abs(xx - phase), 64 - np.abs(xx - phase))
        glow = 0.12 + 0.88 * np.exp(-(d / 9) ** 2)
        idx = np.zeros((64, 64), np.uint16)
        idx[:LAMP_ROWS] = np.repeat(np.clip(1 + np.round(glow * 10), 1, 11)[None], LAMP_ROWS, 0)
        idx[0] = idx[LAMP_ROWS - 1] = 12                # the cage's rings
        idx[:LAMP_ROWS, ::16] = 13                     # its bars
        a = 2 * np.pi * phase / 64                    # where the sweep faces (column c: 2 pi c / 64 from +z)
        da = np.angle(np.exp(1j * (ang - a)))
        beam = np.exp(-(da / 0.38) ** 2) * np.clip(1 - r, 0, 1) ** 1.3
        light = np.clip(beam + 0.35 * np.exp(-(r / 0.16) ** 2), 0, 1)
        idx[LAMP_ROWS:] = np.where(light < 0.06, 0, np.clip(1 + np.floor(light * 8), 1, 7))
        frames.append(idx)
    if not with_steam:
        shown = vr.picture(frames[0], colours)
        return shown, [vr.frame(i, colours) for i in frames], None
    # with the steam: each frame's lamp and steam side by side in the VRAM
    # (one ANIM rectangle 32 halfwords wide), the steam's own palette, all
    # its colours added
    grey = [0] + [0x8000 | int(bgr555(np.array([84, 88, 98], float) * (k / 15) ** 1.2)) for k in range(1, 16)]
    def pair(lamp, puff):
        if vr.slot % 64 == 63:
            vr.slot += 1                                # not across the row's end
        return vr.picture(lamp, colours), vr.picture(puff, grey)
    shown = pair(frames[0], steam_picture(0))
    pairs = [pair(frames[f], steam_picture(f)) for f in range(16)]
    return shown[0], [p[0] for p in pairs], shown[1]


def fog(spec: dict, at) -> list:
    """A sheet of mist standing on the floor ("fogs": [degrees, distance],
    "fog": "width", "height", "level"), facing the fight's middle, seen from
    both sides, the drifting mist picture (fog_frames()) added on what is
    behind it."""
    cx, cz = polar(at)
    w, h = spec.get('width', 500), spec.get('height', 150)
    a = np.radians(at[0])
    dx, dz = w / 2 * np.cos(a), -w / 2 * np.sin(a)
    q = [(cx - dx, -h, cz - dz), (cx + dx, -h, cz + dz), (cx - dx, 0, cz - dz), (cx + dx, 0, cz + dz)]
    uv = [(0, 0), (63, 0), (0, 63), (63, 63)]
    return [dict(corners=[tuple(int(round(v)) for v in q[i]) for i in order], tex='fog', rows=(0, 63), var=0,
                 uv=[uv[i] for i in order], colors=[(spec.get('level', 0x80),) * 3] * 4, added=True)
            for order in ((0, 1, 2, 3), (1, 0, 3, 2))]


def fog_frames(vr: Vram, colour) -> tuple[tuple, list]:
    """The mist as it churns: 16 frames of 64 x 64, soft wisps going over
    from one field into the next and the next (three, round: frame 16 is
    frame 0 again), in place, thickest low and fading to nothing at the
    top, at the very foot (no line where the sheet meets the floor) and
    toward both ends as a triangle (each sheet half over the next: the two
    add up to the same all along). Colour 0 is nothing, the others "colour"
    darker to brighter, added. Returns the shown picture's place and the
    frames'."""
    rnd = np.random.default_rng(23)
    yy, xx = np.mgrid[0:64, 0:64]
    fields = []
    for _ in range(3):
        field = np.zeros((64, 64))
        for _ in range(10):                 # wisps
            y, x, r = rnd.uniform(30, 56), rnd.uniform(-8, 72), rnd.uniform(8, 15)
            field += np.exp(-(((yy - y) / (0.45 * r)) ** 2 + ((xx - x) / (1.8 * r)) ** 2))
        fields.append(field / field.max())
    v = np.arange(64)[:, None] / 63.0       # 0 the top, 1 the floor
    env = np.clip(v * 1.6 - 0.25, 0, 1) ** 1.4 * np.clip((1 - v) * 8, 0, 1)
    env = env * (1 - np.abs(np.arange(64) - 31.5) / 32)[None]
    colours = [0] + [0x8000 | int(bgr555(np.array(colour, float) * (k / 15) ** 1.6)) for k in range(1, 16)]
    frames = []
    for f in range(16):
        t = 3 * f / 16
        a, w = int(t), t - int(t)
        w = w * w * (3 - 2 * w)             # eased
        field = 0.35 + 0.65 * (fields[a] * (1 - w) + fields[(a + 1) % 3] * w)
        k = np.clip(field * env * 1.5, 0, 1)
        frames.append(np.where(k < 0.08, 0, np.clip(np.round(k * 15), 1, 15)).astype(np.uint16))
    shown = vr.picture(frames[0], colours)
    return shown, [vr.frame(i, colours) for i in frames]


def mirror_section(spec: dict) -> bytes:
    """MIRR (src/tekken3_arcade_stages.c): the mirror round the camera as
    T3MD (radius, angle, width, top, bottom, panels; the PS1 stage's units),
    then its style: post width, rail height, glow height, glow period
    (frames), the frame's colours across (hi, lo) and the glow's."""
    m = spec
    return (struct.pack('<6i', m['radius'], m['angle'], m['width'], m['top'], m['bottom'], m['slats'])
            + struct.pack('<4h', m.get('post', 0), m.get('rail', 0), m.get('glow_height', 0), m.get('glow_period', 0))
            + bytes(m.get('hi', [0, 0, 0]) + m.get('lo', [0, 0, 0]) + m.get('glow', [0, 0, 0]) + [0, 0, 0]))


# --- Kyoto at Sunset: stone lanterns, a torii, a bamboo fence, paper lanterns --

FIRE_ROWS, PAPER_ROWS = 16, 16       # the lamp picture: the lantern's window, a paper lantern, then its light on the floor


def lamp_frames(vr: Vram) -> tuple[tuple, list]:
    """The lanterns' picture as their flames flicker: 16 frames (ANIM), one
    palette. Rows 0..15 a stone lantern's firebox side: grey stone round a
    window, the flame's glow in it; 16..31 a paper lantern (chochin): red
    paper lit from inside, brighter in its middle, its ribs, a black word
    down its front; 32..63 the light the stone lantern throws on the floor
    (seen from above), round, added. Colour 0 nothing; 1..11 dark red to
    yellow white; 12..14 stone; 15 black."""
    ramp = np.array([(0, 0, 0), (52, 12, 4), (90, 22, 6), (128, 34, 8), (166, 50, 12), (200, 70, 16), (226, 96, 24),
                     (244, 126, 38), (252, 158, 60), (255, 192, 96), (255, 222, 150), (255, 244, 210),
                     (70, 66, 60), (108, 102, 94), (140, 134, 124), (16, 12, 12)], float)
    colours = [0] + [0x8000 | int(c) for c in bgr555(ramp[1:])]
    rnd = np.random.default_rng(71)
    # the flicker: a smooth wobble round the 16 frames, and the flame's sway
    ph = rnd.uniform(0, 2 * np.pi, 3)
    t = 2 * np.pi * np.arange(16) / 16
    level = 0.82 + 0.1 * np.sin(t + ph[0]) + 0.06 * np.sin(3 * t + ph[1]) + 0.04 * np.sin(5 * t + ph[2])
    sway = 2.0 * np.sin(2 * t + ph[1])
    yy, xx = np.mgrid[0:64, 0:64]
    frames = []
    for f in range(16):
        idx = np.zeros((64, 64), np.uint16)
        # the firebox: stone, its window cols 14..49, rows 3..13, the flame
        # low in its middle
        fire = np.full((FIRE_ROWS, 64), 13, np.uint16)
        fire[:, ::32] = 12
        fire[0] = fire[FIRE_ROWS - 1] = 12
        wy, wx = yy[:FIRE_ROWS], xx[:FIRE_ROWS]
        win = (wx >= 14) & (wx <= 49) & (wy >= 3) & (wy <= 13)
        d = np.hypot((wx - 31.5 - sway[f]) / 9, (wy - 11) / 7)
        glow = level[f] * (0.45 + 0.55 * np.exp(-d ** 2))
        fire[win] = np.clip(np.round(2 + glow[win] * 9), 2, 11)
        idx[:FIRE_ROWS] = fire
        # the paper lantern: brightest across its middle, the ribs darker
        py, px = yy[:PAPER_ROWS], xx[:PAPER_ROWS]
        bulge = np.exp(-((py - 7.5) / 7) ** 2)
        paper = level[f] * (0.35 + 0.5 * bulge)
        rib = (py % 3 == 0)
        paper = np.where(rib, paper * 0.7, paper)
        pidx = np.clip(np.round(2 + paper * 7), 2, 9)
        word = (np.abs(px - 31.5) < 4) & (py >= 3) & (py <= 12) & ((py % 4 != 1) | (np.abs(px - 31.5) < 2))
        if RESERVED:                        # the add-on's: 祭 (matsuri, festival)
            word = np.zeros(px.shape, bool)
            word[3:13, 28:36] = GLYPHS['祭']
        pidx[word] = 15
        pidx[0] = pidx[PAPER_ROWS - 1] = 15
        idx[FIRE_ROWS:FIRE_ROWS + PAPER_ROWS] = pidx
        # the light on the floor
        gh = 64 - FIRE_ROWS - PAPER_ROWS
        gx, gz = np.meshgrid((np.arange(64) - 31.5) / 32, (np.arange(gh) - (gh - 1) / 2) / (gh / 2))
        r = np.hypot(gx, gz)
        light = level[f] * np.clip(1 - r, 0, 1) ** 1.6
        idx[FIRE_ROWS + PAPER_ROWS:] = np.where(light < 0.05, 0, np.clip(1 + np.floor(light * 7), 1, 7))
        frames.append(idx)
    shown = vr.picture(frames[0], colours)
    return shown, [vr.frame(i, colours) for i in frames]


def toro(spec: dict, at, light: dict) -> list:
    """A stone lantern ("toros": [degrees, distance], "toro": "height",
    "pool" its light's size, cm): a hexagonal foot, a post, the platform,
    the firebox (its windows lit, the lamp picture's flicker), a flared
    roof and the jewel on it; its light round its foot on the floor."""
    cx, cz = polar(at)
    k = spec.get('height', 200) / 200
    turn = np.radians(at[0]) + np.pi / 6
    stone = lambda prof, cap=True: turned([(y * k, r * k, rows) for y, r, rows in prof], 6, cx, cz, 'granite', light,
                                          turn=turn, cap='granite' if cap else False, wrap=6)
    out = stone([(0, 40, (0, 63)), (-14, 38, (0, 20)), (-22, 26, (20, 30)), (-28, 15, (30, 40)), (-82, 13, (0, 63)),
                 (-88, 17, (0, 10)), (-92, 30, (10, 20)), (-104, 32, (20, 30))])
    # the firebox: its six sides, each the whole window picture
    out += turned([(-104 * k, 22 * k, (0, FIRE_ROWS - 1)), (-142 * k, 22 * k, (0, FIRE_ROWS - 1))], 6, cx, cz, 'lamp',
                  GLOW, turn=turn, cap=False, wrap=6)
    out += stone([(-142, 24, (0, 10)), (-146, 46, (10, 20)), (-152, 44, (20, 30)), (-168, 16, (30, 63)),
                  (-172, 10, (0, 10)), (-182, 9, (10, 40)), (-192, 1, (40, 63))])
    pool = spec.get('pool', 520)
    x0, x1, z0, z1 = cx - pool / 2, cx + pool / 2, cz - pool / 2, cz + pool / 2
    corners = [(x0, -1, z0), (x1, -1, z0), (x0, -1, z1), (x1, -1, z1)]
    r0 = FIRE_ROWS + PAPER_ROWS
    uv = [(0, r0), (63, r0), (0, 63), (63, 63)]
    if wound(corners, (cx, 1000, cz)) is not corners:
        corners, uv = [corners[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
    out.append(dict(corners=[tuple(int(round(v)) for v in c) for c in corners], tex='lamp', rows=(r0, 63), var=0, uv=uv,
                    colors=[(spec.get('level', 0x80),) * 3] * 4, added=True, under=True))
    return out


def beam(x0: float, x1: float, y0: float, y1: float, depth: float, at, yaw: float, texs, light: dict,
         rise: float = 0.0, pieces: int = 1) -> list:
    """A squared beam along the local x from x0 to x1, y0 its top, y1 its
    foot (y down), depth across; rise cm its ends curve up (a torii's
    kasagi), in pieces; set at [degrees, distance] turned yaw."""
    out = []
    half = max(abs(x0), abs(x1)) or 1
    for i in range(pieces):
        a, b = x0 + (x1 - x0) * i / pieces, x0 + (x1 - x0) * (i + 1) / pieces
        ya, yb = -rise * (a / half) ** 2, -rise * (b / half) ** 2
        foot = [(a, y1 + ya, -depth / 2), (b, y1 + yb, -depth / 2), (b, y1 + yb, depth / 2), (a, y1 + ya, depth / 2)]
        top = [(a, y0 + ya, -depth / 2), (b, y0 + yb, -depth / 2), (b, y0 + yb, depth / 2), (a, y0 + ya, depth / 2)]
        c = posed(foot + top, 0, yaw, at)
        out += box_faces(c, texs, light)
    return out


def torii(spec: dict, light: dict) -> list:
    """A small vermilion torii ("torii": "at" [degrees, distance], "height",
    "width" between its pillars, "lift" cm it stands up on the steps): two
    round pillars, black at their feet, the tie beam (nuki), the strut in
    the middle, the double top beam (shimaki under the black kasagi) its
    ends curving up; facing the fight."""
    at, h, w = spec['at'], spec.get('height', 380), spec.get('width', 300)
    yaw, lift = at[0], spec.get('lift', 0)
    out = []
    for side in (-1, 1):
        px, pz = side * w / 2, 0
        ang = np.radians(yaw)
        x = polar(at)[0] + px * np.cos(ang)
        z = polar(at)[1] - px * np.sin(ang)
        out += turned([(-lift, 21, (0, 63)), (-lift - 30, 21, (0, 63))], 8, x, z, 'torii_black', light, cap=False)
        out += turned([(-lift - 30, 18, (0, 63)), (-lift - h, 16, (0, 63))], 8, x, z, 'torii', light, cap=False)
    y = lambda v: v - lift
    out += beam(-w / 2 - 50, w / 2 + 50, y(-h * 0.78), y(-h * 0.78 + 20), 14, at, yaw, ['torii', 'torii', 'torii'], light)
    out += beam(-8, 8, y(-h + 4), y(-h * 0.78), 12, at, yaw, ['torii', 'torii', 'torii'], light)
    out += beam(-w / 2 - 70, w / 2 + 70, y(-h - 14), y(-h + 4), 24, at, yaw, ['torii', 'torii', 'torii'], light,
                rise=14, pieces=6)
    out += beam(-w / 2 - 95, w / 2 + 95, y(-h - 30), y(-h - 14), 30, at, yaw, ['torii_black', 'torii_black', 'torii_black'],
                light, rise=24, pieces=8)
    return out


def steps(spec: dict, light: dict) -> list:
    """Stone steps up to the torii ("steps": "at" [degrees, distance] of
    the lowest's front, "count", "rise", "tread", "width", "top" the last
    one's depth): blocks side by side, each a step higher than the one
    before (none over another: the engine has no depth test), facing the
    fight."""
    at, out = spec['at'], []
    n, rise, tread, w = spec.get('count', 3), spec.get('rise', 13), spec.get('tread', 45), spec.get('width', 380)
    for k in range(n):
        d0, d1 = tread * k, tread * (k + 1) + (spec.get('top', 120) if k == n - 1 else 0)
        foot = [(-w / 2, 0, d0), (w / 2, 0, d0), (w / 2, 0, d1), (-w / 2, 0, d1)]
        c = posed(foot + [(x, -rise * (k + 1), z) for x, _, z in foot], 0, at[0], at, 0)
        f = box_faces(c, ['slab', 'slab', 'slab'], light)
        f.pop()
        out += f
    return out


def bamboo_fence(spec: dict, run, light: dict) -> list:
    """A low four-eyed bamboo fence (yotsume-gaki: "fences": [degrees from,
    degrees to, distance], "fence": "height", "post"): thick posts, thin
    uprights between, three rails tied across."""
    h, step = spec.get('height', 105), spec.get('post', 180)
    a0, a1, dist = run
    length = np.radians(abs(a1 - a0)) * dist
    n = max(1, round(length / step))
    out = []
    pts = [polar([a0 + (a1 - a0) * k / n, dist]) for k in range(n + 1)]
    for p in pts:
        out += tube([(p[0], 0, p[1]), (p[0], -h - 8, p[1])], [5, 5], 4, 'bamboo', light)
    for k in range(n):
        for j in (1, 2):
            q = polar([a0 + (a1 - a0) * (k + j / 3) / n, dist])
            out += tube([(q[0], 0, q[1]), (q[0], -h + 6, q[1])], [2.5, 2.5], 3, 'bamboo', light)
    for y in (-0.3 * h, -0.62 * h, -0.92 * h):
        out += tube([(p[0], y, p[1]) for p in pts], [2.5] * len(pts), 3, 'bamboo', light)
    return out


def chochin(x: float, y: float, z: float, size: float, light: dict) -> list:
    """A paper lantern hanging with its top at (x, y, z): black rims, the
    lit red paper (the lamp picture's middle, flickering), its word round
    twice."""
    r, h = 0.36 * size, size
    paper = lambda a, b: (FIRE_ROWS + a, FIRE_ROWS + b)
    # one turned shape (few corners: a tile holds 256): black rim, paper, black rim
    return turned([(y + h, 0.5 * r, (0, 63)), (y + h - 5, 0.55 * r, (0, 63)), (y + h * 0.75, 0.92 * r, paper(0, 4)),
                   (y + h * 0.4, r, paper(4, 9)), (y + 5, 0.55 * r, paper(9, PAPER_ROWS - 1)), (y, 0.5 * r, (0, 63))],
                  6, x, z, ['torii_black', 'lamp', 'lamp', 'lamp', 'torii_black'], GLOW, cap='torii_black', wrap=2)


def lantern_line(spec: dict, run, light: dict) -> list:
    """Paper lanterns on a rope between two cedar posts ("lantern_lines":
    [degrees from, degrees to, distance, lanterns], "lantern_line":
    "height" of the posts, "sag", "size" of a lantern, "drop" its cord)."""
    a0, a1, dist, n = run
    h, sag, size, drop = spec.get('height', 300), spec.get('sag', 60), spec.get('size', 48), spec.get('drop', 20)
    p0, p1 = np.array(polar([a0, dist])), np.array(polar([a1, dist]))
    out = []
    for p in (p0, p1):
        out += tube([(p[0], 0, p[1]), (p[0], -h - 10, p[1])], [8, 7], 6, 'wood', light)
    rope_at = lambda t: (p0 + (p1 - p0) * t, -h + sag * 4 * t * (1 - t))
    pts = [rope_at(t) for t in np.linspace(0, 1, 9)]
    out += tube([(p[0], y, p[1]) for p, y in pts], [1.5] * len(pts), 3, 'rope', light)
    for k in range(n):
        p, y = rope_at((k + 1) / (n + 1))
        out += tube([(p[0], y, p[1]), (p[0], y + drop, p[1])], [1, 1], 3, 'rope', light)
        out += chochin(p[0], y + drop, p[1], size, light)
    return out


def flat_stone(at, size: float, height: float, light: dict, rnd, tex: str = 'slab') -> list:
    """A big flat garden stone ("flat_stones": [degrees, distance, size,
    height]; Szechwan's "mossy_stones" in "mossy"): a ragged low slab, its
    foot a little in the ground."""
    cx, cz = polar(at)
    n = 7
    rad = size * (0.75 + 0.25 * rnd.uniform(0, 1, n))
    q0 = rnd.uniform(0, 2 * np.pi)
    q = q0 + 2 * np.pi * np.arange(n) / n
    ring = lambda y, k: [(cx + k * rad[i] * np.sin(q[i]), y, cz + k * rad[i] * np.cos(q[i])) for i in range(n)]
    lo, mid, top = ring(4, 1.0), ring(-0.7 * height, 0.94), ring(-height, 0.78)
    centre = (cx, -height - 2, cz)
    out = []
    for a, b, tex_rows in ((lo, mid, (40, 63)), (mid, top, (0, 40))):
        for i in range(n):
            j = (i + 1) % n
            out.append(face([b[i], b[j], a[i], a[j]], tex, light, (cx, -height / 2, cz), rows=tex_rows, var=i))
    for i in range(n):
        out.append(face([top[i], top[(i + 1) % n], centre, centre], tex, light, (cx, 1e6, cz)))
    return above_ground(out)


# --- the beach: a lifeguard tower, tiki torches, what the sea leaves -----------

def local(at, yaw: float):
    """Points built round (0, y, 0), +z toward the fight, set at [degrees,
    distance] turned yaw degrees more."""
    return lambda pts: posed(pts, 0, at[0] + 180 + yaw, at)


def lifeguard(spec: dict, light: dict) -> list:
    """A lifeguard tower ("lifeguard": "at" [degrees, distance], "yaw",
    "height" of its floor): four legs leaning in, braced at the back and the
    sides, a plank floor, a rail round it, four posts up to a round thatched
    roof with its ragged fringe, a red and yellow flag on top, a ladder down
    its front (toward the fight)."""
    at, h = spec['at'], spec.get('height', 200)
    put = local(at, spec.get('yaw', 0))
    foot, top, out = 100, 75, []
    corners = [(-1, -1), (1, -1), (1, 1), (-1, 1)]
    legs = [(put([(sx * foot, 0, sz * foot)])[0], put([(sx * top, -h, sz * top)])[0]) for sx, sz in corners]
    for a, b in legs:
        out += tube([a, b], [7, 6], 3, 'driftwood', light)
    # braces at half height: the back and the sides (the ladder's side open)
    mid = lambda k, t: tuple(np.array(legs[k][0]) + (np.array(legs[k][1]) - np.array(legs[k][0])) * t)
    for k, j in ((3, 0), (0, 1), (1, 2)):           # the back is z -1: corners 0, 1
        out += tube([mid(k, 0.45), mid(j, 0.45)], [3.5, 3.5], 3, 'driftwood', light)
        out += tube([mid(k, 0.12), mid(j, 0.8)], [3, 3], 3, 'driftwood', light)
    # the floor
    w = top + 12
    fl = put([(-w, -h, -w), (w, -h, -w), (w, -h, w), (-w, -h, w)])
    c = fl + [(x, y - 10, z) for x, y, z in fl]
    out += box_faces(c, ['plank', 'plank', 'plank', 'plank', 'plank', 'plank'], light)
    # posts to the roof, the rail between them
    roof_y = -h - spec.get('room', 190)
    posts = [put([(sx * (w - 6), -h - 10, sz * (w - 6))])[0] for sx, sz in corners]
    for p in posts:
        out += tube([p, (p[0], roof_y, p[2])], [4.5, 4.5], 3, 'driftwood', light)
    rail = [(p[0], -h - 95, p[2]) for p in posts]
    for k, j in ((3, 0), (0, 1), (1, 2), (2, 3)):
        if (k, j) == (2, 3):                        # the front: a gap at the ladder
            gap = 0.3
            a, b = np.array(rail[k]), np.array(rail[j])
            out += tube([tuple(a), tuple(a + (b - a) * gap)], [3, 3], 3, 'driftwood', light)
            out += tube([tuple(b + (a - b) * gap), tuple(b)], [3, 3], 3, 'driftwood', light)
        else:
            out += tube([rail[k], rail[j]], [3, 3], 3, 'driftwood', light)
    # the roof: a low cone, the fringe hanging from its rim
    R, sides = spec.get('roof', 150), 8
    cx, cz = polar(at)
    out += turned([(roof_y + 34, R + 4, (0, 63)), (roof_y, R, (0, 40)), (roof_y - 85, 6, (0, 63))], sides, cx, cz,
                  ['fringe', 'thatch'], light, turn=np.radians(at[0]), cap=False, wrap=2)
    # its underside, seen from below
    ring = [(cx + R * np.sin(np.radians(at[0]) + 2 * np.pi * s / sides), roof_y, cz + R * np.cos(np.radians(at[0]) + 2 * np.pi * s / sides))
            for s in range(sides + 1)]
    under = (cx, roof_y - 20, cz)
    for s in range(sides):
        out.append(face(wound([ring[s], ring[s + 1], under, under], (cx, roof_y - 1000, cz)), 'thatch', light,
                        (cx, roof_y - 1000, cz)))
    # the flag on a pole from the roof's top
    pole = (cx, roof_y - 85, cz)
    out += tube([pole, (cx, roof_y - 165, cz)], [2, 2], 3, 'driftwood', light)
    fq = put([(0, roof_y - 162, 0), (55, roof_y - 157, 0), (0, roof_y - 130, 0), (55, roof_y - 127, 0)])
    out.append(face(fq, 'flag', light, put([(0, roof_y - 140, -50)])[0]))
    out.append(face([fq[1], fq[0], fq[3], fq[2]], 'flag', light, put([(0, roof_y - 140, 50)])[0]))
    # the ladder: two rails from the sand in front up to the floor's edge, rungs
    lad = []
    for sx in (-1, 1):
        lad.append((put([(sx * 26, 0, w + 95)])[0], put([(sx * 26, -h - 10, w - 2)])[0]))
        out += tube(list(lad[-1]), [3.5, 3.5], 3, 'driftwood', light)
    for t in np.linspace(0.16, 0.88, 5):
        a = np.array(lad[0][0]) + (np.array(lad[0][1]) - np.array(lad[0][0])) * t
        b = np.array(lad[1][0]) + (np.array(lad[1][1]) - np.array(lad[1][0])) * t
        out += tube([tuple(a), tuple(b)], [2.5, 2.5], 3, 'driftwood', light)
    return out


def tiki(spec: dict, at, light: dict) -> list:
    """A tiki torch ("tikis": [degrees, distance], "tiki": "height"): a
    bamboo pole, a woven cup on it, its flame two crossed sheets of the
    flame picture (animated: flame_frames()), seen from anywhere."""
    cx, cz = polar(at)
    h = spec.get('height', 175)
    out = tube([(cx, 0, cz), (cx, -h, cz)], [4.5, 4], 4, 'bamboo', light)
    out += turned([(-h + 4, 5, (0, 63)), (-h - 6, 11, (0, 63)), (-h - 22, 12, (0, 63))], 6, cx, cz, 'thatch', light,
                  cap='coconut')
    fw, fh, y0 = spec.get('flame_width', 44), spec.get('flame', 62), -h - 18
    for a in (np.radians(at[0] + 45), np.radians(at[0] - 45)):
        dx, dz = fw / 2 * np.cos(a), -fw / 2 * np.sin(a)
        q = [(cx - dx, y0 - fh, cz - dz), (cx + dx, y0 - fh, cz + dz), (cx - dx, y0, cz - dz), (cx + dx, y0, cz + dz)]
        uv = [(0, 0), (63, 0), (0, 63), (63, 63)]
        for order in ((0, 1, 2, 3), (1, 0, 3, 2)):
            out.append(dict(corners=[tuple(int(round(v)) for v in q[i]) for i in order], tex='flame', rows=(0, 63), var=0,
                            uv=[uv[i] for i in order], colors=[(0x80,) * 3] * 4))
    return out


def coconut(at, light: dict, rnd) -> list:
    """A fallen coconut ("coconuts": [degrees, distance]), 26 cm."""
    cx, cz = polar(at)
    r = rnd.uniform(11, 14)
    prof = [(2, 0.5 * r, (0, 63)), (-0.3 * r, 0.95 * r, (0, 63)), (-r, r, (0, 63)), (-1.6 * r, 0.8 * r, (0, 63)),
            (-1.95 * r, 0.3 * r, (0, 63))]
    return above_ground(turned(prof, 6, cx, cz, 'coconut', light, turn=rnd.uniform(0, np.pi)))


def driftwood(spec, light: dict, rnd) -> list:
    """A bleached log the sea left ("driftwood": [degrees, distance, length,
    yaw]): a bent tube lying half in the sand, a stub of a branch."""
    at, length, yaw = spec[:2], spec[2], spec[3]
    pts = [(length * (t - 0.5), -9 + 3 * np.sin(5 * t), 14 * np.sin(np.pi * t * 1.3)) for t in np.linspace(0, 1, 5)]
    pts = posed(pts, 0, yaw, at)
    out = tube(pts, [13, 12, 11, 9, 6], 5, 'driftwood', light)
    b0 = np.array(pts[2])
    out += tube([tuple(b0), tuple(b0 + np.array([20, -26, 14]))], [6, 3], 4, 'driftwood', light)
    return above_ground(out)


def surfboard(spec: dict, light: dict) -> list:
    """A surfboard stuck nose up in the sand ("surfboard": "at", "yaw",
    "lean" degrees, "length", "width", "sink" cm under): its outline round
    at the nose, square at the tail, both faces, a thin rail round it."""
    L, W, t = spec.get('length', 210), spec.get('width', 56), 7
    ys = np.linspace(0, 1, 9)
    half = W / 2 * np.clip(np.sqrt(np.sin(np.pi * np.clip(ys * 0.92 + 0.04, 0, 1))) * 1.05, 0.12, 1)
    half[0] = W * 0.3
    ys = -ys * L + spec.get('sink', 45)
    put = lambda pts: posed(pts, spec.get('lean', 14), spec.get('yaw', 0), spec['at'])
    out = []
    for z, tex in ((-t / 2, 'board'), (t / 2, 'board')):
        for k in range(len(ys) - 1):
            q = put([(-half[k + 1], ys[k + 1], z), (half[k + 1], ys[k + 1], z), (-half[k], ys[k], z), (half[k], ys[k], z)])
            inside = put([(0, (ys[k] + ys[k + 1]) / 2, -z * 10)])[0]
            out.append(face(wound(q, inside), tex, light, inside, rows=(63 - 8 * (k + 1), 63 - 8 * k)))
    for s in (-1, 1):
        for k in range(len(ys) - 1):
            q = put([(s * half[k + 1], ys[k + 1], -t / 2), (s * half[k + 1], ys[k + 1], t / 2),
                     (s * half[k], ys[k], -t / 2), (s * half[k], ys[k], t / 2)])
            inside = put([(0, (ys[k] + ys[k + 1]) / 2, 0)])[0]
            out.append(face(wound(q, inside), 'ring_white', light, inside))
    return above_ground(out)


def life_ring(spec: dict, light: dict) -> list:
    """A life ring on a post ("life_ring": "at", "yaw", "height"): a wooden
    post, the ring hanging on its front, red and white in turn."""
    at, h = spec['at'], spec.get('height', 160)
    put = local(at, spec.get('yaw', 0))
    cx, cz = polar(at)
    out = tube([(cx, 0, cz), (cx, -h, cz)], [6, 6], 4, 'wood', light)
    R, r, n = 30, 7, 12
    centre = np.array(put([(0, -h + 45, 10)])[0])
    ring = []
    for k in range(n + 1):
        a = 2 * np.pi * k / n
        ring.append([np.array(put([(R * np.sin(a) + r * np.sin(a) * np.cos(b), -h + 45 - R * np.cos(a) - r * np.cos(a) * np.cos(b),
                                    10 + r * np.sin(b))])[0]) for b in 2 * np.pi * np.arange(5) / 4])
    for k in range(n):
        tex = 'ring_red' if (k // 2) % 2 == 0 else 'ring_white'
        mid = np.array(put([(R * np.sin(2 * np.pi * (k + 0.5) / n), -h + 45 - R * np.cos(2 * np.pi * (k + 0.5) / n), 10)])[0])
        for s in range(4):
            q = [tuple(ring[k + 1][s]), tuple(ring[k + 1][s + 1]), tuple(ring[k][s]), tuple(ring[k][s + 1])]
            out.append(face(wound(q, mid), tex, light, mid))
    return out


def beach_bits(spec: dict, light: dict, rnd) -> list:
    """Shells and starfish on the sand ("bits": "count" between "near" and
    "far" cm, "size"): flat quads just over the floor, each a quarter of the
    shells picture (colour 0 nothing), turned at random."""
    out = []
    for k in range(spec['count']):
        a, d = rnd.uniform(0, 360), rnd.uniform(spec['near'], spec['far'])
        x, z = polar([a, d])
        s = spec.get('size', 26) * rnd.uniform(0.7, 1.3) / 2
        q0 = rnd.uniform(0, 2 * np.pi)
        c = [(x + s * np.cos(q0 + q), -1, z + s * np.sin(q0 + q)) for q in (np.pi * 1.25, np.pi * 1.75, np.pi * 0.75, np.pi * 0.25)]
        u0, v0 = (k % 2) * 32, ((k // 2) % 2) * 32
        uv = [(u0, v0), (u0 + 31, v0), (u0, v0 + 31), (u0 + 31, v0 + 31)]
        if wound(c, (x, 1000, z)) is not c:
            c, uv = [c[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
        out.append(dict(corners=[tuple(int(round(v)) for v in p) for p in c], tex='shells', rows=(0, 63), var=0, uv=uv,
                        colors=[(0x80,) * 3] * 4, under=True))
    return out


# --- the Korean temple: stone lanterns, lotus lanterns, guardian posts --------

def seokdeung(spec: dict, at, light: dict) -> list:
    """A Korean stone lantern ("seokdeungs": [degrees, distance],
    "seokdeung": "height", "pool" its light's size and "level" its
    brightness, no pool none): octagonal, a lotus foot, a slim post, the
    lotus under the firebox, four of its eight sides windows (the lamp
    picture's flicker), a broad roof and the jewel on it."""
    cx, cz = polar(at)
    k = spec.get('height', 230) / 230
    turn = np.radians(at[0]) + np.pi / 8
    stone = lambda prof, cap=True: turned([(y * k, r * k, rows) for y, r, rows in prof], 8, cx, cz, 'granite', light,
                                          turn=turn, cap='granite' if cap else False, wrap=8)
    out = stone([(0, 44, (0, 63)), (-12, 42, (0, 16)), (-14, 34, (16, 24)), (-24, 26, (24, 40)), (-30, 13, (40, 48)),
                 (-112, 11, (0, 63)), (-116, 20, (0, 12)), (-126, 32, (12, 30)), (-130, 30, (30, 40))], cap=False)
    box = [(-130 * k, 23 * k, (0, FIRE_ROWS - 1)), (-168 * k, 23 * k, (0, FIRE_ROWS - 1))]
    lamp = turned(box, 8, cx, cz, 'lamp', GLOW, turn=turn, cap=False, wrap=8)
    shut = turned([(y, r, (0, 63)) for y, r, _ in box], 8, cx, cz, 'granite', light, turn=turn, cap=False, wrap=8)
    out += [lamp[i] if i % 2 == 0 else shut[i] for i in range(8)]
    out += stone([(-168, 25, (0, 10)), (-172, 54, (10, 20)), (-180, 50, (20, 30)), (-194, 20, (30, 63)),
                  (-198, 12, (0, 10)), (-206, 13, (10, 30)), (-214, 9, (30, 45)), (-222, 7, (45, 55)), (-230, 1, (55, 63))])
    pool = spec.get('pool', 0)
    if pool:
        x0, x1, z0, z1 = cx - pool / 2, cx + pool / 2, cz - pool / 2, cz + pool / 2
        corners = [(x0, -1, z0), (x1, -1, z0), (x0, -1, z1), (x1, -1, z1)]
        r0 = FIRE_ROWS + PAPER_ROWS
        uv = [(0, r0), (63, r0), (0, 63), (63, 63)]
        if wound(corners, (cx, 1000, cz)) is not corners:
            corners, uv = [corners[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
        out.append(dict(corners=[tuple(int(round(v)) for v in c) for c in corners], tex='lamp', rows=(r0, 63), var=0,
                        uv=uv, colors=[(spec.get('level', 0x80),) * 3] * 4, added=True, under=True))
    return out


LOTUS_LIGHT = {'azimuth': 0, 'elevation': 90, 'ambient': [1.05, 1.05, 1.05], 'sun': [0, 0, 0]}


def lotus(x: float, y: float, z: float, size: float, colour: int) -> list:
    """A lotus lantern hanging with its top at (x, y, z): round, its paper
    petals in two tiers (the lotus picture's band: 0 pink, 1 red, 2
    yellow, 3 green), a tassel under it; lit from inside."""
    r, h, v = 0.5 * size, size, 16 * colour
    band = lambda a, b: (v + a, v + b)
    return turned([(y + 1.5 * h, 0.04 * r, band(14, 15)), (y + h, 0.25 * r, band(12, 15)),
                   (y + 0.8 * h, 0.8 * r, band(8, 15)), (y + 0.5 * h, r, band(0, 8)), (y + 0.2 * h, 0.75 * r, band(8, 15)),
                   (y, 0.2 * r, band(0, 8))], 6, x, z, 'lotus', LOTUS_LIGHT, cap=False, wrap=3)


def lotus_line(spec: dict, run, light: dict) -> list:
    """Lotus lanterns on a rope between two posts ("lotus_lines": [degrees
    from, degrees to, distance, lanterns, first colour], "lotus_line":
    "height" of the posts, "sag", "size" of a lantern, "drop" its cord):
    the lanterns' colours in turn."""
    a0, a1, dist, n = run[:4]
    first = run[4] if len(run) > 4 else 0
    h, sag, size, drop = spec.get('height', 300), spec.get('sag', 50), spec.get('size', 44), spec.get('drop', 16)
    p0, p1 = np.array(polar([a0, dist])), np.array(polar([a1, dist]))
    out = []
    for p in (p0, p1):
        out += tube([(p[0], 0, p[1]), (p[0], -h - 10, p[1])], [7, 6], 6, 'wood', light)
    rope_at = lambda t: (p0 + (p1 - p0) * t, -h + sag * 4 * t * (1 - t))
    pts = [rope_at(t) for t in np.linspace(0, 1, 9)]
    out += tube([(p[0], y, p[1]) for p, y in pts], [1.5] * len(pts), 3, 'rope', light)
    for k in range(n):
        p, y = rope_at((k + 1) / (n + 1))
        out += tube([(p[0], y, p[1]), (p[0], y + drop, p[1])], [1, 1], 3, 'rope', light)
        out += lotus(p[0], y + drop, p[1], size, (first + k) % 4)
    return out


def jangseung(spec: dict, at, light: dict, kind: str) -> list:
    """A village guardian post ("jangseungs": [degrees, distance, "m" or
    "f"], "jangseung": "height"): a round pine post, its carved head
    facing the fight, the guardian's name down its front; he wears a
    black official's hat, her hair is a black band."""
    cx, cz = polar(at)
    k = spec.get('height', 250) / 250
    turn = np.radians(at[0])
    post = lambda prof, tex, cap=False: turned([(y * k, r * k, rows) for y, r, rows in prof], 8, cx, cz, tex, light,
                                               turn=turn, cap=cap, wrap=1)
    body = 'jangseung_body_f' if RESERVED and kind != 'm' else 'jangseung_body'     # the add-on's: her own name
    out = post([(0, 17, (0, 63)), (-170, 16, (0, 63))], body)
    if kind == 'm':
        out += post([(-170, 17, (0, 63)), (-215, 19, (0, 63))], 'jangseung_m')
        out += post([(-215, 22, (0, 63)), (-217, 36, (0, 63)), (-221, 36, (0, 63)), (-222, 14, (0, 63)),
                     (-250, 12, (0, 63))], 'torii_black', cap='torii_black')
    else:
        out += post([(-170, 17, (0, 63)), (-212, 18, (0, 63))], 'jangseung_f')
        out += post([(-212, 18, (0, 63)), (-224, 15, (0, 63)), (-230, 8, (0, 63))], 'torii_black', cap='torii_black')
    return out


def cairn(spec, light: dict, rnd) -> list:
    """A pile of stones pilgrims stacked ("cairns": [degrees, distance,
    size, stones]): flat stones one on another, smaller up it, each turned
    its own way and a little off the middle."""
    cx, cz = polar(spec[:2])
    size, n = spec[2], spec[3]
    out, y = [], 0.0
    radii = [size * (1 - 0.6 * i / max(1, n - 1)) * rnd.uniform(0.85, 1.1) for i in range(n)]
    for i in range(n):
        r, h = radii[i], radii[i] * rnd.uniform(0.38, 0.55)
        top = max(0.6 * radii[i + 1], 0.25 * r) if i < n - 1 else 0.3 * r
        ox, oz = rnd.uniform(-0.08, 0.08, 2) * size
        prof = [(y, 0.8 * r, (36, 63)), (y - 0.5 * h, r, (8, 36)), (y - h, top, (0, 8))]
        out += turned(prof, 6, cx + ox, cz + oz, 'granite', light, turn=rnd.uniform(0, np.pi),
                      cap='granite' if i == n - 1 else False, var=i)
        y -= h
    return above_ground(out)


def seoktap(spec: dict, light: dict) -> list:
    """A small three-storey stone pagoda ("seoktap": "at" [degrees,
    distance], "height"): square, a two-step plinth, three storeys each
    under a broad thin roof, smaller up it, the finial's rings on top; a
    face toward the fight."""
    at = spec['at']
    cx, cz = polar(at)
    k = spec.get('height', 200) / 200
    # [y, half width] up it; the corners' radius the half width times root 2
    prof = [(0, 48), (-22, 48), (-22, 40), (-30, 40), (-30, 36), (-50, 36), (-50, 42), (-54, 42), (-54, 22),
            (-84, 22), (-86, 44), (-92, 43), (-98, 24), (-98, 17), (-118, 17), (-120, 38), (-125, 37), (-130, 19),
            (-130, 13), (-146, 13), (-148, 32), (-152, 31), (-157, 15), (-157, 6), (-162, 8), (-168, 4), (-174, 7),
            (-180, 3), (-188, 5), (-200, 1)]
    return turned([(y * k, w * k * np.sqrt(2), (0, 63)) for y, w in prof], 4, cx, cz, 'granite', light,
                  turn=np.radians(at[0]) + np.pi / 4, cap='granite', wrap=4)


def onggi(spec, light: dict, rnd) -> list:
    """An earthenware jar with its lid ("onggis": [degrees, distance,
    size]): round shouldered, a narrow foot, a short neck, the lid a low
    dome."""
    cx, cz = polar(spec[:2])
    s = spec[2]
    prof = [(0, 0.34 * s, (56, 63)), (-0.45 * s, 0.55 * s, (30, 56)), (-0.78 * s, 0.46 * s, (14, 30)),
            (-0.92 * s, 0.3 * s, (4, 14)), (-0.96 * s, 0.35 * s, (0, 4)), (-1.05 * s, 0.24 * s, (0, 6)),
            (-1.1 * s, 0.03 * s, (0, 3))]
    return turned(prof, 6, cx, cz, 'onggi', light, turn=rnd.uniform(0, np.pi), wrap=2)


def mughal_lamp(spec: dict, at, light: dict) -> list:
    """A Mughal lamp on a post ("mughal_lamps": [degrees, distance],
    "mughal_lamp": "height", "pool" its light's size and "level" its
    brightness): a square marble foot, a slim brass post with collars, a
    hexagonal brass lantern, its pierced sides lit (the lamp picture's
    flicker), a ribbed dome and a finial."""
    cx, cz = polar(at)
    k = spec.get('height', 270) / 270
    turn = np.radians(at[0]) + np.pi / 4
    out = turned([(y * k, r * k * np.sqrt(2), (0, 63)) for y, r in ((0, 24), (-16, 24), (-16, 17), (-26, 13))], 4, cx,
                 cz, 'marble', light, turn=turn, cap='marble', wrap=4)
    out += turned([(y * k, r * k, rows) for y, r, rows in ((-26, 5, (0, 63)), (-110, 4.5, (0, 63)), (-110, 8, (0, 15)),
                   (-118, 8, (0, 15)), (-118, 4.5, (16, 31)), (-198, 4, (0, 63)), (-206, 15, (32, 47)))], 6, cx, cz,
                  'brass', light, turn=turn, cap=False, wrap=3)
    out += turned([(-206 * k, 16 * k, (0, 15)), (-242 * k, 18 * k, (0, 15))], 6, cx, cz, 'mlamp', GLOW, turn=turn,
                  cap=False, wrap=6)
    out += turned([(y * k, r * k, rows) for y, r, rows in ((-242, 21, (0, 8)), (-246, 20, (8, 16)), (-254, 15, (16, 32)),
                   (-262, 7, (32, 48)), (-266, 3, (48, 56)), (-276, 1, (56, 63)))], 6, cx, cz, 'brass', light,
                  turn=turn, cap=False, wrap=3)
    pool = spec.get('pool', 0)
    if pool:
        x0, x1, z0, z1 = cx - pool / 2, cx + pool / 2, cz - pool / 2, cz + pool / 2
        corners = [(x0, -1, z0), (x1, -1, z0), (x0, -1, z1), (x1, -1, z1)]
        uv = [(0, 32), (63, 32), (0, 63), (63, 63)]
        if wound(corners, (cx, 1000, cz)) is not corners:
            corners, uv = [corners[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
        out.append(dict(corners=[tuple(int(round(v)) for v in c) for c in corners], tex='mlamp', rows=(32, 63), var=0,
                        uv=uv, colors=[(spec.get('level', 0x80),) * 3] * 4, added=True, under=True))
    return out


def diya(x: float, z: float, size: float, turn: float, flame: bool = True) -> list:
    """An oil lamp on the ground: a small round clay bowl (size cm across),
    its flame two crossed pictures (the lamp picture's diya rows), seen
    from anywhere, lit from itself; flame False: the runtime draws it
    (DIYA: the rain puts them out)."""
    out = turned([(0, 0.28 * size, (40, 63)), (-0.22 * size, 0.5 * size, (8, 40)), (-0.3 * size, 0.52 * size, (4, 8))],
                 6, x, z, 'clay', GLOW, turn=turn, cap='clay')
    if not flame:
        return out
    h, w = 1.15 * size, 0.55 * size
    for q in (turn, turn + np.pi / 2):
        dx, dz = np.cos(q) * w / 2, np.sin(q) * w / 2
        c = [(x - dx, -0.3 * size - h, z - dz), (x + dx, -0.3 * size - h, z + dz), (x - dx, -0.3 * size, z - dz),
             (x + dx, -0.3 * size, z + dz)]
        for cc in (c, [c[1], c[0], c[3], c[2]]):
            out.append(dict(corners=[tuple(int(round(v)) for v in p) for p in cc], tex='mlamp', rows=DIYA_ROWS,
                            cols=(0, 31), var=0, colors=[(0x80,) * 3] * 4))
    return out


def diya_line(spec: dict, run, rnd) -> list:
    """A row of diyas on the ground ("diya_lines": [degrees from, degrees to,
    distance, lamps], "diya": "size")."""
    out = []
    for x, z in diya_places(run):
        out += diya(x, z, spec.get('size', 22), rnd.uniform(0, np.pi), flame=not spec.get('out'))
    return out


def diya_places(run) -> list:
    """(x, z) of a row's diyas, evenly between its ends."""
    a0, a1, dist, n = run
    return [polar([a0 + (a1 - a0) * (i + 0.5) / n, dist]) for i in range(n)]


def cypress(spec: dict, at, light: dict) -> list:
    """A cypress in a marble pot ("cypresses": [degrees, distance],
    "cypress": "height"): an octagonal pot, flaring at its lip, the tree a
    slim dark spindle, widest a third of its way up, coming to a point."""
    cx, cz = polar(at)
    k = (at[2] if len(at) > 2 else spec.get('height', 360)) / 360
    turn = np.radians(at[0]) + np.pi / 8
    if not spec.get('pot', True):
        # "pot": false: growing out of the ground on a short trunk (Athens')
        out = tube([(cx, 4, cz), (cx, -34 * k, cz)], [9 * k, 7 * k], 6, spec.get('bark', 'olive_bark'), light)
        return out + turned([(y * k, r * k, (0, 63)) for y, r in ((-24, 6), (-40, 30), (-120, 46), (-205, 42), (-282, 30),
                             (-336, 15), (-360, 1))], 8, cx, cz, 'cypress', light, turn=turn, cap=False, wrap=2)
    out = turned([(y * k, r * k, rows) for y, r, rows in ((0, 28, (56, 63)), (-8, 32, (40, 56)), (-40, 38, (8, 40)),
                   (-46, 43, (0, 8)), (-50, 40, (0, 4)))], 8, cx, cz, 'marble', light, turn=turn, cap='clay', wrap=8)
    out += turned([(y * k, r * k, (0, 63)) for y, r in ((-50, 7), (-62, 30), (-130, 44), (-210, 40), (-285, 28),
                   (-335, 14), (-360, 1))], 8, cx, cz, 'cypress', light, turn=turn, cap=False, wrap=2)
    return out


def jali(spec: dict, run, light: dict) -> list:
    """Pierced marble screens on a low plinth ("jalis": [degrees from,
    degrees to, distance], "jali": "height", "panel" width cm): straight
    panels between square posts, a rail along their tops; the stars seen
    through from both sides."""
    a0, a1, dist = run
    h, wide = spec.get('height', 110), spec.get('panel', 130)
    p0, p1 = np.array(polar([a0, dist])), np.array(polar([a1, dist]))
    n = max(1, int(round(np.linalg.norm(p1 - p0) / wide)))
    along = (p1 - p0) / np.linalg.norm(p1 - p0)
    out_dir = np.array([along[1], -along[0]])
    if out_dir @ ((p0 + p1) / 2) < 0: out_dir = -out_dir
    def box(c, half, hw, y0, y1):
        a, o = along * half, out_dir * hw
        foot = [c - a - o, c + a - o, c + a + o, c - a + o]
        return box_faces([(p[0], y0, p[1]) for p in foot] + [(p[0], y1, p[1]) for p in foot],
                         ['marble', 'marble', 'marble', 'marble', 'marble', 'marble'], light)
    out = []
    for i in range(n + 1):
        out += box(p0 + (p1 - p0) * i / n, 8, 8, 0, -h - 8)
    for i in range(n):
        a, b = p0 + (p1 - p0) * i / n + along * 8, p0 + (p1 - p0) * (i + 1) / n - along * 8
        mid = (a + b) / 2
        out += box(mid, np.linalg.norm(b - a) / 2, 6, 0, -12)
        out += box(mid, np.linalg.norm(b - a) / 2, 6, -h + 6, -h)
        q = [(a[0], -h + 6, a[1]), (b[0], -h + 6, b[1]), (a[0], -12, a[1]), (b[0], -12, b[1])]
        for side in (1, -1):
            inside = (mid[0] - side * out_dir[0] * 100, -h / 2, mid[1] - side * out_dir[1] * 100)
            out.append(face(wound(q, inside), 'jali', light, inside))
    return out


def kalasha(spec, light: dict) -> list:
    """A brass pot with marigolds ("kalashas": [degrees, distance, size]):
    round bellied on a low foot, a narrow neck and a lip, a garland round
    its shoulder, a heap of flower heads in its mouth."""
    cx, cz = polar(spec[:2])
    s = spec[2] / 70
    turn = np.radians(spec[0])
    out = turned([(y * s, r * s, rows) for y, r, rows in ((0, 13, (48, 63)), (-6, 18, (40, 48)), (-24, 29, (16, 40)),
                   (-40, 24, (8, 16)), (-50, 12, (0, 8)), (-54, 16, (0, 4)))], 8, cx, cz, 'brass', light, turn=turn,
                 cap=False, wrap=2)
    out += turned([(y * s, r * s, (0, 63)) for y, r in ((-31, 30), (-37, 29))], 8, cx, cz, 'marigold', light,
                  turn=turn, cap=False, wrap=2)
    out += turned([(y * s, r * s, (0, 63)) for y, r in ((-54, 15), (-62, 13), (-68, 4))], 8, cx, cz, 'marigold', light,
                  turn=turn, cap='marigold', wrap=2)
    return out


def fountain(spec: dict, at, light: dict) -> list:
    """A lotus fountain ("fountains": [degrees, distance], "fountain":
    "size" its basin's radius cm, "spray" its height, "level" its
    brightness, added): an octagonal marble
    foot, a wide shallow basin flaring out like a lotus, its water, the
    spray three crossed pictures over its middle (added, seen from
    anywhere); both the fountain picture's play (fountain_frames())."""
    cx, cz = polar(at)
    k = spec.get('size', 90) / 90
    turn = np.radians(at[0]) + np.pi / 8
    out = turned([(y * k, r * k, rows) for y, r, rows in ((0, 52, (56, 63)), (-10, 52, (48, 56)), (-12, 30, (40, 48)),
                   (-40, 24, (8, 40)), (-46, 34, (0, 8)), (-58, 72, (8, 40)), (-68, 90, (40, 56)), (-72, 92, (56, 63)))],
                 8, cx, cz, 'marble', light, turn=turn, cap=False, wrap=8)
    q = turn + 2 * np.pi * np.arange(9) / 8
    ring = [(cx + 86 * k * np.sin(a), -66 * k, cz + 86 * k * np.cos(a)) for a in q]
    centre = (cx, -66 * k, cz)
    for i in range(8):
        out.append(face([ring[i], ring[i + 1], centre, centre], 'fwater', light, (cx, 1e6, cz), cols=BASIN_COLS))
    h, w = spec.get('spray', 130) * k, 115 * k
    for j in range(3):
        a = turn + j * np.pi / 3
        dx, dz = np.cos(a) * w / 2, np.sin(a) * w / 2
        c = [(cx - dx, -66 * k - h, cz - dz), (cx + dx, -66 * k - h, cz + dz), (cx - dx, -66 * k, cz - dz),
             (cx + dx, -66 * k, cz + dz)]
        for cc in (c, [c[1], c[0], c[3], c[2]]):
            out.append(dict(corners=[tuple(int(round(v)) for v in p) for p in cc], tex='fwater', rows=(0, 63),
                            cols=SPRAY_COLS, var=0, colors=[(spec.get('level', 0x80),) * 3] * 4, added=True))
    return out


# --- Szechwan Fields: bamboo, tea, a painted gate, a censer, prayer flags ------

def both_ways(f: dict) -> list:
    """A face and the same seen from behind (a leaf, a flag)."""
    c, col = f['corners'], f['colors']
    back = dict(f, corners=[c[1], c[0], c[3], c[2]], colors=[col[1], col[0], col[3], col[2]])
    if 'uv' in f:
        back['uv'] = [f['uv'][i] for i in (1, 0, 3, 2)]
    else:
        l, r = f.get('cols', (0, 63))
        back['cols'] = (r, l)
    return [f, back]


def bamboo_clump(spec: dict, at, light: dict, rnd) -> list:
    """A clump of living bamboo ("bamboos": [degrees, distance], "bamboo":
    "height", "culms", "sprays" a culm): green culms from one spot,
    leaning out a little, bending toward their tops, sprays of leaves
    (two pictures crossed, seen from both sides) along their upper half,
    hanging out from them."""
    cx, cz = polar(at)
    h0, n, sprays = spec.get('height', 560), spec.get('culms', 5), spec.get('sprays', 3)
    leaf = spec.get('leaf', 120)
    out = []
    for k in range(n):
        a = rnd.uniform(0, 2 * np.pi)
        r0 = rnd.uniform(5, 38)
        bx, bz = cx + r0 * np.sin(a), cz + r0 * np.cos(a)
        h = h0 * rnd.uniform(0.72, 1.08)
        lean = np.radians(rnd.uniform(3, 11))
        path = []
        for t in (0, 0.5, 1):
            off = h * np.sin(lean) * t ** 1.6
            path.append((bx + off * np.sin(a), -h * t, bz + off * np.cos(a)))
        out += tube(path, [5, 4, 2.5], 4, 'bamboo_green', light, turn=0.4)
        for s in range(sprays):
            t = 0.5 + 0.5 * (s + rnd.uniform(0.2, 0.8)) / sprays
            off = h * np.sin(lean) * t ** 1.6
            px, pz = bx + off * np.sin(a), bz + off * np.cos(a)
            q = rnd.uniform(0, 2 * np.pi)
            ox, oz = px + 0.3 * leaf * np.sin(q), pz + 0.3 * leaf * np.cos(q)
            size = leaf * rnd.uniform(0.8, 1.15)
            y = -h * t + 0.15 * size
            for spin in (q, q + np.pi / 2):
                dx, dz = np.cos(spin) * size / 2, -np.sin(spin) * size / 2
                c = [(ox - dx, y - size, oz - dz), (ox + dx, y - size, oz + dz), (ox - dx, y, oz - dz), (ox + dx, y, oz + dz)]
                shade = tuple(int(min(255, 0x80 * v * rnd.uniform(0.85, 1.05))) for v in spec.get('colour', [0.95, 1.0, 0.9]))
                low = tuple(int(v * 0.8) for v in shade)
                out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in c], tex='bamboo_leaves',
                                      rows=(0, 63), var=0, colors=[shade, shade, low, low]))
    return out


def tea_row(spec: dict, run, light: dict) -> list:
    """A row of clipped tea bushes ("tea_rows": [degrees from, degrees to,
    distance], "tea": "width", "height"): one long hedge along the arc,
    rounded over its top, its ends closed."""
    a0, a1, dist = run
    w, h = spec.get('width', 90), spec.get('height', 70)
    n = max(2, int(round(np.radians(abs(a1 - a0)) * dist / 70)))
    prof = [(-w / 2, 0), (-w / 2, -0.55 * h), (-0.3 * w, -0.92 * h), (0, -h), (0.3 * w, -0.92 * h), (w / 2, -0.55 * h),
            (w / 2, 0)]
    rings = []
    for i in range(n + 1):
        a = np.radians(a0 + (a1 - a0) * i / n)
        # a little uneven along it: the clipping
        bump = 1 + 0.06 * np.sin(i * 2.3) + 0.04 * np.sin(i * 5.1)
        rings.append([((dist + d) * np.sin(a), y * bump, (dist + d) * np.cos(a)) for d, y in prof])
    out = []
    for i in range(n):
        a = np.radians(a0 + (a1 - a0) * (i + 0.5) / n)
        inside = (dist * np.sin(a), -0.3 * h, dist * np.cos(a))
        for j in range(len(prof) - 1):
            q = [rings[i][j + 1], rings[i + 1][j + 1], rings[i][j], rings[i + 1][j]]
            out.append(face(wound(q, inside), 'tea', light, inside, var=i))
    for i, k in ((0, 1), (n, n - 1)):
        mid = np.mean(rings[i], 0)
        inside = tuple(np.mean(rings[k], 0))
        for j in range(len(prof) - 1):
            q = [rings[i][j + 1], rings[i][j], tuple(mid), tuple(mid)]
            out.append(face(wound(q, inside) if wound(q, inside) is q else [q[1], q[0], q[2], q[3]], 'tea', light, inside))
    return out


def pailou(spec: dict, light: dict) -> list:
    """A painted gate (pailou: "pailou": "at" [degrees, distance],
    "height" to its eaves, "width" between its pillars): two vermilion
    pillars on drum stones, a lintel and a painted beam over it, the gate's
    name board between them, a green glazed hip roof over all, its eaves
    curving up at the corners, a ridge on top; facing the fight."""
    at, h, w = spec['at'], spec.get('height', 400), spec.get('width', 340)
    yaw = at[0]
    ang = np.radians(yaw)
    out = []
    for side in (-1, 1):
        x = polar(at)[0] + side * w / 2 * np.cos(ang)
        z = polar(at)[1] - side * w / 2 * np.sin(ang)
        out += turned([(0, 34, (0, 63)), (-14, 34, (0, 20)), (-26, 26, (20, 63)), (-34, 22, (0, 10))], 8, x, z, 'granite',
                      light, turn=ang, cap='granite')
        out += turned([(-34, 15, (0, 63)), (-h, 14, (0, 63))], 8, x, z, 'torii', light, turn=ang, cap=False)
    lo, hi = -0.62 * h, -0.86 * h
    out += beam(-w / 2 - 34, w / 2 + 34, lo - 24, lo, 24, at, yaw, ['torii', 'torii', 'torii'], light)
    out += beam(-w / 2 - 44, w / 2 + 44, hi - 34, hi, 28, at, yaw, ['xuanzi', 'torii', 'xuanzi', 'torii', 'torii', 'torii'],
                light)
    out += beam(-70, 70, hi + 4, lo - 30, 10, at, yaw, ['plaque', 'torii', 'plaque', 'torii', 'torii', 'torii'], light)
    out += beam(-w / 2 - 20, w / 2 + 20, -h, hi - 34, 22, at, yaw, ['torii', 'torii', 'torii'], light)
    # the roof: two slopes from the ridge to the eaves, the eaves rising
    # at the ends, the hips closing it
    reach, depth, ridge, rise, pieces = w / 2 + 110, 80, 72, 26, 8
    top = -h - 8
    ridge_half = w / 2 - 10
    eave_y = lambda x: top + 6 - rise * (abs(x) / reach) ** 3
    inside = posed([(0, top + 30, 0)], 0, yaw, at)[0]
    for i in range(pieces):
        x0, x1 = -reach + 2 * reach * i / pieces, -reach + 2 * reach * (i + 1) / pieces
        r0, r1 = np.clip([x0, x1], -ridge_half, ridge_half)
        for s in (-1, 1):
            pts = posed([(r0, top - ridge, 0), (r1, top - ridge, 0), (x0, eave_y(x0), s * depth), (x1, eave_y(x1), s * depth)],
                        0, yaw, at)
            if abs(r1 - r0) < 1:
                pts[1] = pts[0]
            out.append(face(wound(pts, inside), 'tile_green', light, inside, var=i))
    for e in (-1, 1):
        pts = posed([(e * ridge_half, top - ridge, 0), (e * ridge_half, top - ridge, 0), (e * reach, eave_y(reach), -depth),
                     (e * reach, eave_y(reach), depth)], 0, yaw, at)
        q = wound(pts, inside)
        out.append(face(q if q is pts else [pts[0], pts[1], pts[3], pts[2]], 'tile_green', light, inside))
    out += beam(-ridge_half - 16, ridge_half + 16, top - ridge - 14, top - ridge + 2, 14, at, yaw,
                ['tile_green', 'tile_green', 'tile_green'], light, rise=10, pieces=4)
    return out


def censer(spec: dict, light: dict) -> list:
    """A bronze incense burner (ding: "censer": "at" [degrees, distance],
    "height" of its smoke, "level" its brightness): a square stone plinth,
    three legs, the round cauldron with its key pattern round the top, two
    upright handles, three red joss sticks smouldering, their smoke rising
    and swaying (two crossed pictures added: smoke_frames())."""
    at = spec['at']
    cx, cz = polar(at)
    turn = np.radians(at[0])
    out = turned([(y, r * np.sqrt(2), (0, 63)) for y, r in ((0, 44), (-8, 44), (-8, 38), (-40, 36), (-46, 40))], 4, cx, cz,
                 'granite', light, turn=turn + np.pi / 4, cap='granite')
    for k in range(3):
        q = turn + 2 * np.pi * k / 3
        out += tube([(cx + 24 * np.sin(q), -46, cz + 24 * np.cos(q)), (cx + 21 * np.sin(q), -66, cz + 21 * np.cos(q))],
                    [4, 3.5], 3, 'bronze', light)
    out += turned([(-60, 20, (56, 63)), (-68, 30, (40, 56)), (-88, 34, (20, 40)), (-102, 33, (6, 20)), (-106, 37, (0, 6))],
                  8, cx, cz, 'bronze', light, turn=turn, cap='torii_black')
    for side in (-1, 1):
        hx, hz = cx + side * 30 * np.cos(turn), cz - side * 30 * np.sin(turn)
        dx, dz = 7 * np.sin(turn), 7 * np.cos(turn)
        q = [(hx - dx, -128, hz - dz), (hx + dx, -128, hz + dz), (hx - dx, -104, hz - dz), (hx + dx, -104, hz + dz)]
        out += both_ways(face(q, 'bronze', light, (cx, -110, cz), rows=(6, 18)))
    for k in range(3):
        sx = cx + (k - 1) * 5 * np.cos(turn)
        sz = cz - (k - 1) * 5 * np.sin(turn)
        lean = (k - 1) * 4
        out += tube([(sx, -104, sz), (sx + lean * np.cos(turn), -138, sz - lean * np.sin(turn))], [0.9, 0.9], 3, 'torii',
                    light)
    hgt, wid = spec.get('height', 170), spec.get('width', 70)
    for a in (turn + np.pi / 4, turn - np.pi / 4):
        dx, dz = wid / 2 * np.cos(a), -wid / 2 * np.sin(a)
        q = [(cx - dx, -136 - hgt, cz - dz), (cx + dx, -136 - hgt, cz + dz), (cx - dx, -136, cz - dz), (cx + dx, -136, cz + dz)]
        out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in q], tex='smoke', rows=(0, 63), var=0,
                              colors=[(spec.get('level', 0x70),) * 3] * 4, added=True))
    return out


def flag_line(spec: dict, run, light: dict) -> list:
    """Prayer flags on a rope between two posts ("flag_lines": [degrees
    from, degrees to, distance, flags], "flag_line": "height" of the posts,
    "sag", "size" of a flag): the five colours in turn, each hanging from
    the rope, seen from both sides, fluttering (the Szechwan picture's
    flags, animated: szechwan_frames())."""
    a0, a1, dist, n = run
    h, sag, size = spec.get('height', 300), spec.get('sag', 55), spec.get('size', 30)
    p0, p1 = np.array(polar([a0, dist])), np.array(polar([a1, dist]))
    out = []
    for p in (p0, p1):
        out += tube([(p[0], 0, p[1]), (p[0], -h - 10, p[1])], [6, 5], 5, 'wood', light)
    rope_at = lambda t: (p0 + (p1 - p0) * t, -h + sag * 4 * t * (1 - t))
    pts = [rope_at(t) for t in np.linspace(0, 1, 9)]
    out += tube([(p[0], y, p[1]) for p, y in pts], [1.2] * len(pts), 3, 'rope', light)
    flat = {'azimuth': 0, 'elevation': 90, 'ambient': [0.92, 0.92, 0.92], 'sun': [0, 0, 0]}
    half = 0.4 / n
    for k in range(n):
        t = (k + 0.5) / n
        (pa, ya), (pb, yb) = rope_at(t - half), rope_at(t + half)
        q = [(pa[0], ya, pa[1]), (pb[0], yb, pb[1]), (pa[0], ya + size, pa[1]), (pb[0], yb + size, pb[1])]
        c = k % 5
        out += both_ways(face(q, 'flags', flat, (0, 0, 0), rows=FLAG_ROWS, cols=(12 * c + 1, 12 * c + 11)))
    return out


def arc_box(a0: float, a1: float, r0: float, r1: float, top: float, foot: float = 0.0) -> list:
    """A box's corners along the arc from a0 to a1 degrees, r0 to r1 cm out
    (box_faces(): the foot 0..3 at y foot, the top 4..7 at y top)."""
    ring = [polar([a0, r0]), polar([a0, r1]), polar([a1, r1]), polar([a1, r0])]
    return [(x, foot, z) for x, z in ring] + [(x, top, z) for x, z in ring]


def battlement(spec: dict, run, light: dict, rnd) -> list:
    """A stretch of the Great Wall's parapet fallen into ruin ("battlements":
    [degrees from, degrees to, distance], "battlement": "height", "thick",
    "merlon" its width, "gap" the crenel's, "merlon_height", "ruin" how
    much is gone): grey brick along the arc, merlons on its top with the
    crenels between, some fallen, the top broken down here and there, its
    two ends crumbled lowest."""
    a0, a1, dist = run
    h, thick = spec.get('height', 150), spec.get('thick', 55)
    mw, gap, mh = spec.get('merlon', 46), spec.get('gap', 34), spec.get('merlon_height', 42)
    ruin = spec.get('ruin', 0.3)
    n = max(1, int(round(np.radians(abs(a1 - a0)) * dist / (mw + gap))))
    step = (a1 - a0) / n
    r0, r1 = dist - thick / 2, dist + thick / 2
    out = []
    for i in range(n):
        s0, s1 = a0 + step * i, a0 + step * (i + 1)
        broken = rnd.uniform() < 0.6 * ruin
        top = -h * (rnd.uniform(0.55, 0.8) if broken else rnd.uniform(0.97, 1.03))
        if i in (0, n - 1):
            top, broken = -h * rnd.uniform(0.45, 0.72), True
        out += box_faces(arc_box(s0, s1, r0, r1, top), ['wall_brick'] * 6, light)[:-1]
        if not broken and rnd.uniform() > 0.5 * ruin:
            inset = step * (gap / 2) / (mw + gap)
            out += box_faces(arc_box(s0 + inset, s1 - inset, r0, r1, top - mh, top), ['wall_brick'] * 6, light)[:-1]
    return out


def wall_rubble(at, size: float, light: dict, rnd, tex: str = 'wall_brick') -> list:
    """A brick or a lump of the parapet fallen ("rubble_bricks": [degrees,
    distance, size]): long, tipped over, sunk in the ground; tex its
    picture (Cambodia's sandstone)."""
    w, d, h = size * rnd.uniform(1.5, 2.2), size * rnd.uniform(0.8, 1.1), size * rnd.uniform(0.5, 0.7)
    foot = [(-w / 2, 0, -d / 2), (w / 2, 0, -d / 2), (w / 2, 0, d / 2), (-w / 2, 0, d / 2)]
    c = posed(foot + [(x, -h, z) for x, _, z in foot], rnd.uniform(-18, 18), rnd.uniform(0, 360), at,
              h * rnd.uniform(0.15, 0.4))
    return above_ground(box_faces(c, [tex] * 6, light))


def beacon_tower(spec: dict, light: dict) -> list:
    """A beacon tower of the wall ("beacon_towers": [degrees, distance],
    "beacon_tower": "width" at its foot, "top" at its top, "height",
    "fire" [width, height] of the flames, "smoke" [width, height],
    "level" their brightness): a square brick tower narrowing up, its
    door toward the fight, a stone ledge round its top and a deck with
    merlons round it, the signal fire burning on it (crossed
    pictures, added: the Great Wall picture's fire) and its smoke rising
    (smoke_frames()); facing the fight."""
    at = spec['at']
    w0, w1, h = spec.get('width', 260), spec.get('top', 220), spec.get('height', 380)
    cx, cz = polar(at)
    a = np.radians(at[0])
    ux, uz = np.cos(a), -np.sin(a)                   # along the ring
    vx, vz = np.sin(a), np.cos(a)                    # out from the fight
    sq = lambda half, y: [(cx + half * (sx * ux + sy * vx), y, cz + half * (sx * uz + sy * vz))
                          for sx, sy in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
    foot, top = sq(w0 / 2, 0), sq(w1 / 2, -h)
    inside = (cx, -h / 2, cz)
    out = []
    for k in range(4):
        j = (k + 1) % 4
        q = wound([top[k], top[j], foot[k], foot[j]], inside)
        out += grid_faces(*q, 'wall_brick', light, inside, size=130, var=k)
    deck = wound([top[0], top[1], top[3], top[2]], inside)
    out.append(face(deck, 'slab', light, inside))
    # the ledge round the top, the door (dark) on the side toward the fight
    out += box_faces(sq(w1 / 2 + 12, -h + 14) + sq(w1 / 2 + 12, -h - 4), ['slab'] * 6, light)[:-1]
    dh, dw = 0.36 * h, 0.26 * w0
    half = lambda y: w0 / 2 - (w0 - w1) / 2 * -y / h + 2           # the wall's face there, a little in front
    door = [(cx - half(y) * vx + s * dw / 2 * ux, y, cz - half(y) * vz + s * dw / 2 * uz) for y in (-dh, 0)
            for s in (-1, 1)]
    out.append(face(wound(door, inside), 'torii_black', light, inside))
    # the merlons round the deck
    mh, ms = 46, 36
    for k in range(4):
        j = (k + 1) % 4
        for t in (0.0, 0.5):
            px = top[k][0] + (top[j][0] - top[k][0]) * t
            pz = top[k][2] + (top[j][2] - top[k][2]) * t
            px += (cx - px) * ms / w1
            pz += (cz - pz) * ms / w1
            c = [(px + ms / 2 * (sx * ux + sy * vx), y, pz + ms / 2 * (sx * uz + sy * vz))
                 for y in (-h, -h - mh) for sx, sy in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
            out += box_faces(c, ['wall_brick'] * 6, light)[:-1]
    fw, fh = spec.get('fire', [130, 170])
    sw, sh = spec.get('smoke', [220, 520])
    level = spec.get('level', 0x80)
    for spin in (a + np.pi / 4, a - np.pi / 4):
        dx, dz = np.cos(spin), -np.sin(spin)
        q = [(cx - dx * fw / 2, -h - fh, cz - dz * fw / 2), (cx + dx * fw / 2, -h - fh, cz + dz * fw / 2),
             (cx - dx * fw / 2, -h + 6, cz - dz * fw / 2), (cx + dx * fw / 2, -h + 6, cz + dz * fw / 2)]
        out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in q], tex='flags', rows=WALL_FLAME[0],
                              cols=WALL_FLAME[1], var=0, colors=[(level,) * 3] * 4, added=True))
        y0 = -h - 0.6 * fh
        q = [(cx - dx * sw / 2, y0 - sh, cz - dz * sw / 2), (cx + dx * sw / 2, y0 - sh, cz + dz * sw / 2),
             (cx - dx * sw / 2, y0, cz - dz * sw / 2), (cx + dx * sw / 2, y0, cz + dz * sw / 2)]
        out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in q], tex='smoke', rows=(0, 63), var=0,
                              colors=[(spec.get('smoke_level', 0x60),) * 3] * 4, added=True))
    return out


def war_banner(spec: dict, at, light: dict) -> list:
    """A war banner on its pole ("war_banners": [degrees, distance, which
    of the five], "war_banner": "height" of the pole, "width", "length" of
    the banner): a black lacquered pole in a stone foot, a spear point on
    top, the banner hanging from a short arm along the ring, seen from both
    sides, fluttering (the Great Wall picture's banners)."""
    deg, dist, k = at
    H, bw, bl = spec.get('height', 430), spec.get('width', 70), spec.get('length', 170)
    px, pz = polar([deg, dist])
    a = np.radians(deg)
    ux, uz = np.cos(a), -np.sin(a)
    out = turned([(0, 26, (0, 63)), (-14, 26, (0, 63)), (-30, 18, (0, 63))], 6, px, pz, 'granite', light, cap='granite')
    out += tube([(px, -30, pz), (px, -H, pz)], [5, 4], 5, 'torii_black', light)
    out += tube([(px, -H, pz), (px, -H - 34, pz)], [3.5, 0.6], 4, 'granite', light)
    out += tube([(px, -H + 8, pz), (px + ux * (bw + 6), -H + 8, pz + uz * (bw + 6))], [2.5, 2.5], 3, 'torii_black', light)
    flat = {'azimuth': 0, 'elevation': 90, 'ambient': [0.98, 0.88, 0.76], 'sun': [0, 0, 0]}
    y0 = -H + 10
    q = [(px + ux * 4, y0, pz + uz * 4), (px + ux * bw, y0, pz + uz * bw),
         (px + ux * 4, y0 + bl, pz + uz * 4), (px + ux * bw, y0 + bl, pz + uz * bw)]
    out += both_ways(face(q, 'flags', flat, (0, 0, 0), rows=FLAG_ROWS, cols=(12 * k + 1, 12 * k + 11)))
    return out


def wine_jar(spec, light: dict, rnd) -> list:
    """A rice wine jar ("wine_jars": [degrees, distance, size]): brown
    glazed, round shouldered, a red paper diamond on it, its mouth tied
    over with red cloth."""
    cx, cz = polar(spec[:2])
    s = spec[2]
    turn = np.radians(spec[0]) + rnd.uniform(-0.4, 0.4)
    out = turned([(0, 0.3 * s, (56, 63)), (-0.35 * s, 0.5 * s, (36, 56)), (-0.7 * s, 0.48 * s, (16, 36)),
                  (-0.88 * s, 0.26 * s, (4, 16)), (-0.92 * s, 0.2 * s, (0, 4))], 8, cx, cz, 'wine_jar', light,
                 turn=turn, cap=False, wrap=2)
    out += turned([(-0.92 * s, 0.24 * s, (0, 63)), (-0.98 * s, 0.25 * s, (0, 63)), (-1.06 * s, 0.12 * s, (0, 63))], 8,
                  cx, cz, 'torii', light, turn=turn, cap='torii', wrap=2)
    return out


def sheaf(spec, light: dict, rnd) -> list:
    """A rice sheaf standing ("sheaves": [degrees, distance, size]): its
    stalks spread at the foot, bound at the waist, the ears spilling over
    at the top."""
    cx, cz = polar(spec[:2])
    s = spec[2]
    return turned([(0, 0.3 * s, (40, 63)), (-0.45 * s, 0.16 * s, (36, 40)), (-0.52 * s, 0.17 * s, (30, 36)),
                   (-0.82 * s, 0.3 * s, (6, 30)), (-1.0 * s, 0.34 * s, (0, 6)), (-1.06 * s, 0.1 * s, (0, 6))], 6, cx, cz,
                  'sheaf', light, turn=rnd.uniform(0, np.pi), cap='sheaf', wrap=2)


def tea_baskets(spec: dict, light: dict) -> list:
    """Two bamboo baskets of picked tea and the carrying pole laid over
    them ("baskets": "at" [degrees, distance], "size", "yaw")."""
    at, s = spec['at'], spec.get('size', 50)
    a = np.radians(at[0] + spec.get('yaw', 90))
    cx, cz = polar(at)
    out, tops = [], []
    for side in (-1, 1):
        x, z = cx + side * 0.7 * s * np.sin(a), cz + side * 0.7 * s * np.cos(a)
        out += turned([(0, 0.34 * s, (0, 63)), (-0.5 * s, 0.46 * s, (0, 63)), (-0.62 * s, 0.48 * s, (0, 63))], 8, x, z,
                      'wicker', light, cap='tea', wrap=2)
        tops.append((x, -0.62 * s - 4, z))
    (xa, ya, za), (xb, yb, zb) = tops
    ext = 0.9
    out += tube([(xa - ext * (xb - xa), ya, za - ext * (zb - za)), (xb + ext * (xb - xa), yb, zb + ext * (zb - za))], [3, 3], 4,
                'bamboo', light)
    return out


# --- Morning Fields: hay bales, dry-stone walls, a cart, a scarecrow, skeps ---

def hay_bale(spec, light: dict) -> list:
    """A round bale lying on its side ("bales": [degrees, distance, radius,
    yaw, lift]): its rolled hay round it, the spiral on its ends; lift cm
    up (one laid on two others)."""
    at, r = spec[:2], spec[2]
    yaw, lift = (spec[3] if len(spec) > 3 else 0), (spec[4] if len(spec) > 4 else 0)
    half, n = 0.85 * r, 8
    q = np.pi / n + 2 * np.pi * np.arange(n + 1) / n
    ring = lambda x: posed([(x, -lift - r - r * np.cos(a), r * np.sin(a)) for a in q], 0, at[0] + 90 + yaw, at)
    a, b = ring(-half), ring(half)
    inside = posed([(0, -lift - r, 0)], 0, at[0] + 90 + yaw, at)[0]
    out = []
    for s in range(n):
        out.append(face(wound([a[s], a[s + 1], b[s], b[s + 1]], inside), 'hay', light, inside, var=s))
    for end, x in ((a, -half), (b, half)):
        centre = posed([(x, -lift - r, 0)], 0, at[0] + 90 + yaw, at)[0]
        for s in range(n):
            uv = [(round(31.5 + 31 * np.sin(q[s])), round(31.5 + 31 * np.cos(q[s]))),
                  (round(31.5 + 31 * np.sin(q[s + 1])), round(31.5 + 31 * np.cos(q[s + 1]))), (31, 31), (31, 31)]
            f = face([end[s], end[s + 1], centre, centre], 'hay_end', light, inside)
            f['uv'] = uv
            out.append(f if wound(f['corners'], inside) is f['corners'] else
                       dict(f, corners=[f['corners'][i] for i in (1, 0, 2, 3)], colors=[f['colors'][i] for i in (1, 0, 2, 3)],
                            uv=[uv[i] for i in (1, 0, 2, 3)]))
    return out


def drystone_wall(spec: dict, run, light: dict, rnd) -> list:
    """A dry-stone field wall along an arc ("walls": [degrees from, degrees
    to, distance], "wall": "height", "width"): battered sides, its top
    uneven, its ends square."""
    a0, a1, dist = run
    w, h = spec.get('width', 55), spec.get('height', 85)
    n = max(2, int(round(np.radians(abs(a1 - a0)) * dist / 110)))
    rings = []
    for i in range(n + 1):
        a = np.radians(a0 + (a1 - a0) * i / n)
        top = h * (1 + rnd.uniform(-0.08, 0.08))
        prof = [(-w / 2, 0), (-0.36 * w, -top), (0.36 * w, -top), (w / 2, 0)]
        rings.append([((dist + d) * np.sin(a), y, (dist + d) * np.cos(a)) for d, y in prof])
    out = []
    for i in range(n):
        a = np.radians(a0 + (a1 - a0) * (i + 0.5) / n)
        inside = (dist * np.sin(a), -0.4 * h, dist * np.cos(a))
        for j in range(3):
            q = [rings[i][j + 1], rings[i + 1][j + 1], rings[i][j], rings[i + 1][j]]
            out.append(face(wound(q, inside), 'drystone', light, inside, rows=(0, 63) if j != 1 else (0, 12), var=i))
    for i, k in ((0, 1), (n, n - 1)):
        inside = tuple(np.mean(rings[k], 0))
        q = [rings[i][1], rings[i][2], rings[i][0], rings[i][3]]
        out.append(face(wound(q, inside), 'drystone', light, inside))
    return out


def rail_fence(spec: dict, run, light: dict) -> list:
    """A post-and-rail fence ("rail_fences": [degrees from, degrees to,
    distance, gate], "rail_fence": "height", "post"): split posts, two
    rails; with gate a five-bar gate hung on its last post, swung open
    gate degrees."""
    h, step = spec.get('height', 115), spec.get('post', 210)
    a0, a1, dist = run[:3]
    n = max(1, round(np.radians(abs(a1 - a0)) * dist / step))
    pts = [polar([a0 + (a1 - a0) * k / n, dist]) for k in range(n + 1)]
    out = []
    for p in pts:
        out += tube([(p[0], 0, p[1]), (p[0], -h - 6, p[1])], [6, 5], 4, 'wood', light)
    for y in (-0.42 * h, -0.86 * h):
        out += tube([(p[0], y, p[1]) for p in pts], [3.5] * len(pts), 3, 'wood', light)
    if len(run) > 3:
        hinge = np.array(pts[-1])
        along = hinge - np.array(pts[-2])
        along /= np.linalg.norm(along)
        sw = np.radians(run[3])
        d = np.array([along[0] * np.cos(sw) - along[1] * np.sin(sw), along[0] * np.sin(sw) + along[1] * np.cos(sw)])
        end = hinge + 280 * d
        out += tube([(end[0], 0, end[1]), (end[0], -h - 6, end[1])], [6, 5], 4, 'wood', light)
        g0, g1 = hinge + 8 * d, hinge + 270 * d
        for k in range(5):
            y = -0.14 * h - 0.18 * h * k
            out += tube([(g0[0], y, g0[1]), (g1[0], y, g1[1])], [2.5, 2.5], 3, 'wood', light)
        for u, v in ((g0, g1), ):
            out += tube([(u[0], -0.86 * h, u[1]), (v[0], -0.14 * h, v[1])], [2.5, 2.5], 3, 'wood', light)
            out += tube([(v[0], -0.14 * h, v[1]), (v[0], -0.86 * h, v[1])], [3, 3], 3, 'wood', light)
    return out


def wayside_cross(spec: dict, light: dict, rnd) -> list:
    """A stone wayside cross ("cross": "at" [degrees, distance], "height"):
    two steps, a squared shaft, its arms; a clump of poppies at its foot."""
    at, h = spec['at'], spec.get('height', 230)
    yaw = at[0]
    out = beam(-56, 56, -20, 0, 112, at, yaw, ['granite'] * 3, light)
    out += beam(-40, 40, -38, -20, 80, at, yaw, ['granite'] * 3, light)
    out += beam(-11, 11, -h, -38, 22, at, yaw, ['granite'] * 3, light)
    out += beam(-48, 48, -h + 64, -h + 44, 20, at, yaw, ['granite'] * 3, light)
    for k, side in enumerate((-1, 1)):
        x, z = posed([(side * 70, 0, 30)], 0, yaw, at)[0][::2]
        out += flower_clump(x, z, 60, 'poppies' if k else 'daisies', rnd)
    return out


def flower_clump(x: float, z: float, size: float, tex: str, rnd) -> list:
    """Two crossed pictures of flowers, seen from anywhere, lit flat."""
    out, spin = [], rnd.uniform(0, np.pi)
    for k in range(2):
        q = spin + k * np.pi / 2
        dx, dz = np.cos(q) * size / 2, np.sin(q) * size / 2
        top, root = (0x7C, 0x7C, 0x76), (0x5C, 0x60, 0x58)
        out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in
                                       [(x - dx, -size, z - dz), (x + dx, -size, z + dz), (x - dx, 0, z - dz), (x + dx, 0, z + dz)]],
                              tex=tex, rows=(0, 63), var=0, colors=[top, top, root, root]))
    return out


def hay_cart(spec: dict, light: dict) -> list:
    """A farm cart laden with hay ("cart": "at" [degrees, distance], "yaw"):
    a plank bed on two spoked wheels, slatted sides, its shafts resting on
    the ground in front, the hay heaped over it."""
    at = spec['at']
    at_yaw = at[0] + 180 + spec.get('yaw', 90)
    put = lambda pts: posed(pts, 0, at_yaw, at)
    out = []
    bed = put([(x, y, z) for y in (-80, -98) for (x, z) in ((-130, -66), (130, -66), (130, 66), (-130, 66))])
    out += box_faces(bed, ['plank'] * 4 + ['plank', 'plank'], light)
    for z in (-66, 66):
        for x0 in (-130, 120):
            out += box_faces(put([(x, y, zz) for y in (-98, -150) for (x, zz) in ((x0, z - 3), (x0 + 10, z - 3), (x0 + 10, z + 3),
                                                                                   (x0, z + 3))]), ['wood'] * 6, light)
        out += box_faces(put([(x, y, zz) for y in (-126, -136) for (x, zz) in ((-130, z - 2), (130, z - 2), (130, z + 2), (-130, z + 2))]),
                         ['wood'] * 6, light)
    # the wheels: the picture both ways, square to the axle
    for z in (-80, 80):
        q = put([(-58, -116, z), (58, -116, z), (-58, 0, z), (58, 0, z)])
        out += both_ways(face(q, 'wheel', light, put([(0, -58, 0)])[0]))
    out += tube(put([(0, -58, -84), (0, -58, 84)]), [4, 4], 4, 'wood', light)
    for z in (-40, 40):
        out += tube(put([(130, -88, z), (250, -46, z * 0.9), (340, -4, z * 0.8)]), [4, 3.5, 3], 4, 'wood', light)
    # the hay: heaped over the bed, its ends rounded down
    xs = np.linspace(-150, 150, 7)
    prof = [(-80, -98), (-74, -150), (-52, -196), (0, -218), (52, -196), (74, -150), (80, -98)]
    rings = [put([(x, -98 + (y + 98) * (1 - 0.35 * (abs(x) / 150) ** 4), z * (1 - 0.15 * (abs(x) / 150) ** 4)) for z, y in prof])
             for x in xs]
    inside = put([(0, -140, 0)])[0]
    for i in range(len(xs) - 1):
        for j in range(len(prof) - 1):
            out.append(face(wound([rings[i][j + 1], rings[i + 1][j + 1], rings[i][j], rings[i + 1][j]], inside), 'hay', light,
                            inside, var=i))
    for i in (0, len(xs) - 1):
        c = tuple(np.mean(rings[i], 0))
        for j in range(len(prof) - 1):
            q = [rings[i][j], rings[i][j + 1], c, c]
            out.append(face(q if wound(q, inside) is q else [q[1], q[0], c, c], 'hay', light, inside))
    return out


def scarecrow(spec: dict, light: dict) -> list:
    """A scarecrow ("scarecrow": "at" [degrees, distance]) facing the
    fight: a pole and a crossbar, a check shirt on them, straw at its
    cuffs, a sack head with a stitched face, a straw hat, a red rag tied
    to one arm fluttering (the flags' picture)."""
    at = spec['at']
    turn = np.radians(at[0])
    put = lambda pts: posed(pts, 0, at[0] + 180, at)
    cx, cz = polar(at)
    out = tube([(cx, 0, cz), (cx, -206, cz)], [4, 3.5], 4, 'wood', light)
    out += tube(put([(-78, -150, 0), (78, -150, 0)]), [3, 3], 4, 'wood', light)
    out += box_faces(put([(x, y, z) for y in (-92, -168) for (x, z) in ((-24, -11), (24, -11), (24, 11), (-24, 11))]),
                     ['shirt'] * 4 + ['shirt', 'shirt'], light)
    for side in (-1, 1):
        out += box_faces(put([(x, y, z) for y in (-140, -164) for (x, z) in ((side * 22, -9), (side * 70, -9), (side * 70, 9),
                                                                              (side * 22, 9))]), ['shirt'] * 6, light)
        for k, spin in enumerate((0.3, 1.9)):
            sx, sy, sz = put([(side * 74, -146, 0)])[0]
            dx, dz = 10 * np.cos(spin + turn), -10 * np.sin(spin + turn)
            q = [(sx - dx, -164, sz - dz), (sx + dx, -164, sz + dz), (sx - dx, -132 + 6 * k, sz - dz), (sx + dx, -132 + 6 * k, sz + dz)]
            out += both_ways(face(q, 'hay', light, (sx, -146, sz)))
    out += turned([(-166, 9, (63, 63)), (-172, 14, (50, 63)), (-186, 16, (20, 50)), (-200, 14, (2, 20)), (-206, 8, (0, 2))], 8,
                  cx, cz, 'burlap', light, turn=turn, cap=False)
    out += turned([(-199, 34, (0, 63)), (-203, 33, (0, 63)), (-204, 15, (0, 63)), (-222, 12, (0, 63)), (-226, 4, (0, 63))], 8,
                  cx, cz, 'hay', light, turn=turn, cap='hay')
    # the rag: the flags' red, hanging from the right cuff
    rx, ry, rz = put([(66, -140, 0)])[0]
    ax, az = np.cos(turn) * 9, -np.sin(turn) * 9
    flat = {'azimuth': 0, 'elevation': 90, 'ambient': [0.92, 0.92, 0.92], 'sun': [0, 0, 0]}
    out += both_ways(face([(rx - ax, ry, rz - az), (rx + ax, ry, rz + az), (rx - ax, ry + 36, rz - az), (rx + ax, ry + 36, rz + az)],
                          'flags', flat, (0, 0, 0), rows=FLAG_ROWS, cols=(25, 35)))
    return out


def skeps(spec: dict, light: dict) -> list:
    """Straw beehives on a bench ("skeps": "at" [degrees, distance],
    "size"): a plank on two stumps, three domed skeps along it, their doors
    toward the fight."""
    at, s = spec['at'], spec.get('size', 46)
    yaw = at[0] + 90
    turn = np.radians(at[0])
    out = beam(-110, 110, -50, -42, 44, at, yaw, ['plank'] * 6, light)
    for x in (-86, 86):
        p = posed([(x, 0, 0)], 0, yaw, at)[0]
        out += turned([(0, 14, (0, 63)), (-42, 12, (0, 63))], 6, p[0], p[2], 'wood', light, cap=False)
    for k, x in enumerate((-66, 0, 66)):
        p = posed([(x, 0, 0)], 0, yaw, at)[0]
        k_s = s * (0.9 + 0.1 * (k % 2))
        out += turned([(-50, 0.5 * k_s, (56, 63)), (-50 - 0.38 * k_s, 0.52 * k_s, (34, 56)), (-50 - 0.7 * k_s, 0.42 * k_s, (16, 34)),
                       (-50 - 0.9 * k_s, 0.24 * k_s, (4, 16)), (-50 - 0.98 * k_s, 0.06 * k_s, (0, 4))], 8, p[0], p[2], 'skep',
                      light, turn=turn + np.pi, cap='skep')
    return out


def sunflowers(spec, light: dict, rnd) -> list:
    """A few sunflowers ("sunflowers": [degrees, distance, count]): tall
    stems leaning a little, two leaves each, the heads turned to the
    fight (toward the camera), nodding."""
    cx, cz = polar(spec[:2])
    out = []
    face_to = np.radians(spec[0] + 180)
    for k in range(spec[2]):
        a = rnd.uniform(0, 2 * np.pi)
        x, z = cx + rnd.uniform(0, 45) * np.sin(a), cz + rnd.uniform(0, 45) * np.cos(a)
        h = rnd.uniform(160, 215)
        lean = rnd.uniform(-10, 10)
        top = (x + lean * np.cos(face_to), -h, z - lean * np.sin(face_to))
        out += tube([(x, 0, z), (x + 0.5 * lean * np.cos(face_to), -0.6 * h, z - 0.5 * lean * np.sin(face_to)), top], [2.5, 2.2, 2],
                    3, 'leaf_green', light)
        size = rnd.uniform(34, 44)
        # the head: a square facing the fight, nodding forward
        fx, fz = np.sin(face_to), np.cos(face_to)
        rx, rz = np.cos(face_to), -np.sin(face_to)
        nod = 0.35
        hx, hy, hz = top[0] + 6 * fx, top[1] - 4, top[2] + 6 * fz
        q = [(hx - rx * size / 2 - fx * nod * size / 2, hy - size / 2 * np.cos(nod), hz - rz * size / 2 - fz * nod * size / 2),
             (hx + rx * size / 2 - fx * nod * size / 2, hy - size / 2 * np.cos(nod), hz + rz * size / 2 - fz * nod * size / 2),
             (hx - rx * size / 2 + fx * nod * size / 2, hy + size / 2 * np.cos(nod), hz - rz * size / 2 + fz * nod * size / 2),
             (hx + rx * size / 2 + fx * nod * size / 2, hy + size / 2 * np.cos(nod), hz + rz * size / 2 + fz * nod * size / 2)]
        bright = (0x84, 0x84, 0x7C)
        out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in q], tex='sunflower', rows=(0, 63), var=0,
                              colors=[bright] * 4))
        for side, y in ((-1, -0.45 * h), (1, -0.62 * h)):
            lx, lz = x + side * 26 * rx, z + side * 26 * rz
            q = [(x, y - 10, z), (lx, y - 4, lz), (x, y + 6, z), (lx, y + 8, lz)]
            out += both_ways(face(q, 'leaf_green', light, (x, y - 40, z)))
    return out


# --- Church Sanctuary: a dark church lit by its candles, lightning at the windows --

FLAME_COLS, CUP_ROWS = (0, 31), (0, 15)        # the candle picture: a candle's flame (rows 0..31), the votives' strip
CUP_COLS, POOL_ROWS = (32, 63), (32, 63)       # (rows 0..15 of columns 32..63) and the candles' light on the floor


def candle_frames(n: int) -> tuple[list[int], list]:
    """The church's candles as their flames flicker: n frames (round and
    round), one palette. Rows 0..31 of columns 0..31 a candle's flame: a
    teardrop rising from the wick at the foot, swaying; rows 0..15 of
    columns 32..63 the votive rack's strip, four cups of red glass (rows
    7..15) each lit by the flame in it, the flame over it (rows 0..6),
    each flickering its own way; rows 16..31 of columns 32..47 a mote's glow
    (FFLY: the Taj Mahal's place, DIYA_ROWS and GLOW_COLS); rows 32..63
    the light the candles throw on the floor (seen from above), round,
    added. Colour 0 nothing; 1..11 dark red to yellow white; 12..14 the
    red glass, dim to bright; 15 dark."""
    ramp = np.array([(0, 0, 0), (52, 16, 4), (92, 30, 6), (132, 46, 8), (170, 66, 12), (204, 90, 18), (228, 118, 28),
                     (244, 148, 44), (252, 178, 70), (255, 206, 110), (255, 230, 160), (255, 246, 214),
                     (110, 16, 12), (180, 34, 22), (236, 92, 48), (22, 10, 8)], float)
    colours = [0] + [0x8000 | int(c) for c in bgr555(ramp[1:])]
    rnd = np.random.default_rng(74)
    ph = rnd.uniform(0, 2 * np.pi, (6, 3))
    t = 2 * np.pi * np.arange(n) / n
    level = 0.84 + 0.09 * np.sin(t + ph[0, 0]) + 0.05 * np.sin(2 * t + ph[0, 1]) + 0.03 * np.sin(3 * t + ph[0, 2])
    yy, xx = np.mgrid[0:64, 0:64]
    frames = []
    for f in range(n):
        idx = np.zeros((64, 64), np.uint16)
        # the candle's flame
        fy, fx = yy[:32, :32], xx[:32, :32]
        v = 1 - fy / 31.0                              # 0 the wick, 1 the tip
        sway = 2.2 * np.sin(2 * t[f] + ph[0, 1]) * v ** 1.5
        tall = 0.78 + 0.2 * level[f]
        width = 7.5 * np.clip(1 - v / tall, 0, 1) ** 0.6 * np.clip(v * 4 + 0.3, 0, 1)
        k = np.clip(1 - np.abs(fx - 15.5 - sway) / np.maximum(width, 1e-3), 0, 1) * (v < tall)
        heat = np.clip(k ** 0.5 * (1.2 - 0.8 * v / tall), 0, 1)
        idx[:32, :32] = np.where(heat < 0.1, 0, np.clip(np.round(heat * 11), 1, 11))
        # the votives: four cups, each its own flicker
        for c in range(4):
            x0 = 32 + 8 * c
            lv = 0.8 + 0.12 * np.sin(t[f] * (1 + c % 2) + ph[c + 1, 0]) + 0.08 * np.sin(3 * t[f] + ph[c + 1, 1])
            cy, cx = yy[7:16, x0:x0 + 8] - 7, xx[7:16, x0:x0 + 8] - x0
            glass = lv * (1 - cy / 12) * (1 - 0.5 * ((cx - 3.5) / 3.5) ** 2)
            cup = np.clip(np.round(12 + glass * 2.6), 12, 14)
            cup[:, 0] = cup[:, 7] = 0                  # the gap between two cups
            cup[0, 1:7] = 14                           # the lit rim
            cup[8, 1:7] = 15                           # the foot
            idx[7:16, x0:x0 + 8] = cup
            gy, gx = yy[:7, x0:x0 + 8], xx[:7, x0:x0 + 8] - x0
            gv = 1 - gy / 6.0
            w = 2.0 * np.clip(1 - gv / (0.75 + 0.25 * lv), 0, 1) ** 0.6 * np.clip(gv * 3 + 0.4, 0, 1)
            kk = np.clip(1 - np.abs(gx - 3.5 - 0.6 * np.sin(2 * t[f] + ph[c + 1, 2]) * gv) / np.maximum(w, 1e-3), 0, 1)
            idx[:7, x0:x0 + 8] = np.where(kk < 0.15, 0, np.clip(np.round(4 + kk * 7 * lv), 1, 11))
        # a mote's glow: a bright core, a soft halo (tinted when drawn)
        d = np.hypot(xx[16:32, 32:48] - 39.5, yy[16:32, 32:48] - 23.5) / 8
        g = np.clip(1 - d, 0, 1) ** 2
        idx[16:32, 32:48] = np.where(g < 0.04, 0, np.clip(np.round(1 + g * 10), 1, 11))
        # the light on the floor
        gx, gz = np.meshgrid((np.arange(64) - 31.5) / 32, (np.arange(32) - 15.5) / 16)
        light = level[f] * np.clip(1 - np.hypot(gx, gz), 0, 1) ** 2.2
        bayer = (np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]) + 0.5) / 16
        step = light * 10 + np.tile(bayer, (8, 16)) - 0.5         # dithered: no rings
        idx[32:] = np.where(step < 0.4, 0, np.clip(1 + np.floor(step), 1, 10))
        frames.append(idx)
    return colours, frames


def candle_animation(cfg: dict, vr: Vram) -> tuple:
    """The church's one animation ("candles": "frames"): the candle picture
    (candle_frames()) and the incense smoke (smoke_frames()) side by side,
    each frame of them so (one ANIM rectangle 32 halfwords wide), played
    round and round. Returns the shown candle picture, the shown smoke and
    the frames' places."""
    n = cfg['candles'].get('frames', 16)
    ccolours, candles = candle_frames(n)
    scolours, smoke = smoke_frames(n)
    g = vr.reserve(2)
    shown = vr.picture(candles[0], ccolours, at=g)
    puff = vr.picture(smoke[0], scolours, at=g + 1)
    places = []
    for f in range(n):
        g = vr.frame_run(2)
        vr.picture(candles[f], None, at=g, clut=shown[2])
        vr.picture(smoke[f], None, at=g + 1, clut=puff[2])
        places.append(((g % 64) * 16, 512 + (g // 64) * 64))
    return shown, puff, places


def added_picture(img: np.ndarray, vr: Vram) -> tuple:
    """A picture laid on what is under it, added (the light through the
    stained glass): its black nothing (colour 0), the rest 15 colours with
    the semi-transparency bit."""
    lit = img.max(2) > 6
    colours, idx = few(img[lit], 15)
    out = np.zeros((64, 64), np.uint16)
    out[lit] = idx + 1
    return vr.picture(out, [0] + [0x8000 | c for c in colours])


GLASS = ((196, 34, 30), (36, 66, 190), (226, 176, 50), (40, 130, 70), (120, 46, 150), (210, 96, 40), (60, 130, 196))


def glass_pictures(vr: Vram) -> dict:
    """The light of the stained glass: "stained" a lancet window as its
    light falls on the floor (seen from above, its pointed head at row 0):
    panes of red, blue, gold, green and violet in their lead, soft at the
    edges; "shaft" the beam it comes down in, pale and misty, fading out at
    its top (row 0) and brightest at its foot, faint streaks of colour
    along it."""
    from PIL import ImageFilter
    rnd = np.random.default_rng(124)
    yy, xx = np.mgrid[0:64, 0:64].astype(float)
    # the lancet: straight sides, a pointed head
    half = 26 * np.clip(np.where(yy < 22, np.sqrt(np.clip(1 - ((22 - yy) / 22) ** 2, 0, 1)), 1), 0, 1)
    inside = np.abs(xx - 31.5) < half
    soft = np.clip((half - np.abs(xx - 31.5)) / 4, 0, 1) * np.clip((61 - yy) / 4, 0, 1) * np.clip(yy / 3, 0, 1)
    pane = np.zeros((64, 64, 3))
    for r in range(8):
        for c in range(4):
            pane[r * 8:r * 8 + 8, c * 16:c * 16 + 16] = GLASS[rnd.integers(len(GLASS))]
    lead = ((xx % 16) < 1.5) | ((yy % 8) < 1.2)
    k = soft * inside * np.where(lead, 0.35, 1.0) * (0.75 + 0.25 * rnd.uniform(0, 1, (64, 64)))
    stained = np.asarray(Image.fromarray(np.clip(pane * k[..., None], 0, 255).astype(np.uint8)).filter(
        ImageFilter.GaussianBlur(1.6)), float) * 0.42
    # the shaft: across it a soft profile, along it misty streaks
    across = np.exp(-((xx - 31.5) / 22) ** 4)
    along = np.clip(yy / 40, 0, 1) ** 1.3 * (0.75 + 0.25 * yy / 63)
    streak = 0.75 + 0.25 * np.sin(xx * 0.7 + 2 * np.sin(yy * 0.05))
    tint = np.array([GLASS[int(c) % len(GLASS)] for c in (xx[0] // 9)], float)[None] * 0.25 + np.array([150, 160, 190]) * 0.75
    shaft = tint * (across * along * streak)[..., None] * 0.55
    return {'stained': [added_picture(stained, vr)], 'shaft': [added_picture(shaft, vr)]}


def glass_section(cfg: dict) -> bytes:
    """T2GL, the add-on's (D17, the user's, 2026-11-05: the Church's window
    light on the floor fixed to the floor, moving and turning with it, and
    not pixelled): the "glass_patches" and their shafts, drawn by the
    runtime (src/tekken2/tmst_tekken2_extras.c glass_draw()) in the world's
    frame, its panes Gouraud quads, instead of the tiles' stained picture.
    u16 patches, rows, columns, the floor's period (cm: the floor repeats
    every 10 cells, "fixed"); the panes' colours, rows x columns rgb (the
    stained picture's, glass_pictures(): same seed, same order); then each
    patch: s16 x, z (cm, the stage's frame), angle (4096 a turn: out from
    the middle), length, width, the shaft's height and its reach out (cm);
    u8 the light's level, the shaft's, its pieces, 0. The shafts' level and
    pieces 0: they stay the tiles' textured ones (glass_light(), the user's:
    "the runtime's were too sharp and stuck out; back to the old", the
    panes kept)."""
    spec = cfg['scenery'].get('glass', {})
    sh = spec.get('shaft', {})
    rnd = np.random.default_rng(124)
    rows, cols = 8, 4
    panes = b''.join(bytes(GLASS[rnd.integers(len(GLASS))]) for _ in range(rows * cols))
    out = struct.pack('<4H', len(cfg['scenery']['glass_patches']), rows, cols, cfg.get('fixed', 25000) // 10) + panes
    for patch in cfg['scenery']['glass_patches']:
        cx, cz = polar(patch[:2])
        out += struct.pack('<7h4B', round(cx), round(cz), round(patch[0] * 4096 / 360) & 0xFFF, round(patch[3]),
                           round(patch[2]), round(sh.get('height', 1800)), round(sh.get('out', 900)),
                           min(255, spec.get('level', 0x80)), 0, 0, 0)
    return out + bytes(-len(out) % 4)


def floor_light(cx: float, cz: float, size: float, level: int, tex: str = 'candle', rows=POOL_ROWS) -> dict:
    """A light's round on the floor about (cx, cz), size cm across, added,
    under everything else in its tile."""
    x0, x1, z0, z1 = cx - size / 2, cx + size / 2, cz - size / 2, cz + size / 2
    corners = [(x0, -1, z0), (x1, -1, z0), (x0, -1, z1), (x1, -1, z1)]
    uv = [(0, rows[0]), (63, rows[0]), (0, rows[1]), (63, rows[1])]
    if wound(corners, (cx, 1000, cz)) is not corners:
        corners, uv = [corners[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
    return dict(corners=[tuple(int(round(v)) for v in c) for c in corners], tex=tex, rows=rows, var=0, uv=uv,
                colors=[(level,) * 3] * 4, added=True, under=True)


def flame(x: float, y: float, z: float, w: float, h: float, turn: float = 0.0) -> list:
    """A candle's flame on its wick at (x, y, z): two crossed pictures (the
    candle picture's), seen from anywhere, lit from itself."""
    out = []
    for q in (turn, turn + np.pi / 2):
        dx, dz = np.cos(q) * w / 2, np.sin(q) * w / 2
        c = [(x - dx, y - h, z - dz), (x + dx, y - h, z + dz), (x - dx, y, z - dz), (x + dx, y, z + dz)]
        out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in c], tex='candle', rows=(0, 31),
                              cols=FLAME_COLS, var=0, colors=[(0x80,) * 3] * 4))
    return out


def candle(x: float, y: float, z: float, r: float, h: float, light: dict, turn: float = 0.0) -> list:
    """A wax candle standing at (x, y, z), r cm round, h tall, its flame."""
    out = turned([(y, r, (0, 63)), (y - h, r, (0, 63))], 5, x, z, 'wax', light, turn=turn, cap='wax')
    return out + flame(x, y - h + 1, z, 2.8 * r + 4, 4.4 * r + 7, turn)


def candelabrum(spec: dict, at, light: dict) -> list:
    """A tall brass floor candlestick ("candelabras": [degrees, distance],
    "candelabrum": "height" to its pan, "pool" its light's size, "level"
    its brightness): a wide stepped foot, a baluster stem with knops, the
    drip pan, a thick church candle and its flame; its light round its
    foot."""
    cx, cz = polar(at)
    k = spec.get('height', 150) / 150
    turn = np.radians(at[0])
    prof = [(0, 26), (-4, 26), (-8, 18), (-14, 9), (-20, 6), (-30, 5), (-34, 9), (-38, 5), (-70, 4), (-76, 8), (-80, 4),
            (-120, 3.5), (-126, 7), (-130, 4), (-142, 4), (-146, 16), (-150, 15)]
    out = turned([(y * k, r * k, (0, 63)) for y, r in prof], 8, cx, cz, 'brass', light, turn=turn, cap='brass', wrap=2)
    out += candle(cx, -150 * k, cz, 5 * k, 38 * k, light, turn)
    out.append(floor_light(cx, cz, spec.get('pool', 520), spec.get('level', 0x80)))
    return out


def altar(spec: dict, light: dict) -> list:
    """The high altar ("altar": "at" [degrees, distance]): two stone steps,
    the upper under a red runner, the altar its crimson frontal toward the
    fight under a white cloth, a brass cross on its step between four
    candlesticks; their light on the floor before it."""
    at = spec['at']
    put = local(at, 0)
    box = lambda x0, x1, y0, y1, z0, z1: put([(x, y, z) for y in (y1, y0) for x, z in ((x0, z0), (x1, z0), (x1, z1), (x0, z1))])
    out = box_faces(box(-230, 230, -12, 0, -160, 140), ['slab'] * 5 + ['slab'], light)
    out += box_faces(box(-180, 180, -24, -12, -130, 80), ['slab'] * 4 + ['carpet', 'slab'], light)
    out += box_faces(box(-120, 120, -122, -24, -110, -20), ['linen', 'linen', 'frontal', 'linen', 'linen', 'slab'], light)
    out += box_faces(box(-127, 127, -125, -122, -116, -14), ['linen'] * 6, light)
    out += box_faces(box(-16, 16, -135, -125, -104, -82), ['brass'] * 6, light)
    out += box_faces(box(-3, 3, -222, -135, -96, -90), ['brass'] * 6, light)
    out += box_faces(box(-24, 24, -200, -194, -96, -90), ['brass'] * 6, light)
    for x in (-95, -58, 58, 95):
        (sx, _, sz), = put([(x, 0, -66)])
        out += turned([(-125, 8, (0, 63)), (-128, 7, (0, 63)), (-131, 3, (0, 63)), (-150, 2.5, (0, 63)), (-153, 5, (0, 63))],
                      6, sx, sz, 'brass', light, cap='brass', wrap=2)
        out += candle(sx, -153, sz, 2.4, 26 if abs(x) > 80 else 20, light)
    (fx, _, fz), = put([(0, 0, 180)])
    out.append(floor_light(fx, fz, spec.get('pool', 600), spec.get('level', 0x70)))
    return out


def pew_rows(spec: dict, run, light: dict) -> list:
    """A block of pews facing the altar (+z: "pew_blocks": [x0, x1, z0, z1,
    rows], cm, "pew": "height" of the backs): each a seat, a back behind
    it, the carved ends (the side seen from the fight) and a kneeler."""
    x0, x1, z0, z1, rows = run
    h = spec.get('height', 96)
    box = lambda a, b, y0, y1, c, d: [(x, y, z) for y in (y1, y0) for x, z in ((a, c), (b, c), (b, d), (a, d))]
    out = []
    for i in range(rows):
        z = z0 + (z1 - z0) * i / max(1, rows - 1)
        out += box_faces(box(x0 + 5, x1 - 5, -46, -41, z - 20, z + 20), ['pew'] * 6, light)
        out += box_faces(box(x0 + 5, x1 - 5, -h, -41, z - 25, z - 20), ['pew'] * 6, light)
        out += box_faces(box(x0 + 5, x1 - 5, -16, -10, z + 32, z + 40), ['pew'] * 6, light)
        for a, b in ((x0, x0 + 5), (x1 - 5, x1)):
            out += box_faces(box(a, b, -h - 6, 0, z - 28, z + 24), ['pew', 'pew_end', 'pew', 'pew_end', 'pew', 'pew'], light)
    return out


def pillar(spec: dict, at, light: dict) -> list:
    """A column of dark marble ("pillars": [degrees, distance], "pillar":
    "radius", "height" it goes up out of sight): a square plinth, its
    mouldings, the shaft in lengths (the veins not stretched), a brass
    band round it."""
    cx, cz = polar(at)
    r = spec.get('radius', 42)
    top = spec.get('height', 2600)
    turn = np.radians(at[0]) + np.pi / 4
    out = turned([(0, 1.5 * r * np.sqrt(2), (0, 63)), (-40, 1.5 * r * np.sqrt(2), (0, 63))], 4, cx, cz, 'marble_dark', light,
                 turn=turn, cap='marble_dark')
    out += turned([(-40, 1.25 * r, (0, 15)), (-48, 1.3 * r, (16, 31)), (-56, 1.12 * r, (32, 47)), (-62, 1.04 * r, (48, 63))],
                  12, cx, cz, 'marble_dark', light, turn=turn, cap=False, wrap=3)
    ys = [-62] + list(range(-300, -top, -300)) + [-top]
    band = spec.get('band', 360)
    prof = [(y, r * (1 - 0.06 * min(1, -y / top)), (0, 63)) for y in ys]
    out += turned(prof, 12, cx, cz, 'marble_dark', light, turn=turn, cap=False, wrap=3)
    out += turned([(-band + 6, r + 3, (0, 63)), (-band - 6, r + 3, (0, 63))], 12, cx, cz, 'brass', light, turn=turn, cap=False,
                  wrap=3)
    return out


def votive_rack(spec: dict, light: dict) -> list:
    """A stand of votive candles ("votive": "at" [degrees, distance],
    "width", "tiers", "level"): wooden steps rising away from the fight,
    on each a row of red glass cups lit by their flames (the candle
    picture's strip, four cups to a picture), their light before it."""
    at = spec['at']
    put = local(at, 0)
    box = lambda x0, x1, y0, y1, z0, z1: put([(x, y, z) for y in (y1, y0) for x, z in ((x0, z0), (x1, z0), (x1, z1), (x0, z1))])
    w = spec.get('width', 168)
    out = []
    for i in range(spec.get('tiers', 3)):
        zf, top = 30 - 24 * i, -(70 + 17 * i)
        out += box_faces(box(-w / 2, w / 2, top, 0, zf - 24, zf), ['wood'] * 6, light)
        n = int(w // 28)
        for c in range(n):
            a = -n * 14 + 28 * c
            q = put([(a, top - 16, zf - 9), (a + 28, top - 16, zf - 9), (a, top, zf - 9), (a + 28, top, zf - 9)])
            out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in q], tex='candle', rows=CUP_ROWS,
                                  cols=CUP_COLS, var=0, colors=[(0x80,) * 3] * 4))
    (fx, _, fz), = put([(0, 0, 90)])
    out.append(floor_light(fx, fz, spec.get('pool', 460), spec.get('level', 0x70)))
    return out


def thurible(spec: dict, light: dict) -> list:
    """A censer hanging from its stand ("thurible": "at" [degrees,
    distance], "height" of the stand, "smoke" how high its smoke goes,
    "level" its brightness): a brass foot and post, an arm out toward the
    fight, three chains down to the censer (a pierced brass ball on a
    foot), its smoke rising and swaying (two crossed pictures added:
    smoke_frames())."""
    at = spec['at']
    put = local(at, 0)
    h = spec.get('height', 170)
    (px, _, pz), = put([(0, 0, 0)])
    out = turned([(0, 22, (0, 63)), (-5, 22, (0, 63)), (-10, 10, (0, 63)), (-18, 4, (0, 63)), (-h, 3, (0, 63)),
                  (-h - 6, 6, (0, 63)), (-h - 12, 1, (0, 63))], 8, px, pz, 'brass', light, cap='brass', wrap=2)
    (ax, _, az), = put([(0, 0, 44)])
    out += tube([(px, -h + 8, pz), (ax, -h + 8, az), (ax, -h + 18, az)], [1.8, 1.8, 1.4], 4, 'brass', light)
    y = -h + 18 + 52                                        # the censer's lid top
    for k in range(3):
        q = 2 * np.pi * k / 3
        out += tube([(ax, -h + 18, az), (ax + 9 * np.sin(q), y, az + 9 * np.cos(q))], [0.8, 0.8], 3, 'chain', light)
    out += turned([(y + 30, 6, (0, 63)), (y + 26, 4, (0, 63)), (y + 22, 9, (0, 63)), (y + 14, 13, (0, 63)), (y + 6, 12, (0, 63)),
                   (y + 3, 13, (0, 63)), (y, 9, (0, 63)), (y - 4, 3, (0, 63))], 8, ax, az, 'brass', light, cap='brass', wrap=2)
    hgt, wid = spec.get('smoke', 150), spec.get('width', 60)
    turn = np.radians(at[0])
    for a in (turn + np.pi / 4, turn - np.pi / 4):
        dx, dz = wid / 2 * np.cos(a), -wid / 2 * np.sin(a)
        q = [(ax - dx, y - 4 - hgt, az - dz), (ax + dx, y - 4 - hgt, az + dz), (ax - dx, y - 4, az - dz),
             (ax + dx, y - 4, az + dz)]
        out += both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in q], tex='smoke', rows=(0, 63), var=0,
                              colors=[(spec.get('level', 0x60),) * 3] * 4, added=True))
    return out


def chandelier(spec: dict, place, light: dict) -> list:
    """A brass chandelier ("chandeliers": [degrees, distance, height of its
    ring over the floor], "chandelier": "radius", "candles", "pool",
    "level"): a ring of candles on arms from a turned hub, hanging on four
    chains gathered into one that goes up out of sight; its light round
    under it on the floor."""
    cx, cz = polar(place[:2])
    y = -place[2]
    rad, n = spec.get('radius', 80), spec.get('candles', 8)
    ring = [(cx + rad * np.sin(a), y, cz + rad * np.cos(a)) for a in 2 * np.pi * np.arange(n + 1) / n]
    out = tube(ring, [2.5] * len(ring), 4, 'brass', light)
    out += turned([(y + 34, 1, (0, 63)), (y + 26, 6, (0, 63)), (y + 12, 11, (0, 63)), (y, 12, (0, 63)), (y - 12, 8, (0, 63)),
                   (y - 40, 4, (0, 63)), (y - 52, 9, (0, 63)), (y - 66, 3, (0, 63))], 8, cx, cz, 'brass', light, cap='brass',
                  wrap=2)
    for a in 2 * np.pi * (np.arange(4) + 0.5) / 4:
        out += tube([(cx + 10 * np.sin(a), y + 4, cz + 10 * np.cos(a)), (cx + 0.6 * rad * np.sin(a), y + 12, cz + 0.6 * rad * np.cos(a)),
                     (cx + rad * np.sin(a), y, cz + rad * np.cos(a))], [1.8, 1.8, 1.8], 3, 'brass', light)
    for a in 2 * np.pi * np.arange(n) / n:
        x, z = cx + rad * np.sin(a), cz + rad * np.cos(a)
        out += turned([(y - 2, 3, (0, 63)), (y - 6, 6, (0, 63))], 5, x, z, 'brass', light, cap='brass')
        out += candle(x, y - 6, z, 2.2, 16, light, a)
    hub = y - spec.get('gather', 150)
    for a in 2 * np.pi * np.arange(4) / 4:
        out += tube([(cx + rad * np.sin(a), y, cz + rad * np.cos(a)), (cx, hub, cz)], [0.9, 0.9], 3, 'chain', light)
    out += tube([(cx, y - 66, cz), (cx, hub, cz), (cx, -spec.get('top', 3200), cz)], [1.2, 1.2, 1.2], 3, 'chain', light)
    out.append(floor_light(cx, cz, spec.get('pool', 700), spec.get('level', 0x60)))
    return out


def glass_light(spec: dict, patch, patch_tex: str = 'stained', shaft_tex: str = 'shaft', patch_on: bool = True) -> list:
    """The light of a window on the floor ("glass_patches": [degrees,
    distance, width, length], "glass": "level", "shaft" {"height", "out",
    "level"}): the stained picture laid along the way from the window
    (its head toward the fight's middle), added, and the beam it comes down
    in, leaning out and up toward the window (patch_tex, shaft_tex: the
    Pagoda's daylight at its doors instead)."""
    a = np.radians(patch[0])
    cx, cz = polar(patch[:2])
    w, ln = patch[2], patch[3]
    rx, rz = np.sin(a), np.cos(a)                  # out from the middle
    tx, tz = np.cos(a), -np.sin(a)                 # across
    p = lambda s, t, y=-1: (cx + rx * s + tx * t, y, cz + rz * s + tz * t)
    q = [p(-ln / 2, -w / 2), p(-ln / 2, w / 2), p(ln / 2, -w / 2), p(ln / 2, w / 2)]
    uv = [(0, 0), (63, 0), (0, 63), (63, 63)]
    if wound(q, (cx, 1000, cz)) is not q:
        q, uv = [q[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
    out = [dict(corners=[tuple(int(round(v)) for v in c) for c in q], tex=patch_tex, rows=(0, 63), var=0, uv=uv,
                colors=[(spec.get('level', 0x80),) * 3] * 4, added=True, under=True)] if patch_on else []
    sh = spec.get('shaft')
    if sh:
        # in lengths: the GPU draws nothing over 1024 x 512 pixels, near
        up, far, n = sh.get('height', 1800), sh.get('out', 900), sh.get('pieces', 6)
        at = lambda k, side: p(far * k / n, side * (0.45 + 0.2 * k / n) * w, -2 - (up - 2) * k / n)
        for k in range(n):
            q = [at(k + 1, -1), at(k + 1, 1), at(k, -1), at(k, 1)]
            v0, v1 = round(63 * (1 - (k + 1) / n)), round(63 * (1 - k / n))
            out += both_ways(dict(corners=[tuple(int(round(v)) for v in c) for c in q], tex=shaft_tex, rows=(v0, v1),
                                  var=0, colors=[(sh.get('level', 0x80),) * 3] * 4, added=True))
    return out


def candle_points(spec: dict) -> list:
    """The candles' light ("light" "candle": [r, g, b] its colour, "reach"
    cm) as lit()'s "points": the candlesticks', the altar's, the votives'
    and the chandeliers'."""
    colour = spec['light'].get('candle', [0.9, 0.55, 0.22])
    reach = spec['light'].get('reach', 380)
    out = []
    for at in spec.get('candelabras', []):
        x, z = polar(at)
        out.append([x, -spec.get('candelabrum', {}).get('height', 150) - 40, z, reach, colour])
    if spec.get('altar'):
        (x, _, z), = local(spec['altar']['at'], 0)([(0, 0, -60)])
        out.append([x, -165, z, reach * 1.2, colour])
    if spec.get('votive'):
        (x, _, z), = local(spec['votive']['at'], 0)([(0, 0, 10)])
        out.append([x, -100, z, reach * 0.8, [0.8 * v for v in colour]])
    for c in spec.get('chandeliers', []):
        x, z = polar(c[:2])
        out.append([x, -c[2] - 10, z, reach * 1.6, colour])
    return out


def window_skies(cfg: dict, pic: np.ndarray, fb, vr: Vram) -> list:
    """The far background as the lightning outside lights it ("lightning"
    "windows": the stained glass, the panorama picture's pixels this
    "colourful" and "bright" from row "from" down): per brightness of
    "levels" the glass flaring up toward blue white and its glow spilt on
    the wall round it ("glow" pixels, "spill" how much), the cells that
    changes drawn anew. Returns (desc, cluts) per flash ("variants": the
    level each shows), as FARV wants them (bolt_skies())."""
    from PIL import ImageFilter
    spec = cfg['lightning']
    w = spec['windows']
    rows, cols = pic.shape[0] // 64, pic.shape[1] // 64
    sat, lum = pic.max(2) - pic.min(2), pic.mean(2)
    r, g, b = pic[..., 0], pic[..., 1], pic[..., 2]
    warm = (r >= g) & (g > b + 15)                     # candlelight, the altarpiece: not glass
    blur = lambda m, r: np.asarray(Image.fromarray(np.clip(m * 255, 0, 255).astype(np.uint8)).filter(
        ImageFilter.GaussianBlur(r)), float) / 255
    if w.get('rects'):
        # "rects" [[x0, y0, x1, y1], ...] pixels of the player's picture: the
        # windows' bluish night glass there (Eternal Darkness's dark windows)
        x0, y0, x1, y1 = cfg['crop']
        sx, sy = pic.shape[1] / (x1 - x0), pic.shape[0] / (y1 - y0)
        glass = np.zeros(pic.shape[:2], bool)
        for wx0, wy0, wx1, wy1 in w['rects']:
            glass[max(0, int((wy0 - y0) * sy)):int((wy1 - y0) * sy), max(0, int((wx0 - x0) * sx)):int((wx1 - x0) * sx)] = True
        glass &= ((b > r + w.get('blue', 3)) | (lum > 40)) & ~warm
    else:
        glass = (sat > w.get('colourful', 60)) & (lum > w.get('bright', 45)) & ~warm
        glass[:w.get('from', 300)] = False
        glass &= blur(glass.astype(float), 3) > 0.3      # the windows, not a stray pixel
    core = np.clip(blur(glass.astype(float), 1.2) * 1.6, 0, 1)
    halo = blur(glass.astype(float), w.get('glow', 18))
    halo = np.clip(halo / max(1e-6, np.percentile(halo[glass], 50)), 0, 1)
    skies = []
    for s in spec['levels']:
        lit = pic * (1 + 1.4 * s * core[..., None]) + s * core[..., None] * np.array([60, 70, 110])
        lit = lit + s * w.get('spill', 0.7) * halo[..., None] * np.array([70, 80, 120])
        lit = np.clip(lit, 0, 255)
        cells = {}
        for r in range(rows):
            for c in range(cols):
                part = lit[64 * r:64 * r + 64, 64 * c:64 * c + 64]
                if np.abs(part - pic[64 * r:64 * r + 64, 64 * c:64 * c + 64]).max() > 10:
                    colours, idx = sixteen(part)
                    x, y, clut = vr.picture(idx, colours)
                    cells[r, c] = (cell_desc(x, y), clut)
        skies.append(cells)
    out = []
    for v in spec['variants']:
        desc = [list(row) for row in fb.desc]
        cluts = [list(row) for row in fb.cluts]
        for (r, c), (d, (cx, cy)) in skies[v].items():
            desc[r][c], cluts[r][c] = d, cy << 6 | cx >> 4
        out.append((desc, cluts))
    return out


def church_walls(spec: dict) -> list:
    """The church's scenery as walls (COLL): [kind, x, z, a, b, turn] in
    the tiles' cm, kind 0 a circle (a its radius), 1 a box (a, b its half
    width and depth along its turned x and z, turn 4096 a round): the
    columns' plinths, the candlesticks', the censer's stand and the censer,
    the altar with its steps, the votive stand, each block of pews."""
    out = []
    circle = lambda x, z, r: out.append([0, round(x), round(z), round(r), 0, 0])
    for at in spec.get('pillars', []):
        circle(*polar(at), 1.5 * spec.get('pillar', {}).get('radius', 42) * 1.2)
    for at in spec.get('candelabras', []):
        circle(*polar(at), 26 * spec.get('candelabrum', {}).get('height', 150) / 150)
    if spec.get('thurible'):
        put = local(spec['thurible']['at'], 0)
        (x, _, z), (cx, _, cz) = put([(0, 0, 0), (0, 0, 44)])
        circle(x, z, 22)
        circle(cx, cz, 14)

    def box(at, x0, x1, z0, z1):
        put = local(at, 0)
        (cx, _, cz), (ex, _, ez) = put([((x0 + x1) / 2, 0, (z0 + z1) / 2), ((x0 + x1) / 2 + 1, 0, (z0 + z1) / 2)])
        turn = round(np.degrees(np.arctan2(-(ez - cz), ex - cx)) / 360 * 4096) & 0xFFF
        out.append([1, round(cx), round(cz), round((x1 - x0) / 2), round((z1 - z0) / 2), turn])
    if spec.get('altar'):
        box(spec['altar']['at'], -230, 230, -160, 140)
    if spec.get('votive'):
        w = spec['votive'].get('width', 168)
        box(spec['votive']['at'], -w / 2, w / 2, 30 - 24 * spec['votive'].get('tiers', 3), 30)
    for x0, x1, z0, z1, _ in spec.get('pew_blocks', []):
        out.append([1, round((x0 + x1) / 2), round((z0 - 28 + z1 + 40) / 2), round((x1 - x0) / 2),
                    round((z1 + 40 - z0 + 28) / 2), 0])
    return out


def dimmed_flashes(lg: bytes, scale: float, tint) -> bytes:
    """A TTT1 pack's LGHT with the fighters' light of each flash scaled
    and tinted (inside a church a flash at the windows lights them little)."""
    npat = lg[16]
    q = 18 + 2 * npat + sum(lg[18:18 + npat])
    at = (q + 3) & ~3
    n = struct.unpack_from('<H', lg, at)[0]
    out = bytearray(lg)
    for i in range(n):
        rgb = struct.unpack_from('<I', lg, at + 4 + 4 * i)[0]
        new = 0
        for k in range(3):
            new |= min(255, round(((rgb >> (8 * k)) & 0xFF) * scale * tint[k])) << (8 * k)
        struct.pack_into('<I', out, at + 4 + 4 * i, new)
    return bytes(out)


# --- Pagoda Temple: a dark hall of guardian kings, daylight at its doors -------

MOTE_GLOW = (16, 31), (32, 47)                 # the mote picture: rows, columns of a mote's glow (FFLY: DIYA_ROWS, GLOW_COLS)


def pagoda_pictures(vr: Vram) -> dict:
    """The Pagoda's own pictures: "shimenawa" (a sacred rope's twisted rice
    straw, golden, its strands dark between), "shide" (a zigzag of white
    paper, seen through round it), "ema" (a wooden votive plaque, its
    pentagon cut out, a horse in red and black strokes of a wish, a red
    cord at its top), "gong" (a bronze gong's face, round, seen through
    round it: rings hammered in it, a raised boss, the rim dark),
    "lacquer" (a temple's red lacquer, worn black at its edges),
    "black_lacquer", "mikan" (a mandarin's skin), "mochi" (rice cake,
    white), "ceramic" (a sake flask's white glaze, a blue band), "shingle"
    (a small roof's cypress bark shingles); added: "sunlit" (the daylight
    through a door's lattice on the floor, seen from above, its far end
    at row 0), "sunshaft" (the beam it comes in by, dusty), "mote" (a dust
    speck's glow at MOTE_GLOW, tinted when drawn)."""
    from PIL import ImageFilter
    rnd = np.random.default_rng(206)
    yy, xx = np.mgrid[0:64, 0:64].astype(float)
    noise = rnd.normal(0, 1, (64, 64))
    for _ in range(2):
        noise = (noise + np.roll(noise, 1, 0) + np.roll(noise, 1, 1)) / 3
    noise = (noise - noise.mean()) / noise.std()
    out = {}
    solid = lambda img: [vr.picture(*sixteen(np.clip(img, 0, 255))[::-1])]
    holed = lambda rgba: [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    # the rope: strands twisted round it, straw fibres along them
    twist = (np.sin((xx * 0.8 + yy * 1.6) * 2 * np.pi / 16) + 1) / 2
    fibre = (np.sin(yy * 2.1 + 3 * noise) + 1) / 2
    out['shimenawa'] = solid(np.array([196, 168, 96], float)[None, None] * (0.45 + 0.45 * twist[..., None] ** 0.6
                                                                               + 0.1 * fibre[..., None]))
    # the paper: four steps down, each turned back (a lightning zigzag)
    paper = np.zeros((64, 64, 4))
    for k in range(4):
        x0 = 20 + (12 if k % 2 else 0)
        y0 = 16 * k
        paper[y0:y0 + 15, x0:x0 + 14, :3] = np.array([238, 236, 226]) * (0.92 + 0.08 * rnd.uniform(0, 1))
        paper[y0:y0 + 15, x0:x0 + 14, 3] = 255
        paper[y0 + 13:y0 + 16, 20:46, :3] = (226, 224, 214)
        paper[y0 + 13:y0 + 16, 20:46, 3] = 255
    paper[..., :3] *= (1 - 0.05 * noise[..., None])
    out['shide'] = holed(paper)
    # the plaque: a pentagon (a gable on top), wood, a horse's strokes, a cord
    ema = np.zeros((64, 64, 4))
    shape = (yy >= 14 + np.abs(xx - 31.5) * 0.45) & (yy < 60) & (np.abs(xx - 31.5) < 28)
    grain = (np.sin((yy + 2 * noise) * 2 * np.pi / 5) + 1) / 2
    wood = np.array([196, 160, 112], float)[None, None] * (0.85 + 0.15 * grain[..., None])
    ema[..., :3] = wood
    edge = shape & ~((yy >= 17 + np.abs(xx - 31.5) * 0.45) & (yy < 57) & (np.abs(xx - 31.5) < 25))
    ema[edge, :3] *= 0.7
    horse = np.zeros((64, 64), bool)
    horse |= (np.hypot((xx - 30) / 10, (yy - 36) / 5) < 1) & (np.hypot((xx - 30) / 10, (yy - 36) / 5) > 0.7)
    horse |= (np.abs(xx - 39) < 1.5) & (yy > 26) & (yy < 36)
    for lx in (24, 27, 33, 36):
        horse |= (np.abs(xx - lx) < 1) & (yy > 39) & (yy < 50)
    ema[horse & shape, :3] = (30, 22, 18)
    ema[(np.hypot(xx - 40, yy - 25) < 3) & shape, :3] = (180, 30, 24)
    for k in range(3):
        ema[(np.abs(xx - (14 + 4 * k)) < 1) & (yy > 30) & (yy < 52) & shape, :3] = (40, 30, 24)
    ema[..., 3] = np.where(shape, 255, 0)
    cord = (np.abs(xx - 31.5) < 1.2) & (yy < 16)
    ema[cord] = (196, 34, 30, 255)
    out['ema'] = holed(ema)
    # the gong: hammered rings, a boss, its rim
    r = np.hypot(xx - 31.5, yy - 31.5) / 31.5
    gong = np.zeros((64, 64, 4))
    tone = 0.75 + 0.15 * np.sin(r * 40) * (r > 0.3) + 0.1 * noise
    tone = np.where(r < 0.22, 1.15 - r * 1.5, tone)                  # the boss, lit
    tone = np.where(r > 0.9, 0.5, tone)                              # the rim
    gong[..., :3] = np.array([176, 128, 58], float)[None, None] * tone[..., None]
    gong[..., 3] = np.where(r <= 1, 255, 0)
    out['gong'] = holed(gong)
    lac = np.array([150, 30, 22], float)[None, None] * (1 + 0.06 * noise[..., None])
    worn = np.clip(np.maximum(np.abs(xx - 31.5), np.abs(yy - 31.5)) / 31.5 - 0.8, 0, 1)[..., None] * 3
    out['lacquer'] = solid(lac * (1 - 0.5 * np.clip(worn + 0.3 * np.clip(noise[..., None] - 1.2, 0, 1), 0, 1)))
    blk = np.array([26, 22, 22], float)[None, None] * (1 + 0.1 * noise[..., None])
    blk[(yy < 3) | (yy > 60)] = (130, 26, 20)
    out['black_lacquer'] = solid(blk)
    out['mikan'] = solid(np.array([232, 126, 30], float)[None, None] * (1 + 0.07 * noise[..., None])
                         * (0.75 + 0.3 * (1 - yy / 63))[..., None])
    out['mochi'] = solid(np.array([236, 232, 220], float)[None, None] * (1 + 0.02 * noise[..., None])
                         * (0.82 + 0.2 * (1 - yy / 63))[..., None])
    cer = np.array([226, 224, 214], float)[None, None] * (1 + 0.03 * noise[..., None])
    cer[24:30] = (40, 60, 130)
    cer[33:35] = (40, 60, 130)
    out['ceramic'] = solid(cer)
    sh = np.array([84, 62, 44], float)[None, None] * (1 + 0.1 * noise[..., None])
    sh = sh * (0.75 + 0.3 * ((yy % 8) / 8))[..., None]
    sh[(yy % 8) < 1] *= 0.5
    out['shingle'] = solid(sh)
    # the daylight on the floor: the lattice's shadow in it, soft, fading inward
    lattice = ((xx % 8) < 2) | ((yy % 8) < 2)
    soft = np.clip(np.minimum(xx, 63 - xx) / 8, 0, 1) * np.clip((63 - yy) / 6, 0, 1) * (0.35 + 0.65 * (yy / 63) ** 0.7)
    sun = np.array([200, 186, 150], float)[None, None] * (soft * np.where(lattice, 0.35, 1.0))[..., None]
    sun = np.asarray(Image.fromarray(np.clip(sun, 0, 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(0.8)), float)
    out['sunlit'] = [added_picture(sun * 0.55, vr)]
    across = np.exp(-((xx - 31.5) / 24) ** 4)
    along = np.clip(yy / 36, 0, 1) ** 1.2 * (0.7 + 0.3 * yy / 63)
    specks = (rnd.uniform(0, 1, (64, 64)) > 0.985) * 0.5
    shaft = np.array([180, 170, 140], float)[None, None] * (across * along * (0.85 + 0.15 * np.sin(xx * 0.5)) + specks * across)[..., None]
    out['sunshaft'] = [added_picture(shaft * 0.5, vr)]
    mote = np.zeros((64, 64), np.uint16)
    d = np.hypot(xx[16:32, 32:48] - 39.5, yy[16:32, 32:48] - 23.5) / 8
    g = np.clip(1 - d, 0, 1) ** 2
    mote[16:32, 32:48] = np.where(g < 0.04, 0, np.clip(np.round(1 + g * 14), 1, 15))
    ramp = [0] + [0x8000 | int(c) for c in bgr555(np.array([[18 * k, 17 * k, 15 * k] for k in range(1, 16)], float).clip(0, 255))]
    out['mote'] = [vr.picture(mote, ramp)]
    return out


def shimenawa(spec: dict, run, light: dict) -> list:
    """A sacred rope between two posts ("shimenawas": [degrees from, degrees
    to, distance], "shimenawa": "height" of the posts, "sag", "thick" the
    rope's radius, "shide" paper zigzags along it, "tassels"): fat in its
    middle, thinner to its ends, the paper and straw tassels hanging off
    it."""
    a0, a1, dist = run[:3]
    h, sag, thick = spec.get('height', 190), spec.get('sag', 40), spec.get('thick', 7)
    p0, p1 = np.array(polar([a0, dist])), np.array(polar([a1, dist]))
    out = []
    for p in (p0, p1):
        out += tube([(p[0], 0, p[1]), (p[0], -h - 12, p[1])], [6, 5], 6, 'black_lacquer', light)
        out += turned([(-h - 12, 6, (0, 63)), (-h - 18, 8, (0, 63)), (-h - 22, 2, (0, 63))], 6, p[0], p[1], 'wood', light)
    rope_at = lambda t: (p0 + (p1 - p0) * t, -h + sag * 4 * t * (1 - t))
    ts = np.linspace(0, 1, 11)
    pts = [rope_at(t) for t in ts]
    out += tube([(p[0], y, p[1]) for p, y in pts], [thick * (0.55 + 0.45 * np.sin(np.pi * t)) for t in ts], 6, 'shimenawa',
                light, turn=0.6)
    flat = {'azimuth': 0, 'elevation': 90, 'ambient': [0.75, 0.72, 0.68], 'sun': [0, 0, 0]}
    across = (p1 - p0) / max(1e-9, np.linalg.norm(p1 - p0))
    n = spec.get('shide', 4)
    for k in range(n):
        p, y = rope_at((k + 0.5) / n)
        y += thick * 0.8
        w, ln = spec.get('paper', 16), spec.get('paper_length', 38)
        a, b = p - across * w / 2, p + across * w / 2
        q = [(a[0], y, a[1]), (b[0], y, b[1]), (a[0], y + ln, a[1]), (b[0], y + ln, b[1])]
        out += both_ways(face(q, 'shide', flat, (0, 0, 0)))
    for k in range(spec.get('tassels', 3)):
        p, y = rope_at((k + 1) / (spec.get('tassels', 3) + 1))
        out += tube([(p[0], y + thick * 0.7, p[1]), (p[0], y + thick + 18, p[1]), (p[0], y + thick + 30, p[1])],
                    [2.4, 3.2, 1.0], 4, 'shimenawa', light)
    return out


def temple_gong(spec: dict, light: dict) -> list:
    """A bronze gong in its frame ("gong": "at" [degrees, distance],
    "size" across, "height" of the frame): two red lacquered posts on black
    feet, a crossbar with turned-up ends, the gong hanging on two cords
    under it facing the fight, its rim round it; the beater leaning on a
    post."""
    s, h = spec.get('size', 110), spec.get('height', 240)
    at = spec['at']
    loc = lambda pts: posed(pts, 0, at[0] + 180, at)
    half = s / 2 + 26
    out = []
    for side in (-1, 1):
        x = side * half
        out += box_faces(loc([(x - 22, 0, -10), (x + 22, 0, -10), (x + 22, 0, 10), (x - 22, 0, 10),
                              (x - 22, -12, -10), (x + 22, -12, -10), (x + 22, -12, 10), (x - 22, -12, 10)]),
                         ['black_lacquer'] * 4 + ['black_lacquer', 'black_lacquer'], light)
        out += tube(loc([(x, -12, 0), (x, -h, 0)]), [5, 5], 6, 'lacquer', light)
    out += tube(loc([(-half - 22, -h + 14, 0), (-half - 8, -h - 2, 0), (half + 8, -h - 2, 0), (half + 22, -h + 14, 0)]),
                [4, 6, 6, 4], 6, 'black_lacquer', light)
    top = -h + 16 + 12
    cy = top + s / 2 + 14
    for side in (-1, 1):
        out += tube(loc([(side * s * 0.3, -h + 6, 0), (side * s * 0.22, cy - s * 0.45, 0)]), [0.8, 0.8], 3, 'shimenawa', light)
    q = loc([(-s / 2, cy - s / 2, 0), (s / 2, cy - s / 2, 0), (-s / 2, cy + s / 2, 0), (s / 2, cy + s / 2, 0)])
    lit_gong = dict(light, ambient=[v * 1.25 for v in light['ambient']])
    out += both_ways(face(q, 'gong', lit_gong, loc([(0, cy, 40)])[0]))
    ring = [loc([(0.49 * s * np.sin(a), cy - 0.49 * s * np.cos(a), 0)])[0] for a in 2 * np.pi * np.arange(17) / 16]
    out += tube(ring, [2.2] * len(ring), 4, 'black_lacquer', light)
    out += tube(loc([(half + 4, -4, -30), (half + 2, -110, -8)]), [1.6, 1.6], 4, 'wood', light)
    head = loc([(half + 2, -110, -8)])[0]
    out += turned([(head[1] + 8, 6, (0, 63)), (head[1], 8, (0, 63)), (head[1] - 8, 6, (0, 63))], 6, head[0], head[2],
                  'mochi', light)
    return out


def ema_rack(spec: dict, at, light: dict, rnd) -> list:
    """A rack of votive plaques ("ema_racks": [degrees, distance],
    "ema_rack": "width", "height", "rows" of plaques, "per" plaques a
    row): two posts, bars across, a little gabled roof of shingles, the
    plaques hung close on the bars, each a little turned."""
    w, h = spec.get('width', 170), spec.get('height', 190)
    loc = lambda pts: posed(pts, 0, at[0] + 180, at)
    out = []
    for side in (-1, 1):
        out += tube(loc([(side * w / 2, 0, 0), (side * w / 2, -h, 0)]), [4, 4], 5, 'wood', light)
    rows = spec.get('rows', 2)
    for k in range(rows):
        y = -h + 34 + k * 48
        out += tube(loc([(-w / 2, y, 0), (w / 2, y, 0)]), [2, 2], 4, 'wood', light)
        per = spec.get('per', 9)
        for j in range(per):
            x = -w / 2 + 12 + (w - 24) * (j + rnd.uniform(-0.2, 0.2)) / (per - 1)
            yaw = rnd.uniform(-0.25, 0.25)
            pw, ph = 17, 17
            dx, dz = np.cos(yaw) * pw / 2, np.sin(yaw) * pw / 2
            z = rnd.uniform(-3, 3)
            q = loc([(x - dx, y - 1, z - dz), (x + dx, y - 1, z + dz), (x - dx, y + ph, z - dz + 3), (x + dx, y + ph, z + dz + 3)])
            out += both_ways(face(q, 'ema', light, loc([(x, y, 60)])[0]))
    # the roof: two slopes over a ridge
    rw, ry, rd = w / 2 + 22, -h - 4, 34
    for side in (-1, 1):
        q = loc([(-rw, ry - 22, 0), (rw, ry - 22, 0), (-rw, ry + 6, side * rd), (rw, ry + 6, side * rd)])
        out += both_ways(face(q, 'shingle', light, loc([(0, ry + 60, 0)])[0]))
    out += tube(loc([(-rw - 4, ry - 22, 0), (rw + 4, ry - 22, 0)]), [3, 3], 4, 'black_lacquer', light)
    return out


def offering_table(spec: dict, at, light: dict) -> list:
    """An offering ("offerings": [degrees, distance], "offering":
    "height"): a low table of black lacquer, its edge red, on it a
    mirror-rice cake (two white rounds, a mandarin on top), a pyramid of
    mandarins on a stand and two white sake flasks."""
    h = spec.get('height', 50)
    loc = lambda pts: posed(pts, 0, at[0] + 180, at)
    out = []
    tw, td = 70, 40
    out += box_faces(loc([(-tw / 2, -h + 5, -td / 2), (tw / 2, -h + 5, -td / 2), (tw / 2, -h + 5, td / 2), (-tw / 2, -h + 5, td / 2),
                          (-tw / 2, -h, -td / 2), (tw / 2, -h, -td / 2), (tw / 2, -h, td / 2), (-tw / 2, -h, td / 2)]),
                     ['black_lacquer'] * 4 + ['black_lacquer', 'black_lacquer'], light)
    for sx in (-1, 1):
        for sz in (-1, 1):
            out += tube(loc([(sx * (tw / 2 - 5), -h + 5, sz * (td / 2 - 5)), (sx * (tw / 2 - 5), 0, sz * (td / 2 - 5))]),
                        [2.5, 2.5], 4, 'black_lacquer', light)
    top = -h
    sphere = lambda x, z, y, r, tex: turned([(y, 0.4 * r, (0, 63)), (y - r, r, (0, 63)), (y - 2 * r, 0.4 * r, (0, 63))], 5,
                                            *loc([(x, 0, z)])[0][::2], tex, light, cap=False)
    # the rice cake
    out += turned([(top, 12, (0, 63)), (top - 7, 13, (0, 63)), (top - 9, 10, (0, 63))], 8, *loc([(-18, 0, 0)])[0][::2], 'mochi', light)
    out += turned([(top - 9, 9, (0, 63)), (top - 15, 10, (0, 63)), (top - 17, 7, (0, 63))], 8, *loc([(-18, 0, 0)])[0][::2], 'mochi', light)
    out += sphere(-18, 0, top - 17, 4.5, 'mikan')
    # the mandarins on their stand
    out += turned([(top, 9, (0, 63)), (top - 4, 5, (0, 63)), (top - 8, 10, (0, 63))], 6, *loc([(10, 0, 0)])[0][::2], 'black_lacquer', light)
    for dx, dz in ((-4, -3), (4, -3), (0, 4)):
        out += sphere(10 + dx, dz, top - 8, 4.5, 'mikan')
    out += sphere(10, 0, top - 15, 4.5, 'mikan')
    # the flasks
    for x in (26, 33):
        out += turned([(top, 4, (0, 63)), (top - 7, 5.5, (0, 63)), (top - 14, 3, (0, 63)), (top - 19, 1.6, (0, 63)),
                       (top - 21, 2.2, (0, 63))], 6, *loc([(x, 0, 6)])[0][::2], 'ceramic', light)
    return out


# --- Serene Forest: a misty wood of giant trees ------------------------------

SUN_ROW = 440                        # its sunlight's palettes (low half rows): shown, then the frames under it


def forest_pictures(vr: Vram) -> dict:
    """Serene Forest's own pictures: "bark_red" (a giant tree's bark, deep
    red-brown, long fissures), "moss" (thick green moss), "rings" (a cut
    stump's year rings, a crack across), "cap_red" (a fly agaric's red cap,
    white flecks), "cap_brown" (a cep's), "stem" (a mushroom's stalk,
    cream), "fern" (a fern's fronds seen from the side, seen through:
    grass); added: "sunlit" and "sunlit_b" (a patch of sunlight on the
    floor, dappled by the leaves, seen from above, its far end at row 0),
    "sunshaft" and "sunshaft_b" (the beam it comes down in, misty): their
    palettes a run at SUN_ROW ("sun_pals", sun_pulse()); "mote" (a glowing
    bug's glow at MOTE_GLOW, tinted when drawn)."""
    from PIL import ImageDraw
    rnd = np.random.default_rng(207)
    yy, xx = np.mgrid[0:64, 0:64].astype(float)

    def smooth(n=2, along=False):
        a = rnd.normal(0, 1, (64, 64))
        for _ in range(n):
            if along:
                a = (a + np.roll(a, 1, 0) + np.roll(a, -1, 0)) / 3
            else:
                a = (a + np.roll(a, 1, 0) + np.roll(a, -1, 0) + np.roll(a, 1, 1) + np.roll(a, -1, 1)) / 5
        return (a - a.mean()) / a.std()
    out = {}
    solid = lambda img: [vr.picture(*sixteen(np.clip(img, 0, 255))[::-1])]
    holed = lambda rgba: [vr.picture(*see_through(np.clip(rgba, 0, 255))[::-1])]
    # the bark: ridges along it, deep fissures between
    ridge = smooth(8, along=True)
    ridge = (ridge + np.roll(ridge, 1, 1)) / 2
    fis = np.clip(-ridge - 0.6, 0, None)
    out['bark_red'] = solid(np.array([104, 58, 40], float)[None, None] * (1 + 0.18 * ridge[..., None]) - 60 * fis[..., None])
    moss, tufty = smooth(1), smooth(3)
    out['moss'] = solid(np.array([70, 104, 34], float)[None, None] * (0.85 + 0.12 * moss[..., None] + 0.12 * tufty[..., None]))
    r = np.hypot(xx - 31.5 + 0.9 * smooth(4), yy - 31.5)
    rings = (np.sin(r * 1.5) + 1) / 2
    wood = np.array([176, 130, 84], float)[None, None] * (0.78 + 0.18 * rings[..., None])
    wood[(np.abs(xx - 31.5 - 0.25 * (yy - 31.5)) < 1.0) & (yy > 34)] *= 0.45      # the crack
    rim = r > 27
    wood[rim] = np.array([92, 56, 36], float)[None] * (1 + 0.1 * moss[rim, None])   # the bark round it
    out['rings'] = solid(wood)
    cap = np.array([196, 34, 24], float)[None, None] * (0.75 + 0.3 * (1 - yy / 63))[..., None] * np.ones((64, 64, 3))
    for _ in range(14):
        x, y, s = rnd.uniform(4, 60), rnd.uniform(4, 52), rnd.uniform(2, 4)
        cap[np.hypot(xx - x, (yy - y) * 1.3) < s] = (236, 230, 214)
    out['cap_red'] = solid(cap)
    out['cap_brown'] = solid(np.array([132, 86, 48], float)[None, None] * (0.8 + 0.25 * (1 - yy / 63))[..., None]
                             * (1 + 0.06 * moss[..., None]))
    out['stem'] = solid(np.array([222, 212, 186], float)[None, None] * (0.85 + 0.1 * smooth(4, True)[..., None]))
    # a fern: fronds arching up and out from the root, leaflets down both sides
    s = 256
    im = Image.new('RGBA', (s, s), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    for k in range(7):
        lean = -1.1 + 2.2 * k / 6 + rnd.uniform(-0.12, 0.12)
        length = s * rnd.uniform(0.6, 0.9) * (1 - 0.25 * abs(lean))
        pts = []
        for t in np.linspace(0, 1, 14):
            a = lean * (0.5 + 0.9 * t)
            pts.append((s / 2 + np.sin(a) * length * t, s - 1 - np.cos(a) * length * t * (1 - 0.3 * t * abs(lean))))
        g = rnd.uniform(0.75, 1.1)
        for i in range(len(pts) - 1):
            (x0, y0), (x1, y1) = pts[i], pts[i + 1]
            d.line([pts[i], pts[i + 1]], fill=(int(48 * g), int(86 * g), int(30 * g), 255), width=3)
            leaf = 22 * (1 - i / 13) ** 0.8 + 4
            dx, dy = x1 - x0, y1 - y0
            n = np.hypot(dx, dy) or 1
            for side in (-1, 1):
                ex, ey = x0 - side * dy / n * leaf + dx * 0.4, y0 + side * dx / n * leaf + dy * 0.4 + 0.3 * leaf
                d.polygon([(x0, y0), (ex, ey), (x1, y1)], fill=(int(62 * g), int(118 * g), int(38 * g), 255))
    out['fern'] = holed(np.asarray(im.resize((64, 64), Image.LANCZOS)).astype(float))
    # toadstools seen from the side, small at the picture's foot (grass
    # cards the size of a tuft): three stalks, their caps red, white flecks
    im = Image.new('RGBA', (s, s), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    for x, h, w in ((0.42, 0.26, 0.17), (0.6, 0.18, 0.12), (0.3, 0.12, 0.09)):
        cx, foot, top = x * s, s - 1, s - 1 - h * s
        d.rectangle((cx - w * s * 0.18, top, cx + w * s * 0.18, foot), fill=(226, 216, 190, 255))
        d.pieslice((cx - w * s / 2, top - w * s * 0.45, cx + w * s / 2, top + w * s * 0.45), 180, 360, fill=(190, 30, 22, 255))
        for k in range(4):
            fx = cx + (k - 1.5) * w * s * 0.22
            d.ellipse((fx - 4, top - w * s * 0.18 - 4 + abs(k - 1.5) * 4, fx + 4, top - w * s * 0.18 + 4 + abs(k - 1.5) * 4),
                      fill=(240, 234, 220, 255))
    out['toadstools'] = holed(np.asarray(im.resize((64, 64), Image.LANCZOS)).astype(float))
    # the sunlight on the floor: soft blobs of light the leaves let through
    spots = np.zeros((64, 64))
    for _ in range(26):
        x, y, s = rnd.uniform(6, 58), rnd.uniform(6, 58), rnd.uniform(2.5, 7)
        spots += np.exp(-((xx - x) ** 2 + (yy - y) ** 2) / (2 * s * s)) * rnd.uniform(0.5, 1)
    spots = np.clip(spots, 0, 1) ** 0.8
    soft = np.clip(np.minimum(xx, 63 - xx) / 14, 0, 1) * np.clip(np.minimum(yy, 63 - yy) / 14, 0, 1)
    sun = np.array([210, 196, 140], float)[None, None] * (soft * (0.25 + 0.75 * spots))[..., None]
    across = np.exp(-((xx - 31.5) / 22) ** 4)
    along = np.clip(yy / 30, 0, 1) ** 1.2 * (0.6 + 0.4 * yy / 63)
    streak = 0.8 + 0.2 * np.sin(xx * 0.45 + 2 * smooth(6, True))
    shaft = np.array([196, 184, 132], float)[None, None] * (across * along * streak)[..., None]
    pals = []
    for name, img in (('sunlit', sun * 0.85), ('sunshaft', shaft * 0.7)):
        lit = img.max(2) > 6
        colours, idx = few(img[lit], 15)
        pic = np.zeros((64, 64), np.uint16)
        pic[lit] = idx + 1
        k = len(pals)
        x, y, _ = vr.picture(pic, None, clut=(16 * k, SUN_ROW))
        pals.append([0] + [0x8000 | int(c) for c in colours])
        out[name] = [(x, y, (16 * k, SUN_ROW))]
        out[name + '_b'] = [(x, y, (16 * (k + 2), SUN_ROW))]
    out['sun_pals'] = pals + pals
    mote = np.zeros((64, 64), np.uint16)
    dd = np.hypot(xx[16:32, 32:48] - 39.5, yy[16:32, 32:48] - 23.5) / 8
    g = np.clip(1 - dd, 0, 1) ** 2
    mote[16:32, 32:48] = np.where(g < 0.04, 0, np.clip(np.round(1 + g * 14), 1, 15))
    ramp = [0] + [0x8000 | int(c) for c in bgr555(np.array([[17 * k, 17 * k, 17 * k] for k in range(1, 16)], float).clip(0, 255))]
    out['mote'] = [vr.picture(mote, ramp)]
    return out


def sun_pulse(vr: Vram, pals: list, spec: dict) -> tuple[int, list]:
    """The sunlight's palettes (forest_pictures(): patch and beam, twice)
    shown at SUN_ROW, and "frames" runs under it: each light slowly
    brightening and fading (between "low" and 1 of itself, the second pair
    half a turn behind the first), as the leaves high up sway."""
    n, low = spec.get('frames', 48), spec.get('low', 0.45)
    palette_run(vr, SUN_ROW, pals)
    runs = []
    for f in range(n):
        frame = []
        for k, pal in enumerate(pals):
            t = 2 * np.pi * f / n + (np.pi if k >= 2 else 0)
            level = low + (1 - low) * (0.5 + 0.5 * np.sin(t))
            rgb = rgb_of(np.array(pal[1:], np.uint16) & 0x7FFF) * level
            frame.append([0] + [0x8000 | int(c) for c in bgr555(rgb)])
        runs.append(palette_run(vr, SUN_ROW - 1 - f, frame))
    return SUN_ROW, runs


def fallen_log(spec: dict, light: dict) -> list:
    """A giant tree fallen long ago ("logs": [degrees, distance, length,
    radius, yaw]): a trunk lying on the ground, a little sunk in it, moss
    over its top and bark under, its broken ends the year rings, a branch
    stub or two."""
    at, ln, rad, yaw = spec[:2], spec[2], spec[3], spec[4]
    sides = 8
    loc = lambda pts: posed(pts, 0, at[0] + 180 + yaw, at)
    n = max(2, round(ln / 160))
    out = []
    xs = np.linspace(-ln / 2, ln / 2, n + 1)
    rr = [rad * (1.08 - 0.16 * (x + ln / 2) / ln) for x in xs]        # thicker toward its foot
    ring = lambda x, r: loc([(x, -r * 0.8 - r * np.cos(2 * np.pi * s / sides), r * np.sin(2 * np.pi * s / sides))
                             for s in range(sides)])
    rings = [ring(x, r) for x, r in zip(xs, rr)]
    for k in range(n):
        mid = tuple(np.mean(rings[k] + rings[k + 1], 0))
        for s in range(sides):
            t = (s + 1) % sides
            up = np.cos(2 * np.pi * (s + 0.5) / sides) > 0.2                   # its top: moss
            out.append(face(wound([rings[k + 1][s], rings[k + 1][t], rings[k][s], rings[k][t]], mid),
                            'moss' if up else 'bark_red', light, mid, var=s))
    for ends, inner in ((rings[0], rings[1]), (rings[-1], rings[-2])):
        c = tuple(np.mean(ends, 0))
        inside = tuple(np.mean(inner, 0))
        for s in range(sides):
            a0, a1 = 2 * np.pi * s / sides, 2 * np.pi * (s + 1) / sides
            q = [ends[s], ends[(s + 1) % sides], c, c]
            w = wound(q, inside)
            f = face(w, 'rings', light, inside)
            p0 = (round(31.5 + 28 * np.sin(a0)), round(31.5 + 28 * np.cos(a0)))
            p1 = (round(31.5 + 28 * np.sin(a1)), round(31.5 + 28 * np.cos(a1)))
            f['uv'] = [p0, p1, (32, 32), (32, 32)] if w is q else [p1, p0, (32, 32), (32, 32)]
            out.append(f)
    for x, a in ((-0.2 * ln, 0.9), (0.25 * ln, -1.1)):
        base = (x, -rad * 1.5, 0)
        tip = (x + 0.3 * rad, -rad * 1.5 - 0.9 * rad * np.cos(a), 0.9 * rad * np.sin(a))
        out += tube(loc([base, tip]), [0.22 * rad, 0.12 * rad], 5, 'bark_red', light)
    return above_ground(out)


def stump(spec: dict, light: dict, rnd) -> list:
    """A tree's stump ("stumps": [degrees, distance, radius, height]): its
    foot flaring into the ground, moss round it low, bark up it, its top
    cut flat (the year rings), roots going down into the floor."""
    cx, cz = polar(spec[:2])
    r, h = spec[2], spec[3]
    sides = 9
    turn = rnd.uniform(0, 2 * np.pi)
    out = turned([(6, 1.45 * r, (0, 63)), (-0.25 * h, 1.08 * r, (20, 63)), (-0.75 * h, r, (0, 40)), (-h, 0.97 * r, (0, 20))],
                 sides, cx, cz, ['moss', 'bark_red', 'bark_red'], light, turn=turn, cap=False)
    top = [(cx + 0.97 * r * np.sin(turn + 2 * np.pi * s / sides), -h, cz + 0.97 * r * np.cos(turn + 2 * np.pi * s / sides))
           for s in range(sides)]
    for s in range(sides):
        a0, a1 = 2 * np.pi * s / sides, 2 * np.pi * (s + 1) / sides
        f = face([top[s], top[(s + 1) % sides], (cx, -h, cz), (cx, -h, cz)], 'rings', light, (cx, 1e6, cz))
        f['uv'] = [(round(31.5 + 26 * np.sin(a0)), round(31.5 + 26 * np.cos(a0))),
                   (round(31.5 + 26 * np.sin(a1)), round(31.5 + 26 * np.cos(a1))), (32, 32), (32, 32)]
        out.append(f)
    for k in range(4):
        a = turn + 2 * np.pi * (k + rnd.uniform(-0.2, 0.2)) / 4
        out += tube([(cx + 1.1 * r * np.sin(a), -0.15 * h, cz + 1.1 * r * np.cos(a)),
                     (cx + 2.0 * r * np.sin(a), 8, cz + 2.0 * r * np.cos(a))], [0.3 * r, 0.12 * r], 5, 'bark_red', light)
    return above_ground(out)


def mushrooms(spec, light: dict, rnd) -> list:
    """A clump of mushrooms ("mushrooms": [degrees, distance, how many,
    "red" or "brown", size]): each a pale stalk and a domed cap, a little
    apart and of its own size."""
    cx, cz = polar(spec[:2])
    big = spec[4] if len(spec) > 4 else 1.0
    out = []
    for k in range(spec[2]):
        a, d = rnd.uniform(0, 2 * np.pi), rnd.uniform(8, 22) * big * (k > 0)
        x, z = cx + d * np.sin(a), cz + d * np.cos(a)
        size = big * rnd.uniform(0.6, 1.0) * (1.35 if k == 0 else 1)
        h, cr = 16 * size, 10 * size
        out += turned([(2, 1.8 * size, (0, 63)), (-h, 1.4 * size, (0, 63))], 5, x, z, 'stem', light, cap=False)
        tex = 'cap_red' if spec[3] == 'red' else 'cap_brown'
        out += turned([(-h + 2, 0.3 * cr, (40, 63)), (-h, cr, (24, 40)), (-h - 0.45 * cr, 0.8 * cr, (8, 24)),
                       (-h - 0.75 * cr, 0.35 * cr, (0, 8))], 6, x, z, tex, light, turn=rnd.uniform(0, 1), cap=tex)
    return out


# --- Cambodian Wetlands: Khmer ruins by the lake ------------------------------

def card(centre, along, up, w: float, h: float, tex: str, rows, cols, level=(0x80, 0x80, 0x80), low: float = 1.0,
         added: bool = False) -> list:
    """A picture standing on its foot's middle centre, w wide along the
    unit vector along and h tall along up, seen from both sides; lit
    flat, its foot low times as dark."""
    c, a, u = (np.array(v, float) for v in (centre, along, up))
    q = [c - a * w / 2 + u * h, c + a * w / 2 + u * h, c - a * w / 2, c + a * w / 2]
    top, foot = tuple(int(v) for v in level), tuple(int(v * low) for v in level)
    return both_ways(dict(corners=[tuple(int(round(v)) for v in p) for p in q], tex=tex, rows=rows, cols=cols, var=0,
                          colors=[top, top, foot, foot], **({'added': True} if added else {})))


def ruin_wall(spec: dict, light: dict, rnd) -> list:
    """A fragment of a temple wall ("ruin_walls": {"at" [degrees,
    distance], "yaw", "width", "height", "thick", "relief": the side toward
    the fight carved with apsaras, "pieces"}): sandstone blocks side by
    side, each its own height (the top broken down in steps), a moulding
    along its foot."""
    at, yaw = spec['at'], spec.get('yaw', 0)
    w, h, t = spec.get('width', 300), spec.get('height', 240), spec.get('thick', 60)
    n = spec.get('pieces', max(1, round(w / 75)))
    place = local(at, yaw)
    front = 'apsara' if spec.get('relief') else 'sandstone'
    out = []
    for k in range(n):
        x0, x1 = -w / 2 + w * k / n, -w / 2 + w * (k + 1) / n
        top = -h * (rnd.uniform(0.55, 0.85) if k in (0, n - 1) else rnd.uniform(0.82, 1.04))
        c = place([(x0, 0, -t / 2), (x1, 0, -t / 2), (x1, 0, t / 2), (x0, 0, t / 2),
                   (x0, top, -t / 2), (x1, top, -t / 2), (x1, top, t / 2), (x0, top, t / 2)])
        inside = np.mean(np.array(c, float), 0)
        for j, tex in ((0, 'sandstone'), (1, 'sandstone'), (3, 'sandstone')):
            i = (j + 1) % 4
            out.append(face(wound([c[4 + j], c[4 + i], c[j], c[i]], inside), tex, light, inside, var=k))
        out += grid_faces(*wound([c[7], c[6], c[3], c[2]], inside), front, light, inside, size=w / n)
        out.append(face(wound([c[4], c[5], c[7], c[6]], inside), 'sandstone', light, inside))
    # the moulding: a lower, wider course along the foot
    c = place([(-w / 2 - 8, 0, -t / 2 - 10), (w / 2 + 8, 0, -t / 2 - 10), (w / 2 + 8, 0, t / 2 + 10),
               (-w / 2 - 8, 0, t / 2 + 10)] + [(x, -26, z) for x, z in ((-w / 2 - 8, -t / 2 - 10), (w / 2 + 8, -t / 2 - 10),
                                                                       (w / 2 + 8, t / 2 + 10), (-w / 2 - 8, t / 2 + 10))])
    out += box_faces(c, ['sandstone'] * 4 + ['sandstone', 'sandstone'], light)[:-1]
    return out


def naga_balustrade(spec: dict, run, light: dict) -> list:
    """A naga balustrade ("nagas": [degrees from, degrees to, distance],
    "naga": "height" of the posts, "body" radius): squat sandstone posts
    carrying the serpent's scaly body along the arc, at the "from" end its
    seven heads rising in a fan (the hood), as along Angkor's causeways."""
    a0, a1, dist = run
    h, rb = spec.get('height', 60), spec.get('body', 14)
    out = []
    n = max(2, int(round(np.radians(abs(a1 - a0)) * dist / spec.get('post', 110))))
    for k in range(n + 1):
        a = a0 + (a1 - a0) * k / n
        px, pz = polar([a, dist])
        r = np.radians(a)
        ux, uz, vx, vz = np.cos(r), -np.sin(r), np.sin(r), np.cos(r)
        sq = lambda half, y: [(px + half * (sx * ux + sy * vx), y, pz + half * (sx * uz + sy * vz))
                              for sx, sy in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
        out += box_faces(sq(15, 0) + sq(12, -h), ['sandstone'] * 4 + ['sandstone', 'sandstone'], light)[:-1]
    m = max(4, n * 2)
    path = [(x, -h - rb + 2, z) for x, z in (polar([a0 + (a1 - a0) * k / m, dist]) for k in range(m + 1))]
    # the neck rising out of the body's end, bent back toward the hood
    r = np.radians(a0)
    tx, tz = -np.cos(r) * np.sign(a1 - a0), np.sin(r) * np.sign(a1 - a0)   # on past the "from" end
    ex, _, ez = path[0]
    neck = [(ex + tx * d, -h - rb + 2 - lift, ez + tz * d) for d, lift in ((22, 12), (36, 40), (40, 72))]
    out += tube(path[::-1] + neck, [rb] * (m + 1) + [rb, rb * 0.9, rb * 0.8], 6, 'naga', light)
    # the hood: seven heads in a fan on the neck's top, the middle one
    # highest, turned toward the fight (in the plane along the arc)
    cx, cy, cz = neck[-1]
    ux, uz = tx, tz
    for k in range(7):
        th = np.radians(-72 + 24 * k)
        dx, dy = np.sin(th), -np.cos(th)                    # out from the hood's middle (up is -y)
        px, py = np.cos(th), np.sin(th)
        R, w0, w1 = 46, 6, 11
        q = [(cx + ux * (dx * R - px * w1), cy + dy * R - py * w1, cz + uz * (dx * R - px * w1)),
             (cx + ux * (dx * R + px * w1), cy + dy * R + py * w1, cz + uz * (dx * R + px * w1)),
             (cx - ux * px * w0, cy - py * w0, cz - uz * px * w0), (cx + ux * px * w0, cy + py * w0, cz + uz * px * w0)]
        out += both_ways(face(q, 'naga', light, (cx + ex / dist * 100, cy, cz + ez / dist * 100)))
        hx, hy, hz = cx + ux * dx * (R + 9), cy + dy * (R + 9), cz + uz * dx * (R + 9)
        rx, rz = -uz, ux                                    # toward and away from the fight
        c = [(hx + ux * sx * 7 + rx * sz * 7, hy + sy, hz + uz * sx * 7 + rz * sz * 7)
             for sy in (7, -7) for sx, sz in ((-1, -1), (1, -1), (1, 1), (-1, 1))]
        out += box_faces(c, ['sandstone'] * 6, light)
    return out


def fallen_blocks(spec, light: dict, rnd) -> list:
    """Fallen sandstone and laterite blocks ("khmer_blocks": [degrees,
    distance, size, picture]): wall_rubble()'s, of the temple's stone."""
    return wall_rubble(spec[:2], spec[2], light, rnd, spec[3] if len(spec) > 3 else 'sandstone')


def laterite_steps(spec: dict, light: dict) -> list:
    """A flight of laterite steps going nowhere ("terraces": {"at",
    "count", "rise", "tread", "width", "top"}): steps()'s, the temple's
    platform long gone."""
    at, out = spec['at'], []
    n, rise, tread, w = spec.get('count', 3), spec.get('rise', 22), spec.get('tread', 40), spec.get('width', 300)
    for k in range(n):
        d0, d1 = tread * k, tread * (k + 1) + (spec.get('top', 80) if k == n - 1 else 0)
        foot = [(-w / 2, 0, d0), (w / 2, 0, d0), (w / 2, 0, d1), (-w / 2, 0, d1)]
        c = posed(foot + [(x, -rise * (k + 1), z) for x, _, z in foot], 0, at[0], at, 0)
        f = box_faces(c, ['laterite'] * 4 + ['laterite', 'laterite'], light)
        f.pop()
        out += f
    return out


def strangler_fig(spec: dict, light: dict, rnd) -> list:
    """A great strangler fig ("fig": {"at", "height" of the tree, "roots",
    "reach" of them, "crown" its radius, "leaves"}): its trunk flaring into
    buttresses at the foot, its roots snaking away over the ground all
    round, forking as they go; the trunk forking near the top, a wide dome
    of small leaf clusters over it."""
    cx0, cz0 = polar(spec['at'])
    H, reach = spec.get('height', 650), spec.get('reach', 320)
    out = []
    # the roots: from up the trunk down to the ground, then winding away
    # along it, thinner as they go, a fork off each now and then
    for k in range(spec.get('roots', 9)):
        a = 2 * np.pi * (k + rnd.uniform(-0.25, 0.25)) / spec.get('roots', 9)
        ln = reach * rnd.uniform(0.6, 1.1)
        wig, ph = rnd.uniform(0.15, 0.35), rnd.uniform(0, 2 * np.pi)
        r0 = rnd.uniform(13, 17)
        radii = [r0 * 1.3, r0, r0 * 0.7, r0 * 0.45, r0 * 0.2]
        pts = []
        for f, r in zip((0.0, 0.08, 0.3, 0.6, 1.0), radii):
            d = 18 + ln * f
            b = a + wig * np.sin(4 * f + ph) * f
            pts.append((cx0 + d * np.sin(b), -90 if f == 0 else -0.8 * r, cz0 + d * np.cos(b)))
        out += tube(pts, radii, 3, 'fig_root', light)
        if k % 2 == 0:
            p0 = pts[2]
            b = a + rnd.choice([-1, 1]) * rnd.uniform(0.5, 0.9)
            fork = [p0] + [(p0[0] + d * np.sin(b), -3, p0[2] + d * np.cos(b)) for d in (0.25 * ln, 0.5 * ln)]
            out += tube(fork, [r0 * 0.55, r0 * 0.35, r0 * 0.15], 3, 'fig_root', light)
    # the trunk, leaning, forking near the top
    trunk = [(cx0, 0, cz0), (cx0 + 10, -0.45 * H, cz0 + 6), (cx0 - 8, -0.78 * H, cz0 - 6)]
    out += tube(trunk, [34, 24, 18], 6, 'fig_root', light)
    tops = []
    for dx, dz in ((-80, 20), (70, -10), (10, 70)):
        end = (cx0 - 8 + dx, -H, cz0 - 6 + dz)
        out += tube([trunk[-1], end], [15, 8], 5, 'fig_root', light)
        tops.append(end)
    # the crown: small clusters of leaves crossed all over a wide dome
    R = spec.get('crown', 230)
    cx, cy, cz = np.mean(np.array(tops, float), 0)
    for k in range(spec.get('leaves', 34)):
        a = rnd.uniform(0, 2 * np.pi)
        r = R * np.sqrt(rnd.uniform(0.15, 1))
        px, pz = cx + r * np.sin(a), cz + r * np.cos(a)
        py = cy - 10 - 110 * (1 - (r / R) ** 2) * rnd.uniform(0.5, 1.1)
        size = rnd.uniform(100, 140)
        spin = rnd.uniform(0, np.pi)
        lv = 0x80 * (0.75 + 0.15 * (1 - r / R) + 0.15 * rnd.uniform(0, 1))
        out += card((px, py + size / 2, pz), (np.cos(spin), 0, np.sin(spin)), (0, -1, 0), size, size, 'fig_leaves',
                    (0, 63), (0, 63), (lv * 0.92, lv, lv * 0.86), 0.75)
    return out


def sugar_palm(spec: dict, at, light: dict, rnd) -> list:
    """A sugar palm ("palms": [degrees, distance, height], "palm": "fronds",
    "frond" size): a tall slender ringed trunk, a little bent, swelling
    near its top, a round crown of stiff fan leaves on short stalks all
    round (the Cambodia picture's, stirring in the wind)."""
    H = at[2] if len(at) > 2 else spec.get('height', 1000)
    cx, cz = polar(at[:2])
    lean, dirn = rnd.uniform(0, 0.05), rnd.uniform(0, 2 * np.pi)
    off = lambda t: (H * lean * t ** 2 * np.sin(dirn), H * lean * t ** 2 * np.cos(dirn))
    path = [(cx + off(t)[0], -H * t, cz + off(t)[1]) for t in (0, 0.35, 0.7, 0.92, 1.0)]
    out = tube(path, [24, 19, 17, 20, 15], 5, 'palm_trunk', light, turn=0.5)
    tx, ty, tz = path[-1]
    S = spec.get('frond', 190)
    flat = {'azimuth': 0, 'elevation': 90, 'ambient': [0.95, 1.0, 0.92], 'sun': [0, 0, 0]}
    n = spec.get('fronds', 18)
    for k in range(n):
        # each leaf out along a direction of a ball round the crown
        e = np.arcsin(1 - 2 * (k + 0.5) / n) * 0.85 + 0.15
        a = k * 2.39996 + rnd.uniform(-0.2, 0.2)
        d = np.array([np.cos(e) * np.sin(a), -np.sin(e), np.cos(e) * np.cos(a)])
        side = np.cross(d, [0, 1.0, 0]); side /= max(1e-6, np.linalg.norm(side))
        twist = rnd.uniform(-0.6, 0.6)
        side = side * np.cos(twist) + np.cross(d, side) * np.sin(twist)
        foot = np.array([tx, ty, tz]) + d * 45
        out += tube([(tx, ty, tz), tuple(foot)], [4, 3], 3, 'palm_trunk', light)
        lv = 0x80 * (0.75 + 0.25 * (np.sin(e) + 1) / 2)
        out += card(foot, side, d, S, S * 0.9, 'flags', PALM_FROND[0], PALM_FROND[1], tuple(lv * v for v in flat['ambient']), 0.75)
    return out


def stilt_house(spec: dict, light: dict) -> list:
    """A fisher's house on stilts ("stilt_house": {"at", "yaw", "width",
    "depth", "floor" height, "walls" height, "roof"}): wooden posts, a plank
    floor with a ledge before the door, matting walls, a dark doorway and
    a window toward the fight, a steep thatched roof over gable ends, a
    ladder up to the ledge."""
    at, yaw = spec['at'], spec.get('yaw', 0)
    w, d, fh, wh, rh = spec.get('width', 380), spec.get('depth', 280), spec.get('floor', 210), spec.get('walls', 190), \
        spec.get('roof', 170)
    place = local(at, yaw)
    out = []
    for x in (-w / 2 + 12, 0, w / 2 - 12):
        for z in (-d / 2 + 12, d / 2 - 12, d / 2 + 70):
            out += tube(place([(x, 6, z), (x, -fh, z)]), [7, 6], 4, 'wood', light)
    c = place([(-w / 2, -fh + 14, -d / 2), (w / 2, -fh + 14, -d / 2), (w / 2, -fh + 14, d / 2 + 80), (-w / 2, -fh + 14, d / 2 + 80),
               (-w / 2, -fh, -d / 2), (w / 2, -fh, -d / 2), (w / 2, -fh, d / 2 + 80), (-w / 2, -fh, d / 2 + 80)])
    out += box_faces(c, ['plank'] * 4 + ['plank', 'plank'], light)
    y0, y1 = -fh, -fh - wh
    c = place([(-w / 2 + 6, y0, -d / 2 + 6), (w / 2 - 6, y0, -d / 2 + 6), (w / 2 - 6, y0, d / 2 - 6), (-w / 2 + 6, y0, d / 2 - 6),
               (-w / 2 + 6, y1, -d / 2 + 6), (w / 2 - 6, y1, -d / 2 + 6), (w / 2 - 6, y1, d / 2 - 6), (-w / 2 + 6, y1, d / 2 - 6)])
    inside = np.mean(np.array(c, float), 0)
    for j in range(4):
        i = (j + 1) % 4
        out += grid_faces(*wound([c[4 + j], c[4 + i], c[j], c[i]], inside), 'matting', light, inside, size=110)
    # the doorway and a window on the side toward the fight (+z), a little before it
    fz = d / 2 - 4
    for x0, x1, ya, yb in ((-w * 0.32, -w * 0.08, y0, y0 - wh * 0.8), (w * 0.12, w * 0.34, y0 - wh * 0.35, y0 - wh * 0.7)):
        q = place([(x0, yb, fz), (x1, yb, fz), (x0, ya, fz), (x1, ya, fz)])
        out.append(face(wound(q, inside), 'torii_black', light, inside))
    # the roof: two thatched slopes over the eaves, the gables matting
    ov, ridge = 50, y1 - rh
    for s in (-1, 1):
        q = place([(-w / 2 - ov, ridge, 0), (w / 2 + ov, ridge, 0), (-w / 2 - ov, y1 + 20, s * (d / 2 + ov)),
                   (w / 2 + ov, y1 + 20, s * (d / 2 + ov))])
        out += both_ways(face(q, 'thatch', light, place([(0, 0, 0)])[0]))
    for x in (-w / 2 + 6, w / 2 - 6):
        q = place([(x, ridge + 6, 0), (x, ridge + 6, 0), (x, y1, -d / 2 + 6), (x, y1, d / 2 - 6)])
        out += both_ways(face(q, 'matting', light, place([(0, y1, 0)])[0]))
    # the ladder: two rails from the ground to the ledge, rungs across
    lz0, lz1 = d / 2 + 80 + 90, d / 2 + 78
    for x in (-w * 0.2 - 22, -w * 0.2 + 22):
        out += tube(place([(x, 0, lz0), (x, -fh, lz1)]), [4, 4], 3, 'bamboo', light)
    for k in range(1, 6):
        t = k / 6
        z, y = lz0 + (lz1 - lz0) * t, -fh * t
        out += tube(place([(-w * 0.2 - 22, y, z), (-w * 0.2 + 22, y, z)]), [3, 3], 3, 'bamboo', light)
    return out


def longtail_boat(spec: dict, light: dict) -> list:
    """A longtail boat drawn up on the bank ("boat": {"at", "yaw",
    "length", "width", "depth"}): a narrow wooden hull, its bow sweeping up
    in a point, the stern square, painted bands along its sides, two seats
    across; its long engine shaft lying out behind."""
    at, yaw = spec['at'], spec.get('yaw', 0)
    L, W, D = spec.get('length', 560), spec.get('width', 96), spec.get('depth', 46)
    place = local(at, yaw)
    ts = np.linspace(0, 1, 9)
    half = lambda t: W / 2 * (np.clip(np.sin(np.pi * (0.12 + 0.88 * t)) ** 0.7, 0.05, 1) if t > 0.4 else
                               0.75 + 0.25 * np.sin(np.pi * t / 0.8))
    sheer = lambda t: -D - 90 * max(0.0, t - 0.8) ** 2 / 0.04 * 0.6
    out = []
    for k in range(len(ts) - 1):
        a, b = ts[k], ts[k + 1]
        xa, xb = -L / 2 + L * a, -L / 2 + L * b
        for s in (-1, 1):
            q = place([(xa, sheer(a), s * half(a)), (xb, sheer(b), s * half(b)), (xa, 0, s * half(a) * 0.45),
                       (xb, 0, s * half(b) * 0.45)])
            out += both_ways(face(q, 'boat', light, place([(0, -D / 2, 0)])[0]))
        q = place([(xa, 0, -half(a) * 0.45), (xb, 0, -half(b) * 0.45), (xa, 0, half(a) * 0.45), (xb, 0, half(b) * 0.45)])
        out.append(face(q, 'plank', light, place([(0, 100, 0)])[0]))
    q = place([(-L / 2, sheer(0), -half(0)), (-L / 2, sheer(0), half(0)), (-L / 2, 0, -half(0) * 0.45), (-L / 2, 0, half(0) * 0.45)])
    out += both_ways(face(q, 'boat', light, place([(0, -D / 2, 0)])[0]))
    for x in (-L * 0.15, L * 0.2):
        hw = half(0.5 + x / L) - 4
        c = place([(x - 12, -D + 8, -hw), (x + 12, -D + 8, -hw), (x + 12, -D + 8, hw), (x - 12, -D + 8, hw),
                   (x - 12, -D + 2, -hw), (x + 12, -D + 2, -hw), (x + 12, -D + 2, hw), (x - 12, -D + 2, hw)])
        out += box_faces(c, ['plank'] * 6, light)
    out += tube(place([(-L / 2 + 10, -D - 10, 0), (-L / 2 - 260, 4, 30)]), [5, 3], 4, 'iron', light)
    c = place([(-L / 2 + 10 + sx, -D - 10 + sy, sz) for sy in (0, -34) for sx, sz in ((-20, -18), (20, -18), (20, 18), (-20, 18))])
    out += box_faces(c, ['iron'] * 6, light)
    return out


def net_poles(spec: dict, run, light: dict) -> list:
    """A fishing net hung out to dry ("nets": [degrees from, degrees to,
    distance], "net": "height", "drop"): bamboo poles along the arc, the
    net hung between them, sagging, seen from both sides."""
    a0, a1, dist = run
    H, drop = spec.get('height', 250), spec.get('drop', 170)
    n = max(1, int(round(np.radians(abs(a1 - a0)) * dist / 220)))
    pts = [polar([a0 + (a1 - a0) * k / n, dist]) for k in range(n + 1)]
    out = []
    for x, z in pts:
        out += tube([(x, 4, z), (x, -H - 20, z)], [5, 4], 4, 'bamboo', light)
    flat = {'azimuth': 0, 'elevation': 90, 'ambient': [0.9, 0.9, 0.9], 'sun': [0, 0, 0]}
    for (xa, za), (xb, zb) in zip(pts, pts[1:]):
        for j in range(3):
            s0, s1 = j / 3, (j + 1) / 3
            sag = lambda s: -H + 40 * np.sin(np.pi * s)
            p = lambda s, y: (xa + (xb - xa) * s, y, za + (zb - za) * s)
            q = [p(s0, sag(s0)), p(s1, sag(s1)), p(s0, sag(s0) + drop), p(s1, sag(s1) + drop)]
            f = face(q, 'net', flat, (0, 0, 0), cols=(round(63 * s0), round(63 * s1)))
            out += both_ways(f)
    return out


def lotus_pond(spec: dict, at, rnd) -> list:
    """A lotus pool on the ground ("lotus_ponds": [degrees, distance,
    width, depth, flowers]): the Cambodia picture's patch of pond seen from
    above lying flat (drawn first, under the rest), a few leaves held up
    on stalks and pink flowers standing out of it."""
    deg, dist, w, d, nf = at
    cx, cz = polar([deg, dist])
    r = np.radians(deg)
    ux, uz, vx, vz = np.cos(r), -np.sin(r), np.sin(r), np.cos(r)
    q = [(cx + sx * w / 2 * ux + sy * d / 2 * vx, -1, cz + sx * w / 2 * uz + sy * d / 2 * vz) for sy in (1, -1) for sx in (-1, 1)]
    uv = [(0, LOTUS_POND[0][0]), (63, LOTUS_POND[0][0]), (0, LOTUS_POND[0][1]), (63, LOTUS_POND[0][1])]
    if wound(q, (cx, 1000, cz)) is not q:
        q, uv = [q[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
    lv = (0x7C, 0x80, 0x80)
    out = [dict(corners=[tuple(int(round(v)) for v in p) for p in q], tex='flags', rows=LOTUS_POND[0], var=0, uv=uv,
                colors=[lv] * 4, under=True)]
    for k in range(nf):
        sx, sy = rnd.uniform(-0.38, 0.38) * w, rnd.uniform(-0.3, 0.3) * d
        px, pz = cx + sx * ux + sy * vx, cz + sx * uz + sy * vz
        spin = rnd.uniform(0, np.pi)
        along = (np.cos(spin), 0, np.sin(spin))
        if k % 3 == 2:
            out += card((px, 0, pz), along, (0, -1, 0), 44, 44, 'flags', LOTUS_LEAF[0], LOTUS_LEAF[1], (0x78, 0x80, 0x78), 0.8)
        else:
            s = rnd.uniform(34, 46)
            out += card((px, 0, pz), along, (0, -1, 0), s, s * 1.2, 'flags', LOTUS_FLOWER[0], LOTUS_FLOWER[1], (0x84, 0x80, 0x80), 0.85)
    return out


def egret_picture(vr: Vram):
    """Egrets' colours in the gull's regions: "dark", "light" and "white"
    all white (a touch of grey in the wings), in the yellow corner's
    quarters "red" the legs' black and "horn" the beak's yellow."""
    rnd = np.random.default_rng(14)
    img = np.zeros((64, 64, 3))
    for (x, y, w), rgb in (((0, 0, 32), (240, 242, 240)), ((32, 0, 32), (226, 228, 228)), ((0, 32, 32), (248, 248, 246)),
                           ((32, 32, 32), (230, 196, 60)), ((32, 32, 16), (26, 24, 24))):
        img[y:y + w, x:x + w] = np.clip(np.array(rgb) * (1 + 0.04 * rnd.normal(0, 1, (w, w)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def egret_model(pose: str, scale: float = 1.0) -> list[dict]:
    """A crane's faces (crane_model()) as a great egret's: all white, no
    crown, its long legs black (the "red" quarter of egret_picture())."""
    faces = crane_model(pose, 1.0)
    out = []
    for f in faces:
        if f['region'] == 'red':
            continue                                   # no crown
        zs = [c[2] for c in f['corners']]
        if f['region'] == 'light' and max(zs) <= -18 and min(c[1] for c in f['corners']) >= 5:
            f['region'] = 'red'                        # the legs
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
        out.append(f)
    return out


# --- Athens: the Acropolis' marbles among the olives -------------------------

def doric_colonnade(spec: dict, light: dict) -> list:
    """A row of Doric columns on their stepped stylobate ("colonnades":
    {"at" [degrees, distance], "yaw", "columns", "spacing", "radius",
    "height", "broken" [each column's height, 1 whole], "entablature" [first,
    last] the columns still carrying it}): fluted shafts tapering up, the
    whole ones crowned with the echinus and the square abacus, the broken
    ones ending low; over the whole ones the architrave, the frieze of
    triglyphs and metopes and the cornice."""
    at, yaw = spec['at'], spec.get('yaw', 0)
    n, gap, r, H = spec.get('columns', 6), spec.get('spacing', 190), spec.get('radius', 40), spec.get('height', 520)
    broken = spec.get('broken', [1] * n)
    place = local(at, yaw)
    W, D, rise = (n - 1) * gap + 3.4 * r, 3.4 * r, 18
    out = []
    for k in range(3):
        hw, hd = W / 2 + (2 - k) * 22, D / 2 + (2 - k) * 22
        foot = [(-hw, -rise * k, -hd), (hw, -rise * k, -hd), (hw, -rise * k, hd), (-hw, -rise * k, hd)]
        c = place(foot + [(x, -rise * (k + 1), z) for x, _, z in foot])
        out += box_faces(c, ['pentelic'] * 6, light)[:-1]
    y0, sides = -3 * rise, 10
    tops = {}
    for i in range(n):
        x = -(n - 1) * gap / 2 + i * gap
        (cx, _, cz), = place([(x, 0, 0)])
        h = H * broken[i]
        turn = np.radians(at[0] + yaw) + i
        out += turned([(y0, r, (0, 63)), (y0 - h, r * (1 - 0.16 * h / H), (0, 63))], sides, cx, cz, 'flute', light, turn=turn,
                      cap='pentelic' if broken[i] < 1 else False, wrap=2)
        if broken[i] >= 1:
            top = y0 - h
            out += turned([(top, r * 0.84, (0, 63)), (top - 12, r * 1.14, (0, 63)), (top - 20, r * 1.3, (0, 63))], sides, cx, cz,
                          'pentelic', light, turn=turn, cap=False)
            a = 1.32 * r
            c = place([(x + sx, top - 20 - dy, sz) for dy in (0, 13) for sx, sz in ((-a, -a), (a, -a), (a, a), (-a, a))])
            out += box_faces(c, ['pentelic'] * 6, light)[:-1]
            tops[i] = top - 33
    if spec.get('entablature'):
        i0, i1 = spec['entablature']
        x0, x1 = -(n - 1) * gap / 2 + i0 * gap - 1.32 * r, -(n - 1) * gap / 2 + i1 * gap + 1.32 * r
        y = min(tops[i] for i in range(i0, i1 + 1))
        a = 1.32 * r
        for y_a, y_b, grow, tex in ((y, y - 0.95 * r, 0, 'pentelic'), (y - 0.95 * r, y - 1.95 * r, 0, 'doric_frieze'),
                                    (y - 1.95 * r, y - 2.35 * r, 14, 'pentelic')):
            c = place([(xx, yy, zz) for yy in (y_a, y_b) for xx, zz in ((x0 - grow, -a - grow), (x1 + grow, -a - grow),
                                                                         (x1 + grow, a + grow), (x0 - grow, a + grow))])
            inside = np.mean(np.array(c, float), 0)
            if tex == 'doric_frieze':
                for j in (0, 2):
                    i = (j + 1) % 4
                    out += grid_faces(*wound([c[4 + j], c[4 + i], c[j], c[i]], inside), tex, light, inside, size=gap / 2)
                for j in (1, 3):
                    i = (j + 1) % 4
                    out.append(face(wound([c[4 + j], c[4 + i], c[j], c[i]], inside), 'pentelic', light, inside))
                out.append(face(wound([c[0], c[1], c[3], c[2]], inside), 'pentelic', light, inside))
            else:
                out += box_faces(c, ['pentelic'] * 6, light)
    return out


def column_drum(spec, light: dict, rnd) -> list:
    """A fallen column drum ("column_drums": [degrees, distance, radius,
    length, lying]): fluted, standing on its end or lying on its side
    half sunk in the ground, its ends plain marble."""
    deg, dist, r, length = spec[:4]
    cx, cz = polar([deg, dist])
    if len(spec) > 4 and spec[4]:
        a = rnd.uniform(0, np.pi)
        ux, uz = np.cos(a) * length / 2, np.sin(a) * length / 2
        y = -r * 0.8
        out = tube([(cx - ux, y, cz - uz), (cx + ux, y, cz + uz)], [r, r * 0.97], 10, 'flute', light)
        d = np.array([ux, 0, uz]) / max(1e-6, np.hypot(ux, uz))
        u = np.cross(d, [0, 1.0, 0]); u /= np.linalg.norm(u)
        for s, (ex, ez) in ((-1, (cx - ux, cz - uz)), (1, (cx + ux, cz + uz))):
            ring = [np.array([ex, y, ez]) + r * (np.cos(t) * u + np.sin(t) * np.array([0, 1.0, 0]))
                    for t in 2 * np.pi * np.arange(11) / 10]
            centre = (ex, y, ez)
            for k in range(10):
                out += both_ways(face([ring[k], ring[k + 1], centre, centre], 'pentelic', light,
                                      (ex - s * d[0] * 100, y, ez - s * d[2] * 100)))
        return above_ground(out)
    return turned([(0, r, (0, 63)), (-length, r, (0, 63))], 10, cx, cz, 'flute', light, turn=rnd.uniform(0, 1), cap='pentelic',
                  wrap=2)


def ionic_capital(spec, light: dict, rnd) -> list:
    """An Ionic capital fallen in the grass ("ionic_capitals": [degrees,
    distance, size]): its volutes' face toward the fight and away, the
    bolsters at its ends, tipped a little, sunk a little."""
    s = spec[2]
    w, d, h = 1.4 * s, 0.6 * s, 0.5 * s
    foot = [(-w, 0, -d), (w, 0, -d), (w, 0, d), (-w, 0, d)]
    c = posed(foot + [(x, -h, z) for x, _, z in foot], rnd.uniform(-8, 8), spec[0] + 180 + rnd.uniform(-25, 25), spec[:2],
              h * 0.12)
    return above_ground(box_faces(c, ['ionic', 'pentelic', 'ionic', 'pentelic', 'pentelic', 'pentelic'], light))


def architrave_piece(spec, light: dict, rnd) -> list:
    """A length of architrave fallen ("architraves": [degrees, distance,
    length]): its three fasciae along both faces, tipped, sunk."""
    L, h, d = spec[2] / 2, 34, 28
    foot = [(-L, 0, -d), (L, 0, -d), (L, 0, d), (-L, 0, d)]
    c = posed(foot + [(x, -2 * h, z) for x, _, z in foot], rnd.uniform(-6, 6), spec[0] + 90 + rnd.uniform(-30, 30), spec[:2],
              h * 0.2)
    return above_ground(box_faces(c, ['architrave', 'pentelic', 'architrave', 'pentelic', 'pentelic', 'pentelic'], light))


def olive_tree(spec: dict, at, light: dict, rnd) -> list:
    """An old olive ("olives": [degrees, distance, height], "olive":
    "crown" radius, "clumps" of leaves on each limb): a short thick trunk,
    gnarled, twisting and swelling, split in three or four limbs leaning
    out and up; on each limb's end a rounded clump of the Athens
    picture's leaf clumps, crossed every way, the clumps together a broad
    low crown, lit from above (the lower leaves darker), stirring."""
    H = at[2] if len(at) > 2 else spec.get('height', 380)
    cx, cz = polar(at[:2])
    out = []
    lean = rnd.uniform(0, 2 * np.pi)
    trunk = []
    for t in (0, 0.2, 0.42, 0.62, 0.8):
        wob = 18 * t + 6 * np.sin(t * 9 + lean)
        trunk.append((cx + wob * np.sin(lean + t * 2.5), -H * 0.42 * t, cz + wob * np.cos(lean + t * 2.5)))
    out += tube(trunk, [40, 30, 33, 26, 24], 6, 'olive_bark', light, turn=0.5)
    tx, ty, tz = trunk[-1]
    R = spec.get('crown', 220)
    limbs = int(rnd.integers(3, 5))          # a tile holds 256 corners: few limbs, few clumps
    ends = []
    for k in range(limbs):
        a = lean + 2 * np.pi * k / limbs + rnd.uniform(-0.35, 0.35)
        reach = R * rnd.uniform(0.4, 0.62)
        rise = H * rnd.uniform(0.66, 0.8)
        mid = (tx + 0.5 * reach * np.sin(a + 0.3), (ty - rise) / 2 - 10, tz + 0.5 * reach * np.cos(a + 0.3))
        end = (tx + reach * np.sin(a), -rise, tz + reach * np.cos(a))
        out += tube([(tx, ty, tz), mid, end], [17, 11, 6], 4, 'olive_bark', light, turn=0.6)
        ends.append((end, a))
    ends.append(((tx, -H * 0.86, tz), 0.0))                  # the crown's top over the trunk
    for (ex, ey, ez), a in ends:
        n = spec.get('clumps', 7)
        size = R * 0.62
        for j in range(n):
            # round the limb's end: a ball of crossed leaf clumps
            th, ph = rnd.uniform(0, 2 * np.pi), rnd.uniform(-0.5, 1.0)
            r = size * 0.45 * rnd.uniform(0.3, 1.0)
            px, py, pz = ex + r * np.cos(ph) * np.sin(th), ey - r * 0.7 * np.sin(ph), ez + r * np.cos(ph) * np.cos(th)
            spin = th + np.pi / 2 + rnd.uniform(-0.4, 0.4)
            s = size * rnd.uniform(0.7, 1.0)
            bough = j % 3 == 0
            lit = 0.82 + 0.18 * np.clip((ey - py) / size + 0.5, 0, 1)
            lv = tuple(min(255, 0x80 * lit * v) for v in (0.96, 1.0, 0.94))
            rows, cols = OLIVE_BOUGH if bough else OLIVE_SPRAY
            w, h = (s * 1.5, s * 0.75) if bough else (s, s)
            out += card((px, py + h / 2, pz), (np.cos(spin), 0, np.sin(spin)), (0, -1, 0), w, h, 'flags', rows, cols, lv, 0.82)
    return out


def caryatid_figure(x: float, y0: float, z: float, k: float, turn: float, light: dict) -> list:
    """A maiden carrying the roof on her head: a turned figure (the hem, the
    skirt, the waist, the breast, the shoulders, the neck, the head),
    her basket and its abacus on top."""
    # few rings, six sides: a tile holds 256 corners, the porch six maidens
    prof = [(0, 21, (56, 63)), (-22, 22, (44, 56)), (-126, 15, (17, 44)), (-160, 19, (10, 17)), (-176, 6.5, (8, 10)),
            (-196, 9.5, (2, 8)), (-204, 8, (0, 2))]
    out = turned([(y0 + k * y, k * r, rows) for y, r, rows in prof], 6, x, z, 'caryatid', light, turn=turn, cap=False)
    top = y0 - 204 * k
    out += turned([(top, 8 * k, (0, 63)), (top - 16 * k, 17 * k, (0, 63))], 6, x, z, 'pentelic', light, turn=turn, cap=False)
    return out


def caryatid_porch(spec: dict, light: dict) -> list:
    """The Porch of the Maidens ("caryatid_porch": {"at", "yaw", "width",
    "depth", "podium" height, "figure" height}): a marble podium, on it
    four maidens in front and one at each back corner carrying the
    architrave (its fasciae round it) and the flat roof slab over them."""
    at, yaw = spec['at'], spec.get('yaw', 0)
    w, d, ph, fh = spec.get('width', 400), spec.get('depth', 260), spec.get('podium', 150), spec.get('figure', 210)
    place = local(at, yaw)
    out = []
    c = place([(x, y, z) for y in (0, -ph) for x, z in ((-w / 2, -d / 2), (w / 2, -d / 2), (w / 2, d / 2), (-w / 2, d / 2))])
    out += box_faces(c, ['pentelic'] * 6, light)[:-1]
    k = fh / 210
    spots = [(-w / 2 + 40 + (w - 80) * i / 3, d / 2 - 38) for i in range(4)] + [(-w / 2 + 40, -d / 2 + 38), (w / 2 - 40, -d / 2 + 38)]
    for i, (x, z) in enumerate(spots):
        (px, _, pz), = place([(x, 0, z)])
        out += caryatid_figure(px, -ph, pz, k, np.radians(at[0] + yaw + 180) + np.pi / 8, light)
    y = -ph - fh
    for y_a, y_b, grow, texs in ((y, y - 46, 8, ['architrave'] * 4 + ['pentelic', 'pentelic']),
                                 (y - 46, y - 76, 26, ['pentelic'] * 6)):
        c = place([(xx, yy, zz) for yy in (y_a, y_b) for xx, zz in ((-w / 2 - grow, -d / 2 - grow), (w / 2 + grow, -d / 2 - grow),
                                                                     (w / 2 + grow, d / 2 + grow), (-w / 2 - grow, d / 2 + grow))])
        out += box_faces(c, texs, light)
    return out


def stele(spec, light: dict, rnd) -> list:
    """A marble stele with a decree cut in it ("steles": [degrees,
    distance]): a low base block, the slab standing in it, its faces
    lettered, leaning a little."""
    out = box_faces(posed([(x, y, z) for y in (0, -30) for x, z in ((-50, -26), (50, -26), (50, 26), (-50, 26))], 0, spec[0] + 180,
                          spec[:2]), ['pentelic'] * 6, light)[:-1]
    lean = rnd.uniform(-3, 3)
    c = posed([(x, y, z) for y in (-30, -180) for x, z in ((-36, -10), (36, -10), (36, 10), (-36, 10))], lean, spec[0] + 180,
              spec[:2])
    out += box_faces(c, ['inscription', 'pentelic', 'inscription', 'pentelic', 'pentelic', 'pentelic'], light)[:-1]
    return out


def odeon_wall(spec: dict, light: dict) -> list:
    """A fragment of the Odeon's curved outer wall ("odeon": {"from", "to"
    degrees, "distance", "height", "thick"}): brown stone along the arc,
    tiers of round arches on both its faces, its ends broken down in steps."""
    a0, a1, R = spec['from'], spec['to'], spec['distance']
    H, t = spec.get('height', 520), spec.get('thick', 70)
    n = max(2, int(round(np.radians(abs(a1 - a0)) * R / spec.get('bay', 180))))
    out = []
    for k in range(n):
        u = (k + 0.5) / n
        h = H * min(1.0, 0.45 + 1.4 * min(u, 1 - u)) if spec.get('ruined', True) else H
        h = H * round(h / H * 8) / 8
        sa, sb = a0 + (a1 - a0) * k / n, a0 + (a1 - a0) * (k + 1) / n
        c = arc_box(sa, sb, R, R + t, -h)
        inside = np.mean(np.array(c, float), 0)
        rows = (int(round(63 * (1 - h / H))), 63)
        for j in range(4):
            i = (j + 1) % 4
            q = wound([c[4 + j], c[4 + i], c[j], c[i]], inside)
            out.append(face(q, 'odeon', light, inside, rows=(0, 7) if j in (0, 2) else rows))
        out.append(face(wound([c[4], c[5], c[7], c[6]], inside), 'odeon', light, inside, rows=(0, 7)))
    return out


# --- Nowhere Desert: red rock under the moon -----------------------------------

def flat_level(light: dict, k: float = 1.0) -> tuple:
    """A flat lit picture's colour (a card's): the ambient light and half
    the sun's (the moon's), times k."""
    rgb = (np.array(light['ambient'], float) + 0.5 * np.array(light['sun'], float)) * 0x80 * k
    return tuple(int(v) for v in np.clip(np.round(rgb), 0, 255))


def juniper(spec: dict, at, light: dict) -> list:
    """A dead juniper ("junipers": [degrees, distance, height, seed]), as
    the canyon's (dead_tree()): twisted trunk, bare forking branches."""
    tree = dict(spec, at=at[:2], height=at[2] if len(at) > 2 else spec.get('height', 200),
                seed=at[3] if len(at) > 3 else spec.get('seed', 4), perch_azimuth=at[0] + 70)
    return dead_tree(tree, light)[0]


def prickly_pear(spec: dict, at, light: dict, rnd) -> list:
    """A prickly pear ("prickly_pears": [degrees, distance, pad height]): a
    few pads standing on the ground, each bearing one or two more on its
    top edge, each turned its own way, leaning out in its own plane; some
    of the top ones flowering (the picture's second)."""
    cx, cz = polar(at[:2])
    size = at[2] if len(at) > 2 else spec.get('size', 34)
    out = []
    def pad(foot, yaw, tilt, h, depth):
        along = np.array([np.cos(yaw), 0, np.sin(yaw)])
        up = np.array([np.sin(tilt) * np.cos(yaw), -np.cos(tilt), np.sin(tilt) * np.sin(yaw)])
        lv = flat_level(light, 0.85 + 0.1 * depth)
        faces = card(foot, along, up, h * 0.66, h, 'pear_pad', (0, 63), (0, 63), lv, 0.8)
        kids = int(rnd.integers(1, 3)) if depth < 2 else 0
        if depth == 2 or (depth == 1 and not kids):
            for f in faces:
                f['var'] = int(rnd.uniform() < 0.5)
        out.extend(faces)
        for k in range(kids):
            side = (k - (kids - 1) / 2) * 0.5 + rnd.uniform(-0.2, 0.2)
            top = np.array(foot) + up * h * 0.92 + along * side * h * 0.4
            pad(tuple(top), yaw + rnd.uniform(-0.6, 0.6), tilt + side * 0.9 + rnd.uniform(-0.2, 0.2), h * rnd.uniform(0.8, 0.95),
                depth + 1)
    for k in range(int(rnd.integers(2, 4))):
        a = rnd.uniform(0, 2 * np.pi)
        pad((cx + 12 * np.cos(a), 0, cz + 12 * np.sin(a)), rnd.uniform(0, np.pi), rnd.uniform(-0.25, 0.25),
            size * rnd.uniform(0.9, 1.1), 0)
    return out


def cholla(spec, light: dict, rnd) -> list:
    """A cholla ("chollas": [degrees, distance, height]): a short trunk,
    branches of spiny joints bending up and out from it, each a chain of
    short tubes."""
    cx, cz = polar(spec[:2])
    h = spec[2] if len(spec) > 2 else 130
    trunk = [(cx, 0, cz), (cx + rnd.uniform(-5, 5), -h * 0.35, cz + rnd.uniform(-5, 5))]
    out = tube(trunk, [8, 7], 5, 'cholla', light)
    for k in range(int(rnd.integers(4, 6))):
        a = 2 * np.pi * k / 5 + rnd.uniform(-0.4, 0.4)
        p = np.array(trunk[1], float) + np.array([0, rnd.uniform(0, 0.2) * h, 0])
        pts, rise = [tuple(p)], rnd.uniform(0.6, 1.1)
        for j in range(3):
            step = h * rnd.uniform(0.16, 0.24)
            d = np.array([np.cos(rise) * np.sin(a), -np.sin(rise), np.cos(rise) * np.cos(a)])
            p = p + d * step
            pts.append(tuple(p))
            rise = min(1.45, rise + rnd.uniform(0.15, 0.4)); a += rnd.uniform(-0.3, 0.3)
        out += tube(pts, [6, 5, 4.5, 3.5], 4, 'cholla', light)
    return out


def steer_skull(spec, light: dict, rnd) -> list:
    """A steer's skull left in the desert ("skulls": [degrees, distance,
    size, height]): the picture lying on the ground (or height cm up), its
    nose to the fight, its horns raised a little off the sand, and a few
    bleached bones about it."""
    cx, cz = polar(spec[:2])
    size, lift = (spec[2] if len(spec) > 2 else 70), (spec[3] if len(spec) > 3 else 0)
    a = np.radians(spec[0])
    toward = np.array([-np.sin(a), 0, -np.cos(a)])
    across = np.array([np.cos(a), 0, -np.sin(a)])
    up = -0.94 * toward + np.array([0, -0.34, 0])
    foot = np.array([cx, -lift - 1, cz]) + toward * size * 0.45
    out = card(tuple(foot), across, up / np.linalg.norm(up), size, size, 'skull', (0, 63), (0, 63), flat_level(light, 1.0), 0.9)
    for k in range(3):
        q = rnd.uniform(0, 2 * np.pi)
        bx, bz = cx + 60 * np.cos(q + k * 2), cz + 60 * np.sin(q + k * 2)
        d = np.array([np.cos(q), 0, np.sin(q)]) * rnd.uniform(14, 22)
        out += tube([(bx - d[0], -3, bz - d[2]), (bx + d[0], -3, bz + d[2])], [3.2, 2.6], 4, 'bone', light)
    return out


def petroglyph_rock(spec: dict, light: dict) -> list:
    """A boulder of varnished sandstone with petroglyphs ("petroglyphs":
    {"at" [degrees, distance], "width", "height", "depth", "lean"}): its
    pecked face turned to the fight, leaning back a little, the top
    narrower than the foot."""
    w, h, d = spec.get('width', 220), spec.get('height', 160), spec.get('depth', 80)
    pts = [(x, 0, z) for x, z in ((-w / 2, -d / 2), (w / 2, -d / 2), (w / 2, d / 2), (-w / 2, d / 2))]
    pts += [(x * 0.82, -h, z * 0.7) for x, _, z in pts]
    c = posed(pts, -spec.get('lean', 8), spec['at'][0] + 180, spec['at'], 6)
    return above_ground(box_faces(c, ['redrock', 'redrock', 'petroglyph', 'redrock', 'redrock_top', 'redrock'], light))


def barb_fence(spec: dict, light: dict, rnd) -> list:
    """An old ranch fence ("barb_fences": {"from", "to" [degrees,
    distance], "posts", "height", "broken" [spans without wire], "lean"}):
    grey weathered posts, some leaning, three strands of barbed wire
    between them."""
    a, b = np.array(polar(spec['from'])), np.array(polar(spec['to']))
    n, H = spec.get('posts', 5), spec.get('height', 120)
    along = (b - a) / max(1e-6, np.linalg.norm(b - a))
    out, tops = [], []
    for k in range(n):
        p = a + (b - a) * k / (n - 1)
        lean = np.radians(rnd.uniform(-1, 1) * spec.get('lean', 8))
        h = H * rnd.uniform(0.92, 1.05)
        top = (p[0] + np.sin(lean) * h * along[0], -h, p[1] + np.sin(lean) * h * along[1])
        out += tube([(p[0], 4, p[1]), top], [6.5, 5.5], 4, 'driftwood', light)
        tops.append(top)
    for k in range(n - 1):
        if k in spec.get('broken', []):
            continue
        m = (np.array(tops[k]) + np.array(tops[k + 1])) / 2
        span = np.array(tops[k + 1]) - np.array(tops[k])
        w = np.hypot(span[0], span[2])
        out += card((m[0], -0.12 * H, m[2]), (span[0] / w, 0, span[2] / w), (0, -1, 0), w, 0.75 * H, 'barbwire', (0, 63), (0, 63),
                    flat_level(light, 1.0), 1.0)
    return out


def radial(at) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """At [degrees, distance]: the foot (x, 0, z), the way to the fight's
    middle and the way across it (left to right as seen from the middle)."""
    a = np.radians(at[0])
    cx, cz = polar(at[:2])
    return np.array([cx, 0.0, cz]), np.array([-np.sin(a), 0, -np.cos(a)]), np.array([np.cos(a), 0, -np.sin(a)])


def trex_skull(spec: dict, light: dict) -> list:
    """The T-Rex's skull, whole, lying on the ground with its jaws open
    ("trex": {"at" [degrees, distance], "turn" degrees, "length" cm,
    "gape" degrees, "sink" cm, "sides"}): the lower jaw on the ground, the
    skull raised on the hinge at its back. Each a closed body of rings
    ("sides" round) along it, its ends capped: the skull flat underneath
    (the tooth row), narrower toward its top, its snout rounded, deep at
    the eyes and wide at the cheeks; the jaw flat on top. The openings
    (nostril, the big one before the eye, the eye socket, the one behind
    it, the jaw's) dark; long cone teeth hanging from the skull's edges,
    shorter ones standing on the jaw's, the biggest in the middle."""
    foot, toward, across = radial(spec['at'])
    t = np.radians(spec.get('turn', 0))
    axis = across * np.cos(t) + toward * np.sin(t)
    side = np.cross(axis, [0, 1.0, 0]); side /= np.linalg.norm(side)
    L = spec.get('length', 420); k = L / 360
    gape, sink = np.radians(spec.get('gape', 22)), spec.get('sink', 8)
    n = spec.get('sides', 12)
    # (along 0..1 from the snout, height, width)
    skull = [(0.0, 26, 26), (0.03, 46, 38), (0.10, 58, 48), (0.20, 66, 52), (0.32, 76, 56), (0.45, 88, 62),
             (0.56, 100, 70), (0.64, 116, 84), (0.72, 140, 104), (0.80, 152, 124), (0.88, 158, 140), (0.95, 140, 132),
             (1.0, 100, 96)]
    jaw = [(0.02, 30, 32), (0.10, 38, 42), (0.25, 44, 50), (0.45, 52, 58), (0.60, 62, 72), (0.75, 78, 92),
           (0.88, 84, 108), (0.95, 68, 100)]
    hinge, hinge_h = 0.9 * L, 72 * k

    def place(u, y, s, upper):
        if upper:
            du = u - hinge
            u, y = hinge + du * np.cos(gape) - y * np.sin(gape), y * np.cos(gape) + du * np.sin(gape) - hinge_h
        return foot + axis * (u - L / 2) + side * s + np.array([0, y + sink, 0])

    def ring(u, h, w, upper):
        out = []
        for j in range(n):
            a = 2 * np.pi * j / n
            c, sn = np.cos(a), np.sin(a)
            # the skull flat underneath, the jaw flat on top
            e = (0.22 if sn < 0 else 0.7) if upper else (0.22 if sn > 0 else 0.7)
            z = w / 2 * np.sign(c) * abs(c) ** 0.6 * (1 - (0.4 if upper else 0.15) * max(0.0, sn))
            y = -h / 2 * (1 + np.sign(sn) * abs(sn) ** e)
            out.append(place(u, y, z, upper))
        return out

    up_r, up_l = [0, 1, 2], [n // 2 - 3, n // 2 - 2, n // 2 - 1]           # the upper half's faces, right and left
    lo_r, lo_l = [n - 1, n - 2], [n // 2, n // 2 + 1]

    def shade(stations, upper, f, j):
        if upper:
            if 0.03 <= f < 0.10 and j in (up_r[1], up_l[1]): return 0.4               # the nostril
            if 0.45 <= f < 0.64 and j in (up_r[0], up_r[1], up_l[1], up_l[2]):
                return 0.3 if j in (up_r[1], up_l[1]) else 0.45                     # the big opening before the eye
            if 0.72 <= f < 0.80 and j in (up_r[1], up_r[2], up_l[0], up_l[1]):
                return 0.12 if j in (up_r[1], up_l[1]) else 0.3                     # the eye socket
            if 0.88 <= f < 0.95 and j in (up_r[0], up_l[2]): return 0.35             # behind the eye
            return None
        if 0.60 <= f < 0.75 and j in (lo_r[0], lo_l[0]): return 0.35                 # the jaw's
        return None

    def body(stations, upper):
        rings = [ring(f * L, h * k, w * k, upper) for f, h, w in stations]
        out = []
        for i in range(len(rings) - 1):
            inside = (np.mean(rings[i], 0) + np.mean(rings[i + 1], 0)) / 2
            for j in range(n):
                q = [rings[i + 1][j], rings[i + 1][(j + 1) % n], rings[i][j], rings[i][(j + 1) % n]]
                fc = face(wound(q, inside), 'fossil', light, inside, var=j)
                d = shade(stations, upper, stations[i][0], j)
                if d:
                    fc['colors'] = [tuple(int(v * d) for v in c) for c in fc['colors']]
                out.append(fc)
        for r, other in ((rings[0], rings[1]), (rings[-1], rings[-2])):
            centre = np.mean(r, 0)
            inside = (centre + np.mean(other, 0)) / 2
            for j in range(n):
                out.append(face(wound([r[j], r[(j + 1) % n], centre, centre], inside), 'fossil', light, inside, var=j))
        return out

    out = body(skull, True) + body(jaw, False)
    width = lambda tab, f: np.interp(f, [x for x, _, _ in tab], [w for _, _, w in tab]) * k
    height = lambda tab, f: np.interp(f, [x for x, _, _ in tab], [h for _, h, _ in tab]) * k
    size = lambda f: 0.55 + 0.45 * np.sin(np.pi * np.clip((f - 0.02) / 0.62, 0, 1)) ** 0.7  # the middle ones the biggest
    for f in np.arange(0.035, 0.64, 0.046):
        tl, tr = 46 * k * size(f), 8 * k * size(f)
        for sg in (-1, 1):
            s = sg * width(skull, f) * 0.4
            root = place(f * L, -6, s, True)
            tip = place(f * L + 0.18 * tl, tl, s * 1.04, True)
            out += tube([tip, root], [0, tr], 3, 'tooth', light)          # the point first: the engine culls a face by its first three corners
            if 0.06 < f < 0.6:
                s = sg * width(jaw, f) * 0.4
                y0 = -height(jaw, f)
                out += tube([place(f * L - 0.15 * tl, y0 - 0.75 * tl, s * 1.04, False), place(f * L, y0 + 6, s, False)],
                            [0, tr * 0.9], 3, 'tooth', light)
    return out


def rib_cage(spec: dict, light: dict) -> list:
    """A great beast's ribs standing up out of the ground ("rib_cages":
    {"at" [degrees, distance], "turn" degrees, "ribs" pairs, "height",
    "width", "spacing" cm}): it lies on its back, its backbone half sunk
    along the ground, the ribs curving out from it either side, then up and
    in again, their tips apart, the middle ones the longest."""
    foot, toward, across = radial(spec['at'])
    t = np.radians(spec.get('turn', 0))
    axis = across * np.cos(t) + toward * np.sin(t)
    side = np.cross(axis, [0, 1.0, 0]); side /= np.linalg.norm(side)
    n, H, W, gap = spec.get('ribs', 7), spec.get('height', 220), spec.get('width', 120), spec.get('spacing', 44)
    out = []
    half = (n - 1) / 2 * gap
    spine = [foot + axis * s + np.array([0, -4 + 8 * (abs(s) / (half + 160)) ** 2, 0])
             for s in np.linspace(-half - 160, half + 160, 11)]
    out += tube(spine, [12] * 4 + [13] * 3 + [12] * 4, 5, 'fossil', light)
    r = H / 1.74                               # the curve's radius: its top at H
    for k in range(n):
        s = -half + k * gap
        f = 1 - 0.4 * (abs(k - (n - 1) / 2) / max(1, (n - 1) / 2)) ** 2
        for sg in (-1, 1):
            pts = [foot + axis * (s + 5 * th) + side * sg * W * np.sin(th) + np.array([0, -r * f * (1 - np.cos(th)) + 2, 0])
                   for th in np.linspace(0.2, 2.4 * f, 5)]
            out += tube(pts, list(np.linspace(8, 3, 5)), 4, 'fossil', light)
    return out


def bone_bed(spec: dict, light: dict) -> list:
    """A smaller skeleton half dug out where it lay ("skeletons": {"at"
    [degrees, distance], "turn" degrees, "length" cm}): its backbone a
    curve along the ground, sunk to half its depth, its tail thinning to a
    point, short ribs arching out of the earth either side, the skull
    (the raptor's picture) at its head, leaning on the ground."""
    foot, toward, across = radial(spec['at'])
    t = np.radians(spec.get('turn', 0))
    axis = across * np.cos(t) + toward * np.sin(t)
    side = np.cross(axis, [0, 1.0, 0]); side /= np.linalg.norm(side)
    L = spec.get('length', 300)
    g = L / 300                                       # its other measures grow with it
    pts, radii = [], []
    for k in np.linspace(0, 1, 9):
        pts.append(foot + axis * (k - 0.5) * L + side * 26 * g * np.sin(k * 2 * np.pi) + np.array([0, 1, 0]))
        radii.append((8 * (1 - k) ** 0.6 + 1.5) * g)
    out = tube(pts, radii, 5, 'fossil', light)
    head = pts[0] - axis * 30 * g
    for k in range(6):
        c = pts[1] + (pts[3] - pts[1]) * k / 5
        r = 1 - 0.4 * abs(k - 2.5) / 2.5
        for sg in (-1, 1):
            arc = [c + side * sg * 50 * g * r * np.sin(th) + np.array([0, -34 * g * r * np.sin(2 * th) + 6 * th, 0])
                   for th in np.linspace(0.1, 1.45, 5)]
            out += tube(arc, [4 * g, 3.6 * g, 3 * g, 2.6 * g, 2.2 * g], 4, 'fossil', light)
    lean = np.radians(spec.get('skull_lean', 55))
    to_fight = -side if side @ toward < 0 else side
    up = np.array([0, -np.cos(lean), 0]) + to_fight * np.sin(lean)
    up /= np.linalg.norm(up)
    out += card(tuple(head + np.array([0, 2, 0])), -axis, up, spec.get('skull', 80), spec.get('skull', 80), 'raptor',
                (0, 63), (0, 63), flat_level(light, 1.0), 0.85)
    for leg in (0.35, 0.6):
        p = pts[int(leg * 8)]
        sg = 1 if leg < 0.5 else -1
        out += tube([p + side * sg * 12 * g, p + (side * sg * 60 - axis * 20) * g + np.array([0, -10 * g, 0]),
                     p + (side * sg * 95 + axis * 10) * g + np.array([0, 2, 0])], [6 * g, 4.5 * g, 4 * g], 4, 'fossil', light)
    return out


def loose_bone(spec, light: dict, rnd) -> list:
    """A bone lying in the grass ("bones": [degrees, distance, kind, size,
    yaw]): "femur" a thigh bone, its ends knobbed; "claw" a hooked claw;
    "tooth" a long curved tooth; "vertebra" a backbone's piece, its spine
    up."""
    foot = np.array(polar(spec[:2]))
    kind, size = spec[2], spec[3] if len(spec) > 3 else 60
    yaw = np.radians(spec[4] if len(spec) > 4 else rnd.uniform(0, 360))
    d = np.array([np.cos(yaw), 0, np.sin(yaw)])
    c = np.array([foot[0], 0, foot[1]])
    if kind == 'femur':
        r = size / 10
        pts = [c - d * size / 2 + np.array([0, -r * 0.5, 0]), c - d * size * 0.38, c + d * size * 0.38,
               c + d * size / 2 + np.array([0, -r * 0.5, 0])]
        return tube(pts, [r * 1.6, r * 0.9, r * 0.9, r * 1.5], 5, 'fossil', light)
    if kind == 'claw':
        pts = [c - d * size / 2 + np.array([0, -size * 0.1 * np.sin(np.pi * k / 4), 0]) + d * size * k / 4 for k in range(5)]
        pts = [p + np.array([0, -size * 0.35 * (k / 4) ** 2, 0]) for k, p in enumerate(pts)]
        return tube(pts, [size / 7, size / 8, size / 10, size / 15, size / 40], 4, 'fossil', light)
    if kind == 'tooth':
        pts = [c - d * size / 2 + d * size * k / 3 + np.array([0, -size * 0.12 * k * (3 - k) / 2 - 2, 0]) for k in range(4)]
        return tube(pts, [size / 7, size / 8, size / 12, size / 40], 4, 'fossil', light)
    r = size / 2
    out = tube([c - d * r * 0.5 + np.array([0, -r * 0.45, 0]), c + d * r * 0.5 + np.array([0, -r * 0.45, 0])], [r * 0.5, r * 0.5], 6,
               'fossil', light)
    return out + tube([c + np.array([0, -r * 0.8, 0]), c + np.array([0, -r * 1.9, 0]) - d * r * 0.3], [r * 0.25, r * 0.12], 4,
                      'fossil', light)


def ammonite(spec, light: dict) -> list:
    """An ammonite in its stone ("ammonites": [degrees, distance, size cm,
    tilt degrees]): lying in the grass, tilted up toward the fight."""
    foot, toward, across = radial(spec[:2])
    size, tilt = (spec[2] if len(spec) > 2 else 40), np.radians(spec[3] if len(spec) > 3 else 25)
    up = -toward * np.cos(tilt) + np.array([0, -np.sin(tilt), 0])
    return card(tuple(foot + toward * size * 0.45 + np.array([0, -1, 0])), across, up, size, size, 'ammonite', (0, 63), (0, 63),
                flat_level(light, 1.0), 0.85)


def scenery(cfg: dict, tex: dict, tile_size: int, flip: bool) -> tuple[decode.Geometry, dict]:
    """The scenery's faces in the base stage's 16 x 16 tiles: each face in
    the tile of its middle, the nearer first (the game draws a tile's last
    primitive first, the farther tiles' before the nearer)."""
    spec = cfg['scenery']
    light = spec['light']
    if spec['light'].get('candle'):
        light = dict(light, points=candle_points(spec))
    rnd = np.random.default_rng(spec.get('seed', 5))
    faces = rim(spec['rim'], light) if spec.get('rim') and not spec['rim'].get('off') else []
    perch = None
    if spec.get('tree'):
        tree_faces, perch = dead_tree(spec['tree'], light)
        faces += tree_faces
    for b in spec.get('buttes', []):
        faces += butte(b, light, rnd)
    for b in spec.get('boulders', []):
        faces += boulder(b, light, rnd)
    if spec.get('mushroom'):
        faces += mushroom(spec['mushroom'], light, rnd)
    for s in spec.get('stacks', []):
        faces += mushroom(s, light, rnd)
    for s in spec.get('rocks', []):
        faces += layered_rock(s, light, rnd)
    if spec.get('tufts'):
        faces += tufts(spec['tufts'], rnd)
    for b in spec.get('bollards', []):
        faces += bollard(spec.get('bollard', {}), b, light, rnd)
    for d in spec.get('drums', []):
        faces += drum(spec.get('drum', {}), d, light, rnd)
    for r in spec.get('railings', []):
        faces += railing(spec.get('railing', {}), r, light)
    if spec.get('blocks'):
        for b in strewn(spec['blocks'], np.random.default_rng(spec['blocks'].get('seed', 3))):
            faces += block(b[:2], b[2], light, rnd)
    if spec.get('rubble'):
        rr = np.random.default_rng(spec['rubble'].get('seed', 4))
        for b in strewn(spec['rubble'], rr):
            faces += block(b[:2], b[2], light, rr, rock=True)
    for g in spec.get('gears', []):
        faces += gear(g, light)
    if spec.get('phone_box'):
        faces += phone_box(spec['phone_box'], light)
    for c in spec.get('crates', []):
        faces += crate(c, light)
    for b in spec.get('barrels', []):
        faces += barrel(spec.get('barrel', {}), b, light, rnd)
    for sp in spec.get('spools', []):
        faces += spool(sp, light)
    for c in spec.get('cones', []):
        faces += cone(c, light, rnd)
    for b in spec.get('beacons', []):
        faces += beacon(spec.get('beacon', {}), b, light)
    for v in spec.get('steams', []):
        faces += steam(spec.get('steam', {}), v)
    for v in spec.get('fogs', []):
        faces += fog(spec.get('fog', {}), v)
    for t in spec.get('toros', []):
        faces += toro(spec.get('toro', {}), t, light)
    if spec.get('torii'):
        faces += torii(spec['torii'], light)
    if spec.get('steps'):
        faces += steps(spec['steps'], light)
    for f in spec.get('fences', []):
        faces += bamboo_fence(spec.get('fence', {}), f, light)
    for ln in spec.get('lantern_lines', []):
        faces += lantern_line(spec.get('lantern_line', {}), ln, light)
    for st in spec.get('flat_stones', []):
        faces += flat_stone(st[:2], st[2], st[3], light, rnd)
    if spec.get('lifeguard'):
        faces += lifeguard(spec['lifeguard'], light)
    for t in spec.get('tikis', []):
        faces += tiki(spec.get('tiki', {}), t, light)
    for c in spec.get('coconuts', []):
        faces += coconut(c, light, rnd)
    for d in spec.get('driftwood', []):
        faces += driftwood(d, light, rnd)
    if spec.get('surfboard'):
        faces += surfboard(spec['surfboard'], light)
    if spec.get('life_ring'):
        faces += life_ring(spec['life_ring'], light)
    if spec.get('bits'):
        faces += beach_bits(spec['bits'], light, np.random.default_rng(spec['bits'].get('seed', 9)))
    for t in spec.get('seokdeungs', []):
        faces += seokdeung(spec.get('seokdeung', {}), t, light)
    for ln in spec.get('lotus_lines', []):
        faces += lotus_line(spec.get('lotus_line', {}), ln, light)
    for j in spec.get('jangseungs', []):
        faces += jangseung(spec.get('jangseung', {}), j[:2], light, j[2])
    for c in spec.get('cairns', []):
        faces += cairn(c, light, rnd)
    if spec.get('seoktap'):
        faces += seoktap(spec['seoktap'], light)
    for o in spec.get('onggis', []):
        faces += onggi(o, light, rnd)
    for t in spec.get('mughal_lamps', []):
        faces += mughal_lamp(spec.get('mughal_lamp', {}), t, light)
    for ln in spec.get('diya_lines', []):
        faces += diya_line(spec.get('diya', {}), ln, rnd)
    for t in spec.get('cypresses', []):
        faces += cypress(spec.get('cypress', {}), t, light)
    for r in spec.get('jalis', []):
        faces += jali(spec.get('jali', {}), r, light)
    for kk in spec.get('kalashas', []):
        faces += kalasha(kk, light)
    for f in spec.get('fountains', []):
        faces += fountain(spec.get('fountain', {}), f, light)
    for b in spec.get('bamboos', []):
        faces += bamboo_clump(spec.get('bamboo', {}), b, light, rnd)
    for r in spec.get('tea_rows', []):
        faces += tea_row(spec.get('tea', {}), r, light)
    if spec.get('pailou'):
        faces += pailou(spec['pailou'], light)
    if spec.get('censer'):
        faces += censer(spec['censer'], light)
    for ln in spec.get('flag_lines', []):
        faces += flag_line(spec.get('flag_line', {}), ln, light)
    for b in spec.get('battlements', []):
        faces += battlement(spec.get('battlement', {}), b, light, rnd)
    for b in spec.get('rubble_bricks', []):
        faces += wall_rubble(b[:2], b[2], light, rnd)
    for t in spec.get('beacon_towers', []):
        faces += beacon_tower(dict(spec.get('beacon_tower', {}), at=t), light)
    for b in spec.get('war_banners', []):
        faces += war_banner(spec.get('war_banner', {}), b, light)
    for j in spec.get('wine_jars', []):
        faces += wine_jar(j, light, rnd)
    for s in spec.get('sheaves', []):
        faces += sheaf(s, light, rnd)
    if spec.get('baskets'):
        faces += tea_baskets(spec['baskets'], light)
    for st in spec.get('mossy_stones', []):
        faces += flat_stone(st[:2], st[2], st[3], light, rnd, 'mossy')
    if spec.get('flowers'):
        faces += tufts(spec['flowers'], rnd)
    for b in spec.get('bales', []):
        faces += hay_bale(b, light)
    for w in spec.get('walls', []):
        faces += drystone_wall(spec.get('wall', {}), w, light, rnd)
    for f in spec.get('rail_fences', []):
        faces += rail_fence(spec.get('rail_fence', {}), f, light)
    if spec.get('cross'):
        faces += wayside_cross(spec['cross'], light, rnd)
    if spec.get('cart'):
        faces += hay_cart(spec['cart'], light)
    if spec.get('scarecrow'):
        faces += scarecrow(spec['scarecrow'], light)
    if spec.get('skeps'):
        faces += skeps(spec['skeps'], light)
    for f in spec.get('sunflowers', []):
        faces += sunflowers(f, light, rnd)
    for r in spec.get('promenades', []):
        faces += promenade(spec.get('promenade', {}), r, light)
    for b in spec.get('banner_poles', []):
        faces += banner_pole(spec.get('banner', {}), b, light)
    if spec.get('neon_sign'):
        faces += neon_sign(spec['neon_sign'], light)
    for t in spec.get('lamps', []):
        faces += street_lamp(spec.get('lamp', {}), t, light)
    if spec.get('flagpole'):
        faces += flagpole(spec['flagpole'], light)
    for b in spec.get('benches', []):
        faces += bench(b, light)
    for h in spec.get('hydrants', []):
        faces += hydrant(h, light, rnd)
    if spec.get('hot_dog'):
        faces += hot_dog_cart(spec['hot_dog'], light)
    if spec.get('mailbox'):
        faces += mailbox(spec['mailbox'], light)
    for t in spec.get('trash_cans', []):
        faces += trash_can(t, light)
    for m in spec.get('manholes', []):
        faces += manhole(m, light)
    for b in spec.get('pew_blocks', []):
        faces += pew_rows(spec.get('pew', {}), b, light)
    for p in spec.get('pillars', []):
        faces += pillar(spec.get('pillar', {}), p, light)
    if spec.get('altar'):
        faces += altar(spec['altar'], light)
    for c in spec.get('candelabras', []):
        faces += candelabrum(spec.get('candelabrum', {}), c, light)
    if spec.get('votive'):
        faces += votive_rack(spec['votive'], light)
    if spec.get('thurible'):
        faces += thurible(spec['thurible'], light)
    for c in spec.get('chandeliers', []):
        faces += chandelier(spec.get('chandelier', {}), c, light)
    for g in spec.get('flash_patches', []):
        faces += glass_light(spec.get('flash', {}), g, 'flashwin', None)
    for g in spec.get('glass_patches', []):     # the add-on's: the patch drawn at runtime (T2GL), the shaft here
        faces += glass_light(spec.get('glass', {}), g, patch_on=not RESERVED)
    for r in spec.get('shimenawas', []):
        faces += shimenawa(spec.get('shimenawa', {}), r, light)
    if spec.get('gong'):
        faces += temple_gong(spec['gong'], light)
    for e in spec.get('ema_racks', []):
        faces += ema_rack(spec.get('ema_rack', {}), e, light, rnd)
    for o in spec.get('offerings', []):
        faces += offering_table(spec.get('offering', {}), o, light)
    for d in spec.get('door_patches', []):
        faces += glass_light(spec.get('door', {}), d, 'sunlit', 'sunshaft')
    for k, d in enumerate(spec.get('sun_patches', [])):
        # Serene Forest's sunlight: every other one the second palettes (sun_pulse())
        faces += glass_light(spec.get('sun', {}), d, 'sunlit_b' if k % 2 else 'sunlit', 'sunshaft_b' if k % 2 else 'sunshaft')
    for g in spec.get('logs', []):
        faces += fallen_log(g, light)
    for st in spec.get('stumps', []):
        faces += stump(st, light, rnd)
    for m in spec.get('mushrooms', []):
        faces += mushrooms(m, light, rnd)
    for w in spec.get('ruin_walls', []):
        faces += ruin_wall(w, light, rnd)
    for r in spec.get('nagas', []):
        faces += naga_balustrade(spec.get('naga', {}), r, light)
    for b in spec.get('khmer_blocks', []):
        faces += fallen_blocks(b, light, rnd)
    for t in spec.get('terraces', []):
        faces += laterite_steps(t, light)
    if spec.get('fig'):
        faces += strangler_fig(spec['fig'], light, rnd)
    for p in spec.get('palms', []):
        faces += sugar_palm(spec.get('palm', {}), p, light, rnd)
    if spec.get('stilt_house'):
        faces += stilt_house(spec['stilt_house'], light)
    if spec.get('boat'):
        faces += longtail_boat(spec['boat'], light)
    for r in spec.get('nets', []):
        faces += net_poles(spec.get('net', {}), r, light)
    for p in spec.get('lotus_ponds', []):
        faces += lotus_pond(spec, p, rnd)
    for c in spec.get('colonnades', []):
        faces += doric_colonnade(c, light)
    for d in spec.get('column_drums', []):
        faces += column_drum(d, light, rnd)
    for b in spec.get('marble_blocks', []):
        faces += wall_rubble(b[:2], b[2], light, rnd, 'pentelic')
    for c in spec.get('ionic_capitals', []):
        faces += ionic_capital(c, light, rnd)
    for a in spec.get('architraves', []):
        faces += architrave_piece(a, light, rnd)
    for o in spec.get('olives', []):
        faces += olive_tree(spec.get('olive', {}), o, light, rnd)
    if spec.get('caryatid_porch'):
        faces += caryatid_porch(spec['caryatid_porch'], light)
    for s in spec.get('steles', []):
        faces += stele(s, light, rnd)
    if spec.get('odeon'):
        faces += odeon_wall(spec['odeon'], light)
    for j in spec.get('junipers', []):
        faces += juniper(spec.get('juniper', {}), j, light)
    for pp in spec.get('prickly_pears', []):
        faces += prickly_pear(spec.get('prickly_pear', {}), pp, light, rnd)
    for c in spec.get('chollas', []):
        faces += cholla(c, light, rnd)
    for k in spec.get('skulls', []):
        faces += steer_skull(k, light, rnd)
    for g in spec.get('petroglyphs', []):
        faces += petroglyph_rock(g, light)
    for f in spec.get('barb_fences', []):
        faces += barb_fence(f, light, rnd)
    if spec.get('trex'):
        faces += trex_skull(spec['trex'], light)
    for r in spec.get('rib_cages', []):
        faces += rib_cage(r, light)
    for s in spec.get('skeletons', []):
        faces += bone_bed(s, light)
    for b in spec.get('bones', []):
        faces += loose_bone(b, light, rnd)
    for a in spec.get('ammonites', []):
        faces += ammonite(a, light)
    geo =decode.Geometry(tile_size)
    slots = decode.Arcade.slot_table()
    tiles = {i: [] for i in range(256)}
    dropped = 0
    def tile_of(f):
        mx, mz = np.mean([c[0] for c in f['corners']]), np.mean([c[2] for c in f['corners']])
        c, r = int((mx + 8 * tile_size) // tile_size), int((mz + 8 * tile_size) // tile_size)
        return 16 * r + c if 0 <= c < 16 and 0 <= r < 16 else None
    # the tufts ("filler": a tuft's number) last, each whole or not at all:
    # one left out where its tile has no room for its corners
    fillers = {}
    for f in faces:
        if 'filler' in f:
            fillers.setdefault(f['filler'], []).append(f); continue
        i = tile_of(f)
        if i is None: dropped += 1; continue
        tiles[i].append(f)
    corners = {i: {c for f in tiles[i] for c in f['corners']} for i in range(256)}
    thinned = 0
    for group in fillers.values():
        where = [tile_of(f) for f in group]
        if any(i is None for i in where): dropped += 1; continue
        need = {}
        for f, i in zip(group, where):
            need.setdefault(i, set()).update(f['corners'])
        if any(len(corners[i] | cs) > 256 for i, cs in need.items()):
            thinned += 1; continue
        for f, i in zip(group, where):
            tiles[i].append(f)
        for i, cs in need.items():
            corners[i] |= cs
    prims = 0
    for i in range(256):
        # the beacons' light on the floor last: drawn first, under all else
        fs = sorted(tiles[i], key=lambda f: (bool(f.get('under')), np.hypot(np.mean([c[0] for c in f['corners']]),
                                                                            np.mean([c[2] for c in f['corners']]))))
        verts, ps = {}, []
        for f in fs:
            corners = f['corners'] if not flip else [f['corners'][1], f['corners'][0], f['corners'][3], f['corners'][2]]
            colors = f['colors'] if not flip else [f['colors'][1], f['colors'][0], f['colors'][3], f['colors'][2]]
            idx = []
            for c in corners:
                if c not in verts: verts[c] = len(verts)
                idx.append(verts[c])
            pictures = tex[f['tex']]
            x, y, (cx, cy) = pictures[f['var'] % len(pictures)]
            u0, v0 = (x & 63) * 4, y & 0xFF
            a, b = v0 + f['rows'][0], v0 + f['rows'][1]
            l, r = u0 + f.get('cols', (0, 63))[0], u0 + f.get('cols', (0, 63))[1]
            uv = [(l, a), (r, a), (l, b), (r, b)] if 'uv' not in f else [(u0 + u, v0 + v) for u, v in f['uv']]
            if flip: uv = [uv[1], uv[0], uv[3], uv[2]]
            tpage = (x >> 6) | (y & 0x100) >> 4 | (y & 0x200) << 2
            kind = AddedPrim if f.get('added') else decode.Prim
            if f.get('added'): tpage |= 0x20
            ps.append(kind(GT4, 'GT4', tuple(idx), tuple(uv), cy << 6 | cx >> 4, tpage, tuple(colors)))
        if len(verts) > 256:
            raise SystemExit(f'tile {i}: {len(verts)} vertices (256 at most): smaller pieces or a bigger tile size'
                             + ''.join(f', tile {j}' for j in range(i + 1, 256) if len({c for f in tiles[j] for c in f['corners']}) > 256))
        geo.tiles.append(decode.Tile(i, [tuple(v) for v in verts], ps, slot=1 + slots[i]))
        prims += len(ps)
    return geo, dict(faces=len(faces), prims=prims, dropped=dropped, thinned=thinned,
                     most_vertices=max(len(t.verts) for t in geo.tiles), perch=perch)


# --- the eagle ---------------------------------------------------------------
# Four models (OBJS, at most 4): gliding, wings up, wings down, perched; the
# runtime (src/tekken3_arcade_stages.c, object stage 200: OBJA) draws one at
# a time, flies it round the canyon and lands it on a butte (EAGL).

GT3 = 0x35010809
# --- New York City: the Battery's promenade at sunset ------------------------

def few(img: np.ndarray, k: int) -> tuple[list[int], np.ndarray]:
    """k colours (sixteen()'s way) for the pixels img (n x 3) and their
    indices."""
    q = Image.fromarray(img.reshape(-1, 1, 3).astype(np.uint8)).quantize(k, method=Image.Quantize.MEDIANCUT,
                                                                         dither=Image.Dither.NONE)
    pal = np.array((q.getpalette() + [0] * 3 * k)[:3 * k], float).reshape(k, 3)
    x = img.reshape(-1, 3).astype(float)
    for _ in range(12):
        near = ((x[:, None] - pal[None]) ** 2).sum(2).argmin(1)
        for j in range(k):
            if (near == j).any(): pal[j] = x[near == j].mean(0)
    colours = bgr555(pal)
    colours[colours == 0] = 0x0421
    idx = ((x[:, None] - rgb_of(colours)[None]) ** 2).sum(2).argmin(1)
    return [int(c) for c in colours], idx


def city_lights(cfg: dict, pic: np.ndarray) -> tuple[dict, list, list, tuple | None]:
    """New York's windows as the dusk comes ("windows"): in the cells of
    "rows" x "columns" the buildings (pixels off the sky's colour, the sky
    each row the median of "sky_cols", or "sky_sides" [x0, x1]: drawn
    across from the columns beside the city, above the "water" row of the
    picture) get windows, every other pixel each way, "share" of them,
    each in one of "groups"; a cell's picture then has 16 - groups colours
    of its own and one per group, at first the group's own colour (as
    drawn), lighting up group by group over the "frames" ("colour" warm,
    some cooler). The mast's beacon ("beacon": [x, y] of the picture): its
    cell's colour 15 on the pixels round it (blinked by its own palette
    animation). Returns {(row, column): (colours, indices)} for
    far_background(), the lit cells in order, per frame their palettes,
    and the beacon's cell."""
    spec = cfg['windows']
    groups, n = spec.get('groups', 3), spec.get('frames', 16)
    rnd = np.random.default_rng(spec.get('seed', 41))
    h, w = pic.shape[:2]
    a, b = spec['sky_cols']
    if spec.get('sky_sides'):
        # the sky each row drawn across from the columns either side of the
        # city (the HD picture's clouds change along it)
        l0, r0 = spec['sky_sides']
        left, right = np.median(pic[:, l0 - 4:l0 + 4], axis=1), np.median(pic[:, r0 - 4:r0 + 4], axis=1)
        t = np.clip((np.arange(w) - l0) / (r0 - l0), 0, 1)[None, :, None]
        sky = left[:, None] * (1 - t) + right[:, None] * t
    else:
        sky = np.repeat(np.median(pic[:, a:b], axis=1)[:, None], w, 1)
    off = np.abs(pic - sky).sum(-1) > spec.get('edge', 60)
    yy, xx = np.mgrid[0:h, 0:w]
    # the buildings' texture: no smooth cloud of another colour
    lum = pic @ np.array([0.3, 0.59, 0.11])
    mean = sum(np.roll(np.roll(lum, i, 0), j, 1) for i in (-1, 0, 1) for j in (-1, 0, 1)) / 9
    rough = sum(np.abs(np.roll(np.roll(lum, i, 0), j, 1) - mean) for i in (-1, 0, 1) for j in (-1, 0, 1)) / 9
    off &= (yy < spec['water']) & (rough > spec.get('rough', 4))
    # standing on the shore: each column up from the water while it stays
    # building (gaps of a pixel or two bridged)
    ground = np.zeros_like(off)
    gap = np.zeros(w, int)
    for y in range(spec['water'] - 1, -1, -1):
        up = ground[y + 1] if y + 1 < spec['water'] else np.ones(w, bool)
        gap = np.where(off[y], 0, gap + 1)
        ground[y] = up & (gap <= spec.get('gap', 4))
    off &= ground
    inner = off & np.roll(off, 1, 0) & np.roll(off, -1, 0) & np.roll(off, 1, 1) & np.roll(off, -1, 1)
    pick = inner & (yy % 2 == 0) & (xx % 2 == 1) & (rnd.uniform(0, 1, (h, w)) < spec.get('share', 0.4))
    group = rnd.integers(0, groups, (h, w))
    beacon = None
    own, cells = {}, []
    if spec.get('beacon'):
        bx, by = spec['beacon']
        beacon = (by // 64, bx // 64)
        part = pic[64 * beacon[0]:64 * beacon[0] + 64, 64 * beacon[1]:64 * beacon[1] + 64]
        colours, idx = few(part, 15)
        idx = idx.reshape(64, 64)
        idx[max(0, by % 64 - 1):by % 64 + 1, max(0, bx % 64 - 1):bx % 64 + 1] = 15
        own[beacon] = (colours + [int(bgr555(np.array(spec.get('beacon_dark', [60, 40, 40]), float)))], idx)
    lit = np.array(spec.get('colour', [255, 210, 130]), float)
    cool = np.array(spec.get('cool', [200, 220, 236]), float)
    frames = [[] for _ in range(n)]
    for r in range(*spec['rows']):
        for c in range(*spec['columns']):
            if (r, c) == beacon:
                continue
            part = pic[64 * r:64 * r + 64, 64 * c:64 * c + 64]
            win = pick[64 * r:64 * r + 64, 64 * c:64 * c + 64]
            if win.sum() < spec.get('least', 8):
                continue
            g = group[64 * r:64 * r + 64, 64 * c:64 * c + 64]
            colours, idx = few(part[~win], 16 - groups)
            full = np.zeros((64, 64), np.int64)
            full[~win] = idx
            dark = []
            for k in range(groups):
                m = win & (g == k)
                full[m] = 16 - groups + k
                dark.append(part[m].mean(0) if m.any() else part[win].mean(0))
            own[r, c] = (colours + [0] * groups, full)
            cells.append((r, c))
            for f in range(n):
                pal = list(colours)
                for k in range(groups):
                    # group k fully lit at frame (k + 1) (n - 1) / groups, two frames of dimmer light before
                    on = np.clip((f - ((k + 1) * (n - 1) / groups - 2)) / 2, 0, 1)
                    tone = (cool if k == groups - 1 else lit) * (0.8 + 0.2 * k / max(1, groups - 1))
                    pal.append(int(bgr555(np.clip(dark[k] * (1 - on) + tone * on, 0, 255))))
                frames[f].append(pal)
            own[r, c] = (frames[0][-1], full)
    return own, cells, frames, beacon


def palette_run(vr: Vram, y: int, palettes: list) -> int:
    """Palettes side by side on low half row y from x 0 (one run: the
    relocation moves it whole, an animation's frame or the shown one).
    Returns y."""
    for k, colours in enumerate(palettes):
        vr.v[y, 16 * k:16 * k + 16] = colours
    vr.rects.append(('clut', 0, y, 16 * len(palettes), 1, 4))
    return y


def palette_frames(vr: Vram, row: int, frames: list) -> list:
    """An animation's frames of palettes (the shown run on row): each a
    run under it on the low half (palette_run(): the relocation moves it),
    or, in the add-on's packs (RESERVED), past the VRAM, one row of a RAM
    band each (Vram.ram(): (x, y) its place; tools/t2_remake_stages.py
    frames_out() takes it into T2AN), off the high half's palette room;
    only while PALETTE_RAM (the last five: the Pagoda's packs as they were)."""
    if not RESERVED or not PALETTE_RAM or len(frames) > 64:
        return [palette_run(vr, row - 1 - f, pals) for f, pals in enumerate(frames)]
    w = 16 * len(frames[0])
    at = vr._ram_run((w + 15) // 16)
    x, y = (at % 64) * 16, 512 + (at // 64) * 64
    out = []
    for f, pals in enumerate(frames):
        vr.v[y + f, x:x + w] = np.concatenate([np.asarray(p, np.uint16) for p in pals])
        vr.rects.append(('image', x, y + f, w, 1, 4))
        out.append((x, y + f))
    return out


WINDOW_ROW = 511                     # the lit cells' palettes (low half rows): shown, then the frames under it
BLINK_ROW = 488                      # the beacon's colours, one a frame
FAST_ROW = 486                       # Hong Kong's neon and beacons: shown, then the frames under it
CHASE_ROW = 410                      # its floor's running lights: shown, then the frames under it


def blobs(mask: np.ndarray) -> list[tuple[np.ndarray, np.ndarray]]:
    """The 8-connected pieces of mask, each (rows, columns) of its pixels."""
    seen = np.zeros(mask.shape, bool)
    h, w = mask.shape
    out = []
    for y0, x0 in zip(*np.nonzero(mask)):
        if seen[y0, x0]:
            continue
        seen[y0, x0] = True
        stack, ys, xs = [(y0, x0)], [], []
        while stack:
            y, x = stack.pop()
            ys.append(y); xs.append(x)
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    v, u = y + dy, x + dx
                    if 0 <= v < h and 0 <= u < w and mask[v, u] and not seen[v, u]:
                        seen[v, u] = True
                        stack.append((v, u))
        out.append((np.array(ys), np.array(xs)))
    return out


def hk_lights(cfg: dict, pic: np.ndarray) -> tuple[dict, tuple, tuple]:
    """Hong Kong's skyline at night ("lights"), as palettes played (ANI2,
    ANI4): "windows": in "rows" of cells the lit windows (pieces of pixels
    brighter than "bright", "size" at most) "share" of them in "groups",
    each group going dark a while and lighting again over "frames";
    "beacons": the small red lights on the towers' tops (red pieces of
    "size" at most, the sky over them, above row "below" of the picture)
    blinking together ("on" frames of each "every"); "neon": [x, y, w, h]
    of the picture each a sign, its strong coloured pixels (core and glow)
    flickering now and then, each sign in its own way. A cell's picture
    has a colour of its own per group, beacon, sign core and glow it holds,
    the rest of its 16 its own picture's; a cell with a beacon or a sign
    plays with them (fast), the others with the windows. Returns
    {(row, column): (colours, indices)} for far_background(), then
    (cells, per frame their palettes) of the windows and of the fast ones."""
    spec = cfg['lights']
    rnd = np.random.default_rng(spec.get('seed', 51))
    h, w = pic.shape[:2]
    lum = pic @ np.array([0.3, 0.59, 0.11])
    r, g, b = pic[..., 0], pic[..., 1], pic[..., 2]
    # the beacons
    bs = spec['beacons']
    beacon = np.zeros((h, w), bool)
    for ys, xs in blobs((r > 130) & (r > 1.7 * g) & (r > 1.7 * b)):
        above = lum[max(0, ys.min() - 6):max(0, ys.min() - 2), max(0, xs.min() - 1):xs.max() + 2]
        if len(ys) <= bs.get('size', 40) and ys.min() < bs['below'] and above.size and above.mean() < 45:
            beacon[ys, xs] = True
    # the signs: their strong colours, the brighter half the core
    neon = np.zeros((h, w), int)
    sat = pic.max(-1) - pic.min(-1)
    for k, (x, y, ww, hh) in enumerate(spec.get('neon', [])):
        m = np.zeros((h, w), bool)
        m[y:y + hh, x:x + ww] = (sat[y:y + hh, x:x + ww] > 60) & (pic[y:y + hh, x:x + ww].max(-1) > 110)
        m &= ~beacon
        core = m & (lum >= np.median(lum[m])) if m.any() else m
        neon[m] = 2 * k + 2
        neon[core] = 2 * k + 1
    # the windows that go out: whole windows, in groups
    ws = spec['windows']
    groups = ws.get('groups', 3)
    win = np.full((h, w), -1)
    cand = (lum > ws.get('bright', 115)) & ~beacon & (neon == 0)
    cand[:64 * ws['rows'][0]] = False
    cand[64 * ws['rows'][1]:] = False
    for ys, xs in blobs(cand):
        if len(ys) <= ws.get('size', 40) and rnd.uniform() < ws.get('share', 0.25):
            win[ys, xs] = rnd.integers(groups)
    # the channels: a cell's own colours after its picture's
    nsigns = len(spec.get('neon', []))
    channel = np.full((h, w), -1)
    channel[win >= 0] = win[win >= 0]                          # 0..groups-1
    channel[beacon] = groups                                   # groups
    channel[neon > 0] = groups + neon[neon > 0]                # groups + 1 .. groups + 2 nsigns
    on = {groups: np.array(bs.get('colour', [255, 46, 36]), float)}
    for k in range(1, 2 * nsigns + 1):
        m = channel == groups + k
        if m.any():
            on[groups + k] = pic[m].mean(0)
    # the frames: per channel a light level each frame
    fn, wn = spec.get('fast_frames', 64), ws.get('frames', 16)
    every, lit = bs.get('every', 16), bs.get('on', 3)
    blink = np.array([1.0 if f % every < lit else 0.0 for f in range(fn)])
    signs = []
    for k in range(nsigns):
        lv = np.ones(fn)
        for _ in range(int(rnd.integers(1, 4))):
            s, n = int(rnd.integers(fn)), int(rnd.integers(1, 4))
            lv[[(s + i) % fn for i in range(n)]] = spec.get('neon_off', 0.45)
            if rnd.uniform() < 0.5:
                lv[(s + n + 1) % fn] = spec.get('neon_off', 0.45)     # a stutter: off, on, off
        signs.append(lv)
    out_at = {}
    for k in range(groups):
        s, n = int(rnd.integers(wn)), int(rnd.integers(ws.get('dark', [3, 7])[0], ws.get('dark', [3, 7])[1]))
        out_at[k] = {(s + i) % wn for i in range(n)}
    dark_glass = np.array(ws.get('off', [22, 24, 32]), float)
    own, wcells, fcells, wframes, fframes = {}, [], [], [[] for _ in range(wn)], [[] for _ in range(fn)]
    lights = {}                                             # (the add-on's) a cell's window pixels
    for row in range(h // 64):
        for col in range(w // 64):
            part = pic[64 * row:64 * row + 64, 64 * col:64 * col + 64]
            ch = channel[64 * row:64 * row + 64, 64 * col:64 * col + 64]
            here = sorted(set(ch[ch >= 0].tolist()))
            fast = any(c >= groups for c in here)
            if not here or (not fast and (ch >= 0).sum() < ws.get('least', 6)):
                continue
            lights[row, col] = int((ch >= 0).sum())
            if fast:
                ch = np.where(ch < groups, -1, ch)            # a fast cell's windows stay as drawn
                here = [c for c in here if c >= groups]
            rest = ch < 0
            colours, idx = few(part[rest], 16 - len(here))
            full = np.zeros((64, 64), np.int64)
            full[rest] = idx
            lit_of = {}
            for j, c in enumerate(here):
                full[ch == c] = 16 - len(here) + j
                lit_of[c] = on.get(c, part[ch == c].mean(0))
            own[row, col] = (colours + [0] * len(here), full)
            if fast:
                fcells.append((row, col))
                for f in range(fn):
                    pal = list(colours)
                    for c in here:
                        if c == groups:
                            v = lit_of[c] * blink[f] + np.array(bs.get('dark', [46, 18, 20]), float) * (1 - blink[f])
                        else:
                            v = lit_of[c] * signs[(c - groups - 1) // 2][f]
                        pal.append(int(bgr555(np.clip(v, 0, 255))))
                    fframes[f].append(pal)
                own[row, col] = (fframes[0][-1], full)
            else:
                wcells.append((row, col))
                for f in range(wn):
                    pal = list(colours)
                    for c in here:
                        v = dark_glass + 0.12 * lit_of[c] if f in out_at[c] else lit_of[c]
                        pal.append(int(bgr555(np.clip(v, 0, 255))))
                    wframes[f].append(pal)
                own[row, col] = (wframes[0][-1], full)
    if RESERVED:
        # (the add-on's: the Classic's taller frame) the cells of most windows
        # light in turn, the others' windows stay as drawn
        for cells, frames, most in ((wcells, wframes, 64), (fcells, fframes, 63)):
            if len(cells) > most:
                keep = sorted(sorted(range(len(cells)), key=lambda i: -lights[cells[i]])[:most])
                for i in set(range(len(cells))) - set(keep):
                    own.pop(cells[i])
                cells[:] = [cells[i] for i in keep]
                for f in range(len(frames)):
                    frames[f] = [frames[f][i] for i in keep]
    if len(wcells) > 64 or len(fcells) > 63:
        raise SystemExit(f'lights: {len(wcells)} window cells, {len(fcells)} fast ones (a run holds 64)')
    return own, (wcells, wframes), (fcells, fframes)


EYE_ROW = 511                        # the Pagoda's statues' eyes (low half rows): shown, then the frames under it
FLICKER_ROW = 440                    # its candles' light: shown, then the frames under it


def pagoda_lights(cfg: dict, pic: np.ndarray) -> tuple[dict, tuple, tuple]:
    """The Pagoda's dark hall as its candles burn ("flicker", palettes
    played: ANI4, ANI2): the cells within "reach" pixels of a candle's
    flame (the picture's warm pixels brighter than "bright") flicker with
    the nearest group of candles ("group" pixels apart at most), each
    group its own way over "frames", each colour of a cell the more the
    brighter and warmer it is ("depth"); "eyes" {"at": [[x, y], ...]
    pixels of the player's picture, the middle between a statue's two
    eyes, "apart" its eyes' distance, "glow" the glow's reach, "colour",
    "frames", "glows" [[statue, first frame], ...]}: a statue's eyes glow
    red a while now and then, faded in and out; an eye cell's colours its
    picture's, three of them the glow's (its eyes, the glow near them and
    farther), lit toward the red. Returns {(row, column): (colours,
    indices)} for far_background(), then (cells, per frame their
    palettes) of the eyes and of the candles."""
    spec = cfg['flicker']
    h, w = pic.shape[:2]
    lum = pic @ np.array([0.3, 0.59, 0.11])
    r, b = pic[..., 0], pic[..., 2]
    x0, y0, x1, y1 = cfg['crop']
    sx, sy = w / (x1 - x0), h / (y1 - y0)
    # the statues' eyes: per pixel its channel (0 the eyes, 1 near, 2 farther), its statue
    es = spec.get('eyes')
    eye_ch, eye_of = np.full((h, w), -1), np.full((h, w), -1)
    if es:
        yy, xx = np.mgrid[0:h, 0:w]
        apart, glow = es.get('apart', 4) * sx / 2, es.get('glow', 4.5)
        for k, (ex, ey) in enumerate(es['at']):
            cx, cy = (ex - x0) * sx, (ey - y0) * sy
            d = np.minimum(np.hypot(xx - cx + apart, yy - cy), np.hypot(xx - cx - apart, yy - cy))
            near = d <= glow
            eye_of[near] = k
            eye_ch[near] = np.where(d[near] <= 0.75, 0, np.where(d[near] <= glow / 2, 1, 2))
    # the candles: their flames' pieces, grouped along the picture
    flame = (lum > spec.get('bright', 190)) & (r > 1.25 * b) & (eye_ch < 0)
    pieces = sorted(blobs(flame), key=lambda p: p[1].mean())
    groups, centres = [], []
    for ys, xs in pieces:
        if groups and xs.mean() - groups[-1][-1] < spec.get('group', 40):
            groups[-1].append(xs.mean())
        else:
            groups.append([xs.mean()])
        centres.append((ys.mean(), xs.mean(), len(groups) - 1))
    rnd = np.random.default_rng(spec.get('seed', 66))
    fn = spec.get('frames', 48)
    t = 2 * np.pi * np.arange(fn) / fn
    levels = []
    # "fires" [[x, y, depth], ...] pixels of the player's picture: the group
    # nearest burns that deep, faster (Eternal Darkness's hearth)
    fires = {}
    for fx, fy, fd in spec.get('fires', []):
        cx = (fx - x0) * sx
        if groups:
            fires[int(np.argmin([abs(np.mean(gr) - cx) for gr in groups]))] = fd
    for gi, _ in enumerate(groups):
        k = rnd.permutation([3, 5, 7, 11, 13])[:3] * (2 if gi in fires else 1)
        ph = rnd.uniform(0, 2 * np.pi, 3)
        lv = 1 + 0.5 * np.sin(k[0] * t + ph[0]) + 0.3 * np.sin(k[1] * t + ph[1]) + 0.2 * np.sin(k[2] * t + ph[2])
        lv = 1 + fires.get(gi, spec.get('depth', 0.14)) * (lv - 1)
        for _ in range(int(rnd.integers(1, 3))):         # a gust: the flames bow, the light drops a moment
            s = int(rnd.integers(fn))
            for i, d in enumerate((0.8, 0.7, 0.85)):
                lv[(s + i) % fn] *= d
        levels.append(lv)
    ne = es.get('frames', 64) if es else 0
    env = np.zeros((len(es['at']) if es else 0, ne))
    for k, s in (es.get('glows', []) if es else []):
        for i, a in enumerate(es.get('fade', [0.25, 0.6, 0.9, 1.0, 1.0, 0.9, 0.6, 0.3, 0.1])):
            env[k, (s + i) % ne] = max(env[k, (s + i) % ne], a)
    red = np.array(es.get('colour', [255, 40, 18]) if es else [0, 0, 0], float)
    reach = spec.get('reach', 90)
    # "rain" {"windows": [[x0, y0, x1, y1], ...] pixels of the player's
    # picture, "slant", "density", "dash", "colour"}: rain running down the
    # windows' glass (its bluish pixels), its drops' pixels four colours of
    # a cell's palette lit one after another (Eternal Darkness)
    rain = np.full((h, w), -1)
    rs = spec.get('rain')
    if rs:
        yy, xx = np.mgrid[0:h, 0:w]
        glass = np.zeros((h, w), bool)
        for wx0, wy0, wx1, wy1 in rs['windows']:
            glass[max(0, int((wy0 - y0) * sy)):int((wy1 - y0) * sy), max(0, int((wx0 - x0) * sx)):int((wx1 - x0) * sx)] = True
        glass &= b > r + rs.get('blue', 4)
        rr = np.random.default_rng(rs.get('seed', 21))
        track = np.floor(xx - rs.get('slant', 0.12) * yy).astype(int)
        lo = track.min()
        on = rr.uniform(0, 1, track.max() - lo + 1) < rs.get('density', 0.3)
        off = rr.integers(0, 64, track.max() - lo + 1)
        dash = rs.get('dash', 4)
        rain = np.where(glass & on[track - lo], ((yy + off[track - lo]) // dash) % 4, -1)
    rain_rgb = np.array(rs.get('colour', [70, 80, 104]) if rs else [0, 0, 0], float)
    if rs and not es:
        ne = fn                     # the windows' cells: the eyes' run, kept in step with the candles
    own, ecells, ccells, eframes, cframes = {}, [], [], [[] for _ in range(ne)], [[] for _ in range(fn)]
    near = []                                      # (the add-on's) each candle cell's distance
    for row in range(h // 64):
        for col in range(w // 64):
            part = pic[64 * row:64 * row + 64, 64 * col:64 * col + 64]
            ch = eye_ch[64 * row:64 * row + 64, 64 * col:64 * col + 64]
            if (ch >= 0).any():
                # the eyes: the rest in 10 colours, the eyes 1, near 2, farther 3
                who = eye_of[64 * row:64 * row + 64, 64 * col:64 * col + 64]
                k = int(np.bincount(who[who >= 0]).argmax())
                full = np.zeros((64, 64), np.int64)
                colours, alpha, base = [], [], 0
                for c, n, a in ((-1, 10, 0.0), (0, 1, 1.0), (1, 2, 0.5), (2, 3, 0.18)):
                    m = ch == c
                    if not m.any():
                        continue
                    cs, idx = few(part[m], min(n, int(m.sum())))
                    full[m] = base + idx
                    colours += cs; alpha += [a] * len(cs); base += len(cs)
                colours += [0x0421] * (16 - len(colours)); alpha += [0.0] * (16 - len(alpha))
                rgb, alpha = rgb_of(np.array(colours, np.uint16)), np.array(alpha)
                ecells.append((row, col))
                for f in range(ne):
                    a = (alpha * env[k, f])[:, None]
                    lit_rgb = rgb * (1 - a) + red * a + np.clip(a - 0.8, 0, 1) * 300 * np.array([1.0, 0.6, 0.4])
                    eframes[f].append([int(v) or 0x0421 for v in bgr555(np.clip(lit_rgb, 0, 255))])
                own[row, col] = (eframes[0][-1], full)
                continue
            wet = rain[64 * row:64 * row + 64, 64 * col:64 * col + 64]
            if not centres and not (wet >= 0).any():
                continue
            cy, cx = 64 * row + 31.5, 64 * col + 31.5
            # the nearest flame to the cell (its nearest point)
            d, g = min((np.hypot(max(0, abs(fy - cy) - 32), max(0, abs(fx - cx) - 32)), g) for fy, fx, g in centres)                 if centres else (1e9, 0)
            if (wet >= 0).any():
                # a window's cell: 12 colours of its picture, then the drops' 4
                dry = wet < 0
                cs, di = few(part[dry], 12)
                idx = np.zeros((64, 64), np.int64)
                idx[dry] = di
                drop_off = []
                for k in range(4):
                    m = wet == k
                    idx[m] = 12 + k
                    drop_off.append(part[m].mean(0) if m.any() else np.zeros(3))
                cs = cs + [0x0421] * (12 - len(cs))
                rgb = rgb_of(np.array(cs, np.uint16))
                lv = rgb @ np.array([0.3, 0.59, 0.11])
                warm = np.clip((rgb[:, 0] - rgb[:, 2]) / np.maximum(rgb[:, 0], 1) * 1.6, 0, 1)
                wgt = (np.clip((lv - 12) / 90, 0, 1) * warm)[:, None] if d <= reach else np.zeros((12, 1))
                ecells.append((row, col))
                for f in range(fn):
                    v = rgb * (1 + (levels[g][f] - 1) * wgt) if centres else rgb
                    drops = [o * 0.45 + rain_rgb if k == f % 4 else o for k, o in enumerate(drop_off)]
                    full = np.concatenate([v, np.array(drops)])
                    eframes[f].append([int(c) or 0x0421 for c in bgr555(np.clip(full, 0, 255))])
                own[row, col] = (eframes[0][-1], idx)
                continue
            if d > reach:
                continue
            colours, idx = sixteen(part)
            rgb = rgb_of(np.array(colours, np.uint16))
            lv = rgb @ np.array([0.3, 0.59, 0.11])
            warm = np.clip((rgb[:, 0] - rgb[:, 2]) / np.maximum(rgb[:, 0], 1) * 1.6, 0, 1)
            wgt = (np.clip((lv - 12) / 90, 0, 1) * warm)[:, None]
            ccells.append((row, col))
            near.append(d)
            for f in range(fn):
                v = rgb * (1 + (levels[g][f] - 1) * wgt)
                cframes[f].append([int(c) or 0x0421 for c in bgr555(np.clip(v, 0, 255))])
            own[row, col] = (cframes[0][-1], idx)
    if RESERVED and len(ccells) > 64:
        # (the add-on's: the Classic's taller frame) the 64 cells nearest a flame
        keep = sorted(sorted(range(len(ccells)), key=lambda i: near[i])[:64])
        for i in set(range(len(ccells))) - set(keep):
            own.pop(ccells[i])
        ccells = [ccells[i] for i in keep]
        cframes = [[fr[i] for i in keep] for fr in cframes]
    if len(ecells) > 64 or len(ccells) > 64:
        raise SystemExit(f'flicker: {len(ecells)} eye cells, {len(ccells)} candle cells (a run holds 64)')
    return own, (ecells, eframes), (ccells, cframes)


def dilate(m: np.ndarray, k: int) -> np.ndarray:
    out = m.copy()
    for _ in range(k):
        out = out | np.roll(out, 1, 0) | np.roll(out, -1, 0) | np.roll(out, 1, 1) | np.roll(out, -1, 1)
    return out


def hk_sign(vr: Vram, clut: tuple[int, int], n: int, spec: dict) -> tuple[dict, list]:
    """The rooftop's neon sign ("neon_sign"): its face 128 x 64 (two
    pictures: "text", a picture in tools/data/t2_hd of the words, white on
    black, in red neon tubes, a cyan tube round the edge, on a dark panel
    in a steel frame) and the red light it throws on the floor (seen from
    above, added), all on one palette at clut (the fast lights' run: it
    flickers with them). Colour 0 nothing; 1..3 the panel; 4, 5 steel;
    6..8 the red tube, its middle, its glow; 9..11 the cyan's; 12..15 the
    floor's light, dim to bright. Returns the pictures by name and per
    frame (n) the palette, the words and the edge each flickering their
    own way now and then."""
    rnd = np.random.default_rng(spec.get('seed', 57))
    if RESERVED:                    # the add-on's: 香港 (Hong Kong) in tubes, not the user's "鐵拳" picture
        words = Image.fromarray(neon_words('香港'))
    else:
        words = neon_text(spec['text']) if spec['text'] not in [p.name for f in PICTURES for p in Path(f).glob('*.png')]         else Image.open(hd(spec['text'])).convert('L')
    text = np.asarray(words.resize((128, 64))) > 110
    yy, xx = np.mgrid[0:64, 0:128]
    noise = rnd.normal(0, 1, (64, 128))
    idx = np.where(noise > 0.6, 3, np.where(noise < -0.6, 1, 2)).astype(np.uint16)
    edge = (xx < 3) | (xx > 124) | (yy < 3) | (yy > 60)
    idx[edge] = np.where((xx[edge] + yy[edge]) % 5 == 0, 5, 4)
    tube = ((xx >= 6) & (xx <= 121) & (yy >= 6) & (yy <= 57)) & ~((xx >= 8) & (xx <= 119) & (yy >= 8) & (yy <= 55))
    idx[dilate(tube, 1) & ~edge] = 11
    idx[tube] = 10
    idx[tube & (((xx == 6) | (xx == 121)) | ((yy == 6) | (yy == 57)))] = 9
    inner = text & np.roll(text, 1, 0) & np.roll(text, -1, 0) & np.roll(text, 1, 1) & np.roll(text, -1, 1)
    idx[dilate(text, 2) & ~text & ~tube & ~edge] = 8
    idx[text] = 7
    idx[inner] = 6
    gx, gz = np.meshgrid((np.arange(64) - 31.5) / 32, (np.arange(64) - 31.5) / 32)
    k = np.clip(1 - np.hypot(gx, gz * 1.4), 0, 1) ** 1.4
    bayer = (np.array([[0, 8, 2, 10], [12, 4, 14, 6], [3, 11, 1, 9], [15, 7, 13, 5]]) + 0.5) / 16
    level = k * 4 + np.tile(bayer, (16, 16)) - 0.5
    glow = np.where(k < 0.05, 0, np.clip(np.round(level), 0, 3) + 12).astype(np.uint16)
    glow[glow == 12] = np.where(k[glow == 12] < 0.12, 0, 12)
    pics = {'neon_sign': [vr.picture(idx[:, :64], None, clut=clut), vr.picture(idx[:, 64:], None, clut=clut)],
            'neon_glow': [vr.picture(glow, None, clut=clut)]}

    def flicker(lo: float) -> np.ndarray:
        lv = np.ones(n)
        for _ in range(int(rnd.integers(1, 3))):
            s, m = int(rnd.integers(n)), int(rnd.integers(1, 3))
            lv[[(s + i) % n for i in range(m)]] = lo
            lv[(s + m + 1) % n] = lo
        return lv
    words, rim = flicker(0.15), flicker(0.3)
    panel = [(18, 18, 24), (26, 26, 34), (34, 34, 44), (96, 98, 104), (130, 132, 138)]
    red_on, red_off = [(255, 196, 200), (255, 52, 64), (150, 22, 34)], [(84, 34, 40), (62, 24, 30), (26, 26, 34)]
    cyan_on, cyan_off = [(210, 255, 255), (40, 214, 236), (12, 84, 104)], [(40, 64, 70), (30, 50, 56), (26, 26, 34)]
    glow_on = [np.array(spec.get('glow', [150, 28, 40]), float) * ((j + 1) / 4) ** 1.3 for j in range(4)]
    frames = []
    for f in range(n):
        a, b = words[f], rim[f]
        cols = [(0, 0, 0)] + panel
        cols += [tuple(np.array(on, float) * a + np.array(off, float) * (1 - a)) for on, off in zip(red_on, red_off)]
        cols += [tuple(np.array(on, float) * b + np.array(off, float) * (1 - b)) for on, off in zip(cyan_on, cyan_off)]
        cols += [tuple(np.maximum(g * a, 8)) for g in glow_on]
        pal = [0] + [0x8000 | int(c) for c in bgr555(np.array(cols[1:], float))]      # added: the glow
        frames.append(pal)
    return pics, frames


BANNER_ROWS = (32, 63)                # the Hong Kong picture: three banners (columns 16 k .. 16 k + 15)


def hk_frames(n: int) -> tuple[list[int], list]:
    """Hong Kong's picture as the wind plays (ANIM): n frames (round and
    round), one palette. Rows 32..63 three long banners side by side, red
    with a gold disc, gold with a red one, black with a red one, a hem down
    their sides, a wave of shade running down each and its foot swinging;
    rows 16..31 columns 48..63 a raindrop (as mughal_frames(): RAIN).
    Colour 0 nothing; 1..6 the banners' red, gold, black, each light and
    shaded; 7, 8 their discs' red and gold; 10, 11 the drop."""
    pal = [(0, 0, 0), (196, 30, 34), (130, 18, 24), (236, 186, 54), (168, 124, 30), (30, 26, 28), (16, 14, 16),
           (220, 40, 40), (250, 206, 80), (0, 0, 0), (176, 186, 206), (236, 240, 252), (0, 0, 0), (0, 0, 0),
           (0, 0, 0), (0, 0, 0)]
    colours = [0] + [0x8000 | int(c) for c in bgr555(np.array(pal[1:], float))]
    yy, xx = np.mgrid[0:64, 0:64]
    frames = []
    for f in range(n):
        t = 2 * np.pi * f / n
        idx = np.zeros((64, 64), np.uint16)
        for k, (cloth, disc) in enumerate(((1, 8), (3, 7), (5, 7))):
            x0 = 16 * k
            fy, fx = (yy[32:, x0:x0 + 16] - 32).astype(float), (xx[32:, x0:x0 + 16] - x0).astype(float)
            sway = 1.6 * np.sin(t + k * 1.7) * (fy / 31) ** 1.5         # the foot swinging
            sx = fx - sway
            inside = (sx >= 1) & (sx <= 14) & (fy <= 29 + np.sin(t * 2 + k + sx * 0.5))
            wave = np.sin(t * 1.0 + k * 1.7 - fy * 0.35 + sx * 0.2)
            b = np.where(wave < -0.4, cloth + 1, cloth)
            b = np.where((sx < 2.5) | (sx > 12.5), cloth + 1 if cloth != 3 else 4, b)       # the hem
            d = np.hypot(sx - 7.5, fy - 11)
            b = np.where(d < 4.6, disc, b)
            b = np.where((d < 2.6) & (np.abs(sx - 7.5) < 0.9 + (fy % 3 == 0)), cloth, b)    # a word on the disc
            idx[32:, x0:x0 + 16] = np.where(inside, b, 0)
        dy, dx = yy[16:32, 48:64] - 16, xx[16:32, 48:64] - 55.5
        head = np.hypot(dx / 2.6, (dy - 12.5) / 2.6) < 1
        tail = (np.abs(dx) < 0.8 + 1.6 * dy / 15) & (dy >= 2) & (dy < 13)
        drop = np.where(head, 11, np.where(tail, np.where(dy > 8, 11, 10), 0))
        drop[(dy < 6) & tail & ((dy + xx[16:32, 48:64]) % 2 == 0)] = 0
        idx[16:32, 48:64] = drop
        frames.append(idx)
    return colours, frames


def puddle_from_above(seed: int) -> np.ndarray:
    """A rain puddle on the roof seen from above (RGBA, 128 x 128): an
    uneven pool, dark and glossy, the city's neon in it in long smeared
    streaks (pink, cyan, amber), its rim wet."""
    rnd = np.random.default_rng(seed)
    yy, xx = np.mgrid[0:128, 0:128].astype(float)
    field = np.zeros((128, 128))
    for _ in range(10):
        cy, cx, r = rnd.uniform(30, 98), rnd.uniform(26, 102), rnd.uniform(7, 17)
        field += np.exp(-(((yy - cy) / r) ** 2 + ((xx - cx) / (r * rnd.uniform(0.8, 1.6))) ** 2))
    grain = rnd.normal(0, 1, (128, 128))
    for _ in range(3):
        grain = (grain + np.roll(grain, 1, 0) + np.roll(grain, 1, 1) + np.roll(grain, -1, 0) + np.roll(grain, -1, 1)) / 5
    inside = np.clip((field + 0.15 * grain / grain.std() - 0.75) / 0.35, 0, 1)
    img = np.zeros((128, 128, 3)) + np.array([16, 18, 26], float)
    for colour in ([236, 60, 130], [60, 206, 236], [240, 170, 70], [236, 60, 70]):
        x, w = rnd.uniform(20, 108), rnd.uniform(2, 6)
        k = np.exp(-((xx - x) / w) ** 2) * (0.4 + 0.6 * np.clip(np.sin(yy * 0.08 + rnd.uniform(0, 6)), 0, 1))
        img += k[..., None] * np.array(colour, float) * rnd.uniform(0.5, 0.9)
    rim = np.clip(1 - np.abs(inside - 0.5) * 4, 0, 1)
    img = img * (1 - 0.4 * rim[..., None]) + np.array([60, 64, 76], float) * 0.4 * rim[..., None]
    return np.dstack([np.clip(img, 0, 255), inside * 200])


def water_frames(cells: list, row: np.ndarray, spec: dict) -> list:
    """The harbour row's cells as the water ripples ("water": "frames" of
    them, played round and round; each cell's own 16 colours, only its
    indices change): from "from" down (the far shore above stands still)
    the picture drawn aside by ripples of "ripple" pixels, more toward the
    row's foot (the nearer water), running along it ("wave" pixels long);
    on the brightest quarter of the water (the sunset's path on it)
    "glitter" of the pixels take the cell's brightest colour now and then.
    Returns per frame the cells' indices."""
    n, amp, y0 = spec.get('frames', 6), spec.get('ripple', 1.2), spec.get('from', 14)
    h, w = row.shape[:2]
    yy, xx = np.mgrid[0:h, 0:w].astype(float)
    grow = np.clip((yy - y0 + 1) / max(1, h - y0), 0, 1) * (yy >= y0) * (yy < spec.get('to', h))
    lum = row @ np.array([0.3, 0.59, 0.11])
    path = (yy >= y0) & (lum > np.percentile(lum[int(y0):], 75))
    twinkle = np.random.default_rng(spec.get('seed', 33)).uniform(0, 1, (h, w))
    out = [[] for _ in range(n)]
    for f in range(n):
        t = 2 * np.pi * f / n
        dx = amp * (0.35 + grow) * grow * np.sin(t - 2 * np.pi * xx / spec.get('wave', 40) + 0.9 * yy)
        sx = (xx - dx) % w
        x0 = np.floor(sx).astype(int) % w
        fx = (sx - np.floor(sx))[..., None]
        yi = yy.astype(int)
        img = row[yi, x0] * (1 - fx) + row[yi, (x0 + 1) % w] * fx
        for c, (idx, colours, *_) in enumerate(cells):
            shown = rgb_of(np.array(colours, np.uint16))
            part = img[:, 64 * c:64 * c + 64].reshape(-1, 3)
            new = ((part[:, None] - shown[None]) ** 2).sum(2).argmin(1).reshape(64, 64)
            new = np.where(grow[:, 64 * c:64 * c + 64] > 0, new, idx)
            on = path[:, 64 * c:64 * c + 64] & (np.abs(twinkle[:, 64 * c:64 * c + 64] - (f + 0.5) / n) < spec.get('glitter', 0.05) / 2)
            new = np.where(on, int((shown @ np.array([0.3, 0.59, 0.11])).argmax()), new)
            out[f].append(new.astype(np.uint16))
    return out


US_FLAG = (0, 25)                    # the New York picture's rows: the flag (columns 0..41), then the lamps' light
PAPER = (42, 16, 22, 16)             # and a newspaper's double page: u, v, width, height (FTHR shape 3)
LAMP_LIGHT = (32, 63)                # on the floor (seen from above) and the globe's glow (columns 0..63)
GLOBE_COLS = (48, 63)                # rows 0..15 of these: the lamp's lit glass


def nyc_frames(n: int) -> tuple[list[int], list]:
    """New York's picture as the wind plays: n frames (round and round),
    one palette. Rows 0..25 columns 0..41 the Stars and Stripes, thirteen
    stripes, the canton's stars, a wave of shade running out along it and
    its fly end lifting and falling; rows 0..15 columns 48..63 a street
    lamp's lit glass (warm, brighter in its middle); rows 16..31 columns
    42..63 a newspaper's double page (PAPER: masthead, headlines, a photo,
    columns of print, the fold down its middle); rows 32..63 the lamp's
    light on the floor seen from above (added). Colour 0 nothing."""
    pu, pv, pw, ph = PAPER
    page = np.full((ph, pw), 3, np.uint16)
    for x0 in (0, pw // 2):
        x1 = x0 + pw // 2
        for y in range(7, ph - 1, 2):
            page[y, x0 + 1:x1 - 1] = 4                      # lines of print
        page[7:12, x0 + 1:x0 + 5] = 6                       # a photo
        page[8:11, x0 + 2:x0 + 4] = 4
    page[1:3, 1:pw // 2 - 1] = 6                            # the masthead
    page[1:3, 3:6] = 1
    page[4, 1:pw // 2 - 2] = 6; page[4, pw // 2 + 1:pw - 3] = 6        # headlines
    page[:, pw // 2 - 1:pw // 2 + 1] = 4                    # the fold
    ramp = [tuple(int(v) for v in np.array([255, 196, 116]) * (k / 8) ** 1.2) for k in range(1, 9)]
    pal = [(0, 0, 0), (188, 30, 42), (128, 20, 30), (242, 238, 228), (178, 174, 166), (44, 52, 116), (28, 34, 80),
           (255, 236, 196)] + ramp
    colours = [0] + [0x8000 | int(c) for c in bgr555(np.array(pal[1:], float))]
    yy, xx = np.mgrid[0:64, 0:64]
    frames = []
    for f in range(n):
        t = 2 * np.pi * f / n
        idx = np.zeros((64, 64), np.uint16)
        fy, fx = yy[:26, :42].astype(float), xx[:26, :42].astype(float)
        lift = 1.6 * np.sin(t - fx * 0.3) * (fx / 41)
        sy = fy - lift                                    # the row of the flag this pixel shows
        inside = (sy >= 0) & (sy < 26)
        stripe = (np.clip(sy, 0, 25) // 2).astype(int) % 2      # 0 red, 1 white
        flag = np.where(stripe == 0, 1, 3)
        canton = (sy < 14) & (fx < 18)
        flag = np.where(canton, 5, flag)
        star = canton & (np.round(sy) % 3 == 1) & ((fx + 2 * (np.round(sy) % 6 == 4)) % 4 == 1)
        flag = np.where(star, 3, flag)
        shade = np.sin(t - fx * 0.3 + 0.6) < -0.45
        flag = np.where(shade, flag + 1, flag)
        idx[:26, :42] = np.where(inside, flag, 0)
        g = np.hypot(xx[:16, 48:] - 55.5, (yy[:16, 48:] - 7.5) * 1.2) / 8
        idx[:16, 48:] = np.where(g < 0.35, 7, np.clip(15 - np.floor(g * 8), 9, 15))
        idx[pv:pv + ph, pu:pu + pw] = page
        gx, gz = (xx[32:] - 31.5) / 32, (yy[32:] - 47.5) / 16
        k = np.clip(1 - np.hypot(gx, gz), 0, 1) ** 1.5
        idx[32:] = np.where(k < 0.06, 0, np.clip(7 + np.ceil(k * 8), 8, 15))
        frames.append(idx)
    return colours, frames


def vent_steam_frames(n: int) -> tuple[list[int], list]:
    """A manhole's steam rising: steam_picture()'s puffs, the field
    climbing a whole round (32 rows) in n frames, its own palette (a warm
    grey, the sunset in it), added."""
    rnd = np.random.default_rng(61)
    field = np.zeros((32, 64))
    yy, xx = np.mgrid[0:32, 0:64]
    for _ in range(9):
        y, x, r = rnd.uniform(0, 32), rnd.uniform(10, 54), rnd.uniform(6, 11)
        for dy in (-32, 0, 32):
            field += np.exp(-(((yy - y - dy) / r) ** 2 + ((xx - x) / (1.3 * r)) ** 2))
    field = (field / field.max()) ** 1.6
    v = np.arange(64)[:, None] / 63.0
    x = (np.arange(64)[None] - 31.5) / 32
    spread = 0.3 + 0.75 * (1 - v)
    plume = np.exp(-(x / spread) ** 2) * np.clip(1.1 - (1 - v), 0, 1) ** 1.5 * np.clip(v * 6, 0, 1)
    colours = [0] + [0x8000 | int(bgr555(np.array([168, 156, 148], float) * (k / 15) ** 1.2)) for k in range(1, 16)]
    frames = []
    for f in range(n):
        s = (np.arange(64) + 32 * f / n) % 32
        a, w = np.floor(s).astype(int), (s - np.floor(s))[:, None]
        img = field[a] * (1 - w) + field[(a + 1) % 32] * w
        k = np.clip(img * plume * 1.2, 0, 1)
        frames.append(np.where(k < 0.06, 0, np.clip(np.round(k * 15), 1, 15)).astype(np.uint16))
    return colours, frames


def water_animation(cfg: dict, water: list, vr: Vram, pic: np.ndarray) -> tuple:
    """New York's one animation (ANIM): the harbour row (water_frames())
    and, in the two slots after it, the New York picture (nyc_frames())
    and the manholes' steam (vent_steam_frames()), each frame of them side
    by side as the shown ones (as cloud_animation()), played round and
    round. Returns the picture's shown place, the rectangle's place, the
    frames' places and the steam's shown picture."""
    spec = cfg['water']
    n = spec.get('frames', 6)
    r = spec['row']
    waves = water_frames(water, pic[64 * r:64 * r + 64], spec)
    ncolours, flags = nyc_frames(n)
    scolours, steam = vent_steam_frames(n)
    x0, y0 = water[0][3]
    at = (y0 - 512) // 64 * 64 + x0 // 16
    shown = vr.picture(flags[0], ncolours, at=at + len(water))
    puff = vr.picture(steam[0], scolours, at=at + len(water) + 1)
    places = []
    for f in range(n):
        g = vr.frame_run(len(water) + 2)
        for c, cell in enumerate(water):
            vr.picture(waves[f][c], None, at=g + c, clut=cell[2])
        vr.picture(flags[f], None, at=g + len(water), clut=shown[2])
        vr.picture(steam[f], None, at=g + len(water) + 1, clut=puff[2])
        places.append(((g % 64) * 16, 512 + (g // 64) * 64))
    return shown, (x0, y0), places, puff


def promenade(spec: dict, run, light: dict) -> list:
    """The Battery's sea wall ("promenades": [degrees from, degrees to,
    distance], "promenade": "wall", "rail" heights): a granite parapet
    round the arc, its coping, and on it a cast iron railing: posts with a
    ball on top, a top rail, the balusters between them a see-through
    picture (the water shows through). "rail" 0: the wall alone (Hong
    Kong's roof edge, "tex" its sides' and coping's pictures)."""
    a0, a1, dist = run
    wall, rail = spec.get('wall', 40), spec.get('rail', 62)
    side, coping = spec.get('tex', ['granite', 'slab'])
    n = max(1, round(abs(a1 - a0) / spec.get('step', 6)))
    angles = np.linspace(a0, a1, n + 1)
    pts = [np.array(polar([a, dist])) for a in angles]
    out = []
    for k in range(n):
        p, q = pts[k], pts[k + 1]
        d = (q - p) / np.linalg.norm(q - p)
        o = np.array([d[1], -d[0]]) * 16                     # across the wall, half its thickness
        c = [(p[0] - o[0], 0, p[1] - o[1]), (q[0] - o[0], 0, q[1] - o[1]), (q[0] + o[0], 0, q[1] + o[1]), (p[0] + o[0], 0, p[1] + o[1])]
        top = [(x, -wall, z) for x, _, z in c]
        out += box_faces(c + top, [side] * 4 + [coping, side], light)
        if rail <= 0:
            continue
        # the balusters' picture, both ways, from the coping to the top rail
        mid = (p + q) / 2
        quad = [(p[0], -wall - rail, p[1]), (q[0], -wall - rail, q[1]), (p[0], -wall, p[1]), (q[0], -wall, q[1])]
        out += both_ways(face(quad, 'balusters', light, (mid[0] * 0.9, -wall, mid[1] * 0.9)))
    if rail <= 0:
        return out
    for p in pts:
        out += turned([(-wall, 3.5, (0, 63)), (-wall - rail - 6, 3.5, (0, 63)), (-wall - rail - 8, 5, (0, 63)),
                       (-wall - rail - 13, 4, (0, 63)), (-wall - rail - 15, 1, (0, 63))], 4, p[0], p[1], 'iron', light, cap=False)
    out += tube([(p[0], -wall - rail, p[1]) for p in pts], [2.5] * len(pts), 3, 'iron', light)
    return out


def banner_pole(spec: dict, at, light: dict) -> list:
    """A pole on the roof's parapet ("banner_poles": [degrees, distance, which],
    "banner": "height" of the pole over the wall, "wall" its height,
    "width", "length" of the banner): steel, an arm out of its top toward
    the fight, from it a long banner (the Hong Kong picture's, which of the
    three) swinging in the wind, seen from both sides."""
    a, dist, which = at
    wall, h = spec.get('wall', 40), spec.get('height', 300)
    width, length = spec.get('width', 70), spec.get('length', 220)
    x, z = polar([a, dist])
    top = -wall - h
    out = tube([(x, -wall, z), (x, top - 6, z)], [4, 3.2], 5, 'steel', light)
    toward = -np.array([x, z]) / max(1e-6, np.hypot(x, z))
    across = np.array([toward[1], -toward[0]])
    arm = np.array([x, z]) + toward * 18
    out += tube([(x, top, z), (arm[0], top, arm[1])], [2, 2], 4, 'steel', light)
    p0, p1 = arm - across * width / 2, arm + across * width / 2
    out += tube([(p0[0], top + 4, p0[1]), (p1[0], top + 4, p1[1])], [1.8, 1.8], 4, 'steel', light)
    q = [(p0[0], top + 6, p0[1]), (p1[0], top + 6, p1[1]), (p0[0], top + 6 + length, p0[1]), (p1[0], top + 6 + length, p1[1])]
    flat = {'azimuth': 0, 'elevation': 90, 'ambient': [0.8, 0.78, 0.86], 'sun': [0, 0, 0]}
    out += both_ways(face(q, 'banner', flat, (0, 0, 0), rows=BANNER_ROWS, cols=(16 * which, 16 * which + 15)))
    return out


def neon_sign(spec: dict, light: dict) -> list:
    """The rooftop's neon sign ("neon_sign": "at" [degrees, distance],
    "width", "height" of its face, "lift" over the roof): its face (two
    pictures, hk_sign(): the words in red neon) turned to the fight on a
    steel stand (legs, braces, a walkway rail along its foot), its back
    steel, and its red light on the roof before it (added)."""
    a, dist = spec['at']
    w, h, lift = spec.get('width', 380), spec.get('height', 190), spec.get('lift', 160)
    c = np.array(polar([a, dist]))
    toward = -c / np.linalg.norm(c)
    across = np.array([toward[1], -toward[0]])
    l, r = c - across * w / 2, c + across * w / 2
    behind = (c[0] - toward[0] * 50, -lift - h / 2, c[1] - toward[1] * 50)
    q = [(l[0], -lift - h, l[1]), (r[0], -lift - h, r[1]), (l[0], -lift, l[1]), (r[0], -lift, r[1])]
    if wound(q, behind) is not q:
        l, r = r, l                     # the words read left to right from the fight
    out = []
    lit_self = {'azimuth': 0, 'elevation': 90, 'ambient': [1.0, 1.0, 1.0], 'sun': [0, 0, 0]}
    for k in range(2):
        a0, a1 = l + (r - l) * k / 2, l + (r - l) * (k + 1) / 2
        q = [(a0[0], -lift - h, a0[1]), (a1[0], -lift - h, a1[1]), (a0[0], -lift, a0[1]), (a1[0], -lift, a1[1])]
        out.append(face(q, 'neon_sign', lit_self, behind, var=k))
    back = [(r[0], -lift - h, r[1]), (l[0], -lift - h, l[1]), (r[0], -lift, r[1]), (l[0], -lift, l[1])]
    out.append(face(back, 'steel', light, (c[0] + toward[0] * 50, -lift - h / 2, c[1] + toward[1] * 50)))
    behind = toward * -40
    for t in (0.1, 0.5, 0.9):
        p = l + (r - l) * t
        foot = p + behind
        out += tube([(p[0], -lift, p[1]), (p[0], 0, p[1])], [4, 4], 5, 'steel', light)
        out += tube([(p[0], -lift - h * 0.5, p[1]), (foot[0], 0, foot[1])], [3, 3], 4, 'steel', light)
    rail_l, rail_r = l + toward * 12, r + toward * 12
    out += tube([(rail_l[0], -lift + 2, rail_l[1]), (rail_r[0], -lift + 2, rail_r[1])], [2.5, 2.5], 4, 'steel', light)
    glow = c + toward * spec.get('glow_at', 260)
    out.append(floor_light(glow[0], glow[1], spec.get('pool', 620), spec.get('level', 0x80), 'neon_glow', (0, 63)))
    return out


def street_lamp(spec: dict, at, light: dict) -> list:
    """A Battery Park lamp ("lamps": [degrees, distance], "lamp": "height",
    "pool" cm across its light): a fluted cast iron post on a wide base,
    a collar, the acorn globe lit by itself (the New York picture's glass),
    its cap and finial; its light on the floor round the foot, added (the
    picture's lower half)."""
    cx, cz = polar(at)
    h = spec.get('height', 330)
    out = turned([(0, 20, (0, 63)), (-14, 18, (0, 63)), (-22, 11, (0, 63)), (-60, 8, (0, 63)), (-66, 10, (0, 63)),
                  (-74, 6, (0, 63)), (-h + 30, 4.5, (0, 63)), (-h + 24, 9, (0, 63)), (-h + 14, 10, (0, 63))], 6, cx, cz,
                 'iron', light, cap=False)
    glow = {'azimuth': 0, 'elevation': 90, 'ambient': [1.0, 1.0, 1.0], 'sun': [0, 0, 0]}
    out += turned([(-h + 14, 12, (0, 15)), (-h - 6, 19, (0, 15)), (-h - 30, 21, (0, 15)), (-h - 52, 15, (0, 15)),
                   (-h - 62, 9, (0, 15))], 6, cx, cz, 'nyc', glow, cap=False)
    for f in out[-6 * 4:]:
        f['cols'] = GLOBE_COLS
    out += turned([(-h - 60, 11, (0, 63)), (-h - 66, 12, (0, 63)), (-h - 74, 5, (0, 63)), (-h - 90, 1.5, (0, 63))], 6, cx, cz,
                  'iron', light, cap=False)
    size = spec.get('pool', 520)
    x0, x1, z0, z1 = cx - size / 2, cx + size / 2, cz - size / 2, cz + size / 2
    corners = [(x0, -1, z0), (x1, -1, z0), (x0, -1, z1), (x1, -1, z1)]
    uv = [(0, LAMP_LIGHT[0]), (63, LAMP_LIGHT[0]), (0, LAMP_LIGHT[1]), (63, LAMP_LIGHT[1])]
    if wound(corners, (cx, 1000, cz)) is not corners:
        corners, uv = [corners[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
    out.append(dict(corners=[tuple(int(round(v)) for v in c) for c in corners], tex='nyc', rows=LAMP_LIGHT, var=0, uv=uv,
                    colors=[(spec.get('level', 0x60),) * 3] * 4, added=True, under=True))
    return out


def flagpole(spec: dict, light: dict) -> list:
    """A flagpole ("flagpole": "at", "height", "flag" width): a tapered
    white pole on a granite plinth, a gilt ball on top, the Stars and
    Stripes flying from its top (the New York picture, waving), seen from
    both sides."""
    at, h = spec['at'], spec.get('height', 560)
    cx, cz = polar(at)
    out = box_faces(posed([(x, y, z) for y in (0, -30) for x, z in ((-34, -34), (34, -34), (34, 34), (-34, 34))], 0, at[0], at),
                    [spec.get('plinth', 'granite')] * 4 + [spec.get('plinth', 'slab'), spec.get('plinth', 'granite')], light)
    out += turned([(-30, 7, (0, 63)), (-h, 3.5, (0, 63))], 6, cx, cz, 'pole', light, cap=False)
    out += turned([(-h, 3, (0, 63)), (-h - 6, 7, (0, 63)), (-h - 13, 6, (0, 63)), (-h - 16, 1, (0, 63))], 6, cx, cz, 'brass', light,
                  cap=False)
    w = spec.get('flag', 170)
    greek = spec.get('picture') == 'greek'          # Athens: the Greek flag of the Athens picture
    ht = w * (2 / 3 if greek else 26 / 42)
    a = np.radians(at[0] + spec.get('flies', 90))          # the way the wind blows it, from +z
    dx, dz = w * np.sin(a), w * np.cos(a)
    top = -h + 6
    q = [(cx, top, cz), (cx + dx, top + 4, cz + dz), (cx, top + ht, cz), (cx + dx, top + ht + 4, cz + dz)]
    flat = {'azimuth': 0, 'elevation': 90, 'ambient': [0.95, 0.88, 0.84], 'sun': [0, 0, 0]}
    out += both_ways(face(q, 'flags', flat, (0, 0, 0), rows=GREEK_FLAG[0], cols=GREEK_FLAG[1]) if greek else
                     face(q, 'nyc', flat, (0, 0, 0), rows=US_FLAG, cols=(0, 41)))
    return out


def bench(spec, light: dict) -> list:
    """A park bench ([degrees, distance, yaw]): its back to the fight
    (turned yaw degrees more), green slats of seat and back on two cast
    iron ends."""
    put = local(spec[:2], spec[2] if len(spec) > 2 else 0)
    box = lambda x0, x1, y0, y1, z0, z1: put([(x, y, z) for y in (y1, y0) for x, z in ((x0, z0), (x1, z0), (x1, z1), (x0, z1))])
    out = []
    for x in (-80, 80):
        out += box_faces(box(x - 3, x + 3, -44, 0, -24, 24), ['iron'] * 6, light)
        out += box_faces(box(x - 3, x + 3, -88, -44, 22, 28), ['iron'] * 6, light)
    for z in (-18, -4, 10):
        out += box_faces(box(-92, 92, -47, -43, z - 6, z + 6), ['slat'] * 6, light)
    for y in (-52, -64, -76):
        out += box_faces(box(-92, 92, y - 7, y, 26, 29), ['slat'] * 6, light)
    return out


def hydrant(spec, light: dict, rnd) -> list:
    """A New York fire hydrant ([degrees, distance]): red, its bonnet a
    dome, the nut on top, two hose nozzles out at the sides and the pumper
    one in front."""
    cx, cz = polar(spec[:2])
    turn = rnd.uniform(0, np.pi)
    out = turned([(0, 15, (0, 63)), (-6, 15, (0, 63)), (-8, 11, (0, 63)), (-52, 11, (0, 63)), (-56, 13, (0, 63)),
                  (-62, 12, (0, 63)), (-72, 8, (0, 63)), (-78, 3, (0, 63)), (-84, 3, (0, 63))], 8, cx, cz, 'hydrant',
                 light, turn=turn)
    for a, r in ((turn, 5), (turn + np.pi, 5), (turn + np.pi / 2, 7)):
        p0 = (cx + 9 * np.sin(a), -38, cz + 9 * np.cos(a))
        p1 = (cx + 20 * np.sin(a), -38, cz + 20 * np.cos(a))
        out += tube([p0, p1], [r, r], 6, 'hydrant', light)
    return out


def hot_dog_cart(spec: dict, light: dict) -> list:
    """A hot dog cart ("hot_dog": "at", "yaw"): a stainless box on two
    wheels, its sign, a push bar, a blue and yellow umbrella over it."""
    at = spec['at']
    turn = at[0] + spec.get('yaw', 90)
    put = lambda pts: posed(pts, 0, turn, at)
    out = box_faces(put([(x, y, z) for y in (-38, -110) for x, z in ((-75, -38), (75, -38), (75, 38), (-75, 38))]),
                    ['cart_side', 'steel', 'cart_side', 'steel', 'steel', 'steel'], light)
    out += box_faces(put([(x, y, z) for y in (-110, -116) for x, z in ((-80, -42), (80, -42), (80, 42), (-80, 42))]),
                     ['steel'] * 6, light)
    for z in (-44, 44):
        q = put([(-34, -68, z), (34, -68, z), (-34, 0, z), (34, 0, z)])
        out += both_ways(face(q, 'tyre', light, put([(0, -34, 0)])[0]))
    out += tube(put([(0, -34, -46), (0, -34, 46)]), [3, 3], 4, 'steel', light)
    out += beam(70, 110, -96, -92, 70, at, turn, ['steel'] * 6, light)
    for zz in (-32, 32):
        out += tube(put([(75, -80, zz), (110, -94, zz)]), [2, 2], 3, 'steel', light)
    p = put([(0, -116, 0)])[0]
    out += tube([p, (p[0], -250, p[2])], [2.5, 2.5], 4, 'steel', light)
    rim = [(p[0] + 110 * np.sin(a), -212, p[2] + 110 * np.cos(a)) for a in np.linspace(0, 2 * np.pi, 9)]
    tipp = (p[0], -262, p[2])
    for k in range(8):
        tex = 'canvas_blue' if k % 2 == 0 else 'canvas_yellow'
        q = [tipp, tipp, rim[k], rim[k + 1]]
        out += both_ways(face(q, tex, light, (p[0], -150, p[2])))
    return out


def mailbox(spec: dict, light: dict) -> list:
    """A USPS collection box ("mailbox": "at"): blue, its rounded top, the
    chute's flap, on four short legs, its face to the fight."""
    at = spec['at']
    turn = at[0] + 180
    put = lambda pts: posed(pts, 0, turn, at)
    out = box_faces(put([(x, y, z) for y in (-14, -104) for x, z in ((-28, -26), (28, -26), (28, 26), (-28, 26))]),
                    ['usps', 'usps_side', 'usps_side', 'usps_side', 'usps_top', 'usps_side'], light)
    for x in (-24, 24):
        for z in (-22, 22):
            c = put([(xx, y, zz) for y in (0, -14) for xx, zz in ((x - 2, z - 2), (x + 2, z - 2), (x + 2, z + 2), (x - 2, z + 2))])
            out += box_faces(c, ['usps_side'] * 6, light)
    # the rounded top: a half drum along x
    arc = [(-104 - 22 * np.sin(a), 26 * np.cos(a)) for a in np.linspace(0, np.pi, 6)]
    inside = put([(0, -110, 0)])[0]
    for k in range(5):
        (y0, z0), (y1, z1) = arc[k], arc[k + 1]
        q = put([(-28, y1, z1), (28, y1, z1), (-28, y0, z0), (28, y0, z0)])
        out.append(face(wound(q, inside), 'usps_top', light, inside))
    for x in (-28, 28):
        c = put([(x, -104, 0)])[0]
        for k in range(5):
            (y0, z0), (y1, z1) = arc[k], arc[k + 1]
            q = put([(x, y0, z0), (x, y1, z1), (x, -104, 0), (x, -104, 0)])
            out.append(face(wound(q, inside), 'usps_side', light, inside))
    return out


def trash_can(spec, light: dict) -> list:
    """A wire litter basket ([degrees, distance]): the city's green mesh,
    a rim at top and foot."""
    cx, cz = polar(spec[:2])
    return turned([(0, 20, (0, 63)), (-4, 21, (0, 63)), (-80, 25, (0, 63)), (-84, 26, (0, 63))], 8, cx, cz, 'wire', light,
                  cap=False)


def manhole(spec, light: dict) -> list:
    """A manhole cover in the floor ([degrees, distance]), its steam rising
    (steam(): the New York steam picture)."""
    cx, cz = polar(spec[:2])
    out = []
    n = 8
    ring = [(cx + 36 * np.sin(2 * np.pi * k / n), -1, cz + 36 * np.cos(2 * np.pi * k / n)) for k in range(n + 1)]
    for k in range(n):
        a0, a1 = 2 * np.pi * k / n, 2 * np.pi * (k + 1) / n
        uv = [(31, 31), (31, 31), (int(31.5 + 31 * np.sin(a0)), int(31.5 + 31 * np.cos(a0))),
              (int(31.5 + 31 * np.sin(a1)), int(31.5 + 31 * np.cos(a1)))]
        q = [(cx, -1, cz), (cx, -1, cz), ring[k], ring[k + 1]]
        if wound(q, (cx, 1000, cz)) is not q:
            q, uv = [q[i] for i in (1, 0, 3, 2)], [uv[i] for i in (1, 0, 3, 2)]
        f = face(q, 'manhole', light, (cx, 1000, cz))
        f['uv'] = uv
        out.append(f)
    out += steam(spec_steam(spec), spec[:2])
    return out


def spec_steam(spec) -> dict:
    """A manhole's plume: [degrees, distance, width, height]."""
    return {'width': spec[2] if len(spec) > 2 else 150, 'height': spec[3] if len(spec) > 3 else 260, 'level': 0x70}


def pigeon_picture(vr: Vram):
    """The harbour's birds in one picture (REGION): gulls in "dark" (the
    grey mantle), "light" (black wing tips; the pigeons' bars and tail
    band), "white" (head and body) and the yellow corner's quarters: "red"
    the gulls' yellow beak and the pigeons' feet, "fur" a pigeon's blue
    grey, "eye" black, "horn" its neck's green sheen."""
    rnd = np.random.default_rng(13)
    img = np.zeros((64, 64, 3))
    for (x, y, w), rgb in (((0, 0, 32), (150, 156, 166)), ((32, 0, 32), (34, 34, 38)), ((0, 32, 32), (236, 236, 232)),
                           ((32, 32, 16), (230, 172, 58)), ((48, 32, 16), (118, 124, 144)), ((32, 48, 16), (22, 20, 24)),
                           ((48, 48, 16), (82, 112, 102))):
        img[y:y + w, x:x + w] = np.clip(np.array(rgb) * (1 + 0.05 * rnd.normal(0, 1, (w, w)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def harbour_gull_model(pose: str, scale: float = 1.0) -> list[dict]:
    """A gull's faces (gull_model()), its beak the picture's "red" quarter
    (pigeon_picture(): the yellow corner holds the pigeons' colours)."""
    faces = gull_model(pose, 1.0)
    for f in faces:
        if f['region'] == 'yellow': f['region'] = 'red'
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def pigeon_model(pose: str = '', scale: float = 1.0) -> list[dict]:
    """Faces of a feral pigeon standing (as rabbit_model(): cm, x right, y
    down, z forward, its feet on the ground at y 0): a plump blue grey
    body, the green sheened neck, a small round head, a dark beak, folded
    grey wings with two black bars, a dark banded tail, coral feet."""
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    n = 6
    def ring(z, rx, ry, yc):
        return [(rx * np.sin(2 * np.pi * k / n), yc - ry * np.cos(2 * np.pi * k / n), z) for k in range(n)]
    body = [(-10, 2.5, 2, -11), (-6, 6, 5.5, -11), (1, 7, 7, -11), (7, 5.5, 6, -13), (10, 3.5, 3.5, -17)]
    rings = [ring(*b) for b in body]
    for i, (a, b) in enumerate(zip(rings, rings[1:])):
        for k in range(n):
            j = (k + 1) % n
            q = [a[k], a[j], b[k], b[j]]
            mid = np.mean(q, 0)
            add(q, 'horn' if i == 3 else 'fur', outward=mid - np.array([0, -12, mid[2]]))
    head = [(10, 3.5, 3.5, -17), (12, 4, 4, -20), (16, 3, 3, -21)]
    hr = [ring(*h) for h in head]
    for a, b in zip(hr, hr[1:]):
        for k in range(n):
            j = (k + 1) % n
            q = [a[k], a[j], b[k], b[j]]
            add(q, 'fur', outward=np.mean(q, 0) - np.array([0, -20, 13]))
    tip = (0, -20.5, 20)
    for k in range(n):
        add([hr[-1][k], hr[-1][(k + 1) % n], tip], 'eye', outward=np.mean([hr[-1][k], hr[-1][(k + 1) % n], tip], 0) - (0, -21, 16))
    for side in (-1, 1):
        add([(side * 3.6, -21.5, 14.5), (side * 3.6, -21.5, 13.2), (side * 3.6, -20.2, 14.5), (side * 3.6, -20.2, 13.2)], 'red',
            both=True)
        # a folded wing along the flank, the bars across its back end
        add([(side * 6.6, -16, 6), (side * 6.6, -16, -6), (side * 6.2, -9, 5), (side * 5, -9, -12)], 'dark', both=True)
        add([(side * 6.7, -15.5, -2), (side * 6.7, -15.5, -4), (side * 6.3, -9.5, -2), (side * 6.0, -9.5, -4)], 'light', both=True)
        add([(side * 6.7, -15.5, -6), (side * 6.7, -15.5, -8), (side * 6.0, -9.5, -6), (side * 5.6, -9.5, -8)], 'light', both=True)
        # a leg and its foot
        add([(side * 2.5, -6, 1), (side * 2.5, -6, 2.5), (side * 2.5, 0, 1), (side * 2.5, 0, 2.5)], 'red', both=True)
        add([(side * 1.5, -0.4, 0), (side * 3.5, -0.4, 0), (side * 1.5, -0.4, 5), (side * 3.5, -0.4, 5)], 'red', both=True)
    add([(-3, -12, -9), (3, -12, -9), (-4.5, -11, -20), (4.5, -11, -20)], 'fur', both=True)
    add([(-4.5, -11, -20), (4.5, -11, -20), (-5, -10.8, -23), (5, -10.8, -23)], 'light', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


EAGLE_STAGE = 200
GRASS_STAGE = 201                    # the runtime lays the grass round the fighters (grass_place())


def eagle_picture(vr: Vram):
    """Its feathers: dark brown, light brown (wing tips, tail), white
    (head), yellow (beak, feet), each a 32 x 32 corner of one picture."""
    rnd = np.random.default_rng(3)
    img = np.zeros((64, 64, 3))
    for (x, y), rgb in (((0, 0), (70, 44, 26)), ((32, 0), (122, 84, 48)), ((0, 32), (232, 226, 210)),
                        ((32, 32), (226, 176, 40))):
        streak = rnd.normal(0, 1, (32, 32)).cumsum(0) / 6
        img[y:y + 32, x:x + 32] = np.clip(np.array(rgb) * (1 + 0.12 * np.tanh(streak))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


REGION = {'dark': (0, 0), 'light': (32, 0), 'white': (0, 32), 'yellow': (32, 32),
          'red': (33, 33, 13), 'horn': (49, 49, 13),       # a crane's crown and beak: the yellow corner's quarters
          'fur': (49, 33, 13), 'eye': (33, 49, 13)}        # a rabbit's (the doves' picture)


def eagle_model(pose: str, scale: float = 1.0) -> list[dict]:
    """Faces of the eagle (cm; x right, y down, z forward; the body's
    middle at 0): its body rings, beak, tail, two wings of two parts each
    (both their sides), a perched one with folded wings and legs."""
    rings = [(48, 4, 4, 'white'), (36, 7, 7, 'white'), (26, 6, 6, 'white'), (10, 14, 10, 'dark'),
             (-12, 12, 9, 'dark'), (-30, 6, 4, 'dark')]
    def ring(z, rx, ry):
        return [(0, -ry, z), (rx, 0, z), (0, ry, z), (-rx, 0, z)]
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    for (z0, rx0, ry0, r0), (z1, rx1, ry1, _) in zip(rings, rings[1:]):
        a, b = ring(z0, rx0, ry0), ring(z1, rx1, ry1)
        for k in range(4):
            j = (k + 1) % 4
            mid = np.mean([a[k], a[j], b[k], b[j]], 0)
            add([a[k], a[j], b[k], b[j]], r0, outward=mid - np.array([0, 0, mid[2]]))
    tip = (0, 1, 58)
    front = ring(48, 4, 4)
    for k in range(4):
        add([front[k], front[(k + 1) % 4], tip], 'yellow', outward=np.mean([front[k], front[(k + 1) % 4], tip], 0) - (0, 0, 48))
    add([(-6, 0, -30), (6, 0, -30), (-16, 1, -54), (16, 1, -54)], 'light', both=True)
    angles = {'glide': (8, 4), 'up': (48, 28), 'down': (-30, -22)}
    for side in (-1, 1):
        if pose == 'perched':
            pts = [(side * 10, -3, 14), (side * 10, -3, -10), (side * 14, -7, 18), (side * 14, -7, -30),
                   (side * 11, -8, -50), (side * 11, -8, -52)]
        else:
            a1, a2 = np.radians(angles[pose][0]), np.radians(angles[pose][0] + angles[pose][1])
            ex, ey = 10 + 45 * np.cos(a1), -3 - 45 * np.sin(a1)
            tx, ty = ex + 47 * np.cos(a2), ey - 47 * np.sin(a2)
            pts = [(side * 10, -3, 14), (side * 10, -3, -10), (side * ex, ey, 8), (side * ex, ey, -16),
                   (side * tx, ty, -6), (side * (tx - 6), ty, -24)]
        add([pts[0], pts[1], pts[2], pts[3]], 'dark', both=True)
        add([pts[2], pts[3], pts[4], pts[5]], 'light', both=True)
        if pose == 'perched':
            add([(side * 5, 8, 4), (side * 5, 8, -2), (side * 5, 26, 6), (side * 5, 26, 0)], 'yellow', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def eagle_prims(faces: list[dict], picture) -> tuple[list, list]:
    """(vertices, primitives) of a model: each face wound so that its
    outside (or both sides) faces the camera when the runtime keeps it (as
    the tiles: the side away from cross(b - a, c - a) shows), lit from above."""
    x, y, (cx, cy) = picture
    u0, v0 = (x & 63) * 4, y & 0xFF
    tpage = (x >> 6) | (y & 0x100) >> 4 | (y & 0x200) << 2
    verts, prims = {}, []
    def vid(c):
        c = tuple(int(round(v)) for v in c)
        if c not in verts: verts[c] = len(verts)
        return verts[c]
    for f in faces:
        ru, rv, *size = REGION[f['region']]
        e = size[0] if size else 30
        uvq = [(u0 + ru, v0 + rv), (u0 + ru + e, v0 + rv), (u0 + ru, v0 + rv + e), (u0 + ru + e, v0 + rv + e)]
        c = [np.array(p, float) for p in f['corners']]
        n = np.cross(c[1] - c[0], c[2] - c[0])
        sides = [1, -1] if f['both'] else [1 if f['outward'] is None or n @ np.asarray(f['outward'], float) < 0 else -1]
        for s in sides:
            pts, uv = list(f['corners']), uvq[:len(f['corners'])]
            if s < 0:
                pts[0], pts[1] = pts[1], pts[0]; uv[0], uv[1] = uv[1], uv[0]
                if len(pts) == 4: pts[2], pts[3] = pts[3], pts[2]; uv[2], uv[3] = uv[3], uv[2]
            q = [np.array(p, float) for p in pts]
            shown = -np.cross(q[1] - q[0], q[2] - q[0])          # the side that shows
            up = -shown[1] / max(1e-9, np.linalg.norm(shown))
            level = int(np.clip(0x60 + 0x38 * up, 0x30, 0xA0))
            colors = tuple((level, level, level) for _ in pts)
            kind, header = ('GT4', GT4) if len(pts) == 4 else ('GT3', GT3)
            prims.append(decode.Prim(header, kind, tuple(vid(p) for p in pts), tuple(uv), cy << 6 | cx >> 4, tpage, colors))
    return [k for k, _ in sorted(verts.items(), key=lambda kv: kv[1])], prims


def eagle_sections(spec: dict, buttes: list, picture, reloc, arc, tree_perch=None) -> list:
    """OBJS (the four models, each drawn by its own call: the runtime shows
    one), ROTT, OBJA (object stage 200) and EAGL: s16 perches, circle
    radius, height (y, cm, up is negative), 0; then per perch s16 x, y, z
    (the body's middle, cm) and the way it faces (4096 a turn): on a
    butte's top, near its edge on the fight's side, looking at the fight."""
    objs = struct.pack('<I', 4)
    for pose in ('glide', 'up', 'down', 'perched'):
        verts, prims = eagle_prims(eagle_model(pose, spec.get('scale', 1.0)), picture)
        objs += struct.pack('<2H', len(verts), len(prims))
        objs += b''.join(struct.pack('<4h', *v, 0) for v in verts)
        objs += b''.join(pack.prim_bytes(reloc.prim(p)) for p in prims)
    objs += struct.pack('<I', 4) + b''.join(struct.pack('<2B3H', m, 0, spec.get('rank', 1009), 64, 0) for m in range(4))
    perches = []
    if tree_perch is not None:
        # the dead tree's top only: it lands where the camera sees it
        x, y, z = tree_perch
        y -= 26 * spec.get('scale', 1.0)                # the body's middle over its feet
        perches.append((round(x), round(y), round(z), round(np.degrees(np.arctan2(-x, -z)) / 360 * 4096) & 0xFFF))
        buttes = []
    for b in buttes:
        ang, dist, height, radius = b['at']
        a = np.radians(ang)
        r = dist - 0.45 * radius * 0.7
        x, z = r * np.sin(a), r * np.cos(a)
        perches.append((round(x), round(-height - 26), round(z), round(np.degrees(np.arctan2(-x, -z)) / 360 * 4096) & 0xFFF))
    eagl = struct.pack('<4h', len(perches), spec.get('radius', 3200), spec.get('height', -2200), 0)
    eagl += b''.join(struct.pack('<4h', *p) for p in perches)
    return [(b'OBJS', objs), (b'ROTT', struct.pack('<4096I', *arc.word(scene.ROT_TABLE, 4096, 'I'))),
            (b'OBJA', struct.pack('<2H', EAGLE_STAGE, 0)), (b'EAGL', eagl)]


# --- the ship ------------------------------------------------------------------
# An industrial ship out on the bay ("ship"): three models moved together
# (OBJS: hull, house, the rest), OBJA 202 and SHIP: the runtime takes it
# along a lane across the bay and back (ship_step()). Built in cm, x along
# it (the bow at +x), y down, z across; the waterline at y 0.

SHIP_STAGE = 202


def ship_pictures(vr: Vram) -> dict:
    """Its paint at dusk (the dusk in the paint, the lit windows as bright
    as they are; the faces lit nearly evenly): "hull" (grey, a row of portholes, black below a
    white line at the waterline), "house" (white, two rows of windows, some
    lit), "bridge" (one band of glass), "deck" (rust red), "hatch" (green,
    ribbed), "funnel" (black top, red band, grey), "lamp" (red light)."""
    rnd = np.random.default_rng(41)
    out = {}
    def grain(base, k=0.08):
        n = rnd.normal(0, 1, (64, 64))
        n = (n + np.roll(n, 1, 0) + np.roll(n, 1, 1)) / 3
        return np.array(base, float)[None, None] * (1 + k * n[..., None])
    lit_window, dark_window = np.array([255, 214, 130], float), np.array([38, 44, 60], float)
    img = grain((66, 68, 78))
    img[:3] = (118, 118, 122)
    for c in range(64):                             # rust run down from the deck
        if rnd.uniform() < 0.12:
            img[3:3 + int(rnd.integers(8, 30)), c] *= (0.75, 0.62, 0.55)
    for x in (8, 24, 40, 56):
        yy, xx = np.mgrid[0:64, 0:64]
        img[np.hypot(xx - x, yy - 14) < 2.6] = lit_window if rnd.uniform() < 0.5 else dark_window
    img[50:52] = (140, 140, 140)
    img[52:] = grain((34, 34, 38))[52:]
    out['hull'] = img
    img = grain((100, 90, 98), 0.05)
    for row in (10, 38):
        for x in range(4, 64, 12):
            img[row:row + 10, x:x + 7] = lit_window if rnd.uniform() < 0.45 else dark_window
    img[60:] *= 0.8
    out['house'] = img
    img = grain((100, 90, 98), 0.05)
    img[18:40] = dark_window
    for x in range(2, 64, 10):
        if rnd.uniform() < 0.6: img[20:38, x:x + 7] = lit_window
        img[18:40, x + 8:x + 10] = (100, 90, 98)
    out['bridge'] = img
    out['deck'] = grain((64, 44, 40), 0.12)
    img = grain((54, 72, 62))
    img[:, ::8] *= 0.6
    out['hatch'] = img
    img = grain((96, 96, 100), 0.05)
    img[:12] = (26, 24, 24)
    img[22:32] = (130, 34, 30)
    out['funnel'] = img
    out['lamp'] = np.full((64, 64, 3), (255, 48, 36), float)
    return {k: vr.picture(*sixteen(np.clip(v, 0, 255))[::-1]) for k, v in out.items()}


def ship_model(spec: dict) -> list[list[dict]]:
    """The three models' faces: corners, picture, the point they are the
    outside of, the picture's columns repeated "times" across."""
    L = spec.get('length', 5000) / 2
    hull, house, rest = [], [], []
    def quad(where, corners, tex, inside):
        where.append(dict(corners=[tuple(float(v) for v in c) for c in corners], tex=tex, inside=inside))
    def box(where, x0, x1, y0, y1, z, tex, top='deck', piece=400):
        inside = ((x0 + x1) / 2, (y0 + y1) / 2, 0)
        n = max(1, round((x1 - x0) / piece))
        m = max(1, round(2 * z / piece))
        for s in (-1, 1):
            for k in range(n):
                a, b = x0 + (x1 - x0) * k / n, x0 + (x1 - x0) * (k + 1) / n
                quad(where, [(a, y1, s * z), (b, y1, s * z), (a, y0, s * z), (b, y0, s * z)], tex, inside)
            for k in range(m):
                a, b = -z + 2 * z * k / m, -z + 2 * z * (k + 1) / m
                xe = x0 if s < 0 else x1
                quad(where, [(xe, y1, a), (xe, y1, b), (xe, y0, a), (xe, y0, b)], tex, inside)
        if top:
            quad(where, [(x0, y1, -z), (x1, y1, -z), (x0, y1, z), (x1, y1, z)], top, inside)
    # the hull: stations [x, half beam, deck y]
    st = [(-L, 340, -500), (-L + 300, 400, -500), (-L + 1000, 400, -500), (-L + 2000, 400, -500), (-L + 3000, 400, -500),
          (-L + 4000, 400, -510), (L - 500, 330, -550), (L - 200, 200, -610), (L + 50, 0, -690)]
    for (xa, ba, da), (xb, bb, db) in zip(st, st[1:]):
        for s in (-1, 1):
            quad(hull, [(xa, da, s * ba), (xb, db, s * bb), (xa, 0, s * ba * 0.9), (xb, 0, s * bb * 0.9)], 'hull',
                 ((xa + xb) / 2, -250, 0))
        quad(hull, [(xa, da, -ba), (xb, db, -bb), (xa, da, ba), (xb, db, bb)], 'deck', ((xa + xb) / 2, 1e5, 0))
    x0, b0, d0 = st[0]
    quad(hull, [(x0, d0, -b0), (x0, d0, b0), (x0, 0, -b0 * 0.9), (x0, 0, b0 * 0.9)], 'hull', (0, -250, 0))
    # the house aft, the bridge on it, the funnel; hatches, a mast forward
    box(house, -L + 150, -L + 1250, -920, -500, 340, 'house')
    box(house, -L + 250, -L + 950, -1180, -920, 300, 'bridge')
    box(rest, -L + 450, -L + 700, -1600, -1180, 120, 'funnel', top=None, piece=250)
    for a, b in ((-L + 1600, -L + 2200), (-L + 2500, -L + 3100), (-L + 3400, -L + 4000)):
        box(rest, a, b, -600, -500, 280, 'hatch', top='hatch', piece=600)
    box(rest, L - 650, L - 610, -1700, -530, 20, 'funnel', top=None, piece=400)
    box(rest, L - 660, L - 600, -1760, -1700, 30, 'lamp', top='lamp', piece=400)
    box(rest, -L + 560, -L + 600, -1760, -1600, 20, 'funnel', top=None, piece=400)
    box(rest, -L + 550, -L + 610, -1810, -1760, 30, 'lamp', top='lamp', piece=400)
    return [hull, house, rest]


def ship_prims(faces: list[dict], pictures: dict, light: dict) -> tuple[list, list]:
    """(vertices, primitives) of a model: each face wound so that its
    outside faces the camera when the runtime keeps it (cross(b - a, c - a)
    pointing out of it: the objects' frame is y down), lit by the dusk
    (lit()); the lamps as they are, bright."""
    verts, prims = {}, []
    def vid(c):
        c = tuple(int(round(v)) for v in c)
        if c not in verts: verts[c] = len(verts)
        return verts[c]
    for f in faces:
        x, y, (cx, cy) = pictures[f['tex']]
        u0, v0 = (x & 63) * 4, y & 0xFF
        tpage = (x >> 6) | (y & 0x100) >> 4 | (y & 0x200) << 2
        uv = [(u0, v0), (u0 + 63, v0), (u0, v0 + 63), (u0 + 63, v0 + 63)]
        pts = list(f['corners'])
        c = [np.array(p) for p in pts]
        n = np.cross(c[1] - c[0], c[2] - c[0])
        if np.linalg.norm(n) < 1e-6: n = np.cross(c[3] - c[1], c[2] - c[1])
        if n @ (np.mean(c, 0) - np.array(f['inside'])) < 0:
            pts[0], pts[1], pts[2], pts[3] = pts[1], pts[0], pts[3], pts[2]
            uv = [uv[1], uv[0], uv[3], uv[2]]
        colors = [(0x80, 0x80, 0x80)] * 4 if f['tex'] == 'lamp' else lit(pts, light, f['inside'])
        prims.append(decode.Prim(GT4, 'GT4', tuple(vid(p) for p in pts), tuple(uv), cy << 6 | cx >> 4, tpage, tuple(colors)))
    if len(verts) > 256:
        raise SystemExit(f'a ship model of {len(verts)} vertices (256 at most)')
    return [k for k, _ in sorted(verts.items(), key=lambda kv: kv[1])], prims


def ship_sections(spec: dict, pictures: dict, reloc, arc) -> list:
    """OBJS (its models, each drawn by its own call at main rank "rank":
    1022, after the far background, before the floor and the tiles), ROTT,
    OBJA (object stage 202) and SHIP: s16 "lane" (z), "from" and "to" (x),
    "speed" (1/16 cm a frame), "wait" [min, max] frames between passes, y."""
    models = ship_model(spec)
    objs = struct.pack('<I', len(models))
    for faces in models:
        verts, prims = ship_prims(faces, pictures, spec['light'])
        objs += struct.pack('<2H', len(verts), len(prims))
        objs += b''.join(struct.pack('<4h', *v, 0) for v in verts)
        objs += b''.join(pack.prim_bytes(reloc.prim(p)) for p in prims)
    objs += struct.pack('<I', len(models)) + b''.join(struct.pack('<2B3H', m, 0, spec.get('rank', 1022), 128, 0)
                                                     for m in range(len(models)))
    ship = struct.pack('<7h', spec['lane'], spec['from'], spec['to'], spec['speed'], *spec['wait'], spec.get('y', 0))
    return [(b'OBJS', objs), (b'ROTT', struct.pack('<4096I', *arc.word(scene.ROT_TABLE, 4096, 'I'))),
            (b'OBJA', struct.pack('<2H', SHIP_STAGE, 0)), (b'SHIP', ship)]


# --- the gulls -----------------------------------------------------------------
# Herring gulls over the beach ("gulls"): four models (gliding, wings up,
# half way, down), one drawing call a bird (OBJA 203, GULL): the runtime flies
# each round the beach on its own circle, flapping now and then, and draws
# the pose it is in (gull_step()).

GULL_STAGE = 203


def gull_picture(vr: Vram, beak=(228, 188, 52)):
    """Its plumage in the eagle's four corners (REGION): "dark" the grey
    mantle, "light" the black wing tips, "white" the head and body,
    "yellow" the beak."""
    rnd = np.random.default_rng(5)
    img = np.zeros((64, 64, 3))
    for (x, y), rgb in (((0, 0), (150, 156, 166)), ((32, 0), (34, 34, 38)), ((0, 32), (236, 236, 232)),
                        ((32, 32), beak)):
        img[y:y + 32, x:x + 32] = np.clip(np.array(rgb) * (1 + 0.05 * rnd.normal(0, 1, (32, 32)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def gull_model(pose: str, scale: float = 1.0) -> list[dict]:
    """Faces of a gull (as eagle_model(): cm, x right, y down, z forward):
    a slim white body, a short square tail, long wings of three parts, the
    inner grey, the tip black; gliding, they bend down at the wrist."""
    rings = [(30, 3, 3), (22, 5, 5), (14, 6, 6), (0, 8, 7), (-14, 6, 5), (-26, 3, 2)]
    def ring(z, rx, ry):
        return [(0, -ry, z), (rx, 0, z), (0, ry, z), (-rx, 0, z)]
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    for (z0, rx0, ry0), (z1, rx1, ry1) in zip(rings, rings[1:]):
        a, b = ring(z0, rx0, ry0), ring(z1, rx1, ry1)
        for k in range(4):
            j = (k + 1) % 4
            mid = np.mean([a[k], a[j], b[k], b[j]], 0)
            add([a[k], a[j], b[k], b[j]], 'white', outward=mid - np.array([0, 0, mid[2]]))
    tip = (0, 1, 38)
    front = ring(30, 3, 3)
    for k in range(4):
        add([front[k], front[(k + 1) % 4], tip], 'yellow', outward=np.mean([front[k], front[(k + 1) % 4], tip], 0) - (0, 0, 30))
    add([(-3, 0, -24), (3, 0, -24), (-9, 0, -40), (9, 0, -40)], 'white', both=True)
    # wing joints: shoulder, wrist, tip; degrees up from level of each part
    angles = {'glide': (14, -16), 'up': (50, 22), 'half': (12, 0), 'down': (-34, -18)}
    a1, a2 = (np.radians(v) for v in angles[pose])
    a2 = a1 + a2
    for side in (-1, 1):
        ex, ey = 6 + 34 * np.cos(a1), -2 - 34 * np.sin(a1)
        mx, my = ex + 18 * np.cos(a2), ey - 18 * np.sin(a2)
        tx, ty = mx + 22 * np.cos(a2), my - 22 * np.sin(a2)
        add([(side * 6, -2, 10), (side * 6, -2, -10), (side * ex, ey, 8), (side * ex, ey, -8)], 'dark', both=True)
        add([(side * ex, ey, 8), (side * ex, ey, -8), (side * mx, my, 4), (side * mx, my, -9)], 'dark', both=True)
        add([(side * mx, my, 4), (side * mx, my, -9), (side * tx, ty, -4), (side * (tx - 3), ty, -12)], 'light', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def crab_model(pose: str = '', scale: float = 1.0) -> list[dict]:
    """Faces of a crab (as gull_model(): cm, x right, y down, z forward, its
    belly on the ground at y 0): a flat oval shell, four legs a side bent
    down to the sand, two claws held up in front, two black eyes on
    stalks; its shell and legs the picture's "yellow" corner (the beach's
    orange), the eyes "light" (black)."""
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    n = 8
    ring = lambda y, k: [(k * 9 * np.sin(2 * np.pi * s / n), y, k * 6.5 * np.cos(2 * np.pi * s / n)) for s in range(n)]
    low, rim, top = ring(-3, 0.85), ring(-6, 1.0), ring(-8.5, 0.7)
    for a, b in ((low, rim), (rim, top)):
        for s in range(n):
            t = (s + 1) % n
            q = [b[s], b[t], a[s], a[t]]
            add(q, 'yellow', outward=np.mean(q, 0) - np.array([0, -5, 0]))
    cap = (0, -9.5, 0)
    for s in range(n):
        add([top[s], top[(s + 1) % n], cap], 'yellow', outward=(0, -1, 0))
    for side in (-1, 1):
        for k, z in enumerate((3, 0.5, -2, -4.5)):
            knee = (side * 15, -7, z * 1.5)
            add([(side * 8, -5, z), (side * 8, -4, z + 1.5), knee, knee], 'yellow', both=True)
            add([knee, (side * 15, -6, z * 1.5 + 1.5), (side * 19, 0, z * 1.9), (side * 19, 0, z * 1.9)], 'yellow', both=True)
        # a claw: an arm out and up, a pincer at its end
        add([(side * 6, -6, 5), (side * 4, -6, 6), (side * 10, -10, 12), (side * 8, -10, 12)], 'yellow', both=True)
        add([(side * 11, -9, 11), (side * 6, -9, 12), (side * 11, -14, 16), (side * 6, -13, 17)], 'yellow', both=True)
        add([(side * 3, -8, 5), (side * 4, -8, 5), (side * 3, -12, 6), (side * 4, -12, 6)], 'light', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def crow_picture(vr: Vram):
    """A crow's plumage in the gull's regions: all black, the mantle with
    a faint blue sheen, the beak dark grey (against the sunset: a
    silhouette)."""
    rnd = np.random.default_rng(7)
    img = np.zeros((64, 64, 3))
    for (x, y), rgb in (((0, 0), (30, 30, 40)), ((32, 0), (14, 14, 18)), ((0, 32), (24, 22, 28)),
                        ((32, 32), (50, 48, 50))):
        img[y:y + 32, x:x + 32] = np.clip(np.array(rgb) * (1 + 0.08 * rnd.normal(0, 1, (32, 32)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def magpie_picture(vr: Vram):
    """A magpie's plumage in the gull's regions: the inner wing ("dark")
    its white patch, the wing tips ("light") black with a blue sheen, the
    body ("white") black, the beak ("yellow") black."""
    rnd = np.random.default_rng(8)
    img = np.zeros((64, 64, 3))
    for (x, y), rgb in (((0, 0), (232, 234, 236)), ((32, 0), (22, 28, 48)), ((0, 32), (20, 20, 24)),
                        ((32, 32), (30, 30, 32))):
        img[y:y + 32, x:x + 32] = np.clip(np.array(rgb) * (1 + 0.06 * rnd.normal(0, 1, (32, 32)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def magpie_model(pose: str, scale: float = 1.0) -> list[dict]:
    """A gull's faces (gull_model()) and a magpie's long tail, black."""
    faces = gull_model(pose, 1.0)
    faces.append(dict(corners=[(-4, 0, -36), (4, 0, -36), (-6, -2, -74), (6, -2, -74)], region='light', outward=None,
                      both=True))
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def bat_picture(vr: Vram):
    """A flying fox's colours in the gull's regions: the wings' membrane
    ("dark", "light") dark brown, the lower lit a little by the moon, the
    body ("white") its russet mantle, the "yellow" its black face."""
    rnd = np.random.default_rng(9)
    img = np.zeros((64, 64, 3))
    for (x, y), rgb in (((0, 0), (38, 30, 28)), ((32, 0), (26, 20, 20)), ((0, 32), (92, 58, 34)),
                        ((32, 32), (20, 16, 16))):
        img[y:y + 32, x:x + 32] = np.clip(np.array(rgb) * (1 + 0.08 * rnd.normal(0, 1, (32, 32)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def bat_model(pose: str, scale: float = 1.0) -> list[dict]:
    """Faces of a flying fox (as gull_model(): cm, x right, y down, z
    forward): a short furry body, a fox's snout, two pointed ears, no tail;
    each wing a membrane of three parts out to the long fingers, its
    trailing edge scalloped back to the body, the poses the gull's."""
    rings = [(14, 2, 2), (10, 4, 4), (4, 6, 6), (-6, 6, 6), (-14, 3, 3)]
    def ring(z, rx, ry):
        return [(0, -ry, z), (rx, 0, z), (0, ry, z), (-rx, 0, z)]
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    for (z0, rx0, ry0), (z1, rx1, ry1) in zip(rings, rings[1:]):
        a, b = ring(z0, rx0, ry0), ring(z1, rx1, ry1)
        for k in range(4):
            j = (k + 1) % 4
            mid = np.mean([a[k], a[j], b[k], b[j]], 0)
            add([a[k], a[j], b[k], b[j]], 'white', outward=mid - np.array([0, 0, mid[2]]))
    tip = (0, 1, 20)
    front = ring(14, 2, 2)
    for k in range(4):
        add([front[k], front[(k + 1) % 4], tip], 'yellow', outward=np.mean([front[k], front[(k + 1) % 4], tip], 0) - (0, 0, 14))
    for side in (-1, 1):
        add([(side * 1, -3, 10), (side * 3, -3, 8), (side * 2, -8, 9), (side * 2, -8, 9)], 'yellow', both=True)
    angles = {'glide': (10, -10), 'up': (55, 25), 'half': (10, 0), 'down': (-40, -25)}
    a1, a2 = (np.radians(v) for v in angles[pose])
    a2 = a1 + a2
    for side in (-1, 1):
        ex, ey = 5 + 26 * np.cos(a1), -1 - 26 * np.sin(a1)
        tx, ty = ex + 30 * np.cos(a2), ey - 30 * np.sin(a2)
        # shoulder to wrist, the membrane back to the hip; wrist to the
        # fingers' tips, its edge scalloped in between
        add([(side * 5, -1, 8), (side * 5, -1, -12), (side * ex, ey, 6), (side * ex * 0.8, ey * 0.8, -16)], 'dark', both=True)
        add([(side * ex, ey, 6), (side * ex * 0.8, ey * 0.8, -16), (side * tx, ty, 2), (side * (ex + tx) / 2, (ey + ty) / 2, -10)],
            'light', both=True)
        add([(side * ex, ey, 6), (side * (ex + tx) / 2, (ey + ty) / 2, -10), (side * tx, ty, 2), (side * tx, ty, 2)],
            'dark', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def crane_picture(vr: Vram):
    """A red-crowned crane's colours in the gull's regions: "dark" its
    white wings, "light" the black of its neck, secondaries and legs,
    "white" its body; the "yellow" corner's quarters its red crown and its
    horn-coloured beak (REGION "red", "horn")."""
    rnd = np.random.default_rng(10)
    img = np.zeros((64, 64, 3))
    for (x, y, w), rgb in (((0, 0, 32), (238, 238, 234)), ((32, 0, 32), (22, 22, 26)), ((0, 32, 32), (244, 244, 240)),
                           ((32, 32, 32), (150, 150, 120)), ((32, 32, 16), (214, 30, 30))):
        img[y:y + w, x:x + w] = np.clip(np.array(rgb) * (1 + 0.04 * rnd.normal(0, 1, (w, w)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def crane_model(pose: str, scale: float = 1.0) -> list[dict]:
    """Faces of a red-crowned crane in flight (as gull_model(): cm, x right,
    y down, z forward): a white body, the long black neck stretched out
    before it, the red crown, a long beak; the black legs trailing behind
    the short tail; broad wings of three parts, white with their trailing
    half black on the inner two (the secondaries), the poses the gull's
    but slower to the eye (a long wing)."""
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    def ring(z, rx, ry):
        return [(0, -ry, z), (rx, 0, z), (0, ry, z), (-rx, 0, z)]
    rings = [(84, 1.6, 1.6, 'light'), (66, 2.4, 2.4, 'light'), (40, 3, 3, 'light'), (24, 6, 6, 'white'), (10, 10, 9, 'white'),
             (-8, 10, 9, 'white'), (-22, 6, 5, 'white'), (-30, 3, 3, 'white')]
    for (z0, rx0, ry0, reg), (z1, rx1, ry1, _) in zip(rings, rings[1:]):
        a, b = ring(z0, rx0, ry0), ring(z1, rx1, ry1)
        for k in range(4):
            j = (k + 1) % 4
            mid = np.mean([a[k], a[j], b[k], b[j]], 0)
            add([a[k], a[j], b[k], b[j]], reg, outward=mid - np.array([0, 0, mid[2]]))
    # the head: its red crown on top, the beak on before it
    add([(-2, -2, 82), (2, -2, 82), (-2, -2, 90), (2, -2, 90)], 'red', both=True)
    tip, front = (0, 0.5, 104), ring(88, 1.6, 1.6)
    for k in range(4):
        add([front[k], front[(k + 1) % 4], tip], 'horn', outward=np.mean([front[k], front[(k + 1) % 4], tip], 0) - (0, 0, 88))
    add([(-3, -1, 84), (3, -1, 84), (-3, 1, 88), (3, 1, 88)], 'light', both=True)
    # the tail, the drooping black tertials over it, the legs trailing
    add([(-5, 0, -26), (5, 0, -26), (-8, 1, -44), (8, 1, -44)], 'white', both=True)
    add([(-7, -2, -14), (7, -2, -14), (-9, 0, -40), (9, 0, -40)], 'light', both=True)
    for side in (-1, 1):
        add([(side * 2, 5, -18), (side * 4, 5, -18), (side * 2, 7, -86), (side * 3, 7, -86)], 'light', both=True)
    angles = {'glide': (6, -10), 'up': (46, 18), 'half': (8, 0), 'down': (-30, -16)}
    a1, a2 = (np.radians(v) for v in angles[pose])
    a2 = a1 + a2
    for side in (-1, 1):
        ex, ey = 6 + 48 * np.cos(a1), -2 - 48 * np.sin(a1)
        mx, my = ex + 30 * np.cos(a2), ey - 30 * np.sin(a2)
        tx, ty = mx + 34 * np.cos(a2), my - 34 * np.sin(a2)
        lerp = lambda p, q, t: tuple(a + (b - a) * t for a, b in zip(p, q))
        for (p0, p1, q0, q1) in (((side * 6, -2, 14), (side * 6, -2, -14), (side * ex, ey, 12), (side * ex, ey, -16)),
                                 ((side * ex, ey, 12), (side * ex, ey, -16), (side * mx, my, 8), (side * mx, my, -16))):
            # the leading half white, the trailing black
            add([p0, lerp(p0, p1, 0.5), q0, lerp(q0, q1, 0.5)], 'dark', both=True)
            add([lerp(p0, p1, 0.5), p1, lerp(q0, q1, 0.5), q1], 'light', both=True)
        add([(side * mx, my, 8), (side * mx, my, -16), (side * tx, ty, -2), (side * (tx - 4), ty, -14)], 'dark', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def dove_picture(vr: Vram):
    """White doves' colours in the gull's regions ("dark" the wings, white
    a little grey, "light" the primaries' pale grey, "white" the body) and,
    in the yellow corner's quarters, the beak's and feet's coral ("red")
    and Morning Fields' rabbits: "fur" a wild rabbit's grey brown, "eye"
    black, "horn" its pale belly."""
    rnd = np.random.default_rng(11)
    img = np.zeros((64, 64, 3))
    for (x, y, w), rgb in (((0, 0, 32), (236, 236, 236)), ((32, 0, 32), (196, 198, 204)), ((0, 32, 32), (248, 248, 246)),
                           ((32, 32, 16), (222, 120, 110)), ((48, 32, 16), (138, 112, 84)), ((32, 48, 16), (22, 18, 18)),
                           ((48, 48, 16), (204, 188, 160))):
        img[y:y + w, x:x + w] = np.clip(np.array(rgb) * (1 + 0.05 * rnd.normal(0, 1, (w, w)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def dove_model(pose: str, scale: float = 1.0) -> list[dict]:
    """A gull's faces (gull_model()), its beak coral: a white dove."""
    faces = gull_model(pose, 1.0)
    for f in faces:
        if f['region'] == 'yellow': f['region'] = 'red'
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def rabbit_model(pose: str = '', scale: float = 1.0) -> list[dict]:
    """Faces of a wild rabbit sitting (as crab_model(): cm, x right, y
    down, z forward, its feet on the ground at y 0): a round hunched body,
    the head low before it, two long ears laid back, pink inside, black
    eyes, a white scut, the hind feet flat on the ground."""
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    n = 6
    # the body: rings along z, round a middle 10 cm up
    body = [(-15, 4, 4, -9), (-12, 9, 9, -11), (-4, 11, 11, -12), (5, 9, 10, -12), (11, 5, 7, -14)]
    def ring(z, rx, ry, yc):
        return [(rx * np.sin(2 * np.pi * k / n), yc - ry * np.cos(2 * np.pi * k / n), z) for k in range(n)]
    rings = [ring(*b) for b in body]
    for a, b in zip(rings, rings[1:]):
        for k in range(n):
            j = (k + 1) % n
            q = [a[k], a[j], b[k], b[j]]
            mid = np.mean(q, 0)
            # the belly pale
            low = mid[1] > -8
            add(q, 'horn' if low else 'fur', outward=mid - np.array([0, -11, mid[2]]))
    # the head: a smaller ball before the body, its nose a point
    head = [(9, 3, 3, -16), (12, 6, 6, -18), (17, 5, 5, -18)]
    hr = [ring(*h) for h in head]
    for a, b in zip(hr, hr[1:]):
        for k in range(n):
            j = (k + 1) % n
            q = [a[k], a[j], b[k], b[j]]
            add(q, 'fur', outward=np.mean(q, 0) - np.array([0, -18, 13]))
    nose = (0, -17, 21)
    for k in range(n):
        add([hr[-1][k], hr[-1][(k + 1) % n], nose], 'fur', outward=np.mean([hr[-1][k], hr[-1][(k + 1) % n], nose], 0) - (0, -18, 17))
    for side in (-1, 1):
        add([(side * 5.2, -21, 15), (side * 5.2, -21, 13), (side * 5.2, -19, 15), (side * 5.2, -19, 13)], 'eye', both=True)
        # an ear: up from the crown and back, its inside pink
        base0, base1 = (side * 1.5, -23, 13), (side * 4.5, -23, 13)
        tip0, tip1 = (side * 3, -37, 1), (side * 7, -36, 2)
        add([base0, base1, tip0, tip1], 'fur', both=True)
        add([(side * 2.2, -24, 13.6), (side * 4, -24, 13.6), (side * 3.6, -34, 3.2), (side * 6, -33, 4)], 'red', both=True)
        # a hind foot, flat on the ground along the body
        add([(side * 5, -0.5, -12), (side * 9, -0.5, -12), (side * 5, -0.5, 4), (side * 8, -0.5, 4)], 'fur', both=True)
    tail = [(0, -12, -15), (3, -9, -16), (0, -6, -16), (-3, -9, -16)]
    add([tail[0], tail[1], tail[3], tail[2]], 'white', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def vulture_picture(vr: Vram, tint=(1, 1, 1)):
    """Turkey vultures' colours in the gull's regions: "dark" the black
    wing coverts, "light" the silvery flight feathers, "white" the black
    brown body; in the yellow corner's quarters the bare red head ("red")
    and Nowhere Desert's lizards: "fur" a desert iguana's sandy back, "eye"
    black, "horn" its pale belly. tint: the night's light on all of it."""
    rnd = np.random.default_rng(13)
    img = np.zeros((64, 64, 3))
    for (x, y, w), rgb in (((0, 0, 32), (34, 30, 30)), ((32, 0, 32), (124, 120, 122)), ((0, 32, 32), (46, 40, 38)),
                           ((32, 32, 16), (178, 62, 50)), ((48, 32, 16), (164, 142, 112)), ((32, 48, 16), (22, 18, 18)),
                           ((48, 48, 16), (210, 196, 170))):
        img[y:y + w, x:x + w] = np.clip(np.array(rgb) * np.array(tint, float) * (1 + 0.06 * rnd.normal(0, 1, (w, w)))[..., None],
                                        0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def vulture_model(pose: str, scale: float = 1.0) -> list[dict]:
    """Faces of a turkey vulture (as gull_model(): cm, x right, y down, z
    forward): a dark body, the small bare red head and hooked beak, a long
    rounded tail; broad long wings held up in a shallow V when gliding,
    their leading half black, the trailing silver, the primaries spread at
    the tip in five fingers."""
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    def ring(z, rx, ry):
        return [(0, -ry, z), (rx, 0, z), (0, ry, z), (-rx, 0, z)]
    rings = [(30, 2.5, 2.5, 'red'), (24, 3.5, 3.5, 'red'), (16, 7, 7, 'white'), (2, 10, 9, 'white'), (-14, 8, 7, 'white'),
             (-26, 4, 3, 'white')]
    for (z0, rx0, ry0, reg), (z1, rx1, ry1, _) in zip(rings, rings[1:]):
        a, b = ring(z0, rx0, ry0), ring(z1, rx1, ry1)
        for k in range(4):
            j = (k + 1) % 4
            mid = np.mean([a[k], a[j], b[k], b[j]], 0)
            add([a[k], a[j], b[k], b[j]], reg, outward=mid - np.array([0, 0, mid[2]]))
    tip, front = (0, 2, 36), ring(30, 2.5, 2.5)
    for k in range(4):
        add([front[k], front[(k + 1) % 4], tip], 'red', outward=np.mean([front[k], front[(k + 1) % 4], tip], 0) - (0, 0, 30))
    add([(-5, 0, -22), (5, 0, -22), (-9, 1, -50), (9, 1, -50)], 'white', both=True)
    angles = {'glide': (22, -6), 'up': (52, 18), 'half': (16, 0), 'down': (-30, -14)}
    a1, a2 = (np.radians(v) for v in angles[pose])
    a2 = a1 + a2
    lerp = lambda p, q, t: tuple(u + (v - u) * t for u, v in zip(p, q))
    for side in (-1, 1):
        ex, ey = 6 + 42 * np.cos(a1), -2 - 42 * np.sin(a1)
        wx, wy = ex + 34 * np.cos(a2), ey - 34 * np.sin(a2)
        for p0, p1, q0, q1 in (((side * 6, -2, 14), (side * 6, -2, -14), (side * ex, ey, 14), (side * ex, ey, -16)),
                               ((side * ex, ey, 14), (side * ex, ey, -16), (side * wx, wy, 10), (side * wx, wy, -14))):
            add([p0, lerp(p0, p1, 0.5), q0, lerp(q0, q1, 0.5)], 'dark', both=True)
            add([lerp(p0, p1, 0.5), p1, lerp(q0, q1, 0.5), q1], 'light', both=True)
        for f in range(5):
            z0 = 10 - 6 * f
            fx, fy = wx + (22 - 2 * abs(f - 1.5)) * np.cos(a2 - 0.08 * f), wy - 20 * np.sin(a2 - 0.08 * f)
            add([(side * wx, wy, z0), (side * wx, wy, z0 - 4), (side * fx, fy, z0 - 3 - 2 * f), (side * fx, fy, z0 - 6 - 2 * f)],
                'dark', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def lizard_model(pose: str = '', scale: float = 1.0) -> list[dict]:
    """Faces of a desert iguana (as crab_model(): cm, x right, y down, z
    forward, its belly just off the ground): a long low body, the blunt
    head before it with black eyes, the long tail tapering behind, four
    legs splayed out to the ground; its back the picture's "fur", its
    belly "horn"."""
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    n = 6
    def ring(z, rx, ry, yc):
        return [(rx * np.sin(2 * np.pi * k / n), yc - ry * np.cos(2 * np.pi * k / n), z) for k in range(n)]
    body = [(-30, 0.4, 0.4, -1.5), (-20, 1.2, 1.1, -1.8), (-10, 2.2, 1.8, -2.2), (-2, 3.4, 2.4, -2.8), (6, 4.2, 2.8, -3.2),
            (13, 3.6, 2.5, -3.2), (16, 2.4, 2.0, -3.4), (19, 3.0, 2.2, -3.6), (24, 2.0, 1.6, -3.2), (27, 0.6, 0.6, -2.8)]
    rings = [ring(*b) for b in body]
    for a, b in zip(rings, rings[1:]):
        for k in range(n):
            j = (k + 1) % n
            q = [a[k], a[j], b[k], b[j]]
            mid = np.mean(q, 0)
            add(q, 'horn' if mid[1] > -2.4 else 'fur', outward=mid - np.array([0, -2.8, mid[2]]))
    for side in (-1, 1):
        add([(side * 2.2, -5.2, 21), (side * 2.2, -5.2, 19.5), (side * 2.2, -4.0, 21), (side * 2.2, -4.0, 19.5)], 'eye', both=True)
        for z, back in ((9, 1), (-3, -1)):
            hip = (side * 3, -3, z)
            knee = (side * 8, -3.5, z + 2 * back)
            foot = (side * 10, 0, z + 4 * back)
            add([hip, (side * 3, -3, z - 2), knee, (side * 8, -3.5, z + 2 * back - 1.5)], 'fur', both=True)
            add([knee, (side * 8, -3.5, z + 2 * back - 1.5), foot, (side * 11, 0, z + 4 * back - 2)], 'fur', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def flash_picture(vr: Vram, cfg: dict) -> tuple:
    """The lightning's light through a tall window as it falls on the
    floor (seen from above, the window's arched head at row 0): its
    mullions and transoms dark, pale blue white, soft at the edges, added.
    Its palette left black (nothing, added) in the pack; the colours go to
    FLIT (cfg["_flit"]): the runtime lights them while a flash is."""
    from PIL import ImageFilter
    yy, xx = np.mgrid[0:64, 0:64].astype(float)
    half = 24 * np.where(yy < 20, np.sqrt(np.clip(1 - ((20 - yy) / 20) ** 2, 0, 1)), 1)
    inside = np.abs(xx - 31.5) < half
    soft = np.clip((half - np.abs(xx - 31.5)) / 5, 0, 1) * np.clip((62 - yy) / 6, 0, 1)
    bars = (np.abs(xx - 31.5) < 1.6) | (np.abs(np.abs(xx - 31.5) - 12) < 1.2) | (np.abs(yy - 30) < 1.4) | (np.abs(yy - 46) < 1.2)
    k = soft * inside * np.where(bars, 0.15, 1.0) * (0.85 + 0.15 * (yy / 63))
    img = np.asarray(Image.fromarray(np.clip(k * 255, 0, 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(1.2)), float) / 255
    rgb = img[..., None] * np.array(cfg['scenery'].get('flash', {}).get('colour', [150, 164, 210]), float)
    x, y, (cx, cy) = added_picture(rgb, vr)
    cfg['_flit'] = [int(c) for c in vr.v[cy, cx:cx + 16]]
    vr.v[cy, cx:cx + 16] = 0
    return x, y, (cx, cy)


def dark_bat_picture(vr: Vram):
    """Eternal Darkness's bats and spiders in the gull's regions: the
    wings' membrane ("dark", "light") near black, the body ("white") dark
    brown; in the yellow corner's quarters the spiders: "red" the body
    black brown, "fur" the legs, "eye" dull red eyes, "horn" the thread's
    pale silk."""
    rnd = np.random.default_rng(29)
    img = np.zeros((64, 64, 3))
    for (x, y, w), rgb in (((0, 0, 32), (58, 46, 50)), ((32, 0, 32), (84, 68, 70)), ((0, 32, 32), (90, 66, 54)),
                           ((32, 32, 16), (92, 74, 60)), ((48, 32, 16), (70, 56, 46)), ((32, 48, 16), (190, 40, 30)),
                           ((48, 48, 16), (170, 170, 182))):
        img[y:y + w, x:x + w] = np.clip(np.array(rgb) * (1 + 0.07 * rnd.normal(0, 1, (w, w)))[..., None], 0, 255)
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def spider_model(pose: str = '', scale: float = 1.0) -> list[dict]:
    """Faces of a big house spider (as crab_model(): cm, x right, y down, z
    forward): its round abdomen behind, the head and thorax before it with
    red eyes, eight legs bent high at the knee out to the ground; the body
    "red", the legs "fur", the eyes "eye" (the body's first vertex the
    abdomen's top: the runtime's nearness test). pose "hang": the same on
    a thread up out of sight ("horn": two crossed strips in pieces)."""
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    n = 6
    def ring(z, r, yc):
        return [(r * np.sin(2 * np.pi * k / n), yc - 0.8 * r * np.cos(2 * np.pi * k / n), z) for k in range(n)]
    body = [(-9, 0.4, -3.4), (-7.5, 2.6, -3.6), (-5, 3.4, -3.8), (-2, 2.8, -3.6), (0, 1.2, -3.2), (1, 2.0, -3.0),
            (3.5, 2.2, -3.0), (5.5, 1.0, -2.8)]
    rings = [ring(*b) for b in body]
    for a, b in zip(rings, rings[1:]):
        for k in range(n):
            j = (k + 1) % n
            q = [a[k], a[j], b[k], b[j]]
            mid = np.mean(q, 0)
            add(q, 'red', outward=mid - np.array([0, mid[1] if False else -3.4, mid[2]]))
    for side in (-1, 1):
        add([(side * 0.6, -4.6, 5.4), (side * 1.4, -4.4, 5.2), (side * 0.6, -3.8, 5.8), (side * 1.4, -3.6, 5.6)], 'eye', both=True)
        for i, (z, spread) in enumerate(((4.5, 50), (3.0, 20), (1.5, -15), (0.0, -45))):
            a = np.radians(spread)
            out = np.array([np.cos(a) * side, 0, np.sin(a)])
            hip = np.array([side * 1.6, -3.0, z])
            knee = hip + out * 6.5 + np.array([0, -4.5, 0])
            foot = hip + out * 13 + np.array([0, 3.0, 0])
            w = np.array([0, 0, 0.55]) if abs(out[2]) < 0.7 else np.array([0.55, 0, 0])
            add([tuple(hip - w), tuple(hip + w), tuple(knee - w), tuple(knee + w)], 'fur', both=True)
            add([tuple(knee - w), tuple(knee + w), tuple(foot - w * 0.5), tuple(foot + w * 0.5)], 'fur', both=True)
    if pose == 'hang':
        top, pieces, wdt = -1800.0, 8, 0.45 / max(scale, 0.1)
        for k in range(pieces):
            y0, y1 = -4.0 + (top + 4) * k / pieces, -4.0 + (top + 4) * (k + 1) / pieces
            add([(-wdt, y1, -5), (wdt, y1, -5), (-wdt, y0, -5), (wdt, y0, -5)], 'horn', both=True)
            add([(0, y1, -5 - wdt), (0, y1, -5 + wdt), (0, y0, -5 - wdt), (0, y0, -5 + wdt)], 'horn', both=True)
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def drone_picture(vr: Vram):
    """A drone's paint in the eagle's four corners (REGION): "dark" its
    black carbon body and rotor guards, "light" its grey arms, "white" the
    bright lamp under it, "yellow" its red warning lights."""
    rnd = np.random.default_rng(6)
    img = np.zeros((64, 64, 3))
    for (x, y), rgb in (((0, 0), (40, 42, 48)), ((32, 0), (120, 124, 132)), ((0, 32), (250, 250, 240)),
                        ((32, 32), (240, 40, 30))):
        img[y:y + 32, x:x + 32] = np.clip(np.array(rgb) * (1 + 0.05 * rnd.normal(0, 1, (32, 32)))[..., None], 0, 255)
    img[0:32:8, 0:32] *= 0.7                             # the body's panel seams
    colours, idx = sixteen(img)
    return vr.picture(idx, colours)


def drone_model(pose: str, scale: float = 1.0) -> list[dict]:
    """Faces of an inspection drone (as gull_model(): cm, x right, y down,
    z forward): a flat body, four arms to four rotor rings (their blades a
    blur, a dark disc), a lamp under its nose, red lights at the arms' ends.
    Every pose the same (the runtime's flapping changes nothing)."""
    faces = []
    def add(corners, region, outward=None, both=False):
        faces.append(dict(corners=corners, region=region, outward=outward, both=both))
    def box(x0, x1, y0, y1, z0, z1, region):
        c = np.array([(x, y, z) for y in (y0, y1) for z in (z0, z1) for x in (x0, x1)], float)
        mid = c.mean(0)
        for q in ((0, 1, 2, 3), (4, 5, 6, 7), (0, 1, 4, 5), (2, 3, 6, 7), (0, 2, 4, 6), (1, 3, 5, 7)):
            pts = [tuple(c[i]) for i in q]
            add(pts, region, outward=np.mean(pts, 0) - mid)
    box(-14, 14, -9, 0, -20, 20, 'dark')                 # the body
    box(-8, 8, -12, -9, -12, 8, 'light')                 # its battery on top
    box(-5, 5, 0, 4, 12, 20, 'white')                    # the lamp under its nose
    for sx in (-1, 1):
        for sz in (-1, 1):
            ex, ez = sx * 40, sz * 36
            add([(sx * 12, -5, sz * 14), (sx * 12, -3, sz * 18), (ex, -5, ez - sz * 2), (ex, -3, ez + sz * 2)], 'light',
                both=True)
            ring = [(ex + 22 * np.sin(2 * np.pi * k / 8), -8, ez + 22 * np.cos(2 * np.pi * k / 8)) for k in range(8)]
            for k in range(8):
                add([ring[k], ring[(k + 1) % 8], (ex, -8, ez)], 'dark', both=True)
                # the guard round the rotor: a light band, seen edge on too
                p0, p1 = ring[k], ring[(k + 1) % 8]
                add([(p0[0], -12, p0[2]), (p1[0], -12, p1[2]), (p0[0], -4, p0[2]), (p1[0], -4, p1[2])], 'light',
                    both=True)
            box(ex - 2, ex + 2, -3, 1, ez - 2, ez + 2, 'yellow')
    for f in faces:
        f['corners'] = [tuple(scale * v for v in c) for c in f['corners']]
    return faces


def gull_sections(spec: dict, picture, reloc, arc, model=gull_model, grass: tuple | None = None) -> list:
    """OBJS (the four poses, each in a model; a drawing call a bird, its
    model the bird's number: the runtime draws its pose instead), ROTT,
    OBJA (object stage 203) and GULL: s16 birds, "radius" [min, max] of
    their circles, "height" [min, max] (y, cm, up is negative), "speed"
    (1/16 cm a frame), "centre" [z, spread]: their circles' middles, before
    the camera (it looks at +z). grass (spec, pictures): a square of
    "grass" too (Morning Fields' flowers), the 5th model, its four layouts
    after the critters, GRAS."""
    n = min(8 if spec.get('flock') else 4, spec.get('count', 4))
    # "rabbits" ground creatures as the crabs, hopping (GULL's critters' kind 1)
    ground = spec.get('crabs') or spec.get('rabbits') or spec.get('pigeons') or spec.get('lizards') or spec.get('spiders') or {}
    # "spiders" "hang": that many more on threads, coming down from above (the 5th model)
    hang = ground.get('hang', 0) if spec.get('spiders') else 0
    if hang and grass:
        raise SystemExit('spiders hanging and grass: both are the 5th model')
    crabs = ground.get('count', 0) + hang
    if n + crabs > 8:
        raise SystemExit(f'{n} birds and {crabs} crabs: 8 drawing calls at most')
    objs = struct.pack('<I', 5 if grass or hang else 4)
    # with crabs ("crabs": "count", "near", "far" cm, "scale") the birds
    # flap without the half way pose: model 3 is the crab
    poses = [(p, model) for p in ('glide', 'up', 'down')] + [('crab', rabbit_model if spec.get('rabbits') else pigeon_model if spec.get('pigeons') else
                 lizard_model if spec.get('lizards') else spider_model if spec.get('spiders') else crab_model)] if crabs else         [(p, model) for p in ('glide', 'up', 'half', 'down')]
    for pose, make in poses:
        scale = ground.get('scale', 1.0) if pose == 'crab' else spec.get('scale', 1.0)
        verts, prims = eagle_prims(make(pose, scale), picture)
        objs += struct.pack('<2H', len(verts), len(prims))
        objs += b''.join(struct.pack('<4h', *v, 0) for v in verts)
        objs += b''.join(pack.prim_bytes(reloc.prim(p)) for p in prims)
    if grass:
        objs += grass_model(grass[0], grass[1], reloc)
    if hang:
        verts, prims = eagle_prims(spider_model('hang', ground.get('scale', 1.0)), picture)
        objs += struct.pack('<2H', len(verts), len(prims))
        objs += b''.join(struct.pack('<4h', *v, 0) for v in verts)
        objs += b''.join(pack.prim_bytes(reloc.prim(p)) for p in prims)
    objs += struct.pack('<I', n + crabs + (4 if grass else 0))
    objs += b''.join(struct.pack('<2B3H', m & 3, 0, spec.get('rank', 1009), 64, 0) for m in range(n))
    # the crabs on the sand: over the floor and the tiles, under the fighters
    objs += b''.join(struct.pack('<2B3H', 3, 0, ground.get('rank', 980), 64, 0) for _ in range(crabs - hang))
    # the hanging ones behind the gauges (their threads up through the screen's top)
    objs += b''.join(struct.pack('<2B3H', 3, 0, ground.get('hang_rank', 1008), 64, 0) for _ in range(hang))
    if grass:
        objs += b''.join(struct.pack('<2B3H', 4, 0, grass[0].get('rank', 980), 128, 0) for _ in range(4))
    gull = struct.pack('<8h', n, *spec['radius'], *spec['height'], spec.get('speed', 40), *spec.get('centre', [0, 1000]))
    if spec.get('flock') or crabs or spec.get('flutter'):
        # "flock": the birds fly together, a V behind the first (cm apart)
        gull += struct.pack('<h', spec.get('flock', 0))
    if crabs or spec.get('flutter'):
        gull += struct.pack('<3h', crabs, ground['near'], ground['far']) if crabs else bytes(6)
    if spec.get('flutter') or spec.get('rabbits') or spec.get('pigeons') or spec.get('lizards') or spec.get('spiders'):
        # "flutter" (cm): a bat's way, all flapping, darting from side to side
        gull += struct.pack('<h', spec.get('flutter', 0))
    if spec.get('rabbits') or spec.get('pigeons') or spec.get('lizards') or spec.get('spiders'):
        # the critters' kind: 1 rabbits, hopping about, fleeing the fighters;
        # 2 New York's pigeons, walking, pecking, fluttering off; 3 Nowhere
        # Desert's lizards, still a long while, darting off; 4 Eternal
        # Darkness's spiders, scuttling in short runs
        gull += struct.pack('<h', 1 if spec.get('rabbits') else 2 if spec.get('pigeons') else 3 if spec.get('lizards') else 4)
    if hang:
        # the last critters hanging on threads, and how low they come (cm, up negative)
        gull += struct.pack('<2h', hang, ground.get('low', -110))
    return [(b'OBJS', objs), (b'ROTT', struct.pack('<4096I', *arc.word(scene.ROT_TABLE, 4096, 'I'))),
            (b'OBJA', struct.pack('<2H', GULL_STAGE, 0)), (b'GULL', gull)] +         ([(b'GRAS', struct.pack('<2h', grass[0]['period'] * 10, 0))] if grass else [])


def dust(spec: dict, vr: Vram) -> tuple[int, int, tuple[int, int]]:
    """The dust's puff ("dust"): a round cloud of 16 x 16 texels in the
    corner of a picture, sand brown ("colour"), lighter in its middle, its
    edge strewn (dithered: the GPU blends half and half, nothing finer).
    Every colour has the semi-transparency bit: it is laid half over what
    is behind it (blending mode 0), a brown haze on the ground, never a
    glow. The runtime raises it where a fighter hits the ground (DUST: the
    texture page, palette, u, v as SNOW), lets it settle again."""
    rnd = np.random.default_rng(9)
    r = np.hypot(np.arange(16)[:, None] - 7.5, np.arange(16)[None] - 7.5) / 7.5
    keep = (1 - r) > rnd.uniform(0, 0.55, (16, 16))
    idx = np.zeros((64, 64), np.uint8)
    idx[:16, :16] = np.where(keep, np.clip(np.round(15 * (1 - r) + 1), 1, 15), 0)
    colour = np.array(spec['colour'], float)
    ramp = [0] + [0x8000 | int(bgr555(colour * (0.72 + 0.28 * i / 15))) for i in range(1, 16)]
    return vr.picture(idx, ramp)


def add_sections(path: Path, extra: list) -> None:
    """Sections appended to a pack (as tools/ttt1_arcade_stage.py does)."""
    b = path.read_bytes()
    magic, version, stage, n, _ = struct.unpack_from('<4s2H2I', b, 0)
    sections = [(sid, b[off:off + size]) for sid, off, size in
                (struct.unpack_from('<4s2I', b, 16 + 12 * i) for i in range(n))] + list(extra)
    table, body, off = b'', b'', 16 + 12 * len(sections)
    for sid, data in sections:
        data = pack.pad4(data)
        table += struct.pack('<4s2I', sid, off, len(data))
        body += data
        off += len(data)
    path.write_bytes(struct.pack('<4s2H2I', magic, version, stage, len(sections), off) + table + body)


def stage_of(owner: str) -> int:
    """The Tekken 2 stage an owner's pack is: "stage" of its "arcade"
    entry (a stage lent to a fighter to try it, not in "owners": no music
    or old stage goes with it), else its "owners" one."""
    return DATA['arcade'][owner].get('stage', DATA['owners'].get(owner))


def build(owner: str, src, tekken3: Path, out: Path, cfg: dict | None = None) -> dict:
    """src: the disc (tools/tekken2_stages.py Source: entry()); cfg: the
    recipe (default the owner's; classic_cfg()'s for the add-on's frame)."""
    cfg = cfg or DATA['arcade'][owner]
    stage = stage_of(owner)
    # "source": the Tekken 2 fighter whose stage it is, for an owner who
    # is not one of Tekken 2's (its music stays its own)
    group = t2_stage.GROUPS + t2_stage.GROUP * cfg.get('source', DATA['fighters'].get(owner))
    vram, rec = t2_stage.vram_of(src.entry(group)), t2_stage.record(src.entry(group + 1))
    arc = decode.Arcade(tekken3, cache=False)
    st = scene.Stage(arc, cfg['base'])
    vr = Vram()
    for rect in RESERVED:
        vr.block(*rect)
    if RESERVED:
        vr.clut_run = 16                 # (the add-on's) the VRAM fuller: shorter runs find room
    heli = None
    if cfg.get('helicopter'):
        # King's helicopter (Tekken 3's stage 11 objects): its pictures at
        # their own place, the slots under them kept free
        for t in decode.load_tims(arc.archive_files(scene.OBJECTS[11][0])[:1]):
            for kind, (x, y, w, h, raw) in (('image', t['image']), ('clut', t['clut'])):
                vr.v[y:y + h, x:x + w] = np.frombuffer(raw[:w * h * 2], '<u2').reshape(h, w)
                vr.rects.append((kind, x, y, w, h, t['bpp']))
                if kind == 'image':
                    vr.block(x, y, w, h)
        heli = scene.parse_objects(arc, 11)
    animated = cfg.get('sea') or cfg.get('pool') or cfg.get('clouds') or cfg.get('water')
    if animated or cfg.get('candles') or cfg.get('lights') or cfg.get('flicker') or cfg.get('forest'):
        vr.clut_run = 16                 # the VRAM full of pictures: shorter runs find room
    pic = panorama_picture(cfg, vram, rec)
    sea = [] if animated else None
    lights = city_lights(cfg, pic) if cfg.get('windows') else None
    hk = hk_lights(cfg, pic) if cfg.get('lights') else None
    pg = pagoda_lights(cfg, pic) if cfg.get('flicker') else None
    own = lights[0] if lights else hk[0] if hk else pg[0] if pg else None
    weave = None
    if cfg.get('weave'):
        # Eternal Darkness's spider spinning its web: a cell of its own
        cell, wcolours, widx = weave_frames(pic, cfg['weave'])
        own = dict(own or {})
        if cell in own:
            raise SystemExit(f'weave: cell {cell} is a lit one (a candle or a window), take another')
        own[cell] = ([c or 0x0421 for c in wcolours], widx[0])
    fb = far_background(cfg, st.farbg, pic, vr, sea, own)
    if cfg.get('weave'):
        d = fb.desc[cell[0]][cell[1]]
        wx, wy = (d >> 8 & 15) * 64 + (d & 3) * 16, 512 + (d >> 12 & 3) * 256 + (d >> 2 & 3) * 64
        if RESERVED:
            # (the add-on's) the cell's y: desc's page row is already the
            # VRAM's (vram_desc(): y // 256), the old fork's 512 more put the
            # spider's frames past the VRAM's foot
            wy -= 512
        wclut = (fb.cluts[cell[0]][cell[1]] & 63) * 16, fb.cluts[cell[0]][cell[1]] >> 6
        made, places = {}, []
        for k, ix in enumerate(widx):
            key = ix.astype(np.uint8).tobytes()
            if key not in made:
                # (the add-on's: the frames past the VRAM, T2AN)
                made[key] = (wx, wy) if k == 0 else vr.frame(ix, None, clut=wclut)[:2]
            places.append(made[key])
        weave = ((wx, wy), places)
        print(f'weave: {len(made)} pictures, {len(places)} frames')
    if pg:
        # the Pagoda's lights: the eyes' palettes one run, the candles' another
        _, (ecells, eframes), (ccells, cframes) = pg
        pg_runs = []
        for row, cells, frames in ((EYE_ROW, ecells, eframes), (FLICKER_ROW, ccells, cframes)):
            if not cells:
                pg_runs.append(None); continue
            if PG_CHUNK and RESERVED:
                # (the add-on's, a stage whose long runs find no room) the
                # palettes in runs of PG_CHUNK, each its own row and animation
                parts = []
                for j in range(0, len(frames[0]), PG_CHUNK):
                    r0 = row - j // PG_CHUNK
                    palette_run(vr, r0, frames[0][j:j + PG_CHUNK])
                    for k, (r, c) in enumerate(cells[j:j + PG_CHUNK]):
                        fb.cluts[r][c] = r0 << 6 | k
                    parts.append((r0, len(frames[0][j:j + PG_CHUNK]),
                                  palette_frames(vr, r0, [f[j:j + PG_CHUNK] for f in frames])))
                pg_runs.append(parts)
                continue
            palette_run(vr, row, frames[0])
            for k, (r, c) in enumerate(cells):
                fb.cluts[r][c] = row << 6 | k
            pg_runs.append((row, len(cells), palette_frames(vr, row, frames)))
        print(f'flicker: {len(ecells)} eye (or rain) cells, {len(ccells)} candle cells')
    sign = None
    if hk:
        # Hong Kong's lights: the windows' palettes one run, the beacons'
        # and the signs' (with the rooftop's neon sign after them) another
        _, (wcells, wframes), (fcells, fframes) = hk
        nf = len(fframes)
        if cfg.get('scenery', {}).get('neon_sign'):
            sign, sframes = hk_sign(vr, (16 * len(fcells), FAST_ROW), nf, cfg['scenery']['neon_sign'])
            fframes = [pals + [sp] for pals, sp in zip(fframes, sframes)]
        palette_run(vr, WINDOW_ROW, wframes[0])
        palette_run(vr, FAST_ROW, fframes[0])
        for k, (r, c) in enumerate(wcells):
            fb.cluts[r][c] = WINDOW_ROW << 6 | k
        for k, (r, c) in enumerate(fcells):
            fb.cluts[r][c] = FAST_ROW << 6 | k
        win_runs = [palette_run(vr, WINDOW_ROW - 1 - f, pals) for f, pals in enumerate(wframes)]
        fast_runs = [palette_run(vr, FAST_ROW - 1 - f, pals) for f, pals in enumerate(fframes)]
        hk_width = (len(wcells), len(fframes[0]))
        print(f'lights: {len(wcells)} window cells, {len(fcells)} beacon and neon cells')
    if lights:
        # the lit cells' palettes: one run shown, a run a frame under it
        own, cells, frames, blink = lights
        shown_run = palette_run(vr, WINDOW_ROW, frames[0])
        for k, (r, c) in enumerate(cells):
            fb.cluts[r][c] = shown_run << 6 | (16 * k) >> 4
        frame_runs = [palette_run(vr, WINDOW_ROW - 1 - f, pals) for f, pals in enumerate(frames)]
        beacon_clut = fb.cluts[blink[0]][blink[1]] if blink else None
        if blink:
            seq = [int(bgr555(np.array(v, float))) for v in
                   cfg['windows'].get('blink', [[255, 40, 30], [150, 24, 20], [60, 40, 40], [60, 40, 40]])]
            vr.v[BLINK_ROW, :len(seq)] = seq
            vr.rects.append(('clut', 0, BLINK_ROW, 16, 1, 4))
    surf = water_animation(cfg, sea, vr, pic) if cfg.get('water') else cloud_animation(cfg, sea, vr, pic) if cfg.get('clouds') else         (beach_animation if cfg.get('sea') else pool_animation)(cfg, sea, vr) if sea else None
    skies = (window_skies if cfg['lightning'].get('windows') else bolt_skies)(cfg, pic, fb, vr) \
        if cfg.get('lightning') else None
    # "floor_base": another arcade stage's floor grid and its light (kind 2:
    # a light at the fighters' midpoint, the floor dark toward its edges)
    fl_base = scene.Stage(arc, cfg['floor_base']).floor if 'floor_base' in cfg else st.floor
    fl = floor(fl_base, vram, rec, vr, cfg.get('floor_tufts'), cfg.get('grade', {}).get('floor', cfg.get('floor_tint')),
               cfg.get('floor_recolour'), cfg.get('floor_flowers'), cfg.get('floor_chase'), cfg.get('floor_puddles'), cfg.get('floor_roll'),
               cfg.get('floor_grains'), cfg.get('floor_blend'))
    mote = dust(cfg['dust'], vr) if cfg.get('dust') else None
    feathers = eagle_picture(vr) if cfg.get('eagle') else None
    paint = ship_pictures(vr) if cfg.get('ship') else None
    birds = cfg.get('gulls') or cfg.get('crows') or cfg.get('drones') or cfg.get('magpies') or cfg.get('bats') or         cfg.get('cranes') or cfg.get('doves') or cfg.get('egrets') or cfg.get('vultures')
    plumage = pigeon_picture(vr) if cfg.get('gulls', {}).get('pigeons') else \
        gull_picture(vr, tuple(cfg['gulls'].get('beak', (228, 188, 52)))) if cfg.get('gulls') else crow_picture(vr) if cfg.get('crows') else \
        drone_picture(vr) if cfg.get('drones') else magpie_picture(vr) if cfg.get('magpies') else \
        (dark_bat_picture(vr) if cfg['bats'].get('spiders') else bat_picture(vr)) if cfg.get('bats') else crane_picture(vr) if cfg.get('cranes') else \
        dove_picture(vr) if cfg.get('doves') else egret_picture(vr) if cfg.get('egrets') else \
        vulture_picture(vr, cfg['vultures'].get('tint', (1, 1, 1))) if cfg.get('vultures') else None
    lamp = beacon_frames(vr, bool(cfg['scenery'].get('steams'))) if cfg.get('scenery', {}).get('beacons') else None
    mist = fog_frames(vr, cfg['scenery'].get('fog', {}).get('colour', [96, 88, 128])) \
        if cfg.get('scenery', {}).get('fogs') else None
    flames = lamp_frames(vr) if any(cfg.get('scenery', {}).get(k) for k in ('toros', 'lantern_lines', 'seokdeungs')) \
        else None
    chapel = candle_animation(cfg, vr) if cfg.get('candles') else None
    temple = pagoda_pictures(vr) if cfg.get('flicker') else forest_pictures(vr) if cfg.get('forest') else None
    # Serene Forest: its sunlight pulsing (ANI4), its mist the second animation (ANI2)
    sun_pals = temple.pop('sun_pals') if cfg.get('forest') else None
    sun = sun_pulse(vr, sun_pals, cfg['forest']) if sun_pals and cfg['scenery'].get('sun_patches') else None
    mist2 = None
    if mist and cfg.get('forest'):
        mist2, mist = mist, None
    flutter = None
    if cfg.get('banners'):
        # Hong Kong's picture (hk_frames()): shown, then its frames
        n = cfg['banners'].get('frames', 8)
        bcolours, bframes = hk_frames(n)
        shown = vr.picture(bframes[0], bcolours)
        flutter = (shown, [vr.frame(fr, None, clut=shown[2])[:2] for fr in bframes])
    if sum(bool(a) for a in (lamp, mist, flames, surf, chapel, flutter)) > 1:
        raise SystemExit('beacons, fog, lanterns, candles, banners and the sea: all are the animation (one of them)')
    stats = {}
    if cfg.get('scenery'):
        for key in ('tufts', 'flowers'):
            if cfg['scenery'].get(key, {}).get('on_grass'):
                cfg['scenery'][key]['grass'] = grass_cells(vram, rec)
        tex = textures(cfg, vram, rec, vr, arc)
        if lamp:
            tex['beacon'] = [lamp[0]]
            if lamp[2]: tex['steam'] = [lamp[2]]
        if mist or mist2:
            tex['fog'] = [(mist or mist2)[0]]
        if flames:
            tex['lamp'] = [flames[0]]
        if chapel:
            tex['candle'], tex['smoke'] = [chapel[0]], [chapel[1]]
        if flutter:
            tex['banner'] = [flutter[0]]
        if sign:
            tex.update(sign)
        if temple:
            tex.update(temple)
        if cfg['scenery'].get('glass_patches'):
            tex.update(glass_pictures(vr))
        if cfg['scenery'].get('flash_patches'):
            tex['flashwin'] = [flash_picture(vr, cfg)]
        if surf and cfg.get('water'):
            tex['nyc'], tex['steam'] = [surf[0]], [surf[3]]
        elif surf and surf[0]:
            tex['flame' if cfg.get('sea') else 'flags' if cfg.get('clouds') else 'mlamp'] = [surf[0]]
            if cfg.get('pool', {}).get('spray'):
                tex['fwater'] = [surf[3]]
            if cfg.get('clouds') and surf[3]:
                tex['smoke'] = [surf[3]]
        geo, stats = scenery(cfg, tex, cfg['scenery'].get('tile_size', st.geo.tile_size), cfg['scenery'].get('flip', False))
    else:
        geo = copy.deepcopy(st.geo)
        for t in geo.tiles:
            t.verts, t.prims = [], []
    fb.stage = st.stage = PACK_STAGE + stage
    st.tims, st.vram, st.rects, st.patches, st.anim = [], vr.v, vr.rects, [], []
    st.objects, st.rot_table = [], None
    if heli:
        st.objects, st.object_stage = heli, 11
        st.rot_table = arc.word(scene.ROT_TABLE, 4096, 'I')
    st.geo, st.floor, st.farbg = geo, fl, fb
    out.parent.mkdir(parents=True, exist_ok=True)
    layout = scene.object_layout
    if getattr(st, 'object_stage', None) is not None:
        scene.object_layout = lambda s, _o=st.object_stage: layout(_o)
    try:
        pack.save(st, out)
    finally:
        scene.object_layout = layout
    reloc, extra = pack.Relocation(st), []
    if heli:
        if cfg.get('eagle') or cfg.get('grass') or birds or cfg.get('ship'):
            raise SystemExit('a helicopter and other objects: all are the objects (one of them)')
        # OBJA: the objects act as King's stage's (its helicopter flying round)
        extra.append((b'OBJA', struct.pack('<2H', 11, 0)))
    if mote:
        # DUST: u16 texture page (blending mode 0: half and half), u16 palette (relocated), u8 u, v
        (mx, my, (cx, cy)) = mote
        tpage = (mx >> 6) | (my & 0x100) >> 4 | (my & 0x200) << 2
        extra.append((b'DUST', struct.pack('<2H2B2x', tpage, reloc.clut(cy << 6 | cx >> 4), (mx & 63) * 4, my & 0xFF)))
    # "grass" "tex": its pictures by name (Morning Fields' flowers among green tufts)
    turf = [p for name in cfg['grass'].get('tex', ['tuft']) for p in tex[name]] if cfg.get('grass') else None
    if cfg.get('grass') and not plumage:
        if feathers:
            raise SystemExit('an eagle and grass: both are the objects (one or the other)')
        extra += grass_sections(cfg['grass'], turf, reloc, arc)
    if paint:
        if feathers or cfg.get('grass'):
            raise SystemExit('a ship and an eagle or grass: all are the objects (one of them)')
        extra += ship_sections(cfg['ship'], paint, reloc, arc)
    if plumage:
        if feathers or paint:
            raise SystemExit('gulls and a ship or an eagle: all are the objects (one of them)')
        # drones fly as the gulls do (OBJA 203, GULL), their model their own
        extra += gull_sections(birds, plumage, reloc, arc, drone_model if cfg.get('drones') else
                               magpie_model if cfg.get('magpies') else bat_model if cfg.get('bats') else
                               crane_model if cfg.get('cranes') else dove_model if cfg.get('doves') else
                               egret_model if cfg.get('egrets') else vulture_model if cfg.get('vultures') else
                               harbour_gull_model if cfg.get('gulls', {}).get('pigeons') else gull_model,
                               (cfg['grass'], turf) if cfg.get('grass') else None)
    if lamp:
        # ANIM: the shown picture's place, its size (halfwords), the period, the frames' places
        (sx, sy, _), frames, puff = lamp
        extra.append((b'ANIM', struct.pack('<6H', sx, sy, 32 if puff else 16, 64, decode.ANIM_PERIOD, len(frames))
                      + b''.join(struct.pack('<2H', fx, fy) for fx, fy, _ in frames)))
    if flames:
        (sx, sy, _), frames = flames
        extra.append((b'ANIM', struct.pack('<6H', sx, sy, 16, 64, cfg['scenery'].get('flicker', decode.ANIM_PERIOD),
                                           len(frames)) + b''.join(struct.pack('<2H', fx, fy) for fx, fy, _ in frames)))
    if surf:
        # the sea row and the flame: one rectangle, its frames forth and back
        (sx, sy), _, places = surf[1], None, surf[2]
        period = animated.get('period', 10)
        extra.append((b'ANIM', struct.pack('<6H', sx, sy, 16 * (len(sea) + after_sea(cfg)), 64, period, len(places))
                      + b''.join(struct.pack('<2H', fx, fy) for fx, fy in places)))
    if lights:
        # ANI2: the windows, once a round (mode 1), each frame a run of
        # palettes where the relocation put it; ANI3: the mast's beacon, its
        # colour 15 blinking round and round
        spec = cfg['windows']
        at = lambda x, y: (reloc.map[x // 16, y][0] * 16 + x % 16, reloc.map[x // 16, y][1])
        sx, sy = at(0, shown_run)
        extra.append((b'ANI2', struct.pack('<6H', sx, sy, 16 * len(cells), 1, spec.get('period', 150), len(frame_runs))
                      + b''.join(struct.pack('<2H', *at(0, y)) for y in frame_runs) + struct.pack('<H', 1)))
        if blink:
            bx, by = (beacon_clut & 63) * 16 + 15, beacon_clut >> 6
            extra.append((b'ANI3', struct.pack('<6H', *at(bx, by), 1, 1, spec.get('blink_period', 18), len(seq))
                          + b''.join(struct.pack('<2H', *at(k, BLINK_ROW)) for k in range(len(seq))) + struct.pack('<H', 0)))
    if hk:
        # ANI2: the windows going out and lighting again, ANI4: the beacons
        # and the neon, ANI3: the floor's running lights; each frame a run
        # of palettes where the relocation put it, round and round (mode 0)
        at = lambda x, y: (reloc.map[x // 16, y][0] * 16 + x % 16, reloc.map[x // 16, y][1])
        for sid, row, runs, width, period in ((b'ANI2', WINDOW_ROW, win_runs, hk_width[0], cfg['lights']['windows'].get('period', 120)),
                                              (b'ANI4', FAST_ROW, fast_runs, hk_width[1], cfg['lights'].get('fast_period', 8))):
            if width:
                extra.append((sid, struct.pack('<6H', *at(0, row), 16 * width, 1, period, len(runs))
                              + b''.join(struct.pack('<2H', *at(0, y)) for y in runs) + struct.pack('<H', 0)))
    if pg:
        # ANI2: the statues' eyes, ANI4: the candles' light; each frame a run
        # of palettes where the relocation put it, round and round (mode 0)
        at = lambda x, y: (reloc.map[x // 16, y][0] * 16 + x % 16, reloc.map[x // 16, y][1])
        for sid, run, period in ((b'ANI2', pg_runs[0], cfg['flicker'].get('eyes', {}).get('period', 30) if cfg['flicker'].get('eyes')
                                  else cfg['flicker'].get('period', 3)),
                                 (b'ANI4', pg_runs[1], cfg['flicker'].get('period', 3))):
            for row, width, runs in (run if isinstance(run, list) else [run] if run else []):
                extra.append((sid, struct.pack('<6H', *at(0, row), 16 * width, 1, period, len(runs))
                              + b''.join(struct.pack('<2H', *(y if isinstance(y, tuple) else at(0, y))) for y in runs)
                              + struct.pack('<H', 0)))
    if sun:
        # ANI4: the forest's sunlight, each frame a run of palettes where the
        # relocation put it, round and round (mode 0)
        at = lambda x, y: (reloc.map[x // 16, y][0] * 16 + x % 16, reloc.map[x // 16, y][1])
        row, runs = sun
        extra.append((b'ANI4', struct.pack('<6H', *at(0, row), 16 * 4, 1, cfg['forest'].get('period', 8), len(runs))
                      + b''.join(struct.pack('<2H', *at(0, y)) for y in runs) + struct.pack('<H', 0)))
    if mist2:
        # ANI2: the forest's mist (the ANIM its crowns)
        (sx, sy, _), frames = mist2
        extra.append((b'ANI2', struct.pack('<6H', sx, sy, 16, 64, cfg['scenery'].get('fog', {}).get('period', decode.ANIM_PERIOD),
                                           len(frames)) + b''.join(struct.pack('<2H', fx, fy) for fx, fy, _ in frames)
                      + struct.pack('<H', 0)))
    if weave:
        # ANI3: the web being spun, round and round (mode 0)
        (wx, wy), places = weave
        extra.append((b'ANI3', struct.pack('<6H', wx, wy, 16, 64, cfg['weave'].get('period', 30), len(places))
                      + b''.join(struct.pack('<2H', *pl) for pl in places) + struct.pack('<H', 0)))
    if getattr(fl, 'chase', None):
        # T2CH (issue #12; src/tekken2/tmst_tekken2_stages.c): the running
        # lights' palettes where the relocation put them, each frame's
        # colours, stepped as Tekken 2 steps them
        at = lambda x, y: (reloc.map[x // 16, y][0] * 16 + x % 16, reloc.map[x // 16, y][1])
        frames, places = fl.chase
        spec = cfg['floor_chase']
        extra.append((b'T2CH', struct.pack('<4H', spec.get('period', 60), len(frames), len(places), spec.get('add', 1))
                      + b''.join(struct.pack('<2H', *at(*pl)) for pl in places)
                      + b''.join(struct.pack('<16H', *pal) for pals in frames for pal in pals)))
    if getattr(fl, 'blend', None):
        # T2BL (issue #12): the fading palettes where the relocation put them
        at = lambda x, y: (reloc.map[x // 16, y][0] * 16 + x % 16, reloc.map[x // 16, y][1])
        spec = cfg['floor_blend']
        extra.append((b'T2BL', struct.pack('<4H', len(fl.blend), spec.get('first', 0), spec.get('last', 15), spec.get('add', 17))
                      + b''.join(struct.pack('<2H', *at(*pl)) + struct.pack('<16H', *a) + struct.pack('<16H', *b)
                                 for pl, a, b in fl.blend)))
    if flutter:
        (sx, sy, _), places = flutter
        extra.append((b'ANIM', struct.pack('<6H', sx, sy, 16, 64, cfg['banners'].get('period', 6), len(places))
                      + b''.join(struct.pack('<2H', fx, fy) for fx, fy in places)))
    if chapel:
        # the candles and the smoke: one rectangle, round and round
        (sx, sy, _), places = chapel[0], chapel[2]
        extra.append((b'ANIM', struct.pack('<6H', sx, sy, 32, 64, cfg['candles'].get('period', decode.ANIM_PERIOD),
                                           len(places)) + b''.join(struct.pack('<2H', fx, fy) for fx, fy in places)))
    if mist:
        (sx, sy, _), frames = mist
        period = cfg['scenery'].get('fog', {}).get('period', decode.ANIM_PERIOD)      # frames each
        extra.append((b'ANIM', struct.pack('<6H', sx, sy, 16, 64, period, len(frames))
                      + b''.join(struct.pack('<2H', fx, fy) for fx, fy, _ in frames)))
    if cfg.get('fireflies'):
        if not cfg.get('pool') and not cfg.get('clouds') and not chapel and not temple:
            raise SystemExit('fireflies: their glow is in the pool animation\'s lamp picture')
        # FFLY: count, distance min and max, height low and high (mm, up
        # negative), size (mm); texture page (added), palette (relocated), u,
        # v of the glow's 16 x 16; colour; then the kind (1: butterflies,
        # the Szechwan picture's, solid, beating their wings)
        # (the church's motes: the candle picture's glow, at the same place)
        # (the Pagoda's dust: the mote picture's glow, at the same place)
        f, (lx, ly, (cx, cy)) = cfg['fireflies'], (chapel or (surf if surf and surf[0] else None) or temple['mote'])[0]
        tpage = (lx >> 6) | (ly & 0x100) >> 4 | (ly & 0x200) << 2 | 0x20
        extra.append((b'FFLY', struct.pack('<6h2H2B', f['count'], f['near'], f['far'], *f['height'], f.get('size', 60),
                                           tpage, reloc.clut(cy << 6 | cx >> 4), (lx & 63) * 4 + GLOW_COLS[0],
                                           (ly & 0xFF) + DIYA_ROWS[0]) + bytes(f.get('colour', [150, 190, 60]))
                      + bytes([{'butterfly': 1, 'bugs': 2, 'dragonfly': 3}.get(f.get('kind'), 0)])))
    if skies:
        # FARV (the lit skies, palettes relocated as pack.save did); LGHT (the
        # flashes) and THND (the thunder) a TTT1 pack's ("like": its file)
        farv = struct.pack('<2H', len(skies), 0)
        for desc, cluts in skies:
            farv += b''.join(struct.pack('<%dH' % fb.cols, *row) for row in desc)
            farv += b''.join(struct.pack('<%dH' % fb.cols, *[reloc.clut(c) for c in row]) for row in cluts)
        extra.append((b'FARV', farv))
        like = like_pack(cfg['lightning']['like'])
        if like.is_file():
            b = like.read_bytes()
            for i in range(struct.unpack_from('<I', b, 8)[0]):
                sid, off, size = struct.unpack_from('<4s2I', b, 16 + 12 * i)
                if sid in (b'LGHT', b'THND'):
                    part = b[off:off + size]
                    if sid == b'LGHT' and cfg['lightning'].get('flash'):
                        # "flash" [scale, [r, g, b] tint]: the fighters lit less
                        part = dimmed_flashes(part, *cfg['lightning']['flash'])
                    extra.append((sid, part))
        else:
            print(f'lightning: {like} not there, no flashes')
    if cfg.get('scenery', {}).get('diya', {}).get('out'):
        # DIYA: lamps, the flame's height and width (cm), the frames of the
        # round they go out in (min, max); texture page, palette, u, v of the
        # flame (32 x 16; the floor light 64 x 32 16 rows under it) and of
        # the glow (16 x 16); the floor light's size (cm) and brightness;
        # per lamp x, y, z (cm)
        spec = cfg['scenery']['diya']
        size = spec.get('size', 22)
        places = [p for run in cfg['scenery'].get('diya_lines', []) for p in diya_places(run)]
        (lx, ly, (cx, cy)) = surf[0]
        tpage = (lx >> 6) | (ly & 0x100) >> 4 | (ly & 0x200) << 2
        u0, v0 = (lx & 63) * 4, (ly & 0xFF)
        extra.append((b'DIYA', struct.pack('<5h2H4B2h', len(places), round(1.15 * size), round(0.55 * size),
                                           *(round(60 * t) for t in spec['out']), tpage, reloc.clut(cy << 6 | cx >> 4),
                                           u0, v0 + DIYA_ROWS[0], u0 + GLOW_COLS[0], v0 + DIYA_ROWS[0],
                                           spec.get('pool', 160), spec.get('level', 0x50))
                      + b''.join(struct.pack('<3h', round(x), round(-0.3 * size), round(z)) for x, z in places)))
    if cfg.get('fighter_light'):
        # FDIM: the fighters' ambient light times this (0x80: as drawn), the
        # lightning's flash still added
        # then "fighter_tint" [r, g, b] times (0x80 each: as drawn) and
        # "fighter_flicker" its level's waver (the Pagoda's candles)
        tint = bytes(round(0x80 * v) for v in cfg['fighter_tint']) + bytes([round(0x80 * cfg.get('fighter_flicker', 0))])             if cfg.get('fighter_tint') else b''
        extra.append((b'FDIM', struct.pack('<2H', round(0x80 * cfg['fighter_light']), 0) + tint))
    if RESERVED and cfg.get('scenery', {}).get('glass_patches'):
        extra.append((b'T2GL', glass_section(cfg)))
    if cfg.get('rain'):
        if not cfg.get('pool') and not flutter:
            raise SystemExit('rain: its streak is in the pool animation\'s lamp picture (or the banners\')')
        # RAIN: drops, fall (mm a frame), length (mm), drift x and z (mm a
        # frame), distance min and max (mm); texture page (added), palette,
        # u, v of the drop's 16 x 16; colour
        f, (lx, ly, (cx, cy)) = cfg['rain'], (surf or flutter)[0]
        tpage = (lx >> 6) | (ly & 0x100) >> 4 | (ly & 0x200) << 2 | 0x20
        extra.append((b'RAIN', struct.pack('<7h2H2B', f['count'], f['fall'], f['length'], *f['drift'], *f['distance'],
                                           tpage, reloc.clut(cy << 6 | cx >> 4), (lx & 63) * 4 + RAIN_COLS[0],
                                           (ly & 0xFF) + DIYA_ROWS[0]) + bytes(f.get('colour', [70, 84, 110])) + bytes(1)))
    if cfg.get('scenery', {}).get('flash_patches'):
        # FLIT: the lightning's light on the floor, its palette (relocated)
        # black but while a flash is, then these colours
        cx, cy = tex['flashwin'][0][2]
        word = reloc.clut(cy << 6 | cx >> 4)
        lit = cfg['_flit']
        extra.append((b'FLIT', struct.pack('<2H', (word & 63) * 16, word >> 6 & 0x3FF) + struct.pack('<16H', *[int(c) for c in lit])))
    if cfg.get('mirror'):
        extra.append((b'MIRR', mirror_section(cfg['mirror'])))
    if cfg.get('fixed'):
        # FIXW: the scenery fixed to the floor (its pattern's period, world
        # units), not following the fighters round as the arcade's does
        extra.append((b'FIXW', struct.pack('<i', cfg['fixed'])))
        if cfg.get('walls'):
            # COLL: the scenery as walls ("walls": "radius" the fighters', cm)
            shapes = church_walls(cfg['scenery'])
            extra.append((b'COLL', struct.pack('<2h', len(shapes), cfg['walls'].get('radius', 30))
                          + b''.join(struct.pack('<6h', *sh) for sh in shapes)))
    if cfg.get('feathers'):
        # FTHR: count, distance min and max, start height, length (mm), fall
        # (1/16 mm a frame); colour
        # (the colour's pad byte), then the shape when not a feather: 1 a petal,
        # 2 petals strewn on the floor, kicked up by the fighters
        # 3 New York's newspapers, blown about: then their picture's texture
        # page, palette, u, v, width, height (the New York picture's PAPER)
        # 5 the Great Wall's dust blown along the ground: the same, the
        # Great Wall picture's puff (WALL_DUST)
        # 7 the Hills' dandelion seeds drifting on the wind: the same, the
        # Hills picture's seed (HILLS_FLUFF)
        f = cfg['feathers']
        paper = b''
        if f.get('shape') == 'newspaper':
            (px, py, (cx, cy)), (pu, pv, pw, ph) = surf[0], PAPER
            paper = struct.pack('<2H4B', (px >> 6) | (py & 0x100) >> 4 | (py & 0x200) << 2, reloc.clut(cy << 6 | cx >> 4),
                                (px & 63) * 4 + pu, (py & 0xFF) + pv, pw, ph)
        if f.get('shape') == 'tumbleweed':
            # the Nowhere Desert picture's tumbleweed (solid), then its dust
            # puff's u, v, width, height, the dust devil's colour, devils
            (px, py, (cx, cy)), (pu, pv, pw, ph) = surf[0], TUMBLEWEED
            paper = struct.pack('<2H4B', (px >> 6) | (py & 0x100) >> 4 | (py & 0x200) << 2, reloc.clut(cy << 6 | cx >> 4),
                                (px & 63) * 4 + pu, (py & 0xFF) + pv, pw, ph)
            du, dv, dw, dh = DESERT_PUFF
            paper += struct.pack('<4B', (px & 63) * 4 + du, (py & 0xFF) + dv, dw, dh) + \
                bytes(f.get('devil_colour', [28, 26, 30])) + bytes([f.get('devils', 1)])
        if f.get('shape') == 'fluff':
            # the Hills picture's dandelion seed (solid, colour 0 nothing)
            (px, py, (cx, cy)), (pu, pv, pw, ph) = surf[0], HILLS_FLUFF
            paper = struct.pack('<2H4B', (px >> 6) | (py & 0x100) >> 4 | (py & 0x200) << 2, reloc.clut(cy << 6 | cx >> 4),
                                (px & 63) * 4 + pu, (py & 0xFF) + pv, pw, ph)
        if f.get('shape') == 'dust':
            # the Great Wall picture's puff, added (texture page's mode 1)
            (px, py, (cx, cy)), (pu, pv, pw, ph) = surf[0], WALL_DUST
            paper = struct.pack('<2H4B', (px >> 6) | (py & 0x100) >> 4 | (py & 0x200) << 2 | 0x20,
                                reloc.clut(cy << 6 | cx >> 4), (px & 63) * 4 + pu, (py & 0xFF) + pv, pw, ph)
        extra.append((b'FTHR', struct.pack('<6h', f['count'], f['near'], f['far'], f['height'], f['length'], f['fall'])
                      + bytes(f.get('colour', [236, 236, 244])) + bytes(1)
                      + (struct.pack('<h', {'petal': 1, 'strewn': 2, 'newspaper': 3, 'leaves': 4, 'dust': 5, 'tumbleweed': 6, 'fluff': 7}[f['shape']]) if f.get('shape') else b'')
                      + paper))
    if feathers:
        extra += eagle_sections(cfg['eagle'], cfg['scenery'].get('buttes', []), feathers, reloc, arc, stats.pop('perch', None))
    if extra:
        add_sections(out, extra)
    return dict(stage=st.stage, far_cells=f'{fb.cols} x {fb.rows}', degrees=fb.degrees, ring=fb.ring,
                pictures=vr.slot, palettes=vr.cluts, bytes=out.stat().st_size, **stats)


# --- the Classic's frame (the add-on's) ---------------------------------------
#
# The user's (2026-11-03): the Remake's panorama as high on the floor as the
# Classic's (Tekken 2's row of 32 texels 64 tall, its foot at the Classic's
# "bottom"), its picture Tekken 2's own panorama (tools/t2_remake_stages.py
# classic_picture()). The recipes' places in the old pictures are brought to it by
# the stage's "frame" (tools/data/t2_remake_stages.json, measured once
# against the old in-game panorama): an old panorama's x times new width /
# old width (the picture turned by "shift" so it faces where the old one
# did: the scenery stands where it stood), its y to a * y + b (rows of the
# panorama, each of Tekken 2's twice) + ext rows added above it.

CLASSIC_BOTTOM = 160                     # tools/tekken2_stages.py DEFAULT "bottom"


def frame_y(frame: dict) -> tuple:
    """A frame's y (a, b): the old panorama's y to a * y + b, b less its
    "lift" (texels): the user's picture that much higher in the frame, its
    foot out of the floor (London: the buses in the sand, the user's,
    2026-11-08: "raise London Ruins' background a little")."""
    a, b = frame['y']
    return a, b - frame.get('lift', 0)


def classic_cfg(cfg: dict, picture: np.ndarray, ext: int, frame: dict, repeat: int = 2) -> dict:
    """The recipe for the add-on's picture (ext rows over Tekken 2's
    panorama in the Classic's frame: tools/t2_remake_stages.py classic_picture())."""
    c = copy.deepcopy(cfg)
    h, w = picture.shape[:2]
    wo, ho = frame['old']
    a, b = frame_y(frame)
    sx = w / wo / (2 / repeat)
    X = lambda x: x * sx
    Y = lambda y: a * y + b + ext
    # the old picture's (the user's, before its crop): x, y to the old panorama's
    hw, hh = frame.get('hd', (2000, 667))
    x0, y0, x1, y1 = cfg.get('crop', (0, 0, hw, hh))
    roll = round(cfg.get('roll', 0) * wo / (x1 - x0))
    HX = lambda x: X(((x - x0) * wo / (x1 - x0) + roll) % wo)
    HY = lambda y: Y((y - y0) * ho / (y1 - y0))

    def row(r: int) -> tuple[int, callable]:
        """An old cell row: the new one its middle falls in, and its own y within it."""
        new = int(Y(64 * r + 32) // 64)
        return new, lambda v: int(np.clip(round(Y(64 * r + v) - 64 * new), 0, 63))

    for k in ('crop', 'squeeze', 'erase', 'wrap', 'seam', 'seam_repeat', 'roll', 'sky', 'foot', 'yoff'):
        c.pop(k, None)
    if 'grade' in c:
        c['grade'] = {'floor': c['grade']['floor']} if 'floor' in c['grade'] else None
        if c['grade'] is None:
            c.pop('grade')
    c['_picture'] = np.roll(picture, -frame.get('shift', 0), axis=1)
    c['rows'], c['columns'] = h // 64, w // 64
    c['degrees'] = 0x2D000 // (w * repeat)
    c['yoff'] = CLASSIC_BOTTOM - h // 2              # 64 texels a 32-line cell row
    c['crop'] = [0, 0, w, h]                         # (what pagoda_lights() measures by)
    for spec in c.get('materials', {}).values():
        bx0, by0, bx1, by1 = spec['box']
        spec['box'] = [round(HX(bx0)), round(HY(by0)), round(HX(bx0) + (bx1 - bx0) * sx * wo / (x1 - x0)), round(HY(by1))]
    for key in ('sea', 'pool', 'clouds', 'water'):
        if key in c:
            spec = c[key]
            r, inside = row(spec['row'])
            spec['row'] = r
            for k in ('foam_from', 'water_to', 'water_from', 'from'):
                if k in spec:
                    spec[k] = inside(spec[k])
            for k in ('wash', 'reach'):
                if k in spec:
                    spec[k] = max(1, round(spec[k] * a))
    if 'windows' in c:                               # New York's
        spec = c['windows']
        spec['rows'] = [int(Y(64 * spec['rows'][0]) // 64), -int(-Y(64 * spec['rows'][1]) // 64)]
        spec['columns'] = [int(X(64 * spec['columns'][0]) // 64), -int(-X(64 * spec['columns'][1]) // 64)]
        for k in ('sky_cols', 'sky_sides'):
            if k in spec:
                spec[k] = [round(X(v)) for v in spec[k]]
        spec['water'] = round(Y(spec['water']))
        if 'beacon' in spec:
            spec['beacon'] = [round(X(spec['beacon'][0])), round(Y(spec['beacon'][1]))]
    if 'lights' in c:                                # Hong Kong's
        spec = c['lights']
        ws = spec['windows']
        ws['rows'] = [int(Y(64 * ws['rows'][0]) // 64), -int(-Y(64 * ws['rows'][1]) // 64)]
        if 'beacons' in spec:
            spec['beacons']['below'] = round(Y(spec['beacons']['below']))
        spec['neon'] = [[round(X(nx)), round(Y(ny)), max(1, round(nw * sx)), max(1, round(nh * a))]
                        for nx, ny, nw, nh in spec.get('neon', [])]
    if 'flicker' in c and c['flicker'].get('eyes'):  # the Pagoda's (the old picture's own places)
        es = c['flicker']['eyes']
        es['at'] = [[round(HX(ex)), round(HY(ey))] for ex, ey in es['at']]
        es['apart'] = es.get('apart', 4) * wo / (x1 - x0) * sx
    if 'lightning' in c:
        spec = c['lightning']
        if 'bolts' in spec:
            spec['bolts'] = [[round(X(bx)), round(Y(foot))] for bx, foot in spec['bolts']]
        if 'horizon' in spec:
            spec['horizon'] = round(Y(spec['horizon']))
        if 'windows' in spec and 'from' in spec['windows']:
            spec['windows']['from'] = round(Y(spec['windows']['from']))
    return c


def neon_text(name: str) -> Image.Image:
    """The rooftop sign's words ("鐵拳", Tekken) white on black, 128 x 64:
    from a font of the system's that has them, else the same in Latin."""
    from PIL import ImageDraw, ImageFont
    img = Image.new('L', (128, 64), 0)
    d = ImageDraw.Draw(img)
    fonts = [Path(os.environ.get('WINDIR', 'C:/Windows')) / 'Fonts' / f for f in
             ('msjhbd.ttc', 'msjh.ttc', 'mingliub.ttc', 'mingliu.ttc', 'simsun.ttc', 'msgothic.ttc', 'YuGothB.ttc')] + \
            [Path(p) for p in ('/usr/share/fonts/opentype/noto/NotoSansCJK-Bold.ttc',
                               '/System/Library/Fonts/PingFang.ttc')]
    for f in fonts:
        try:
            font = ImageFont.truetype(str(f), 44)
        except OSError:
            continue
        if font.getmask('鐵拳').getbbox():
            box = d.textbbox((0, 0), '鐵拳', font=font)
            d.text(((128 - box[2] - box[0]) // 2, (64 - box[3] - box[1]) // 2), '鐵拳', fill=255, font=font)
            return img
    font = ImageFont.load_default()
    d.text((34, 26), 'TEKKEN', fill=255, font=font)
    return img


def main(argv=None) -> int:
    import tekken2_stages as classic
    from disc_image import open_disc
    ap = argparse.ArgumentParser(description=__doc__.split('\n')[0])
    ap.add_argument('--owner', action='append', help='a fighter of "arcade" (default: all)')
    ap.add_argument('--disc', type=Path, help='the Tekken 2 disc (default: the one the setup remembered)')
    ap.add_argument('--tekken3', type=Path, help="the Tekken 3 arcade zip (default: the Expanded setup's)")
    ap.add_argument('--pictures', type=Path, action='append', default=[], help='a folder of pictures')
    ap.add_argument('--like', action='append', default=[], help='NAME=PATH: a lightning "like" pack')
    ap.add_argument('--out', type=Path, required=True, help='the folder written to (arcade-<owner>.ast)')
    args = ap.parse_args(argv)
    PICTURES[:] = args.pictures
    LIKE.update(dict(a.split('=', 1) for a in args.like))
    disc = args.disc or classic.find_disc(ADDON / 'workspace')
    tekken3 = args.tekken3 or decode.default_zip()
    with open_disc(disc) as d:
        src = classic.Source(d)
        for owner in args.owner or list(DATA['arcade']):
            report = build(owner, src, tekken3, args.out / f'arcade-{owner}.ast')
            print(f'T2 {DATA["stages"][stage_of(owner)]} -> arcade-{owner}: '
                  + ', '.join(f'{k} {v}' for k, v in report.items()), flush=True)
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
