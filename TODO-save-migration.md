# Save Migration Plan

## Goal

All save data is tagged, named, bounded, and safe to add/remove/reorder.
Old saves are refused after hard save-version bump. No dual-path migration.

## Read This First

This file is the active LLM migration guide.
Keep it short. Do not add diary notes.
If a detail is not needed to perform the next migration safely, do not put it here.

## Hard Forbidden

Do not add these in save-node code:

- `val()`
- `readStream()` / `writeStream()` / `legacyReadStream()`
- `readStream().read*` / `writeStream().write*`
- raw loops over stream data
- direct `payloadField` for runtime action blobs

Forbidden runtime blobs in action saves:

- `mCurrentAction`
- `mFinishAction` / `mFailAction` / `mDeleteFailAction`
- `eCharActFunc` as save truth
- `eGodAct` as save truth
- paused action snapshots holding `eCharacterAction*` / `fA`
- lambdas, pathfinders, walk/wait child actions, missile child actions

## Allowed Helpers

Use named archive helpers:

```cpp
ar.field("name", value, defaultValue);
ar.tileField("targetTile", board(), tile);
ar.characterField("worker", &board(), worker);
ar.buildingField("targetBuilding", &board(), building);
ar.buildingAsField("sourceBuilding", &board(), buildingWithResource);
ar.objectField("child", child);
ar.archiveField("group", [&](eSaveArchive& groupAr) { ... });
ar.arrayField("items", items, [](eSaveArchive& itemAr, Item& item) { ... });
ar.dequeField("items", items, [](eSaveArchive& itemAr, Item& item) { ... });
```

Helper-internal `payloadField` is allowed only for archive/ref helper internals.
Action classes must not introduce direct payload blobs for runtime objects.

Current helper exception:

- `missileTargetField(...)` wraps `eMissileTarget` in one bounded helper. OK until replaced by typed fields.

## P1 Contract

For each polymorphic save root:

- Base owns `read(eReadStream&)` and `write(eWriteStream&)`.
- Base opens one `eSaveArchive` and calls virtual `serializeFields(ar)`.
- Base `read/write` are final.
- Subclasses do not override `read/write` unless they are a new owned save root.
- Subclasses implement `serializeFields(eSaveArchive& ar)`.
- Subclasses call parent first.
- Field names describe durable data, not member vars. No `m` prefix.
- Once a field name ships, never rename or reuse it.

Pattern:

```cpp
void Child::serializeFields(eSaveArchive& ar) {
    Parent::serializeFields(ar);
    ar.field("stage", mStage);
    ar.buildingField("targetBuilding", &board(), mTargetBuilding);
}
```

## FSM-FINAL Rule

P1 is not done until transient runtime saves are removed and load rebuilds runtime actions.
No "plumbing now, FSM later" for a class.

Bucket every saved thing:

- KEEP: stage enum, target ref, home/source ref, retry count, remaining time, resource/task, progress.
- DROP: child action, callback object, lambda, pathfinder, paused action stack, missile/wait/walk runtime action.

When dropping runtime state, add stable fields and `resumeFromSavedState()` in the same pass.

## Gameplay Preservation Gate

Migration may change save shape. It must not drop gameplay meaning.

Before editing any class, fill this mentally or in a short note:

```md
Class:
OLD saved runtime thing:
- `mCurrentAction` / `eCharActFunc` / paused `fA`
Runtime meaning it carried:
- timer / target / callback effect / resume stage / resource / path goal
NEW saved stable fields:
- `stage`, `targetBuilding`, `remainingTime`, `killOnFinish`, etc.
Load rebuild:
- method that recreates runtime action/callback
Finish path:
- method that advances/restores stage after callback fires
```

Blocking checklist per class:

- [ ] list every `setFinishAction`
- [ ] list every `setFailAction`
- [ ] list every `pauseAction`
- [ ] list every child action timer
- [ ] state what gameplay effect survives load
- [ ] only then migrate

If any row is unknown: STOP. Do not edit.

## Action FSM Rebuild Rules

On load:

