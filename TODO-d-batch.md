# D Building Save Migration

Source: `TODO-save-migration.md` D1-D8.

Goal: every building save is tagged, named, bounded, and safe to add/remove/reorder.
No save-version handling in this batch.
No version bumps, version gates, version readers, migration readers, or old-shape readers.

## Target Shape

Old:

```text
building.N {
  buildingType
  buildingData = blob {
    raw factory bytes
    state bytes
  }
}
```

New:

```text
building.N {
  buildingType
  factory {
    named ctor fields
  }
  state {
    named serializeFields fields
  }
}
```

No `buildingData` blob.
No `factoryLegacy` blob.
No raw `src >>` / `dst <<` in building reader/writer.
No raw `readBuilding()` / `writeBuilding()` owner refs in building reader/writer.

## Migration Order

1. [x] Add public building archive entry point.
   - Keep `serializeFields(eSaveArchive&)` protected.
   - Add public wrapper, e.g. `serialize(eSaveArchive& ar)`.
   - Wrapper calls virtual `serializeFields(ar)`.
2. [x] Change `e-game-board-read.cpp` building loop.
   - Keep `building.N` archive.
   - Keep named `buildingType`.
   - Reader creates object from `factory`.
   - Reader calls building archive entry point on `state`.
   - Writer writes `factory`.
   - Writer calls building archive entry point on `state`.
3. [x] Rewrite `eBuildingReader::sRead` / `eBuildingWriter::sWrite`.
   - They only handle `factory`.
   - They do not read/write state.
   - They do not use `payloadField("state")`.
   - They do not use `payloadField("factoryLegacy")`.
4. [x] Migrate all factory args by type.
   - `cityId`
   - orientation/rotation
   - ids
   - sizes/elevation/special/type
   - costs/collections via `arrayField()` helpers
5. [x] Move owner refs out of reader/writer.
   - Child class owns refs in `serializeFields()`.
   - Use typed helpers like `buildingAsField()`.
   - Rebuild caches/links with post-load funcs.
   - DONE: palaceTile, godMonumentTile, sanctuary pieces, pyramid pieces, vendors.
   - Sanctuary/pyramid post-load uses `tptr->ref<eSanctBuilding>()` via `eStdSelfRef` to call `registerElement(stdsptr)`.
   - Vendors store `agoraId`+`agoraSpaceId` in `eVendor::serializeFields()`; post-load rewires via `buildingWithIOID`.
6. [x] Remove duplicate owner writes.
   - [x] Palace tile owns palace ref (no dup in factory).
   - [x] God monument tile owns monument ref (dup removed).
   - [x] Sanctuary/pyramid pieces own monument refs in `eSanctBuilding::serializeFields()`.
   - [x] Vendors own agora refs in `eVendor::serializeFields()`; factory only writes `cityId`.
7. [x] Delete old raw helpers if unused. Legacy `payloadField("factoryLegacy"/"state")` deleted from reader+writer.

## Type Groups

- [x] Palace + palace tiles
- [x] God monument + god monument tiles
- [x] Agora + vendors — agora ref lives in `eVendor::serializeFields()`
- [x] Sanctuary / pyramid pieces — factory+state clean, monument ref lives in `eSanctBuilding::serializeFields()` (post-load `mon->registerElement(tptr->ref<eSanctBuilding>())`)
- [x] Commemorative
- [x] Remaining building types (housing, road, farms, animals, trees, granary, warehouse, wall, tower, gatehouse, watchpost, maintenance, taxOffice, mint, foundry, refinery, blackMarbleWorkshop, timberMill, masonryShop, huntingLodge, corral, dairy, cardingShed, columns, avenue, park, aesthetics decor, growersLodge, orangeTendersLodge, stadium, gatehouse, pier, tradePost, horseRanch, horseRanchEnclosure, urchinQuay, fishery, triremeWharf, hero halls, pyramid bigs, ruins, placeholder, hippodromePiece)

## Per-Type Checklist

- [x] Factory fields are named.
- [x] Factory field names are stable and durable.
- [x] New field names do not use member prefixes like `m`.
- [x] Renamed/addable fields use explicit defaults.
- [x] `cityId` is in `factory`.
- [x] Constructor args are in `factory`.
- [x] Owner refs are in child `serializeFields()` or post-load.
- [x] No duplicate owner refs remain.
- [x] No raw `src >>` / `dst <<` remains for this type.
- [x] No raw `readBuilding()` / `writeBuilding()` remains for this type.
- [ ] Smoke save/load when feasible.

## D-batch Status

- All migration order steps 1-7 done.
- All type groups done.
- All static invariants verified (zero matches on `buildingData`/`factoryLegacy`/`payloadField("state")`, zero raw streams, zero factory cross-refs).
- Only remaining: live smoke save/load test of a save with sanctuary, pyramid, pier, tradePost, horseRanch, vendors, palace.

## Stop Conditions

- Constructor field name is unclear.
- A raw field seems needed before object creation and cannot be represented as named `factory`.
- A post-load invariant is unclear.

## Verification

```powershell
rg -n "buildingData|factoryLegacy|payloadField\(\"state\"" engine fileIO buildings
rg -n "src >>|dst <<|readBuilding\(|writeBuilding\(" fileIO/ebuildingreader.cpp fileIO/ebuildingwriter.cpp
rg -n "readStream\(|writeStream\(|legacyReadStream\(|\.val\(" buildings fileIO/ebuildingreader.cpp fileIO/ebuildingwriter.cpp
rg -n "field\(\"m" buildings fileIO/ebuildingreader.cpp fileIO/ebuildingwriter.cpp
git diff --check
```

Build only when asked:

```powershell
.\build.bat
```
