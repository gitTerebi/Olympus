# Save Migration Plan

Single active guide. Keep terse. Do not add diary notes.

## Goal
All save data is tagged, named, bounded, schema-readable, and safe to add/remove/reorder.
Old saves may be refused by save-version bump. No dual-path compatibility unless explicitly marked.

## Hard Rules
No migrated save-node code may add:
- `val()`
- `readStream()` / `writeStream()` / `legacyReadStream()`
- `readStream().read*` / `writeStream().write*`
- raw stream loops
- direct `payloadField()` for refs/runtime objects

`payloadField()` is allowed only:
- inside central typed helpers, writing one primitive/ref id behind schema API (`characterField`, `buildingField`, `tileField`, `bannerField`, etc.)
- in `SAVE_COMPAT_LEGACY_FALLBACK` branches

Not done:
- moving raw stream code into `payloadField()`
- `payloadField("cart"/"worker"/"boat"/"building"/"tile"/"banner"/"soldier"/"trailer"/"ox")` in migrated save code
- raw object bytes inside helper unless legacy fallback

Schema-readability gate:
- reading `serializeFields(ar)` must show what is saved
- good: `ar.characterField("trailer", &board, mTrailer)`
- bad: `ar.payloadField("trailer", ...)`

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
- never rename shipped field names
- use defaults for addable fields
- refs use typed helpers
- collections use `arrayField` / `dequeField` / `countedArrayField`
- post-load fixups use `ar.addPostFunc`, not stream access

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

### A — action-adjacent helper roots: DONE, audit only
- `eFindFailFunc` family migrated strict helper-root pattern
- `eObsticleHandler` family migrated strict helper-root pattern
- audit: no direct `payloadField` in `findFailFuncField` / `obsticleHandlerField`

### B — remaining action helper roots: DONE, audit only
- `eWalkableObject`
- `eHasResourceObject`
- `eMissileTarget`
- `ePatrolMoveAction`
- helper roots use `serialize(ar)` + `archiveField`, not direct payload call sites

### C — character save roots: DONE, audit/fix leftovers only
Completed spine:
- `eCharacterBase` P1 final read/write
- `eCharacter` + subclasses use parent-first `serializeFields`
- `eSoldierBanner` standalone root uses archive + post-load hook
- `eRacingHorse` excluded; it is `eMissile`, not character spine

Known C audit targets:
- `eCartTransporter` ox/trailer refs ? must be `characterField`
- `eSoldier` banner ref ? `soldierBannerField`
- `eWildAnimal` spawner ref ? `bannerField`
- `eSoldierBanner` places/soldiers arrays ? `characterField`, not payload refs

C clean audit expected:
```powershell
rg -n "readStream\(|writeStream\(|legacyReadStream\(|payloadField\(" characters -g "!actions/**" -g "!eracinghorse.*"
rg -n "void\s+(read|write)\s*\(\s*e(Read|Write)Stream" characters -g "!actions/**" -g "!eracinghorse.*"
```
Allowed read/write only:
- `characters/echaracterbase.*`
- `characters/esoldierbanner.*`

### D — building save roots: NEXT
Status:
- `eBuilding::serialize` already renamed/prepped as protected virtual `serializeFields(ar)`
- `eBuilding::read/write` still virtual, NOT final
- subclasses still old two-archive pattern
- do not call D done until all subclass overrides are gone and base is final

#### D-stage1 — mechanical building spine
Goal: one archive across full building inheritance tree.
Do not rewrite payload bodies in stage1.
For every building subclass:
- header: remove `read/write` override decls
- header: replace private `serialize(ar)` with protected `serializeFields(ar) override`
- cpp: rename `Klass::serialize` ? `Klass::serializeFields`
- first line: `Parent::serializeFields(ar)`
- delete `Klass::read` / `Klass::write`
- preserve post-load logic at end of `serializeFields`; use `ar.addPostFunc` if refs must resolve first

Final D-stage1 step:
- make `eBuilding::read/write final`
- only after last subclass override is removed

