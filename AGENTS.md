# AGENTS.md

## Code Style

Refactor repeated functions into shared local helpers where possible.
Debug log using printf, not std out.
New files: kebab-case, no `e` or `e-` prefix (e.g. `storage-widget.cpp` not `estorage-widget.cpp` or `e-storage-widget.cpp`).
New classes: PascalCase, no `e` prefix (e.g. `StorageWidget` not `eStorageWidget`).
New include guards: no `E` prefix (e.g. `STORAGE_WIDGET_H` not `ESTORAGE_WIDGET_H`).

## Build

Build only when explicitly requested.
Use `.\build.bat` for builds.
Avoid `cmake --build build` for verification.

## Codebase Navigation

Game world/state: `engine/egameboard.*` handles tile/building changes, money, undo, city/player checks, terrain scheduling.

Text strings: `zeus-text strings/Zeus_Text.xml` READ ONLY - reference strings at runtime, re-use for messages.

Save archive: `eSaveArchive::field()` tags only top-level fields in `eZeus.ez2`; raw `val()`, direct stream helper calls, base/derived read/write order, duplicate field names, and nested payload layout remain order-dependent. Add new save members in shared `serialize(eSaveArchive&)` with unique stable names and defaults: `ar.field("mName", mName, def);`. Never rename old field names. Append raw legacy data only. Keep base calls and pointer/tile/character helper order matched between read/write. New saved arrays must use `ar.arrayField()`, `ar.dequeField()`, `ar.countedArrayField()`, or `ar.fixedArrayField()`; do not add implicit loops over current container size. `readStream()` and `writeStream()` are deprecated legacy escape hatches; use `field()`, `object()`, `tile()`, `characterAction()`, or array helpers instead.

Options menu hotkeys: Add `eHotkeyId` + setting in `esettings.h/cpp`, handler in `egamewidget.cpp keyPressEvent`, menu entry in `eoptionsdata.cpp getOptionsPages()`.

Popup buttons: Prefer `eAcceptButton`/`eCancelButton` for dialogs; `eOkButton` is smaller/older.
