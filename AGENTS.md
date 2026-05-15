# AGENTS.md

## Answers
ELI5 by default: short, simple, cause -> effect.

## Style
Refactor repeated fns into helpers. Debug log with `printf`. New files: kebab-case, no `e`/`e-` prefix. New classes: PascalCase, no `e` prefix. New include guards: no `E` prefix.

## Build
Build only when asked. Use `.\build.bat`. Do not verify with `cmake --build build`.

## Map
Game state: `engine/egameboard.*`. Cart pathing: `characters/actions/ecarttransporteraction.*`; deliver=`give`, pickup=`take`, max dist=`eCartTransporter::maxDistance()`. Storage/trade orders: `buildings/estoragebuilding.*`, `buildings/etradepost.*`; `setOrders()` maps exports to accept unless explicit get/empty. Text: `zeus-text strings/Zeus_Text.xml` is read-only; reuse runtime strings.

## Save
`eSaveArchive::field()` tags top-level fields only. Raw `val()`, stream helpers, base/derived order, duplicate names, and nested raw payloads stay order-dependent. New saved members go in shared `serialize(eSaveArchive&)` with stable unique names + defaults. Never rename old fields. Append raw legacy data only. Keep read/write base calls and pointer/tile/character helper order matched. New arrays use archive array helpers, not current-size loops. Avoid legacy `readStream()`/`writeStream()`; use field/object/tile/characterAction/array helpers.

## Save Subobjects
Critical: do not call `child.serialize(ar)` on a parent archive that interleaves raw stream writes. Missing-field scan can enter raw bytes and corrupt/bail. Trigger: any new child field absent from old saves. Fix: `ar.objectField("name", child)` length-prefixed sub-archive; child needs `read(eReadStream&)` and `write(eWriteStream&) const`. One-time subtree format break. Applied: `mAvailableBuildings` in `eboardcityread.cpp`.

## UI
Hotkeys: add `eHotkeyId` + setting in `esettings.*`, handler in `egamewidget.cpp`, menu entry in `eoptionsdata.cpp`. Dialogs: prefer `eAcceptButton`/`eCancelButton`; `eOkButton` is smaller/older.