D-stage1 chunks:
1. simple/base: `eaestheticsbuilding`, `eagorabase`, `eanimalbuilding`, `ebuildingwithresource`, `ecorral`, `eelitehousing`, `eemployingbuilding`, `efarmbase`, `egrowerslodge`, `ehousebase`, `eheroshall`, `eprocessingbuilding`, `eresourcebuilding`, `eresourcecollectbuildingbase`, `epatrolsourcebuilding`, `epatroltarget`
2. resource/farm extras: `efishery`, `ehuntinglodge`, missed resource collectors
3. storage/trade/agora/vendor: `estoragebuilding`, `etradepost`, `evendor`, `eartisansguild`, `echariotfactory`, `eurchinquay`
4. sanctuaries/monuments/pyramids: `esanctbuilding`, `esanctuary`, `etemplebuilding`, `etemplealtarbuilding`, `eartemissanctuary`, `emonument`, `epyramid*`
5. roads/hippodrome/specials: `eroad`, `ehippodrome`, `ehippodromepiece`, `epatrolbuildingbase`, `ehorseranch`, `ehorseranchenclosure`, `eshepherbuildingbase`, `epalace`; then flip `eBuilding` final

D-stage1 audit:
```powershell
rg -n "void\s+(read|write)\s*\(\s*e(Read|Write)Stream" buildings
```
Expected after stage1: only `eBuilding` root and documented non-building helper/value types.

#### D-stage2 — building payload refs ? typed helpers
Do only after D-stage1 builds.
Known ref-style payloads to inspect/fix:
- `eartisansguild`: artisan ? `characterField`
- `echariotfactory`: woodCart/horseCart ? `characterField`
- `efishery`: boat ? `characterField`
- `ehippodromepiece`: cart ? `characterField`
- `ehorseranch`: takeCart ? `characterField`
- `ehorseranchenclosure`: horse array ? `characterField`
- `ehuntinglodge`: hunter ? `characterField`
- `emonument`: cart ? `characterField`; stored/used inspect ownership/value
- `epalace`: tile ? `tileField` if ref
- `epatrolbuildingbase`: directionTimes ? typed helper; patroler ? `characterField`
- `eresourcebuildingbase`: cart ? `characterField`
- `eresourcecollectbuilding`: collector ? `characterField`
- `eroad`: underAgora/underGatehouse/aboveHippodrome ? `buildingField`
- `eshepherbuildingbase`: shepherd ? `characterField`
- `estoragebuilding`: cart1/cart2 ? `characterField`
- `eurchinquay`: gatherer ? `characterField`
- `evendor`: cart ? `characterField`
- `eartemissanctuary`: banner ? `soldierBannerField` / `bannerField`
- `epyramidbuildingpart`: paint inspect; likely value/archive field
- `ehippodrome`: piece inspect ownership; ref vs owned decides `buildingField` vs archive subobject

Add missing helpers before caller migration:
- raw pointer `buildingField(name, board, T*& value)` if needed
- any banner/soldierBanner helper if absent

D-stage2 audit:
```powershell
rg -n "payloadField\(" buildings
rg -n "readStream\(|writeStream\(|legacyReadStream\(" buildings
```
No direct payload refs allowed.

### E — game events: NOT STARTED
Targets:
- `eGameEvents`
- `eGameEvent`
- all event subclasses
- event value classes
- `eEventTrigger`

Rules:
- migrate base root first
- keep type discriminator stable
- children call parent first
- replace `eventData` payload blobs with type + archive serializer
- replace event value payload blobs with typed value helpers

Audit:
```powershell
rg -n "void\s+(read|write)\s*\(e(Read|Write)Stream&|payloadField\(" gameEvents
```

### F — engine/world graph: NOT STARTED
Targets:
- `eCampaign`
- `eWorldBoard`
- `eWorldCity`
- `eGameBoard`
- `eBoardCity`
- `eBoardPlayer`
- `eTile`
- military aid, plague, reinforcements, finances, episode goals

Rules:
- top-level graph last
- keep load ordering explicit
- named sections for cities/players/buildings/chars/missiles/events/goals/disasters
- arrays use helpers, not raw numbered loops
- post-load only through `ar.addPostFunc`

## Resume Commands
```powershell
git status
git diff --check
rg -n "readStream\(|writeStream\(|legacyReadStream\(|\.val\(|payloadField\(" <changed-files>
rg -n "void\s+(read|write)\s*\(\s*e(Read|Write)Stream" <target-dir>
```

Build only when asked:
```powershell
.\build.bat
```
