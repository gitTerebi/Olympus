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

Save archive — embedded subobjects (CRITICAL): never call `child.serialize(ar)` on a shared parent archive when the parent also contains raw stream writes (events, characters, buildings via helpers) interleaved with tagged fields. Reason: missing-field lookups call `takeField` which scans forward; if the scan walks into raw bytes it misreads them as field headers → bails out (`mTaggedEnded=true`) or eats real bytes as bogus records → corruption cascade. The bug stays latent until ANY new field is added inside the child that an old save lacks — that is the trigger. How to apply: wrap each embedded subobject via `ar.objectField("name", child)` which serializes it inside a length-prefixed sub-archive (its own terminator, bounded scan). Requires `child.read(eReadStream&)` / `child.write(eWriteStream&) const`. Wrapping is a format break for existing saves of that subtree; accept it once, then future field additions to that subtree are safe. Applied to `mAvailableBuildings` in `eboardcityread.cpp`.

Options menu hotkeys: Add `eHotkeyId` + setting in `esettings.h/cpp`, handler in `egamewidget.cpp keyPressEvent`, menu entry in `eoptionsdata.cpp getOptionsPages()`.

Popup buttons: Prefer `eAcceptButton`/`eCancelButton` for dialogs; `eOkButton` is smaller/older.
