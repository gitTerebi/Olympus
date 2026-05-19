# Save Migration Plan

Single active guide. Keep terse. Do not add diary notes.

## Goal
All save data is tagged, named, bounded, schema-readable, and safe to add/remove/reorder.
Do not add save-version handling in migration batches.
Do not add version bumps, version gates, version readers, migration readers, or old-shape readers unless explicitly asked.

## Hard Rules
No migrated save-node code may add:
- `val()`
- `readStream()` / `writeStream()` / `legacyReadStream()`
- `readStream().read*` / `writeStream().write*`
- raw stream loops
- direct `raw payload fields` for refs/runtime objects
- save-version handling
- version bumps/gates/readers
- migration readers or old-shape readers

`raw payload fields` is allowed only:
- inside central typed helpers, writing one primitive/ref id behind schema API (`characterField`, `buildingField`, `tileField`, `bannerField`, etc.)

Not done:
- moving raw stream code into `raw payload fields`
- `raw payload refs such as cart/worker/boat/building/tile/banner/soldier/trailer/ox` in migrated save code
- raw object bytes inside helper

Schema-readability gate:
- reading `serializeFields(ar)` must show what is saved
- good: `ar.characterField("trailer", &board, mTrailer)`
- bad: `raw trailer payload`

## Save Root Patterns

### P1 owned polymorphic root
Use for top-level owned roots: actions, characters, buildings, events.
- base owns `read(eReadStream&) final`
- base owns `write(eWriteStream&) const final`
- base opens one `eSaveArchive`
- base calls virtual `serializeFields(ar)`
- children do not override `read/write`
- children call parent `serializeFields(ar)` first

```cpp
void Child::serializeFields(eSaveArchive& ar) {
    Parent::serializeFields(ar);
    ar.field("stage", mStage, defaultStage);
    ar.characterField("target", &board(), mTarget);
}
```

### Helper polymorphic root
Use for nested helper families not saved as top-level objects.
- no base `read/write`
- public `serialize(ar)` thin wrapper
- protected virtual `serializeFields(ar)`
- helper writes `has` + `type` + `archiveField(name, child.serialize)`

## Field Rules
- names describe durable data, not member vars
- no `m` prefix for new fields
- never reuse field names
- use defaults for addable fields
- refs use typed helpers
- collections use `arrayField` / `dequeField` / `countedArrayField`
- post-load fixups use `ar.addPostFunc`, not stream access

## Load Timing Rule
- rebuild owned skeleton/state immediately during `serializeFields(ar)` read
- use `ar.addPostFunc` only for refs needing other saved objects
- do not move old immediate lifetime/setup code into post-load blindly
- old `read()` extra work must be classified before deletion:
  - own containers/slots/shells -> immediate read rebuild
  - refs to other roots -> post-load fixup
  - runtime callbacks/actions -> rebuild from stable fields after load

Example:
- `eAgoraBase::fillSpaces()` is owned skeleton/lifetime setup
- it must run immediately on read, not as post-func
- vendor slot attach can be post-load because it depends on building refs

## Runtime Rebuild Rule
Do not save runtime action/callback truth.
Drop/rebuild:
- child walk/wait/missile actions
- lambdas/callbacks/pathfinders
- paused runtime action stacks
- `mCurrentAction`
- `eCharActFunc` / `eGodAct` as action truth unless that family is explicitly migrated as typed root

Keep stable state:
- stage enum
- target refs
- source/home refs
- remaining time
- resource/task/progress
- retry/count flags

On load:
- rebuild current stage directly
- do not advance stage accidentally
- if callback may delete action, keep local `stdptr<T>` before `resumeAction()`

## Done Definition
A batch is done only when:
- build clean when requested (`.\build.bat`; do not use cmake build unless user asks)
- `git diff --check` clean
- target scan has no forbidden APIs except documented helper internals
- no orphan subclass `read/write`
- direct refs use typed helpers, not payload blobs
- post-load semantics preserved
- smoke save/load done for at least one mid-state when possible

