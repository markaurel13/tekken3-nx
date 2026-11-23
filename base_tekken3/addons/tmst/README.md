# TMST add-on for Tekken 3 Expanded

> **Disclaimer:** This project is a fan-made, non-commercial endeavor and is in no way affiliated with, authorized,
> maintained, sponsored, or endorsed by Bandai Namco Entertainment Inc. or any of its affiliates. "Tekken" and all
> associated trademarks, characters, and assets are registered trademarks of Bandai Namco Entertainment Inc.

New features for [Tekken 3 Expanded](https://github.com/omarma/tekken3-expanded), packaged as mods that show up in the game's own **Mods**
tab, in the **TMST** group. Each mod can be switched on and off there; nothing in the Expanded's own files is changed.

No game data is included: everything a mod needs from the game is made on your machine by the add-on's setup.

## Add-On Features

### Jukebox
Choose the music of every fighter.
- New Menu item: **JUKEBOX**
- **Buttons:**
    · ✕ a fighter's music list 
    · L2/R2 switch the character panel (with TTT1 characters)
    · □ reset every fighter to their default music
    · ○ exit (asks to save changes)
- **Music Player:**
    · L2/R2 category 
    · △ preview / stop 
    · ✕ give the track to the fighter 
    · □ reset this fighter 
    · ○ back.
- **Categories** (console and arcade each): Tekken 1 and Tekken 2 (with **Tekken 2 Content**'s Music on, from your Tekken 2
  disc: it plays ten Tekken 1 stage themes again), Tekken 3 (the game's disc), Tekken Tag Tournament (the TTT music
  the Expanded imported), and **Tekken 5 DR** (one category: the music of the Dark Resurrection PSP ISO the
  Expanded's setup was given).
- In a fight the opponent's (P2's) chosen music plays, whatever stage is picked; without a choice, the game's own.
  It starts, fades and stops with the game's music (the match's end, the pause, the BGM SELECT's silence).
- The choices are saved beside the memory card (`saves\jukebox.txt`). The track titles are plain text lists in
  `addons\tmst\workspace\jukebox\` (`file = title` per line; `--disable` in a title leaves the track out; the add-on's own `--disable` tracks are not even made, unless
  the Tournament plays them).

### Photo Mode
Free camera view during any paused match, as well as the ability to take your own custom screenshots
- **Open:** pause a fight and choose **PHOTO** (Arcade, VS, Team Battle, Time Attack, Survival, Tekken Ball). In
  Practice, press **SELECT** while paused. Not in Tekken Force.
- **Camera controls:**
    · D-pad move 
    · L1/R1 down/up 
    · □ + D-pad turn and look 
    · L2/R2 zoom 
    · SELECT reset the camera.
  The camera stops at the edge of the stage.
- **Panel (START):** ZOOM, FOCUS and BLUR (depth of field: the fighters sharp, or a sharp band), FILTER (14: black &
  white, sepia, noir, vintage, high contrast, cold, warm, teal & orange, duotone, poster, night vision, scanlines,
  negative), FRAME (8: letterbox, polaroid, film strip, vignette, T3E Red, T3E Black, CRT TV, comic; the T3E frames
  carry the T3E logo and Tekken 3's own letters), RESET.
- ✕ take a photo · △ hide the HUD · ○ back to the pause menu, the fight exactly as it was.
- Photos are saved as PNG at the renderer's resolution (16:9 with its sides) in `addons\tmst\screenshots\`
  (`photo_YYYYMMDD_HHMMSS.png`).
- Filters that need every pixel's colour (black & white, sepia, negative ...) and, in 16:9, the blur on the screen's
  sides are shown exactly once the camera stands still; while it moves, a close look.
- Best with the OpenGL renderer. With Vulkan the panel's text and the exact preview are not shown.

### Tournament Mode
The King of Iron Fist Tournament, as an additional game mode of its own.
- New Menu item: **TOURNAMENT MODE**
- **Tournaments:** At first only Tekken 1 is open; winning a tournament opens the next (Free after Tekken Tag).
    · Tekken 1 Tournament
    · Tekken 2 Tournament
    · Tekken 3 Tournament
    · Tekken Tag Tournament
    · Free Tournament.
- Up/down choose · ✕ enter · ○ back to the main menu.
- **Your fighter:** Only the highlighted fighters take part in the tournament; the face
  buttons pick the costume (a fighter with a TTT1 moveset asks for it on the MOVESET card); the costume and model
  you pick stay for every match
    · R1 the third Tekken 3 costume, once unlocked (Start does not pick a costume here, as in Team Battle)
    · L2/R2 switch the character panel (with TTT1 characters added)
    · SELECT back to the tournaments.
- **The bracket:** 16 fighters, single elimination: first round, quarter final, semi final, final, then the Grand
  Final against the boss. A match is won with two rounds, each round starting with full life; the CPU gets stronger
  from round to round of the tournament. The other matches of a round are decided as you play yours.
- **The bosses:**
    · Tekken 1 - Heihachi
    · Tekken 2 - Kazuya, then Devil
    · Tekken 3 Ogre, then True Ogre (one round each, you take the life you have left on to the second)
    · Tekken Tag - Unknown (one round: she heals and changes her style)
    · Free - a boss drawn at the start randomly (three rounds). The bosses cannot be picked.
- **Boss intro:** before Devil, a short cinematic: Kazuya standing in the dark, the camera coming to him, then his
  Tekken Tag change in lightning and a white flash, and Devil stands there; his music starts with it, the move's
  own sounds too. START skips it (from the change on). Made from the fighters' own TTT1 moves in your install
  (TTT1 Characters needed; without them the fight starts as before). Before Heihachi (the Tekken 1 Grand Final,
  after the VS screen): he sits cross-legged, his back to you, gets up and turns to you in his blue lightning, into
  his stance. Before True Ogre (the Tekken 3 Grand Final, after Ogre's knock-out): Ogre gets up from the ground,
  draws himself up, arms spread, in his lightning, rises into the air, a white flash, and True Ogre lands on his
  wings; he fights on the moves the tournament picked for him (CPU MOVESET). Unknown is coming in the future.
- **Screens:** the bracket after each match (✕ next match · ○ leave the tournament), the next match before each
  fight, CHAMPION (and the tournament it unlocks) or TOURNAMENT OVER at the end. The Tekken 3 fighters' big VS
  portraits on them come from your Tekken 3 disc (the add-on's setup takes them, USA SLUS-00402); without them a
  fighter has its grid icon until its first loading screen.
- **Options:** OPTION → GAME OPTION shows TOURNAMENT in the box of modes at the top right, in gold where the
  option affects the tournament (as Team Battle's, but FIGHT COUNT: the tournament's rounds are its own; and TEKKEN 2 STAGES,
  which its Tekken 1 and Tekken 2 tournaments use).
- The tournaments won are saved beside the memory card (`saves\tournament.txt`; deleting it locks them again).
- The screens' backgrounds are PNG files you may redraw: `addons\tmst\data\tournament\` (`progress`, `match`,
  `champion`, `over`, 854×480); `data\tournament\guides\` shows where the mod draws on each.

### VS Mode+
VS against the CPU, or the CPU against itself, with a single controller.
- The main menu's **VS MODE** becomes **VS MODE PLUS**, and it can be chosen with any number of controllers.
- ✕ on it opens a band with three choices (left/right, ✕ to take, ○ back):
    · **PLAYER VS CPU** you against an opponent you pick
    · **PLAYER VS PLAYER** the game's own VS (only with a second controller; see-through without one)
    · **CPU VS CPU** two fighters you pick fight each other.
- **Choosing:** you pick your fighter first (CPU VS CPU: the left one), then the same controller picks the CPU's fighter,
  its handicap, the MOVESET card, the TTT1 characters' page (L2/R2) and the STAGE. The CPU's cursor is the Practice
  grid's green one with the CPU tag. A second controller plays no part in PLAYER VS CPU.
- **The CPU's strength** is GAME OPTION's DIFFICULTY LEVEL (with the Expanded's extra levels); in GAME OPTION the VS
  lamp lights on that row.
- **In the fight:** the CPU fights as in the Arcade; START pauses (CPU VS CPU: START only, the pause menu as always).
  After the match the select screen comes back for a rematch; START + SELECT there goes back to the main menu.

### Tekken 2 Content
What the add-on takes from your own Tekken 2 disc (given to `Setup TMST.cmd`), in one package. The setup makes
everything whether it is switched on or not, so switching it on needs only a restart.

#### Switches
- **Tekken 2 Content** in the TMST group switches the whole package.
- Its parts are in the group right below it, **TMST / Tekken 2 Content**, each on its own.
- With the package off, none of its parts runs; switching a part on switches the package on too.

#### Music
- Tekken 2's music and the ten Tekken 1 stage themes it plays again, Console and Arcade.
- They are the Jukebox's **TEKKEN 1** and **TEKKEN 2** categories (switch the Jukebox on too).
- The Tekken 2 stages and Tournament Mode play their own pieces with the Jukebox off too.
- **Off:** those categories are hidden; a fighter given such music plays the game's own until it is on again (the
  choice stays saved).

#### Stages
Tekken 2's 23 stages, drawn by the game's stage engine as Tekken 3 arcade's are.
- **Needs:** **Arcade Stages** switched on.
- **Picking one:** in VS and Practice (Stage Select on), **TEKKEN 2** is a game on the STAGE line, after the others;
  press **Down** for its stages (RANDOM, then the 23).
- **Every mode:** GAME OPTION's **STAGES** row has **TEKKEN 2** (with TTT's stages installed): the fighter whose stage
  it is (the CPU in Arcade) fights on its own stage, a Tekken 2 fighter on its Tekken 2 stage, the others on their
  Tekken 3 or TTT stage. **RANDOM** draws a Tekken 2 fighter's Tekken 2 stage too, one chance in three. The STAGE line
  and Tournament Mode come first.
- **Version:** GAME OPTION's **TEKKEN 2 STAGES** row, **CLASSIC** or **REMAKE**.
- **Light:** on both versions the fighters are lit by daylight, dimmed only on the dark stages (Church, Pagoda,
  Serene Forest, Taj Mahal, Mirror and Eternal Darkness).

**Classic**
- Tekken 2's own panorama and floor; tekken3.zip is not needed.
- The dark stages lit round the fighters, Mirror Darkness with its mirror, Hong Kong's running lights.

**Remake**
- **All 23 stages remade:** scenery, animations and new painted backgrounds. Nowhere Desert's Remake is at night,
  its fighters lit as on the dark stages.
- **Scenery on the ground:** it stands where it is as the fighters move and sidestep round each other; only when
  the fight goes more than 4 m away does it come along. The Church is a closed room: its pews, columns and altar
  stop the fighters.
- **What it is built from:**
  - the backgrounds open from patches that only your Tekken 2 disc opens;
  - the stages are built with the Expanded setup's tekken3.zip; without it, every stage is the Classic.
- **Life on the stages:**
  - dust where a fighter falls;
  - butterflies, fireflies and dragonflies;
  - falling petals, leaves and feathers, newspapers in the wind, Nowhere Desert's tumbleweeds and dust devils, the
    Hills' dandelion seeds;
  - grass on the ground, lit windows and candles, the Taj Mahal's rain and oil lamps;
  - birds and bats, Grand Canyon's eagle, crabs, rabbits, pigeons, lizards and spiders (some coming down on their
    threads);
  - Akihabara's ship and Hong Kong's helicopter.
- **Lightning:** over the Taj Mahal and at the Church's and Eternal Darkness's windows (there its light falls on the
  floor too). Its flashes and thunder come from TTT1 Stages'
  storm (the Expanded's TTT stage packs); without them, there is no lightning.
- **Writing:** the signs are written in each country's language.
- **Unknown:** when Unknown fights, Szechwan, Akihabara, the Great Wall, Launch Pad, Kyoto, Arizona and Eternal
  Darkness have the top of their background trimmed, so that her wolf and the add-on's letters keep their room in the
  video memory. In every other fight, these stages show the whole background. Hong Kong is always trimmed. In
  Tournament Mode's team fights, the stage switches when she comes in or leaves.

#### Wireframe Mode
Tekken 2's hidden Wireframe Mode: your fighter drawn in glowing green lines, no fill, and the camera just behind its
head, looking the way the body faces.
- **Switching it on:** not a button at the selector as in Tekken 2, but GAME OPTION's **TEKKEN 2 WIREFRAME MODE**
  row, **OFF** or **ON** (kept beside the memory card, in `t2-wireframe.txt`).
- **Modes:** Arcade, Team Battle, Tournament, Time Attack, Survival and Practice, with one human player. A
  challenger's fight in the Arcade, or a Team Battle of two players, keeps the game's own look; VS, Tekken Ball and
  Tekken Force too.
- **The game's own camera and the solid fighter:** at the fighters' entrance, during throws and from the KO on (the
  replay, the winner's pose); in Photo Mode the fighter is solid too.
- **The camera follows:** it turns the way the game turns the fighter, smoothly, so that a combo's blows do not
  swing it; TTT1 guests too.
- **Left and right:** the d-pad's right always walks forward, left always back, whichever side you stand on (the
  game would judge the side from the camera, which behind the head swings); up and down as the game gives them.
- Your fighter's shadow stays.

## Installation

The add-on runs where the Expanded runs: Windows 10/11, macOS, Linux and the Steam Deck (macOS, Linux and Steam Deck
experimental). You need a Tekken 3 Expanded that has been set up once with its own setup (the game starts), the
add-on's ZIP for your platform from the [latest release](https://github.com/ttanko96/t3-expanded-addons/releases/latest)
(`TMST-<version>-Windows.zip`, `-macOS.zip` or `-Linux.zip`; or `git clone` this repository) and about 1 GB of free
space: the Jukebox's music takes it in `addons/tmst/workspace/`
(Tekken 1 and 2 about 320 MB, TTT about 280 MB, Tekken 5 DR about 210 MB; Tekken 1, 2, TTT console and Tekken 5 DR
keep the games' own ADPCM, which the Jukebox decodes as it plays).

### Tekken 3 Expanded 1.5.0 and later

The Expanded builds add-ons in by itself
([its add-on support](https://github.com/omarma/tekken3-expanded/blob/main/addons/README.md)):

1. **Close the game**, then **put the add-on in place**, exactly at `<the Expanded's folder>/addons/tmst/`: extract
   your platform's ZIP into the Expanded's `addons` folder (it holds one folder, `tmst`), or use the `git clone` below
   for your platform. `addons/tmst/addon.cmake` must exist.
2. **Start the Expanded's setup and click Update & play.** It runs the add-on's setup (the Jukebox's music, and the
   Tekken 2 content from a disc it already knows), then builds the game with the add-on. Its mods show in the
   launcher's **Mods** page, in the **TMST** group.
3. **The Tekken 2 disc** (optional): Update & play asks nothing, so give it once with `Setup TMST` (step 3 below for
   your platform); every later Update & play reuses it.

If the add-on cannot be set up or built with your release, the setup leaves it out, builds the game without it and
says so (`.setup/setup.log` tells why): the game stays playable.

With **Tekken 3 Expanded 1.4.1 and earlier**, follow the steps for your platform below: there `Setup TMST` builds
the add-on in.

### Windows

Below, `C:\<your_folder>\tekken3-expanded` is the Expanded's folder: change it if yours is elsewhere.

1. **Close the game.** A running game cannot be rebuilt.
2. **Put the add-on in place**, exactly at `C:\<your_folder>\tekken3-expanded\addons\tmst\`: extract
   `TMST-<version>-Windows.zip` into `C:\<your_folder>\tekken3-expanded\addons\` (it holds the `tmst` folder;
   `addons\tmst\addon.cmake` must exist). Or with git:
   ```bash
   git clone https://github.com/ttanko96/t3-expanded-addons.git "C:\<your_folder>\tekken3-expanded\addons\tmst"
   ```
3. **Double-click `addons\tmst\Setup TMST.cmd`.** It opens the add-on's setup window (the Expanded's look, with a
   progress bar and a setup log), which uses the Expanded's own Python and compilers. Click **Install & play**; it:
   - takes your **Tekken 2** disc (optional; it gives the Jukebox's Tekken 1 and 2 music and the Tekken 2 stages):
     - the window fills it in when it finds one beside the discs the Expanded's setup was given; **Browse** for
       another, or clear the field to go on without it; a disc dropped on `Setup TMST.cmd` is filled in too;
     - USA (SLUS-00213) and Europe (SCES-00255), as a `.cue` with its `.bin` files, a `.bin` or a `.chd`;
   - makes what the mods need on your machine:
     - from the Tekken 2 disc: the Jukebox's Tekken 1 and 2 music and the Tekken 2 stages (the Remake's stages take a
       few minutes the first time);
     - the Jukebox's TTT console and arcade music (from the TTT PS2 disc's and tektagt.zip's music the Expanded's
       setup imported), its Tekken 5 DR music (from the Dark Resurrection ISO the Expanded's setup was given) and its
       title lists;
     - from the Tekken 3 disc the Expanded runs: Tournament Mode's portraits of the Tekken 3 fighters;
   - rebuilds the game with every mod of the add-on (Jukebox, Photo Mode, Tournament Mode, VS Mode+, Tekken 2 Content), as the
     Expanded's setup builds it: your mod settings and game data stay.

   The window says so if the Expanded has not been set up yet or the game is running. When it reads
   `The TMST add-on is installed`, **Play** opens the game's launcher. From Expanded 1.5.0 on, its **Update & play**
   does all of this too.

### macOS (experimental)

The add-on is built into the Expanded's macOS game, with the Homebrew tools its
[macOS setup](https://github.com/omarma/tekken3-expanded#install-on-macos) installed (nothing more is needed; for the
setup window, `brew install python-tk@3.13`). Below, `~/<your_folder>/tekken3-expanded` is the Expanded's folder.

1. **Close the game.**
2. **Put the add-on in place**, exactly at `~/<your_folder>/tekken3-expanded/addons/tmst/`: extract
   `TMST-<version>-macOS.zip` into `~/<your_folder>/tekken3-expanded/addons/` (it holds the `tmst` folder). Or with
   git:
   ```bash
   git clone https://github.com/ttanko96/t3-expanded-addons.git ~/<your_folder>/tekken3-expanded/addons/tmst
   ```
3. **Double-click `addons/tmst/Setup TMST.command`** (or run it in Terminal: `./"Setup TMST.command"`; if macOS says
   it cannot be opened, right-click it and choose **Open**). With Tk it
   opens the same setup window as on Windows; without it, it asks for the Tekken 2 disc in the Terminal window
   (drag the file onto it; Return goes on without it). Then it makes the same files and rebuilds the game with the
   add-on.

Afterwards play as before, with `Tekken 3 Expanded.app`.

### Linux (experimental)

The add-on is built into the Expanded's native Linux game, with the tools its
[Linux setup](https://github.com/omarma/tekken3-expanded#install-on-linux-experimental) installed (nothing more is
needed). Below, `~/<your_folder>/tekken3-expanded` is the Expanded's folder.

1. **Close the game.**
2. **Put the add-on in place**, exactly at `~/<your_folder>/tekken3-expanded/addons/tmst/`: extract
   `TMST-<version>-Linux.zip` into `~/<your_folder>/tekken3-expanded/addons/` (it holds the `tmst` folder). Or with
   git:
   ```bash
   git clone https://github.com/ttanko96/t3-expanded-addons.git ~/<your_folder>/tekken3-expanded/addons/tmst
   ```
3. **Run the setup** in a terminal:
   ```bash
   cd ~/<your_folder>/tekken3-expanded/addons/tmst && ./"Setup TMST.sh"
   ```
   (*Permission denied*: `bash "Setup TMST.sh"`.) With Tk and a desktop it opens the same setup window as on Windows;
   without them it asks for the Tekken 2 disc in the terminal (drag the file onto the window, or type its path;
   Return goes on without it). A disc can also be given at once: `./"Setup TMST.sh" ~/discs/tekken2.cue`. Then it
   makes the same files and rebuilds the game with the add-on, and offers to open the game's launcher.

Afterwards play as before: **Tekken 3 Expanded** in the applications menu, or `./"Tekken 3 Expanded.sh"` in the
Expanded's folder.

### Steam Deck (experimental)

- **With Proton** (the Windows build copied to the Deck, as the Expanded's guide says): install the add-on on the
  Windows PC as above, then copy the whole `tekken3-expanded` folder to the Deck. Nothing more is built on the Deck.
- **Natively, inside Distrobox** (the Expanded's guide's `tekken-build` container): enter the container
  (`distrobox enter tekken-build`), then follow the Linux steps above in it, with `/home/deck/Games/tekken3-expanded`.
  The game keeps starting through `distrobox-enter`, as the guide sets it up in Steam.

### Android (experimental)

The add-on goes into the Expanded's Android APK, built on your Windows PC or Mac as the Expanded's
[Build Android APK](https://github.com/omarma/tekken3-expanded#android-experimental) builds it (its first run
downloads Google's Android tools, about 1 GB, into `.setup/android`).

1. Install the add-on on the PC game as above (`Setup TMST`), so that what it makes from your discs is there.
2. Double-click **`addons/tmst/Build Android APK (TMST).cmd`** (macOS: `.command`). It runs the add-on's setup steps
   again (nothing is made twice), then the Expanded's APK build with the add-on in it: `Tekken3Expanded.apk` in the
   Expanded's folder, signed with the same key as the Expanded's own APK (it installs over it, memory cards kept).
3. Install it on the phone as the Expanded's guide says; with the phone on USB, `--install` does it:
   ```bash
   "C:\<your_folder>\tekken3-expanded\addons\tmst\Build Android APK (TMST).cmd" --install
   ```

- **The Jukebox's music:** the Tekken 1, 2, TTT and Tekken 5 DR music is in the APK (about 800 MB). The phone unpacks it
  at the install, so it needs about that much free space again. Add `--no-music` to leave it out: the Jukebox then
  shows only the Tekken 3 disc's music.
- **Photo Mode** works, but its pictures stay in the game's private storage on the phone: they cannot be copied off
  it yet (a screenshot of the phone is the way for now).
- The Expanded's own Build Android APK keeps building the add-on in afterwards (as Update & play does on PC; from
  Expanded 1.5.0 on it builds add-ons in by itself); `--without` builds the APK without it. Its APK has every mod
  with its pictures, stories and Jukebox lists (the build copies them under the game's `mods/tmst-data`), but never
  the Jukebox's Tekken 1, 2, TTT and Tekken 5 DR music: for that, use `Build Android APK (TMST)`.

## Settings

Start the game (`Tekken 3 Expanded.exe`; macOS: `.app`, Linux: `.sh`), open the **Mods** tab and switch the mods on in the **TMST** group (all are off by
default). Restart the game after switching a mod.

## Updating and removing

- **Expanded update** (Update & play): the add-on stays in the build. If the TMST group is missing afterwards, run
  `Setup TMST.cmd` (macOS: `Setup TMST.command`, Linux: `Setup TMST.sh`) again. **Expanded 1.5.0 needs TMST 0.2.0
  or later** (`TMST_VERSION` in `addon.cmake`): update the add-on too, or 1.5.0 builds the game without it.
- **Add-on update:** close the game, then replace the add-on:
  - from the ZIP: in `addons/tmst`, delete everything **except the `workspace` folder** (the music and stages the
    setup made; without it they are made again, which takes a few minutes), then extract the new ZIP into `addons/`
    as at the install. Deleting first matters: files the new version no longer has must not stay behind;
  - with git: `git -C "C:\<your_folder>\tekken3-expanded\addons\tmst" pull`.

  Then, with Expanded 1.5.0 or later, **Update & play** (the setup asks for it); with 1.4.1 or earlier,
  `Setup TMST.cmd` (macOS: `Setup TMST.command`, Linux: `Setup TMST.sh`). Your mod settings, Jukebox choices and
  tournament progress stay; what the setup made is made again only where the new version needs it.
- **Removing, Expanded 1.5.0 or later:** close the game, delete the `addons/tmst` folder, then **Update & play**.
- **Removing, 1.4.1 or earlier:** close the game, rebuild without the add-on, then delete the folder:
  ```bash
  "C:\<your_folder>\tekken3-expanded\addons\tmst\dev\build.cmd" --without
  ```
  On macOS and Linux:
  ```bash
  ~/<your_folder>/tekken3-expanded/addons/tmst/dev/build.sh --without
  ```
  If the folder is deleted first, the Expanded's next Update & play builds the game without the add-on by itself.

## License

[PolyForm Noncommercial 1.0.0](LICENSE). `tools/ttm_render.c` adapts the C352 replay of
[Tekken 3 Expanded](https://github.com/omarma/tekken3-expanded) by Matthew Stan (PolyForm Noncommercial 1.0.0), itself
an adaptation of MAME's C352 by R. Belmont and superctr (BSD-3-Clause, [licenses/MAME-BSD-3-Clause.txt](licenses/MAME-BSD-3-Clause.txt)).
The add-on's lettering is Fira Sans Medium by The Mozilla Foundation and Telefonica S.A. (SIL Open Font License 1.1,
[tools/data/fonts/OFL.txt](tools/data/fonts/OFL.txt)), drawn into `src/core/tmst_ui_font_data.c` by `tools/ui_font.py` (in the repository, not in the release ZIPs).
The Tekken 2 Remake's backgrounds (`tools/data/t2-remake-patches/`) are painted over Tekken 2's panoramas; they are
kept as patches mixed with a key made from the Tekken 2 disc's own data, so they open only with your disc.