- rebuild current stage activity, do not advance stage accidentally.
- `goTo` stage rebuilds the go-to action.
- `patrol` stage rebuilds patrol action/timer.
- `wait` stage rebuilds wait/moveAround timer from remaining time.
- attack/missile-in-flight stage rebuilds missile from saved target.

On finish callback:

- keep a local `stdptr<T>` before calling `resumeAction()` if callback says action can be deleted.
- if `resumeAction()` stack is empty after load, call class rebuild method for restored stage.

## Migration Workflow

1. Hard bump save version and refuse old saves.
2. Ensure archive helpers are scan-safe and item-scoped.
3. Pick one class.
4. Run Gameplay Preservation Gate.
5. Replace raw/runtime save with named stable fields.
6. Add/verify `resumeFromSavedState()` rebuild.
7. Scan class for forbidden APIs.
8. Build only when asked: `./build.bat`.
9. Gameplay smoke: save/load mid-stage.
10. Mark status only after scan + build + smoke.

## Known Gameplay Fix Patterns

### Monster attack pattern

Saved fields:

- `stage`
- `preAttackStage`
- `attackTarget`
- `attackActionType`
- `attackTime`
- `attackBuilding`
- `waitRemaining`
- `patrolRemaining`

Rules:

- `resumeFromSavedState()` calls `rebuildCurrentStage()`.
- attack/building stages rebuild missile from saved target.
- finish callback restores previous stage.
- if paused stack empty, callback calls `rebuildCurrentStage()`.

### God help missile-in-flight pattern (aphrodite reference)

Saved fields:

- `stage`
- `prePopulatingStage` (or per-class equivalent)
- `populateTarget` (target building/tile/char as `stdptr<T>`)

Rules:

- enum gains `populating` (or `attacking`, etc) stage = missile in flight.
- trigger method records prev stage, sets stage to populating, saves target, calls helper that spawns missile only.
- `decide()` has `case populating:` that rebuilds missile from saved target. if target gone -> finish + recurse decide.
- dedicated finish callback (e.g. `eAHA_populateFinish`) in same header. lifetime-safe: local `stdptr<T> t = mTptr` before `resumeAction()`. after resume, if no current action, call class `rebuildCurrentStage()`.
- `resumeFromSavedState()` -> `rebuildCurrentStage()` switch by stage; populating rebuilds; other stages delegate to base.
- enum + creator switch must be wired in `echaracteractionfunction.h` and `echaracteractionfunctioncreator.cpp`.

### Dionysus follower gap

Old runtime meaning:

- converted follower has finish/fail callback `eChar_killWithCorpseFinish`.

Required stable save:

- bool `killOnFinish` or equivalent.

Load rebuild:

- if `killOnFinish`, reattach kill finish/fail callback.

## Save Ownership Graph

```text
eCampaign / eWorldBoard / eGameBoard
  -> eBoardCity / eTile / board-owned objects
  -> eBuilding tree
  -> eCharacter tree
      -> eCharacterAction tree
          -> runtime child actions/callbacks are rebuilt, not saved as truth
  -> eGameEvent tree
```

## Section Checklist

Use only these labels:

- `[ ]` not done
- `[x]` done
- `NEED FIX`
- `AUDIT`

Do not use `[~]` in sections C/D/E/F.
If runtime blobs remain, class is `[ ]` or `NEED FIX`.

### Detailed Individual TODOs

### A - Archive Engine

- [x] Add outer save version gate.
- [x] Make `payloadField` use `takeField`.
- [x] Scope `arrayField`.
- [x] Scope `dequeField`.
- [x] Scope `countedArrayField`.
- [x] Verify duplicate field names still work where intended.
- [x] Add debug `printf` for bad version, bad count, and missing required payload.

### B - Board / Campaign Roots

When migrating child classes (`eGameBoard`, episodes, `eSetAside`), make them default-constructible with `setBoard()` / `setWorldBoard()` post-construct wiring. After that, return to `eCampaign::serialize` and strip the remaining `ar.reading()` branches that construct children with ctor args.