## Stop Conditions
Stop and ask if:
- payload owns an object and ownership is unclear
- load order depends on raw stream order
- target lifetime is unclear
- callback/action carried gameplay meaning not represented by stable fields
- collection count/order semantics are unclear
- field name would need rename/reuse
- helper exception seems necessary

## Current State

- A action-adjacent helper roots: DONE, verified.
- B remaining action helper roots: DONE, verified.
- C character save roots: DONE, verified.
- D building save roots: DONE, scan-verified.
- F-tile (eTile/eBanner/eSpawner): DONE, scan-verified.
- F quick wins (`eCityFinances`, `eEmploymentDistributor`, `eReinforcements`, `eMilitaryAid`, `ePlague`): DONE, scan-verified.
- A-D, F-tile, F quick-win scans: no forbidden direct raw payload-field hits in migrated dirs.
- `git diff --check`: clean.

Known gaps:
- Build not run. Use `.\build.bat` only when asked.
- Smoke save/load not run.
- Runtime load semantics not proven beyond code review.
- Existing non-migrated E/F areas still contain legacy payload/raw stream patterns.

Intentional boundaries:
- `eSoldierBanner` is a standalone save root, not part of the character spine.
- `eRacingHorse` is missile-side, not part of C.
- `eMissileTarget` helper is done; broader missile roots can be migrated as a later chunk.

## Work Left

### E - game events: DONE
Detailed batch: `TODO-e-batch.md`.

Audit findings (initial scan):
- `eGameEvents` container: already on `serialize(ar)` + helpers. Done.
- `eGameEvent` base: `read/write` thin wrappers over `serialize(ar)`. Uses `payloadField` for child events. Warning array via `fixedArrayField`. mNextDate still uses raw `readStream/writeStream`.
- `eEventTrigger`: standalone, uses `payloadField` already; may be fine — verify.
- `ewarning`: virtual `read/write`; mNextDate raw stream.
- ~44 subclass headers still declare `read/write` overrides.
- ~21 subclass cpp files use raw `readStream()/writeStream()` (city refs, building refs, dates, parent calls to value helpers).
- Value classes (`eCityEventValue`, `eCountEventValue`, `eResourceEventValue`, `eAttackingCityEventValue`, `eMonsterEventValue`, `eMonstersEventValue`, `ePointEventValue`, `eGodEventValue`, `eGodReasonEventValue`): all on raw stream API, called via `parent::read(ar.readStream(), board)` from subclasses.

Rules:
- migrate `eGameEvent` spine to P1 owned polymorphic root (virtual `serializeFields(ar)`); base owns `read/write`.
- children drop `read/write`, override `serializeFields(ar)`, call `Parent::serializeFields(ar)` first.
- replace mNextDate raw stream with typed date field (add helper or use composite `ar.object`/`ar.objectField`).
- replace city refs (`ar.readStream().readCity`) with typed `worldCityField` / `cityField` helper (verify name in esavearchive).
- replace building refs with `buildingField`.
- promote value classes to **helper polymorphic root** (P2): public `serialize(ar)` thin wrapper, protected `serializeFields(ar)`; subclasses compose via `ar.objectField("cityValue", *this)` style or direct call to `parent::serializeFields(ar)`.
- `eEventTrigger`: convert to `serialize(ar)` if not already; review payloadField uses for schema readability.
- `ewarning`: same P1 treatment if subclassed; otherwise inline `serializeFields`.
- keep type discriminator stable in `eGameEvent` consequences array (already correct).
- runtime/cached fields (`mParent`, `mBoard`, computed warnings) must not be saved.

