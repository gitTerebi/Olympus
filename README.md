<h1 align="center">eZeus</h1>

eZeus is an open-source implementation of Zeus: Master of Olympus game

[Watch trailers](https://www.youtube.com/@eZeus-e)

## How To Play

To play eZeus you will need original game files (Zeus: Master of Olympus base game and Poseidon expansion).

Currently only English and Polish versions are supported due to font glyphs limitations.

Only windows binaries are provided. If you want to play on Linux you have to build eZeus yourself.

[You can download eZeus from GitHub](https://github.com/MaurycyLiebner/eZeus/releases)

## How To Build on macOS

Make sure you have **git** installed on your system.

Clone the eZeus repository:
```
git clone --recurse-submodules https://github.com/MaurycyLiebner/eZeus
```
Take a look in the head of compile_apple.sh for the needed dependencies. When you are ready simply execute the script. All is done fully automatic.
At the end you have 2 folders. build_arm and build_x86. Follow the instructions at the end of my script.

## How To Build on Linux

Make sure you have **git** installed on your system.

Clone the eZeus repository:
```
git clone --recurse-submodules https://github.com/MaurycyLiebner/eZeus
```

You will need to install following packeges:

```
sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-mixer-dev libnoise-dev g++
```

You will need Qt Creator (qt.io).

Open eZeus.pro in Qt Creator and use it to build eZeus.

Before running eZeus binary make sure to setup all files as specified in **How To Play** section and the release description. Put eZeus binary in (...)/Zeus and Poseidon/eZeus/Bin directory and run it.

## Authors

**Maurycy Liebner** - 2021 - 2025 - [MaurycyLiebner](https://github.com/MaurycyLiebner)
