# AGENTS.md

## Answers
ELI5 by default: short, simple, cause -> effect.

## Style
Refactor repeated fns into helpers. Debug log with `printf`. New files: kebab-case, no `e`/`e-` prefix. New classes: PascalCase, no `e` prefix. New include guards: no `E` prefix.

## Build
After making code changes, build with `.\build.bat` to verify. Do not verify with `cmake --build build`.

## Map
Game state: `engine/game-board.*`, `engine/game-board-read.cpp`, `engine/egameboardwrite.cpp`. Cart pathing: `characters/actions/ecarttransporteraction.*`; deliver=`give`, pickup=`take`, max dist=`eCartTransporter::maxDistance()`. Storage/trade orders: `buildings/estoragebuilding.*`, `buildings/trade-post.*`; `setOrders()` maps exports to accept unless explicit get/empty. Text: `text/Zeus_Text.xml` is read-only; reuse runtime strings.

## Save
Goal: all save data is tagged, named, bounded, and safe to add/remove/reorder.
No backwards compatibility for save migrations unless explicitly asked.

If bug appears after save/reload, inspect serialization first.

If crash happens after reload, crash site is only evidence. Check saved fields and restored invariants first.

Do not fix reload crashes with null guards first. Fix the bad writer/reader/owner first.

No new raw save bytes. Do not add `val()`, `readStream()`, `writeStream()`, raw read loops, or raw write loops.

Use stable unique field names. Never rename or reuse. Names describe data, not variables. Bad: `"val"`. Good: `"huntDistance"`.

Enums saved as raw ints (e.g. `eCharActionType`, action `mStage`) are positional. Pin existing entries to explicit integer values (`triremeAction = 49`) so old saves keep loading. New entries pick an unused number (max + 1). Never change an existing value or reuse a retired one.

Before save work, read `fileIO/esavearchive.h`: `field`, `archiveField`, pointer helpers, collection helpers.

Base `read`/`write` open one `eSaveArchive` and call virtual `serializeFields`.
Children serialize fields by calling parent first.

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
