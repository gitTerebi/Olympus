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
Goal: robust saves. New save data must be labeled and bounded so vars can be added/removed without crashing old/new loads.
Use `ar.field("name", value, default)` for new simple members. Defaults are required for old saves.
Use stable unique field names. Never rename old fields. Never reuse a field name for a different type/meaning. Names must be semantically meaningful — describe the data, not the variable. Bad: `"tptr"`, `"val"`, `"data"`. Good: `"wolfAction"`, `"targetBuilding"`, `"huntDistance"`.
Use `objectField()` for child/subobject payloads. Use array helpers for arrays.
For pointer refs use tagged helpers grouped under `ar.archiveField("groupName", [&](eSaveArchive& refsAr) { ... })`:
  `refsAr.buildingAsField("name", &board(), ptr)` / `characterAsField` / `tileAsField`
`archiveField` returns bool (field present); on false + `ar.reading()`, fall back via `ar.legacyReadStream()`.
Old-save fallback branches must be marked `SAVE_COMPAT_LEGACY_FALLBACK` so later cleanup can find and review them.
Do not add new raw `val()`, `readStream()`, or `writeStream()` data. Raw data is legacy only and stays order-dependent.
If migrating old raw bytes, first put a length-prefixed boundary around the parent payload, then add tagged fields inside it.
`payloadField()` is for immediate next-field payloads only; do not use it as a normal out-of-order field lookup.
Keep read/write base calls and pointer/tile/character helper order matched. Append raw legacy data only.

## Save Subobjects
Critical: do not call `child.serialize(ar)` on a parent archive that interleaves raw stream writes. Missing-field scan can enter raw bytes and corrupt/bail. Trigger: any new child field absent from old saves. Fix: `ar.objectField("name", child)` length-prefixed sub-archive; child needs `read(eReadStream&)` and `write(eWriteStream&) const`. One-time subtree format break. Applied: `mAvailableBuildings` in `eboardcityread.cpp`.