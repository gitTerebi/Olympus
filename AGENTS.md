# AGENTS.md

Terse like caveman. Technical substance exact. Only fluff die.
Don't use "I" no need subject.
Drop: articles, filler (just/really/basically), pleasantries, hedging.
Fragments OK. Short synonyms. Code unchanged.
Pattern: [thing] [action] [reason]. [next step].
ACTIVE EVERY RESPONSE. No revert after many turns. No filler drift.
Code/commits/PRs: normal.
Use AntD v6 api spec.
Where possible, refactor repeated functions into shared local helpers.

## Build

Use `.\build.bat` for default build.

If calling CMake directly, use Ninja build dir and 8 jobs:

```powershell
cmake --build build-ninja --config Release -j 8
```

Do not use `cmake --build build` or `cmake --build build-cmake` for normal verification.

## Codebase Navigation

**Terrain rendering:** `textures/etiletotexture.cpp` `eTileToTexture::get` — every `eTerrain` enum value needs a case in the switch or it returns `fInvalidTex`.

**Texture loading:** `textures/eterraintextures.cpp` — terrain textures loaded lazily. Check `fBlackMarbleLoaded` pattern for new terrain types.

**Tile color mods:** `egamewidgetpaint.cpp` paintEvent — patrol/editor tints via `setColorMod`, not terrain.

**Render flow:** paint → `updateTerrainTextures` → `eTileToTexture::get` → terrain switch.

**Options menu hotkeys:** `eoptionsdata.cpp getOptionsPages()` builds `eHotkeyItem` list; called from `eMainWindow::showOptionsMenu(). Enum `eHotkeyId` + `SDL_Scancode fHotkey*` in `esettings.h`, read/write `esettings.cpp`, handled in `egamewidget.cpp keyPressEvent`. To add: enum val, scancode+default in `eSettings`, read/write, `keyPressEvent` case, `getOptionsPages()` entry.