- [~] `eCampaign` - done except `reading()` branches for child construction (revisit after children migrated)
- [~] `eWorldBoard` - done except `reading()` for `eWorldCity` construction (revisit after `eWorldCity`)
- [~] `eWorldCity` - done except `reading()` for `mConqueredBy` city ref (revisit when killing reading() branches globally)
- [~] `eGameBoard` - done except `reading()` branches for ctor-required children
- [~] `eBoardCity` - done except `reading()` branches for ctor-required children (plagues/aid/banners/hippodromes)
- [x] `eBoardPlayer`
- [x] `eTile`
- [x] `eAvailableBuildings`
- [x] `eAICityPlan`
- [x] `eAIDistrict`
- [x] `eEmploymentDistributor`
- [x] `ePopulationData`
- [x] `eHusbandryData`
- [x] `eEmploymentData`
- [x] `eCityFinances`
- [~] `eMilitaryAid` - done except `payloadField` for city ref (uses raw stream callback via lambda; OK)
- [x] `ePlague`
- [x] `eReinforcements`
- [~] `eEpisode` - done except `reading()` for `eGameEvent` factory construction
- [x] `eParentCityEpisode`
- [x] `eColonyEpisode`
- [x] `eEpisodeGoal`
- [x] `eSetAside`
- [x] `eCityRequest`
- [x] `eTributePayment`
- [x] `eGodQuest`


### C - Buildings

- [x] `eBuilding`
- [x] `eAestheticsBuilding`
- [x] `eWaterPark`
- [x] `eCommemorative`
- [x] `eGodMonument`
- [x] `eGodMonumentTile`
- [x] `eAgoraSpace`
- [x] `eAnimalBuilding`
- [x] `eAvenue`
- [x] `eBuildingWithResource`
- [x] `eEmployingBuilding`
- [x] `eResourceBuildingBase`
- [x] `eCorral`
- [x] `eFarmBase`
- [x] `eProcessingBuilding`
- [x] `eResourceCollectBuildingBase`
- [x] `eFishery`
- [x] `eHuntingLodge`
- [x] `eResourceCollectBuilding`
- [x] `eUrchinQuay`
- [x] `eShepherBuildingBase`
- [x] `eArtisansGuild`
- [x] `eChariotFactory`
- [x] `eGrowersLodge`
- [x] `eHorseRanch`
- [x] `eMonument`
- [x] `ePyramid`
- [x] `eSanctuary`
- [x] `eSanctuaryWithWarriors`
- [x] `eHephaestusSanctuary`
- [x] `ePatrolBuildingBase`
- [x] `eAgoraBase`
- [x] `ePatrolBuilding`
- [x] `eStorageBuilding`
- [x] `eTower`
- [x] `eTradePost`
- [x] `eTriremeWharf`
- [x] `eVendor`
- [x] `eHippodromePiece`
- [x] `eHorseRanchEnclosure`
- [x] `eGatehouse`
- [x] `eHerosHall`
- [x] `eHouseBase`
- [x] `eEliteHousing`
- [x] `eSmallHouse`
- [x] `ePalace`
- [x] `ePalaceTile`
- [x] `ePark`
- [x] `ePier`
- [x] `ePlaceholder`
- [x] `eResourceBuilding`
- [x] `eRoad`
- [x] `eRuins`
- [x] `eSanctBuilding`
- [x] `ePyramidElement`
- [x] `eTempleAltarBuilding`
- [x] `eTempleBuilding`
- [x] `eTempleMonumentBuilding`
- [x] `eTempleStatueBuilding`
- [x] `eTempleTileBuilding`
- [x] `eWall`
- [x] `eHippodrome`

### D - Characters

