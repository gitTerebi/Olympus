# Save System TODO

Goal: make JSON saves boring. Bad refs should become logged nulls, owner repair should restore invariants, and the game should not tick until loaded state is coherent.

## Rules

- Save data, not live pointer state.
- Serializers only read/write fields.
- Serializers must not kill, cleanup, repair, or mutate gameplay state beyond assigning loaded data.
- Loaded refs resolve centrally by stable IOID.
- Missing or wrong-type refs become null and print field name + IOID.
- Owner classes repair their own invariants after all refs resolve.
- Validation runs after repair and before first tick.
- Old save readers stay compatible.
- New JSON writer emits clean structural schema.

## Phase 1: Core Ref Safety

- Make `stdptr` use weak ref as source of truth.
- Remove trust in cached raw ptrs for persisted refs.
- Add central JSON ref helpers:
  - `characterRef<T>(key, stdptr<T>&, board)`
  - `buildingRef<T>(key, stdptr<T>&, board)`
  - `soldierBannerRef(key, stdsptr<eSoldierBanner>&, board)`
- Add central binary/tagged ref helpers with same missing-ref behavior.
- Every missing ref logs:
  - save field path
  - IOID
  - expected type
  - owner object type when available

## Phase 2: Load Pipeline

- Split load into phases:
  1. create all objects
  2. assign IOIDs
  3. load scalar fields
  4. resolve refs
  5. run owner repair
  6. validate world
  7. allow `incTime`
- Add `eGameBoard::repairLoadedRefs()`.
- Add `eGameBoard::validateLoadedState()`.
- Stop game tick if validation has fatal errors.

## Phase 3: Owner Repair

- Cart transporter:
  - preserve saved follower slots during read.
  - after refs resolve, prune invalid follower tail.
  - rebuild follow chain only if needed.
- Soldier banner:
  - remove missing soldiers.
  - ensure each soldier points back to banner.
  - clear stale tile/banner links.
- Buildings with workers/carts:
  - null missing worker/cart refs.
  - respawn only through normal runtime logic, not serializer.
- Actions:
  - null missing targets.
  - finish/cancel actions with missing required owner/target.

## Phase 4: JSON Schema Cleanup

- Use arrays for repeated data:
  - `followers: [12, null, 18]`
  - `soldiers: [4, 5, null]`
- Keep backward-compatible readers for old `count` + `field.N` shape.
- Store refs as JSON values:
  - `null`
  - `{ "ioid": 12, "type": "character" }`
- Add schema version to save root.
- Add per-object version only where needed.

## Phase 5: Diagnostics

- Add debug save validation flag.
- On save, report:
  - refs to scheduled-delete objects
  - refs to unregistered objects
  - stale owner/backref mismatches
- On load, report:
  - missing refs by field
  - wrong type refs
  - repaired owner counts
  - fatal validation errors

## Phase 6: Tests

- Add round-trip tests:
  - old binary save -> load -> JSON save -> load -> tick 100 frames
  - JSON save -> load -> JSON save -> load -> compare counts
- Add fixtures:
  - cart with null follower slot
  - cart with missing follower tail
  - banner with missing soldier
  - action with missing target
  - building with missing worker/cart
- Test invariants:
  - no crash after load
  - no active list contains scheduled-delete object
  - all soldiers have correct banner backref
  - all cart follower chains are coherent

## Current Hot Spots

- `pointers/estdpointer.h`
- `fileIO/ejsonarchive.*`
- `fileIO/ereadstream.*`
- `fileIO/ewritestream.*`
- `characters/ecarttransporter.*`
- `characters/esoldierbanner.*`
- `characters/esoldier.*`
- `engine/e-game-board-read.cpp`
- `engine/e-game-board.cpp`
