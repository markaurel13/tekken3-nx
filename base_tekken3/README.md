# Tekken 3 Recompiled

## v0.1.4 Easy Setup

Download `Tekken3Recompiled-v0.1.4-Easy-Setup.zip` from the
[v0.1.3 release](https://github.com/FishB0nes98/Tekken3Recompiled/releases/tag/v0.1.4),
extract the entire ZIP to a writable folder, and open `Play Tekken 3.exe`.
Choose your Tekken 3 USA PlayStation CUE/BIN image and click **Set up & play**.
Jun also needs the supported non-merged `tektagt.zip` and `tekken3.zip` arcade
sets. First setup downloads pinned build tools and prepares the game locally;
later starts open the native launcher with Mods, controls, disc library, and
Patches & Updates. At least 4 GB of free working space is required.

Existing v0.1.2 players need to extract this full package once. v0.1.3 users
with a completed setup can update through the launcher. If Jun setup failed on
v0.1.3, extract v0.1.4 over the same folder and retry setup. From v0.1.4
onward, the launcher can install newer full releases in place. See
[RELEASE-NOTES.md](RELEASE-NOTES.md) for this patch's changelog and
[LAUNCHER.md](LAUNCHER.md) for the update workflow.

Native-port and static-recompilation research for **Tekken 3 (USA, SLUS-00402)** built on
[psxrecomp](https://github.com/mstan/psxrecomp) and
[recomp-ui](https://github.com/mstan/recomp-ui).

This repository now has two explicit products for the US PlayStation release:

- `Tekken_3_PC_Port.exe`: the independent host-native port under `native/`;
- `Tekken_3_Recompiled.exe`: the hybrid recompilation, retained as a behavior
  and rendering reference while systems are migrated.

Both require a legally owned disc image. The native port is an early playable
vertical slice, not yet a content-complete recreation.

| | |
|---|---|
| Players | 2 |
| Region | USA |
| Publisher | Namco |
| Year | 1998 |

Scaffolded with the New Project Layout. See
`psxrecomp/docs/GAME_PROJECT_SETUP.md` for the full flow.

## Legal

You must own the original game. Disc images under `disc/` are gitignored and
must never be committed. Retail BIOS dumps are not redistributed; OpenBIOS is
used for Generate unless you supply your own SCPH locally.

Optional box art under `launcher_assets/img/` may come from
[libretro-thumbnails](https://github.com/libretro-thumbnails/libretro-thumbnails)
(`Named_Boxarts`); see `BOXART_SOURCE.txt` when present.

## Quick start (dev)

For the Recompiled game, open `Play-Tekken3-Recompiled.cmd`. Its new native launcher
always appears first, with mods, separate keyboard/controller mapping, and a local
disc library. See [LAUNCHER.md](LAUNCHER.md) for usage and verification scope.

For the native PC-port slice, build it once and then double-click
[`Play-Tekken3-PC-Port.cmd`](Play-Tekken3-PC-Port.cmd):

```powershell
.\scripts\dev.ps1 native-build
.\scripts\dev.ps1 native-test
.\scripts\dev.ps1 native-capture
```

The native renderer uses the window's real aspect ratio: its vertical field of
view stays constant and 16:9 exposes more stage horizontally. It does not issue
PS1 GPU packets or synthesize black side wedges. See
[`native/README.md`](native/README.md) for controls, architecture, and the next
asset-import milestone.

For the already-built hybrid reference, double-click
[`Play-Tekken3-Recompiled.cmd`](Play-Tekken3-Recompiled.cmd). It launches the
native executable against the verified private BIN/CUE and never modifies the
disc files. Double-click
[`Configure-Tekken3-Recompiled.cmd`](Configure-Tekken3-Recompiled.cmd) to open
the graphical settings and Mods manager first.

The hybrid reference's built-in **True Widescreen** mod is enabled by default.
It reveals extra horizontal stage geometry and gives the main menu, character
selectors, and Arcade, Team Battle and Tekken Force loading native 16:9 layouts.
Portraits and text keep their proportions. Unrecognized menus and movies remain
4:3. The optional **Wider fight framing (experimental)** setting is on by
default: regular fights can use the wider view before the camera pulls back,
with the original close-range view, movement and collision. Turn that option
off to compare the original fight camera, or disable **True 16:9 Widescreen**
to restore the original 4:3 presentation.

The game's **Options → Sounds** menu has separate **Music Volume** and
**SFX Volume** controls (0–100, in steps of 5). Left/right previews changes;
choose **Save** to remember them across launches. **Back** discards unsaved
changes. Preferences live in `tekken3-sound.ini` in the active save directory.
The original Game Option menu still controls BGM selection and stereo/mono.

On this Windows workspace, the repeatable driver verifies the disc, regenerates
the private game code, builds the runtime, runs tests, and checks captured
overlays:

```powershell
.\scripts\dev.ps1 all
.\scripts\dev.ps1 run
```

Native actions are `native-build`, `native-run`, `native-test`, and
`native-capture`. Hybrid-reference actions remain `verify`, `generate`,
`build`, `run`, `test`, and `overlay-check`. The lower-level hybrid flow is:

```bash
git submodule update --init --recursive
./psxrecomp/tools/ci/build_emitters.sh
python3 psxrecomp/psxrecomp_cli.py generate \
  --config game.toml --project-root . --disc disc/<your>.cue
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-release --target psx-runtime
```

Zip prefix for CI artifacts: `tekken3`.

## Symbols

Progressive map: `symbols.toml` → `python3 tools/sync_symbols.py` →
`psx_symbols.h` (`PSX_FN_*`). See `psxrecomp/docs/SYMBOLS.md`.

## Modding

See [`MODDING.md`](MODDING.md) for the verified BNS asset-ID namespace,
local extraction workflow, `.psxmod` packaging, proposed loose overrides,
testing checklist, and release-safe boundary.

The standard-library-only [`tools/bns_tool.py`](tools/bns_tool.py) performs the
local inventory and extraction; its commands and output schema are documented
in [`tools/BNS_TOOL.md`](tools/BNS_TOOL.md).

The companion [`tools/bns_mod.py`](tools/bns_mod.py) builds guarded,
exact-size BNS record and strict ARC-member package sources plus optional
`.psxmod` archives without altering the BIN/CUE. ARC-member packages contain
only the authored member payload. See [`tools/BNS_MOD.md`](tools/BNS_MOD.md).

The standard-library-only [`tools/tim_tool.py`](tools/tim_tool.py) scans a
locally extracted record for strict standard PS1 TIM images and exports exact
`.tim` files plus deterministic PNG previews. See
[`tools/TIM_TOOL.md`](tools/TIM_TOOL.md).

[`tools/tim_import.py`](tools/tim_import.py) re-encodes an identical-dimension
RGBA PNG into the original TIM layout under explicit palette, STP, and
quantization rules. Rebuilt TIMs remain private derived staging artifacts; see
[`tools/TIM_IMPORT.md`](tools/TIM_IMPORT.md).

For BNS records detected as strict ARC containers,
[`tools/arc_tool.py`](tools/arc_tool.py) catalogs, extracts and deterministically
rebuilds numeric members in a local workspace. Its format and package workflow
are documented in [`tools/ARC_TOOL.md`](tools/ARC_TOOL.md).

The read-only [`tools/vab_tool.py`](tools/vab_tool.py) audits all 48 verified
split VH/ARC sound-bank pairs and reconstructs a selected local `.vab` after
strict Sony VH, ARC, size-table, and SPU-ADPCM validation. See
[`tools/VAB_TOOL.md`](tools/VAB_TOOL.md).

The read-only [`tools/model_map.py`](tools/model_map.py) gives all 52 `3DMK`
records and the 15 structurally unique no-magic model candidates numeric,
bounds-checked header/table/stream maps, entropy blocks and raw section
extracts without inventing fighter or stage names. See
[`tools/MODEL_MAP.md`](tools/MODEL_MAP.md).

The read-only [`tools/xas_tool.py`](tools/xas_tool.py) verifies the exact raw
Track 1, proves the executable's 50 strided XA descriptors, and catalogs 22
strict STR movie regions without guessed labels. Selected raw-sector streams
can be extracted only to the local ignored workspace. See
[`tools/XAS_TOOL.md`](tools/XAS_TOOL.md).

The guarded [`tools/xas_mod.py`](tools/xas_mod.py) builder accepts already
encoded, author-owned XA/STR raw sectors and packages only their sector bodies
as default-off strided overlays. It leaves sibling XA channels and the stock
disc untouched. See [`tools/XAS_MOD.md`](tools/XAS_MOD.md).

Run [`tools/catalog_assets.py`](tools/catalog_assets.py) to build one
path-independent, payload-free JSON catalog spanning BNS, VAB, model, XA, and
STR metadata. See [`tools/CATALOG_ASSETS.md`](tools/CATALOG_ASSETS.md).

## Current limits

The native PC-port executable currently has independent simulation, hit
detection, input, aspect-correct camera/rendering, HUD, and strict legal-disc
verification. Its stage and fighters are procedural stand-ins until original
asset semantics are decoded. It does not yet reproduce the full roster,
animation, moves, modes, stages, UI, audio, or movies.

The reference executable is a playable hybrid recompilation, not a finished
source-code recreation. It still uses guest RAM, PS1 hardware models, generated
MIPS translations, and an interpreter for uncovered dynamic code. It will not
be the architecture of the finished PC port.

The asset tools can inventory/package BNS records and ARC members, preview TIM
textures, reconstruct VAB banks, expose conservative model-file structure,
catalog verified XA/STR streams, and package already encoded XAS replacements.
The strict TIM re-encoder supports same-dimension PNG edits under explicit
palette/STP rules, while keeping derived TIMs private. Remaining semantic
milestones include a 3DMK/model-candidate importer, resized or palette-changing
texture workflows, VAB sample encoding/reinsertion, and user-friendly XA/STR
audio/video encoding.

## Framework pins

Submodule gitlinks (`psxrecomp`, optional `recomp-ui`, nested `recomp-net`)
are authoritative. `framework_pins.txt` is an optional scaffold snapshot;
release CI logs SHAs with `record_pins.sh` but builds whatever the gitlinks
resolve to. Bump submodules deliberately — do not float on `main`/`master`
in release CI.
