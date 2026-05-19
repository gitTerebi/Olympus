# D-Spine Batch Plan

Source: TODO-save-migration.md D1-D8. Leaf dup keys deferred — resolve via D6 after root migrated.

## Batches (in order)

- [ ] Batch 1: palace + palace tiles — tagged factory fields
- [ ] Batch 2: god monument
- [ ] Batch 3: agora + vendors
- [ ] Batch 4: sanctuary / pyramid pieces
- [ ] Batch 5: commemorative
- [ ] Batch 6: remaining building types

## Per-Batch Steps

1. Enumerate raw factory args in `eBuildingReader::sRead` / `eBuildingWriter::sWrite` for the type.
2. Name tagged factory fields (durable, no `m` prefix).
3. Move ctor args into named fields inside `building.N` archive (D4).
4. Move `cityId` into tagged factory field if not already (D3).
5. Strip raw owner refs from reader/writer; child class owns refs via typed helpers or post-load (D5).
6. Remove duplicate owner writes once factory tagged (D6).
7. Verify: no `src >>` / `dst <<` for this type in reader/writer.
8. Smoke save/load mid-state for the type when feasible.

## Stop Conditions

- ctor field name unclear
- raw field needed pre-instantiation to resolve refs

## Notes

- `e-game-board-read.cpp` building loop still uses `payloadField("buildingData")` wrapping raw factory bytes + tagged `b->write()`. Replace with named `factory` + `state` archives once all batches done (D1 framing).
- Skip leaf dup-key patches during batches — D6 resolves automatically.