Work breakdown (do in order):
- [x] E1: base `eGameEvent` — split `read/write` → final base, virtual `serializeFields(ar)`. Replace mNextDate raw stream with date helper.
- [x] E2: `ewarning` — same treatment; ensure no subclass overrides remain.
- [x] E3: value classes — migrate to helper polymorphic root. Update all call sites in subclasses.
- [x] E4: `eEventTrigger` — verify or migrate to `serialize(ar)`.
- [x] E5: subclass batch 1 (army/raid/conquest): `earmyeventbase`, `earmyreturnevent`, `eattackingcityeventvalue` callers, `eplayerconquestevent(base)`, `eplayerraidevent`, `erivalarmyawayevent`.
- [x] E6: subclass batch 2 (disaster/landscape): `eearthquakeevent`, `elandslideevent`, `elavaevent`, `esinklandevent`, `etidalwaveevent`.
- [x] E7: subclass batch 3 (economic/trade/wage): `edemandchangeevent`, `eeconomicchangeevent`, `eeconomicmilitarychangeeventbase`, `epricechangeevent`, `esupplychangeevent`, `esupplydemandchangeevent`, `ewagechangeevent`, `etradeopenupevent`, `etradeshutdownevent`, `emilitarychangeevent`.
- [x] E8: subclass batch 4 (city interactions): `ecitybecomesevent`, `egiftfromevent`, `egifttoevent`, `ereceivetributeevent`, `emakerequestevent`, `eraidresourceevent`, `eresourcegrantedeventbase`.
- [x] E9: subclass batch 5 (military/aid/requests): `erequestaidevent`, `erequeststrikeevent`, `ereinforcementsevent`, `etroopsrequestevent`, `etroopsrequestfulfilledevent`, `requests/e-fulfill-request-event`, `requests/e-pay-tribute-event`.
- [x] E10: gods family: `egodattackevent`, `egoddisasterevent`, `egodquestevent(base)`, `egodquestfulfilledevent`, `egodtraderesumesevent`, `egodvisitevent`, `egodquest`.
- [x] E11: invasions family: `invasions/invasion-event`, `invasion-handler`, `invasion-warning`, `monster-*-event*`.
- [x] E12: final scan, build (when asked), `git diff --check`, document done.

Stop and ask if:
- value class is shared across non-event call sites (call hierarchy needs check before reshaping).
- `eGameEvent` consequences payload ownership is unclear.
- mNextDate or eDate has no typed helper — propose helper before forcing inline.

Audit:
```powershell
rg -n "void\s+(read|write)\s*\(e(Read|Write)Stream&|payload" gameEvents
rg -n "readStream\(|writeStream\(|legacyReadStream\(|\.val\(" gameEvents
```

### F - engine/world graph: IN PROGRESS
Targets:
- `eCampaign`
- `eWorldBoard`
- `eWorldCity`
- `eGameBoard`
- `eBoardCity`
- `eBoardPlayer`
- `eTile`
- episode goals

Done:
- [x] `eCityFinances`
- [x] `eEmploymentDistributor`
- [x] `eReinforcements`
- [x] `eMilitaryAid`
- [x] `ePlague`

Rules:
- top-level graph last.
- keep load ordering explicit.
- named sections for cities/players/buildings/chars/missiles/events/goals/disasters.
- arrays use helpers, not raw numbered loops.
- post-load only through `ar.addPostFunc`.

#### F-tile - `eTile` save root
Detailed batch: `TODO-f-tile-batch.md`.

Scope:
- files: `engine/etile.*`, `engine/etilebase.*`, `spawners/ebanner.*`, `spawners/espawner.*`.
- do not migrate whole F graph.

Plan:
- remove `eBanner` / `eSpawner` raw `read/write` wrappers.
- make `eBanner::serialize(eSaveArchive& ar)` the helper root entry point.
- make `eSpawner::serialize(eSaveArchive& ar)` override and call `eBanner::serialize(ar)` first.
- keep tile banner factory fields: `bannerType`, `bannerId`.
- replace `bannerData` payload with named `factory` + `state` archives.
- prefer `countedArrayField("banners", count, ...)`; manual named count is okay if clearer.

Verify:
- `rg -n "bannerData|payloadField\(" engine/etile.cpp spawners` shows none.
- `rg -n "void\s+(read|write)\s*\(\s*e(Read|Write)Stream" spawners/ebanner.h spawners/espawner.h spawners/ebanner.cpp spawners/espawner.cpp` shows none.
- `git diff --check` clean.

## Resume Commands
```powershell
git status
git diff --check
rg -n "readStream\(|writeStream\(|legacyReadStream\(|\.val\(|payload" <changed-files>
rg -n "void\s+(read|write)\s*\(\s*e(Read|Write)Stream" <target-dir>
```

Build only when asked:
```powershell
.\build.bat
```
