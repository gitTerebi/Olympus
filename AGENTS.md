# AGENTS.md

## Code Style

Refactor repeated functions into shared local helpers where possible.
Debug log using printf, not std out.
Create new files in kebab-case.

## Save Serialization

Add new saved fields only if state cannot be derived from existing.
Use `eSaveArchive::field(...)` with stable names for saved fields. Avoid appending raw streams; older saves omit new fields and must remain readable.

## Build

Build only when explicitly requested.
Use `.\build.bat` for builds.
Avoid `cmake --build build` for verification.

## Codebase Navigation

Game world/state: `engine/egameboard.*` handles tile/building changes, money, undo, city/player checks, terrain scheduling.

Text strings: `zeus-text strings/Zeus_Text.xml` READ ONLY - reference strings at runtime, re-use for messages.

Options menu hotkeys: Add `eHotkeyId` + setting in `esettings.h/cpp`, handler in `egamewidget.cpp keyPressEvent`, menu entry in `eoptionsdata.cpp getOptionsPages()`.

Popup buttons: Prefer `eAcceptButton`/`eCancelButton` for dialogs; `eOkButton` is smaller/older.