- [x] `eCharacterBase`
- [x] `eCharacter`
- [x] `eAnimal`
- [x] `eArcher`
- [x] `eArtisan`
- [x] `eBasicPatroler`
- [x] `eActor`
- [x] `eAstronomer`
- [x] `eButcher`
- [x] `eCartTransporter`
- [x] `eCompetitor`
- [x] `eCurator`
- [x] `eDonkey`
- [x] `eGymnast`
- [x] `eHealer`
- [x] `eHomeless`
- [x] `eInventor`
- [x] `eOx`
- [x] `ePeddler`
- [x] `ePhilosopher`
- [x] `ePorter`
- [x] `eScholar`
- [x] `eSettler`
- [x] `eTaxCollector`
- [x] `eTrader`
- [x] `eWaterDistributor`
- [x] `eBoatBase`
- [x] `eEnemyBoat`
- [x] `eTradeBoat`
- [x] `eTrireme`
- [x] `eCattle`
- [x] `eChariot`
- [x] `eFightingPatroler`
- [x] `eDisgruntled`
- [x] `eEliteCitizen`
- [x] `eSick`
- [x] `eSoldier`
- [x] `eAmazon`
- [x] `eArcherBase`
- [x] `eAresWarrior`
- [x] `eChariotBase`
- [x] `eHopliteBase`
- [x] `eHorsemanBase`
- [x] `eRangeSoldier`
- [x] `eSpearthrowerBase`
- [x] `eWatchman`
- [x] `eFireFighter`
- [x] `eGod`
- [x] `eBasicGod`
- [x] `eDionysus`
- [x] `eExtendedGod`
- [x] `eHermes`
- [x] `eGrower`
- [x] `eHero`
- [x] `eBasicHero`
- [x] `eHorse`
- [x] `eMonster`
- [x] `eBasicMonster`
- [x] `eWaterMonster`
- [x] `eResourceCollectorBase`
- [x] `eFishingBoat`
- [x] `eResourceCollector`
- [x] `eShepherdBase`
- [x] `eUrchinGatherer`
- [x] `eTrailer`

### E - Character Actions

- [x] `eCharacterAction`
- [x] `eBuildAction`
- [x] `eCollectAction`
- [x] `eComplexAction`
- [ ] `eActionWithComeback`
- [x] `eArtisanAction`
- [x] `eCartTransporterAction`
- [x] `eDeliverCartAction`
- [x] `eGetCartAction`
- [x] `eVendorCartAction`
- [x] `eCollectResourceAction`
- [x] `eGrowerAction`
- [x] `eHeroAction`
- [x] `eHuntAction`
- [x] `ePatrolAction`
- [x] `eFireFighterAction`
- [x] `eReplaceCattleAction`
- [x] `eSettlerAction`
- [x] `eShepherdAction`
- [x] `eSickDisgruntledAction`
- [x] `eTakeCattleAction`
- [x] `eTraderAction`
- [x] `eAnimalAction`
- [x] `eArcherAction`
- [x] `eFightingAction`
- [x] `eSoldierAction`
- [x] `eTriremeAction`
- LIMITATION `eGodMonsterAction` - paused stack not saved; fightGod mid-flight transient loss accepted
- [x] `eDefendAttackCityAction`
- [x] `eAttackCityAction`
- [x] `eDefendCityAction`
- [x] `eGodAttackAction` - attacking/destroyingBuilding stages with eGodActType-tagged intent; virtual lookForRangeAction override rebuilds missile
- [ ] `eGodAction` - AUDIT: visit/worship/favor flows use runtime callbacks
- [x] `eAphroditeHelpAction`
- [x] `eApolloHelpAction`
- [x] `eAresHelpAction`
- [x] `eAtlasHelpAction`
- [x] `eHadesHelpAction`
- [x] `eHephaestusHelpAction`
- [x] `eHeraHelpAction`
- [x] `eHermesHelpAction`
- [x] `eProvideResourceHelpAction`
- [x] `eZeusHelpAction`
- [ ] `eGodMonsterActionInd`
- [x] `eMonsterAction`
- [ ] `eMoveToAction`
- [x] `eDieAction`
- [x] `eFightAction`
- [x] `eMoveAction`
- [x] `eFollowAction`
- [x] `eDionysusFollowAction`
- [x] `eMoveAroundAction`
- [x] `eMovePathAction`
- [x] `ePatrolMoveAction`
- [x] `eWaitAction`
- [ ] `eWalkableObject`
- [ ] `eHasResourceWalkableObject`
- [ ] `eRectWalkableObject`
- [ ] `eHasResourceObject`
- [ ] `eHasNonBusyResourceObject`

