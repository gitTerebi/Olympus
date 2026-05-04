# AGENTS.md

## Response Style — MANDATORY, EVERY RESPONSE, NO EXCEPTIONS

CAVEMAN MODE. These rules override all default assistant behavior:
- Terse. Fragments OK. No articles, no filler, no pleasantries, no hedging.
- No subject "I". Pattern: [thing] [action] [reason]. [next step].
- No apology for breaking rules — just fix and continue.
- Code/commits/PRs: normal style.

## Code Style

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

**Text strings:** `zeus-text strings/Zeus_Text.xml` reference strings provided by the game at runtime, re-use for messages.

**Terrain rendering:** `textures/etiletotexture.cpp` `eTileToTexture::get` — every `eTerrain` enum value needs a case in the switch or it returns `fInvalidTex`.

**Texture loading:** `textures/eterraintextures.cpp` — terrain textures loaded lazily. Check `fBlackMarbleLoaded` pattern for new terrain types.

**Render flow:** `egamewidgetpaint.cpp` paint → `updateTerrainTextures` → `eTileToTexture::get` → terrain switch. `drawTerrain` lambda (~line 476) iterates via `iterateOverVisibleTiles`; `tex` null → black tile; patrol/editor/fog tints via `setColorMod`, not terrain data.

**Texture painter:** `engine/etileterrainpainter.h` `eTileTerrainPainter` — `fTex`/`fColl`/`fDrawDim`; both null → painter uninitialized (map corruption). `drawDim=0` → under-building subtile (normal skip).

**Options menu hotkeys:** `eoptionsdata.cpp getOptionsPages()` builds `eHotkeyItem` list; called from `eMainWindow::showOptionsMenu(). Enum `eHotkeyId` + `SDL_Scancode fHotkey*` in `esettings.h`, read/write `esettings.cpp`, handled in `egamewidget.cpp keyPressEvent`. To add: enum val, scancode+default in `eSettings`, read/write, `keyPressEvent` case, `getOptionsPages()` entry.

**Message list widget:** `widgets/emessagelistwidget.h/cpp` - Displays up to 50 recent game messages with formatted titles, dates, and read/unread status. Supports clicking to open messages and mark as read.

**efilewidget:** `widgets/efilewidget.cpp` - loads adventure save games, deletes them with confirmation.

**Walker/cart rendering + tinting:** `widgets/egamewidgetpaint.cpp` `drawCharacters` lambda — iterates tile characters, applies `setColorMod` for hover (blue), patrol selected (green), walker-building selected (red). Color constants defined just above the lambda.

**Building click → walker highlight:** `widgets/egamewidget.cpp` `rightClickRelease` sets `mWalkerBuilding`; paint loop checks `eCartTransporterAction::src()` against it. Cart action source building stored in `characters/actions/ecarttransporteraction.h`.
