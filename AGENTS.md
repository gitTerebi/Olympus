# AGENTS.md

## Code Style

Refactor repeated functions into shared local helpers where possible.
Debug log using printf, not std out.
New files: kebab-case, no `e` or `e-` prefix (e.g. `storage-widget.cpp` not `estorage-widget.cpp` or `e-storage-widget.cpp`).
New classes: PascalCase, no `e` prefix (e.g. `StorageWidget` not `eStorageWidget`).
New include guards: no `E` prefix (e.g. `STORAGE_WIDGET_H` not `ESTORAGE_WIDGET_H`).

## Save Serialization

Add new saved fields only if state cannot be derived from existing.
Use `eSaveArchive::field(...)` with stable names for saved fields. Avoid appending raw streams; older saves omit new fields and must remain readable.
For optional/new fields, use the 3-arg overload `ar.field("name", var, defaultValue)` — sets default before read so missing fields in old saves don't leave variables uninitialized.

## Build

Build only when explicitly requested.
Use `.\build.bat` for builds.
Avoid `cmake --build build` for verification.

## Codebase Navigation

Game world/state: `engine/egameboard.*` handles tile/building changes, money, undo, city/player checks, terrain scheduling.

Text strings: `zeus-text strings/Zeus_Text.xml` READ ONLY - reference strings at runtime, re-use for messages.

Options menu hotkeys: Add `eHotkeyId` + setting in `esettings.h/cpp`, handler in `egamewidget.cpp keyPressEvent`, menu entry in `eoptionsdata.cpp getOptionsPages()`.

Popup buttons: Prefer `eAcceptButton`/`eCancelButton` for dialogs; `eOkButton` is smaller/older.