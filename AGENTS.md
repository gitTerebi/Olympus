# AGENTS.md

## Code Style

Where possible, refactor repeated functions into shared local helpers.

## Build

Use `.\build.bat` for default build.

Do not use `cmake --build build` or `cmake --build build-cmake` for normal verification.

## Codebase Navigation

**Game build actions:** `widgets/gamebuild/` owns extracted build/preview placement logic; keep shared placement rules there when build and ghost preview must match.

**Game world/state:** `engine/egameboard.*` owns tile/building state changes, money, undo snapshots, city/player checks, and terrain update scheduling.

**Text strings:** `zeus-text strings/Zeus_Text.xml` READ ONLY reference strings provided by the game at runtime, re-use for messages. READ ONLY.

**Terrain rendering:** `textures/etiletotexture.cpp` maps `eTerrain` to textures; missing terrain cases return `fInvalidTex`.

**Terrain textures:** `textures/eterraintextures.cpp` loads terrain textures lazily; follow existing loaded-flag patterns for new terrain.

**Render flow:** `egamewidgetpaint.cpp` paint → `updateTerrainTextures` → `eTileToTexture::get`; `tex` null draws black tile.

**Options menu hotkeys:** Add `eHotkeyId` + setting in `esettings.h/cpp`, handler in `egamewidget.cpp keyPressEvent`, menu entry in `eoptionsdata.cpp getOptionsPages()`.

**Popup buttons:** Prefer `eAcceptButton`/`eCancelButton` for confirm/dismiss dialogs; `eOkButton` is smaller/older style.

**Tooltip system:** Widgets use `setTooltip()`; `eTooltip` renders tooltip text from `eWidget::sTooltip()`.

**Toast notifications:** `mTips` and `mToasts` stack after paused/speed notices; position updates live in `updateTipPositions()` and `updateToastPositions()`.
