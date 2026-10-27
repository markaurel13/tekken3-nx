# Tekken 3 (USA) — Nintendo Switch Native Port

[![Platform: Nintendo Switch](https://img.shields.io/badge/Platform-Nintendo%20Switch-E60012.svg?logo=nintendoswitch&logoColor=white)](https://github.com/markaurel13/tekken3_nx)
[![Performance: Locked 60 FPS](https://img.shields.io/badge/Performance-Locked%2060%20FPS-brightgreen.svg)]()
[![Graphics: deko3d](https://img.shields.io/badge/Graphics-deko3d%20(Low--Level%20GPU)-blue.svg)]()
[![Audio: audren](https://img.shields.io/badge/Audio-audren%20(Dedicated%20DSP)-purple.svg)]()
[![Author: markaurel13](https://img.shields.io/badge/Author-markaurel13-orange.svg)]()

A high-performance, native static recompilation port of **Tekken 3** (NTSC-U / SLUS-00402) for the Nintendo Switch.

Unlike traditional emulation, this port statically recompiles original PlayStation 1 MIPS binaries directly into native ARM64 machine instructions, coupled with a purpose-built **deko3d** graphics renderer

---

## ✨ Features

- **Rock-Solid 60 FPS on STOCK CLOCK:** Direct native execution with zero emulation overhead and 60 solid fps.
- **Internal Supersampling (SSAA):**
  - **HD Resolution: 2X anti-aliasing by default, having the option to set 3X resolution on switch/tekken3_nx/config.txt (Game will still run on 60 solid fps anyways, it's on X2 by default because the switch screen is just 1280 x 720, so 4K resolution will not be noticed that much)
  - **16:9 Fullscreen Widescreen:** Fills the full 720p/1080p display without sidebars (THis option is deactivated by default on switch/tekken3_nx/config.txt to preserve the original experience, but you can activate it anytime by simple changing the parameter widescreen_16_9 = OFF for widescreen_16_9 = ON) 
- **2-Player Local Battles:** Automatic controller assignment supporting any Gamepad controller
- **Save State & Memory Card Support:** Full persistence for character unlocks, high scores, and game settings saved automatically to `sdmc:/switch/tekken3/saves/`.
- **All Modes Supported:** Arcade, VS Battle, Team Battle, Survival, Time Attack, Practice, Tekken Force, and Tekken Ball.

---

## 📁 Installation & File Structure

1. Extract the release package onto your Nintendo Switch microSD card.
2. Place your legally obtained **Tekken 3 (USA)** disc image (`.cue` and `.bin`) inside the `disc/` subfolder. It must be NTSC-U / SLUS-00402 (USA version); otherwise it won't work).

  Your initial directory structure on your microSD should look like this:

```text
sdmc:/
└── switch/
    └── tekken3/
        ├── tekken3_nx.nro              # The Switch executable
        └── disc/
            ├── Tekken 3 (USA).cue       
            └── Tekken 3 (USA) (Track 1).bin
```

> **Note:** Supported disc filenames include `Tekken 3 (USA).cue`, `Tekken 3.cue`, `tekken3.cue`, or direct `.bin` tracks.

---

## ⚙️ Graphics Configuration (`config.txt`)

You can easily adjust graphical preferences by editing [`config.txt`](config.txt) located next to `tekken3_nx.nro`:

```ini
# ==============================================================================
# Tekken 3 (USA) - Graphics Configuration for Nintendo Switch
# ==============================================================================
# Place this file in the same directory as 'tekken3_nx.nro' on your microSD card
# (e.g. sdmc:/switch/tekken3/config.txt).
# If this file is missing, the game will automatically create it with default values.
#
# ------------------------------------------------------------------------------
# Internal Supersampling Anti-Aliasing (SSAA):
#   1 = 1x PS1 Native (1024x512)
#   2 = 2x HD (2048x1024) [RECOMMENDED]
#       -> Sharp edges, locked 60 FPS, optimal battery life, pristine on TV
#   3 = 3x Ultra HD (3072x1536)
#       -> Maximum polygon edge definition for 4K / large TV screens
# ------------------------------------------------------------------------------
supersampling = 2

# ------------------------------------------------------------------------------
# Screen Aspect Ratio:
#   OFF = 4:3 Original with black sidebars (Pillarbox) [RECOMMENDED]
#         -> Preserves authentic retro proportions for characters and stages.
#   ON  = 16:9 Fullscreen Widescreen
#         -> Fills the entire Switch screen and 16:9 TV display without bars.
# ------------------------------------------------------------------------------
widescreen_16_9 = OFF
```

---

## 🎮 Controls

### Player 1 (Handheld or Pro Controller)
| PS1 Button | Nintendo Switch | Action |
|:---:|:---:|:---|
| **D-Pad / Left Stick** | D-Pad / Left Stick | Movement & Crouching |
| **Square ($\Box$)** | **Y** | Left Punch (LP) |
| **Triangle ($\Delta$)** | **X** | Right Punch (RP) |
| **Cross ($\times$)** | **B** | Left Kick (LK) |
| **Circle ($\bigcirc$)** | **A** | Right Kick (RK) |
| **L1 / L2** | **L / ZL** | Customizable shortcut |
| **R1 / R2** | **R / ZR** | Customizable shortcut |
| **Start** | **+ (Plus)** | Pause Menu (Arcade / VS / Practice) |
| **Select** | **- (Minus)** | Select / Options |

---

## 🛠️ Building from Source

### Prerequisites
- [devkitPro](https://devkitpro.org/) with `devkitA64`, `libnx`, `deko3d`, and `switch-tools`.
- CMake (version 3.20 or newer) and Ninja.

### Build Steps
```bash
# Clone the repository
git clone https://github.com/markaurel13/tekken3_nx.git
cd tekken3_nx

# Configure CMake with devkitPro Switch toolchain
cmake -B build-switch -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE=/opt/devkitpro/cmake/Switch.cmake \
  -DCMAKE_BUILD_TYPE=Release

# Build the .nro
ninja -C build-switch
```

The compiled binary and assets will be output to:
- `build-switch/tekken3_nx.nro`
- `build-switch/config.txt`

---

## ⚖️ Disclaimer & Legal

Tekken 3 is a trademark of Bandai Namco Entertainment Inc. This project is an independent open-source native reimplementation for homebrew-enabled Nintendo Switch consoles. No proprietary game assets, disc ROMs, or copyrighted code are distributed with this software. Users must provide their own legally purchased copy of *Tekken 3* for PlayStation.

---

## 🏆 Credits & Acknowledgments

- **Port Author & Maintainer:** **markaurel13**
- **Namco:** Creators of the legendary *Tekken* franchise.
- **devkitPro & libnx contributors:** The foundation of the Nintendo Switch homebrew toolchain.
- **FinCS & deko3d contributors:** Incredible low-level GPU API for Tegra X1.
- **psxrecomp project:** Pioneer in PlayStation 1 static recompilation technology.
