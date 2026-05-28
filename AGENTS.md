# AGENTS.md

## Answers
ELI5 by default: short, simple, cause -> effect.

## Style
Refactor repeated fns into helpers. Debug log with `printf`. New files: kebab-case, no `e`/`e-` prefix. New classes: PascalCase, no `e` prefix. New include guards: no `E` prefix.

## Build
After making code changes, build with `.\build.bat` to verify. Do not verify with `cmake --build build`.

## Map
Game state: `engine/egameboard.*`. Cart pathing: `characters/actions/ecarttransporteraction.*`; deliver=`give`, pickup=`take`, max dist=`eCartTransporter::maxDistance()`. Storage/trade orders: `buildings/estoragebuilding.*`, `buildings/etradepost.*`; `setOrders()` maps exports to accept unless explicit get/empty. Text: `zeus-text strings/Zeus_Text.xml` is read-only; reuse runtime strings.

## Save
Goal: all save data is tagged, named, bounded, and safe to add/remove/reorder.
No backwards compatibility for save migrations unless explicitly asked.
Do not add legacy readers, optional migration bridges, or old-shape fallbacks by default.

If bug appears after save/reload, inspect serialization first.

If crash happens after reload, the crash site is only evidence. Do not treat it as source until saved fields and restored invariants are checked.

Do not fix reload crashes with null guards first. First find the corrupted invariant and fix the writer/reader/owner that creates bad state.

Null guards are allowed only after the root cause is fixed, the state is truly optional, and the final answer explains why the guard is not hiding corruption.

For save/load bugs, fix source fields and post-load restore before changing callers, deleters, or UI paths.

No new raw save bytes. Do not add `val()`, `readStream()`, `writeStream()`, `readStream().read*`, or `writeStream().write*`.

Use stable unique field names. Never rename or reuse. Names describe data, not variables. Bad: `"val"`. Good: `"huntDistance"`.

Before save work, read `payloadField` + `takeField` + `readField` in `fileIO/esavearchive.h`.

For new or migrated save code, base `read`/`write` are entry points. They open one `eSaveArchive` and call the virtual field serializer.

For new or migrated save code, children do not override `read`/`write`. Children serialize fields by calling parent first.

```cpp
void MyAction::serializeFields(eSaveArchive& ar) {
    ParentAction::serializeFields(ar);
    ar.field("myField", mMyField, default);
}
```

Simple values:

```cpp
ar.field("name", value, default);
```

Pointer refs:

```cpp
ar.buildingAsField("sourceBuilding", &board(), mBuilding);
ar.characterField("worker", &board(), mWorker);
ar.tileField("targetTile", board(), mTargetTile);
```

Child object:

```cpp
ar.objectField("name", child);
```

Collections:

```cpp
ar.arrayField("items", items, [](eSaveArchive& itemAr, Item& item) {
    itemAr.field("count", item.count, 0);
});
```

Use `archiveField()` for grouped subfields.

Use `arrayField()` / `dequeField()` / `countedArrayField()` for collections.

Never loop raw reads/writes.
