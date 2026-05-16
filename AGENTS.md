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
Goal: robust saves. New data must be labeled and bounded so fields can be added/removed without crashing old/new loads.
Use stable unique field names. Never rename or reuse. Names describe data, not variables. Bad: `"val"`. Good: `"huntDistance"`.
Before any save migration, read `payloadField` + `takeField` + `readField` in `fileIO/esavearchive.h` — the impl answers stream behavior questions.
Do not add new raw `val()`, `readStream()`, or `writeStream()` calls. Raw bytes are legacy only and order-dependent.

### Simple value
```cpp
ar.field("name", value, default);  // default required for old-save compat
```

### Pointer refs (building / character / tile)
```cpp
const bool hasRefs = ar.archiveField("refs", [&](eSaveArchive& refsAr) {
    refsAr.buildingAsField("sourceBuilding", &board(), mBuilding);
    refsAr.buildingField("targetBuilding", &board(), mTarget);
});
if(!hasRefs && ar.reading()) {
    // SAVE_COMPAT_LEGACY_FALLBACK: old saves wrote these raw
    ar.legacyReadStream().readBuilding(...);
}
```
`payloadField` seeks back on tag mismatch — `legacyReadStream()` fallback always reads from the correct position.
Mark all fallback branches `SAVE_COMPAT_LEGACY_FALLBACK` for later cleanup.

### Child subobject
```cpp
ar.objectField("name", child);  // child needs read(eReadStream&) / write(eWriteStream&)
```
Never call `child.serialize(ar)` on a parent archive that has raw stream writes — tag scan corrupts on missing fields.

### Subclass fields — use serializeFields, never override read/write
```cpp
void MyAction::serializeFields(eSaveArchive& ar) {
    ParentAction::serializeFields(ar);  // always call parent first
    ar.field("myField", mMyField, default);
}
```
Parent `read`/`write` open one archive and call `serializeFields` — subclasses never touch `read`/`write` directly.

### Arrays
Use `ar.arrayField()` / `ar.dequeField()` / `ar.countedArrayField()`. Never loop raw reads/writes.