### F - Game Events

- [ ] `eGameEvents`
- [ ] `eGameEvent`
- [ ] `eArmyEventBase`
- [ ] `eArmyReturnEvent`
- [ ] `ePlayerConquestEventBase`
- [ ] `ePlayerConquestEvent`
- [ ] `ePlayerRaidEvent`
- [ ] `eTroopsRequestFulfilledEvent`
- [ ] `eReinforcementsEvent`
- [ ] `eCityBecomesEvent`
- [ ] `eEarthquakeEvent`
- [ ] `eEconomicMilitaryChangeEventBase`
- [ ] `eEconomicChangeEvent`
- [ ] `eMilitaryChangeEvent`
- [ ] `eFulfillRequestEvent`
- [ ] `eGiftToEvent`
- [ ] `eGodAttackEvent`
- [ ] `eGodDisasterEvent`
- [ ] `eGodQuestEventBase`
- [ ] `eGodQuestEvent`
- [ ] `eGodQuestFulfilledEvent`
- [ ] `eGodTradeResumesEvent`
- [ ] `eGodVisitEvent`
- [ ] `eInvasionEvent`
- [ ] `eLandSlideEvent`
- [ ] `eLavaEvent`
- [ ] `eMonsterInvasionEventBase`
- [ ] `eMonsterInCityEvent`
- [ ] `eMonsterInvasionEvent`
- [ ] `eMonsterUnleashedEvent`
- [ ] `ePayTributeEvent`
- [ ] `ePriceChangeEvent`
- [ ] `eReceiveTributeEvent`
- [ ] `eRequestAidEvent`
- [ ] `eRequestStrikeEvent`
- [ ] `eResourceGrantedEventBase`
- [ ] `eGiftFromEvent`
- [ ] `eMakeRequestEvent`
- [ ] `eRaidResourceEvent`
- [ ] `eRivalArmyAwayEvent`
- [ ] `eSinkLandEvent`
- [ ] `eSupplyDemandChangeEvent`
- [ ] `eDemandChangeEvent`
- [ ] `eSupplyChangeEvent`
- [ ] `eTidalWaveEvent`
- [ ] `eTradeOpenUpEvent`
- [ ] `eTradeShutDownEvent`
- [ ] `eTroopsRequestEvent`
- [ ] `eWageChangeEvent`
- [ ] `eEventTrigger`
- [ ] `eWarning`
- [ ] `eAttackingCityEventValue`
- [ ] `eCityEventValue`
- [ ] `eCountEventValue`
- [ ] `eGodEventValue`
- [ ] `eGodReasonEventValue`
- [ ] `eMonsterEventValue`
- [ ] `eMonstersEventValue`
- [ ] `ePointEventValue`
- [ ] `eResourceEventValue`
- [ ] `eResourceGrantedEventValues`


## Scan Commands

```powershell
git diff --check

git diff --name-only

# changed action files: forbidden raw/blob APIs
$files = git diff --name-only
foreach($f in $files){
  if(($f -like 'characters/actions/*' -or $f -like 'characters/gods/actions/*') -and (Test-Path $f)){
    Select-String -Path $f -Pattern 'readStream\(|writeStream\(|payloadField\('
  }
}

# top-level read/write override candidates
foreach($f in $files){
  if(($f -like 'characters/actions/*.h' -or $f -like 'characters/actions/godHelp/*.h' -or $f -like 'characters/gods/actions/*.h') -and (Test-Path $f)){
    Select-String -Path $f -Pattern '^\s*void\s+(read|write)\s*\('
  }
}
```

## Stop Conditions

Stop and ask before editing if:

- gameplay meaning of a dropped callback/action is unknown.
- finish path after load is unclear.
- a stage enum value means both "doing X" and "X finished".
- target lifetime/ref helper is unclear.
- a raw stream call seems necessary outside helper internals.
