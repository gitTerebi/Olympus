# AGENTS.md

## Response Style — MANDATORY, EVERY RESPONSE, NO EXCEPTIONS

CAVEMAN MODE. These rules override all default assistant behavior:
- Terse. Fragments OK. No articles, no filler, no pleasantries, no hedging.
- No subject "I". Pattern: [thing] [action] [reason]. [next step].
- No apology for breaking rules — just fix and continue.
- Code/commits/PRs: normal style.

## Code Style

Where possible, refactor repeated functions into shared local helpers.

## Build

Use `.\build.bat` for default build.

Do not use `cmake --build build` or `cmake --build build-cmake` for normal verification.

## Codebase Navigation

**Text strings:** `zeus-text strings/Zeus_Text.xml` READ ONLY reference strings provided by the game at runtime, re-use for messages. READ ONLY.

**Terrain and Texture System:**
- **Terrain rendering:** `textures/etiletotexture.cpp` `eTileToTexture::get` — every `eTerrain` enum value needs a case in the switch or it returns `fInvalidTex`.
- **Texture loading:** `textures/eterraintextures.cpp` — terrain textures loaded lazily. Check `fBlackMarbleLoaded` pattern for new terrain types.
- **Texture painter:** `engine/etileterrainpainter.h` `eTileTerrainPainter` — `fTex`/`fColl`/`fDrawDim`; both null → painter uninitialized (map corruption). `drawDim=0` → under-building subtile (normal skip).

**Render flow:** `egamewidgetpaint.cpp` paint → `updateTerrainTextures` → `eTileToTexture::get` → terrain switch. `drawTerrain` lambda (~line 476) iterates via `iterateOverVisibleTiles`; `tex` null → black tile; patrol/editor/fog tints via `setColorMod`, not terrain data.

**Options menu hotkeys:** `eoptionsdata.cpp getOptionsPages()` builds `eHotkeyItem` list; called from `eMainWindow::showOptionsMenu(). Enum `eHotkeyId` + `SDL_Scancode fHotkey*` in `esettings.h`, read/write `esettings.cpp`, handled in `egamewidget.cpp keyPressEvent`. To add: enum val, scancode+default in `eSettings`, read/write, `keyPressEvent` case, `getOptionsPages()` entry.

**eloadgame:** `widgets/eloadgame.h/cpp` - loads adventure save games, deletes them with confirmation.

**Popup buttons:** `eAcceptButton` (green checkmark, uses `fAcceptButton` texture), `eCancelButton` (red X, uses `fCancelButton` texture), `eOkButton` (uses `fOkButton` texture) — all extend `eBasicButton`. Prefer `eAcceptButton`/`eCancelButton` pair for confirm/dismiss dialogs; `eOkButton` is smaller/older style.

**Tooltip system:** Widgets set tooltips via `setTooltip()` method. `eWidget::sTooltip()` returns tooltip text of widget under mouse. `eTooltip` class in `widgets/etooltip.h` renders tooltips in main window render loop.

**Toast Notification Systems:**
- **mTips:** Deque of eTip structs for temp tips. Managed by updateTipPositions().
- **mToasts:** Deque of eToast structs for event toasts. Managed by updateToastPositions().
- Stack order: Paused, Speed, Tips, Toasts.
