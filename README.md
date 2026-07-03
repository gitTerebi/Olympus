# Olympus

Olympus is a Zeus Poseidon open source remake project: a fork of eZeus, an
open-source implementation of Zeus: Master of Olympus and the Poseidon
expansion. We thank the eZeus developers for their excellent work on the engine,
which forms the foundation of this project.

Keywords: Zeus Poseidon open source remake, Zeus: Master of Olympus open source,
Poseidon expansion remake, city builder.

Visit to read install directions:
https://github.com/MaurycyLiebner/eZeus

Based on their work I have been using AI tools to code many personal UI improvements I wanted to see happen.

Hotkeys, smooth map scroll, 240FPS mode etc.

The goal of this project is to get vanilla systems in place as much as possible.

Many functions and features still need to be added to get it to parity with the vanilla game.

Its entirely playable though, but some systems are still missing or not reproduced 100% faithfully.

## Changes

- **Storage Management Enhancements**: Added an extra button to warehouses for quickly emptying contents or resetting item counts, streamlining inventory management and resource redistribution in your city.
- **Reversible Storage Switches**: Right-clicking on storage switches now reverses their direction, allowing players to quickly adjust resource flow without rebuilding connections, improving efficiency in complex supply chains.
- **Options Menu Improvements**: Refactored the options menu for better organization and usability, now including new settings such as adjustable game speed, option to disable mouse edge scrolling, and a scrollbar for improved navigation.
- **Production Statistics**: Added detailed production statistics for processing buildings (winery, armory, olive press, sculpture studio, corral, hunting lodge, chariot factory), helping players monitor and optimize their industrial output for better economic management.
- **Farm Productivity Tracking**: Introduced production statistics for farms, allowing players to track crop yields and make informed decisions about agriculture in their city.
- **Hotkey for Watchhouses**: Added a keyboard shortcut for constructing watchhouses, speeding up city defense setup and reducing repetitive mouse clicks.
- **Settler Spawning**: Settlers now arrive in groups of 4, matching the original game's behavior, which affects population growth pacing and city expansion strategies.
- **Population Tab Hotkeys**: Added number keys 1-8 as hotkeys for population tabs, allowing faster navigation between different citizen groups for quicker social management.
- **Enhanced Hotkey System**: Comprehensive keyboard shortcuts now available for various functions, including watchhouses, population tabs, and pause controls, improving overall game efficiency and accessibility.
- **Housing Dynamics**: Houses now devolve in gradual steps when services decline, and evicted residents become homeless walkers, creating more realistic and challenging population management scenarios.
- **Housing Stability**: Fixed an issue where fully occupied houses would randomly lose residents, stabilizing population retention and city growth.
- **Toast Notifications**: Made most message popups non-blocking, allowing gameplay to continue uninterrupted while viewing notifications.

## How To Play

To play Olympus you will need original game files (Zeus: Master of Olympus base game and Poseidon expansion).

Currently only English and Polish versions are supported due to font glyphs limitations.

A pre-built Windows release is available on the [Releases](../../releases) page. Download
`Olympus-0.1.0-rc.1.zip`, extract it into your `Zeus and Poseidon` directory, and run
`Bin/Olympus.exe`. See the bundled `README.txt` for details. Alternatively, build it yourself
(see below).

## How To Build on Windows

Use the provided `build.bat` script for the default build:

```
.\build.bat
```

Alternatively, if calling CMake directly, use Ninja build directory with 8 jobs:

```
cmake --build build-ninja --config Release -j 8
```

## Attribution

Olympus is based on eZeus by Maurycy Liebner. We acknowledge and thank the original developers for their foundational work.

https://github.com/MaurycyLiebner/eZeus
