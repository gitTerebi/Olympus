# Save Migration Plan

## Goal

Every saved byte goes through a named `eSaveArchive` tag.

New fields can be added, removed, or reordered without corrupting later reads.

Current game logic and member types stay the same.

Old positional saves must not be half-read. Either migrate them deliberately or version-gate and fail clean.

## Core Rules

- No new raw save bytes.
- Do not add `val()`, `readStream()`, `writeStream()`, `readStream().read*`, or `writeStream().write*`.
- Use stable unique field names.
- Never rename or reuse a shipped field name.
- Names describe data, not variables. Bad: `"val"`. Good: `"huntDistance"`.
- Parent fields serialize first.
- Child classes do not open a second archive on the same stream.
- Child classes should not override `read`/`write` once the base entry point is virtual-safe.
- Action saves must store stable FSM intent/state, not transient runtime actions.
- Runtime pathfinding, lambdas, queued searches, and current child movement actions are rebuilt after load from saved FSM state.
- Parent action FSM is the source of truth; `mCurrentAction` is transient execution detail.

## Read First

Before editing save code, read these in `fileIO/esavearchive.h`:

- `field(name, value, default)` — tagged primitive.
- `payloadField(name, write, read)` — tagged blob.
- `archiveField(name, func)` — named sub-archive.
- `objectField(name, obj)` — named object payload.
- `buildingAsField`, `buildingField`, `characterField`, `characterAsField`, `tileField`, `cityField`, `gameEventField`, `soldierBannerField` — tagged refs.
- `arrayField`, `dequeField`, `countedArrayField`, `fixedArrayField` — tagged collections.
- `takeField` — scans forward and caches unmatched fields.
- `readField` — reads next field only.

## Save Ownership Graph

```text
eCampaign
├── eWorldBoard
│   ├── eWorldCity[]
│   │   └── eResourceTrade[]
│   └── eWorldRegion[]
├── parent eGameBoard
├── colony eGameBoard[]
├── eParentCityEpisode[]
├── eColonyEpisode[]
├── eSetAside[] for colony
└── eSetAside[] for parent
```

`eCampaign` is the outer save wrapper.

`eBoardCity` is not the outer wrapper. It is saved inside `eGameBoard`.

```text
eGameBoard
├── eBoardCity[]
├── eBoardPlayer[]
├── eTile[][]  (each eTile derives eTileBase)
├── eBuilding[]
├── eCharacter[]
│   └── eCharacterAction
│       └── child actions
├── eMissile[]
├── eEpisodeGoal[]
├── eEarthquake[]
├── eTidalWave[]
├── eLavaFlow[]
├── eLandSlide[]
├── ePlannedAction[]
│   ├── eRegrowForestAction
│   └── eColonyMonumentAction
├── eYearlyProduction[]
└── eLoggedMessage[]
```

`eGameBoard` also keeps registries for lookup and post-load fixups:

- `mAllBuildings`
- `mCharacters`
- `mCharacterActions`
- `mAllGameEvents`
- `mAllSoldierBanners`
- `mBanners`

These registries are not all independent save roots.

```text
eBoardCity
├── eAvailableBuildings
├── eGameEvents
│   └── eGameEvent[]
├── eAICityPlan
│   └── eAIDistrict[]
│       └── eDistrictReadyCondition[]
├── eEmploymentDistributor
├── eMilitaryAid[]
├── ePlague[]
├── eSoldierBanner[]
├── eHippodrome[]
├── eReinforcements[]
├── ePopulationData
├── eHusbandryData
├── eEmploymentData
└── registered refs to buildings, chars, monsters, banners, trade posts, invasions
```

Building and character objects are owned by `eGameBoard` lists.

`eYearlyProduction`, `eLoggedMessage`, and `eSetAside` are structs, but they are save nodes.

## Building Class Tree

```text
eBuilding
├── eAestheticsBuilding
├── eWaterPark
├── eCommemorative
├── eGodMonument
├── eGodMonumentTile
├── eAgoraSpace
├── eAnimalBuilding
├── eAvenue
├── eBuildingWithResource
│   ├── eEmployingBuilding
│   │   ├── eResourceBuildingBase
│   │   │   ├── eCorral
│   │   │   ├── eFarmBase
│   │   │   ├── eProcessingBuilding
│   │   │   ├── eResourceCollectBuildingBase
│   │   │   │   ├── eFishery
│   │   │   │   ├── eHuntingLodge
│   │   │   │   ├── eResourceCollectBuilding
│   │   │   │   └── eUrchinQuay
│   │   │   └── eShepherBuildingBase
│   │   ├── eArtisansGuild
│   │   ├── eChariotFactory
│   │   ├── eGrowersLodge
│   │   ├── eHorseRanch
│   │   ├── eMonument
│   │   │   ├── ePyramid
│   │   │   └── eSanctuary
│   │   │       ├── eSanctuaryWithWarriors
│   │   │       └── eHephaestusSanctuary
│   │   ├── ePatrolBuildingBase
│   │   │   ├── eAgoraBase
│   │   │   └── ePatrolBuilding
│   │   ├── eStorageBuilding
│   │   ├── eTower
│   │   ├── eTradePost
│   │   ├── eTriremeWharf
│   │   └── eVendor
│   ├── eHippodromePiece
│   └── eHorseRanchEnclosure
├── eGatehouse
├── eHerosHall
├── eHouseBase
│   ├── eEliteHousing
│   └── eSmallHouse
├── ePalace
├── ePalaceTile
├── ePark
├── ePier
├── ePlaceholder
├── eResourceBuilding
├── eRoad
├── eRuins
├── eSanctBuilding
│   ├── ePyramidElement
│   ├── eTempleAltarBuilding
│   ├── eTempleBuilding
│   ├── eTempleMonumentBuilding
│   ├── eTempleStatueBuilding
│   └── eTempleTileBuilding
└── eWall
```

`eHippodrome` is not an `eBuilding`; it is a separate `eBoardCity` save object.

Renderer classes are not save nodes.

## Character Class Tree

```text
eCharacterBase
└── eCharacter
    ├── eAnimal
    ├── eArcher
    ├── eArtisan
    ├── eBasicPatroler
    │   ├── eActor
    │   ├── eAstronomer
    │   ├── eButcher
    │   ├── eCartTransporter
    │   ├── eCompetitor
    │   ├── eCurator
    │   ├── eDonkey
    │   ├── eGymnast
    │   ├── eHealer
    │   ├── eHomeless
    │   ├── eInventor
    │   ├── eOx
    │   ├── ePeddler
    │   ├── ePhilosopher
    │   ├── ePorter
    │   ├── eScholar
    │   ├── eSettler
    │   ├── eTaxCollector
    │   ├── eTrader
    │   └── eWaterDistributor
    ├── eBoatBase
    │   ├── eEnemyBoat
    │   ├── eTradeBoat
    │   └── eTrireme
    ├── eCattle
    ├── eChariot
    ├── eFightingPatroler
    │   ├── eDisgruntled
    │   ├── eEliteCitizen
    │   ├── eSick
    │   ├── eSoldier
    │   │   ├── eAmazon
    │   │   ├── eArcherBase
    │   │   ├── eAresWarrior
    │   │   ├── eChariotBase
    │   │   ├── eHopliteBase
    │   │   ├── eHorsemanBase
    │   │   ├── eRangeSoldier
    │   │   └── eSpearthrowerBase
    │   └── eWatchman
    ├── eFireFighter
    ├── eGod
    │   ├── eBasicGod
    │   ├── eDionysus
    │   ├── eExtendedGod
    │   └── eHermes
    ├── eGrower
    ├── eHero
    │   └── eBasicHero
    ├── eHorse
    ├── eMonster
    │   ├── eBasicMonster
    │   └── eWaterMonster
    ├── eResourceCollectorBase
    │   ├── eFishingBoat
    │   ├── eResourceCollector
    │   ├── eShepherdBase
    │   └── eUrchinGatherer
    └── eTrailer
```

`eThreadCharacter` also derives from `eCharacterBase`, but it is thread-side state, not the city character save tree.

## Character Action Class Tree

```text
eCharacterAction
├── eBuildAction
├── eCollectAction
├── eComplexAction
│   ├── eActionWithComeback
│   │   ├── eArtisanAction
│   │   ├── eCartTransporterAction
│   │   │   ├── eDeliverCartAction
│   │   │   ├── eGetCartAction
│   │   │   └── eVendorCartAction
│   │   ├── eCollectResourceAction
│   │   ├── eGrowerAction
│   │   ├── eHeroAction
│   │   ├── eHuntAction
│   │   ├── ePatrolAction
│   │   │   └── eFireFighterAction
│   │   ├── eReplaceCattleAction
│   │   ├── eSettlerAction
│   │   ├── eShepherdAction
│   │   ├── eSickDisgruntledAction
│   │   ├── eTakeCattleAction
│   │   └── eTraderAction
│   ├── eAnimalAction
│   ├── eArcherAction
│   ├── eFightingAction
│   │   ├── eSoldierAction
│   │   └── eTriremeAction
│   ├── eGodMonsterAction
│   │   ├── eDefendAttackCityAction
│   │   │   ├── eAttackCityAction
│   │   │   └── eDefendCityAction
│   │   ├── eGodAction
│   │   │   ├── eAphroditeHelpAction
│   │   │   ├── eApolloHelpAction
│   │   │   ├── eAresHelpAction
│   │   │   ├── eAtlasHelpAction
│   │   │   ├── eHadesHelpAction
│   │   │   ├── eHephaestusHelpAction
│   │   │   ├── eHeraHelpAction
│   │   │   ├── eHermesHelpAction
│   │   │   ├── eProvideResourceHelpAction
│   │   │   └── eZeusHelpAction
│   │   ├── eGodMonsterActionInd
│   │   └── eMonsterAction
│   └── eMoveToAction
├── eDieAction
├── eFightAction
├── eMoveAction
│   ├── eFollowAction
│   │   └── eDionysusFollowAction
│   ├── eMoveAroundAction
│   ├── eMovePathAction
│   └── ePatrolMoveAction
└── eWaitAction
```

Walkable helper nodes used by actions:

```text
eWalkableObject
├── eHasResourceWalkableObject
└── eRectWalkableObject

eHasResourceObject
└── eHasNonBusyResourceObject
```

God action helper nodes such as `eGodAct`, `eFindFailFunc`, and `eMissileTarget` are nested action payload helpers, not top action roots.

## Game Event Class Tree

```text
eGameEvent
├── eArmyEventBase
│   ├── eArmyReturnEvent
│   ├── ePlayerConquestEventBase
│   │   ├── ePlayerConquestEvent
│   │   ├── ePlayerRaidEvent
│   │   └── eTroopsRequestFulfilledEvent
│   └── eReinforcementsEvent
├── eCityBecomesEvent
├── eEarthquakeEvent
├── eEconomicMilitaryChangeEventBase
│   ├── eEconomicChangeEvent
│   └── eMilitaryChangeEvent
├── eFulfillRequestEvent
├── eGiftToEvent
├── eGodAttackEvent
├── eGodDisasterEvent
├── eGodQuestEventBase
│   ├── eGodQuestEvent
│   └── eGodQuestFulfilledEvent
├── eGodTradeResumesEvent
├── eGodVisitEvent
├── eInvasionEvent
├── eLandSlideEvent
├── eLavaEvent
├── eMonsterInvasionEventBase
│   ├── eMonsterInCityEvent
│   ├── eMonsterInvasionEvent
│   └── eMonsterUnleashedEvent
├── ePayTributeEvent
├── ePriceChangeEvent
├── eReceiveTributeEvent
├── eRequestAidEvent
├── eRequestStrikeEvent
├── eResourceGrantedEventBase
│   ├── eGiftFromEvent
│   ├── eMakeRequestEvent
│   └── eRaidResourceEvent
├── eRivalArmyAwayEvent
├── eSinkLandEvent
├── eSupplyDemandChangeEvent
│   ├── eDemandChangeEvent
│   └── eSupplyChangeEvent
├── eTidalWaveEvent
├── eTradeOpenUpEvent
├── eTradeShutDownEvent
├── eTroopsRequestEvent
└── eWageChangeEvent
```

Event helper payload nodes:

```text
eEventTrigger
eWarning
eAttackingCityEventValue
eCityEventValue
eCountEventValue
eGodEventValue
eGodReasonEventValue
eMonsterEventValue
eMonstersEventValue
ePointEventValue
eResourceEventValue
eResourceGrantedEventValues
eReceiveRequestType
```

`eGameEvents` is the event container.

`eGameBoard::mAllGameEvents` is a registry for ids and lookup.

## Standalone / Embedded Save Nodes

```text
eAvailableBuildings
eDate
eCityFinances
eMilitaryAid
ePlague
eAIBuilding
eAIDistrict
eAICityPlan
eBoardPlayer
eEmploymentDistributor
eReinforcements
eEpisode
├── eParentCityEpisode
└── eColonyEpisode
eEpisodeGoal
eSetAside
eCityRequest
eTributePayment
eGodQuest
```

## Prerequisite Fixes

### P0 — Version Gate

Add a save version at the outer save entry.

Best owner: campaign save file / `eCampaign` path, because `eCampaign` is the outer wrapper.

On load:

- Read version before nested payloads.
- If version is unsupported, print an error.
- Abort load cleanly.
- Do not partially read old positional bytes into live state.

### P1 — Virtual Field Serializer

Make base serializers virtual for polymorphic save roots:

```cpp
virtual void serializeFields(eSaveArchive& ar);
```

Apply to:

- `eBuilding`
- `eCharacter`
- `eCharacterAction`
- `eGameEvent`

Base `read`/`write` open one archive and call the virtual serializer.

Subclass serializers call parent first.

```cpp
void Child::serializeFields(eSaveArchive& ar) {
    Parent::serializeFields(ar);
    ar.field("fieldName", mField, defaultValue);
}
```

### P2 — `payloadField` Must Scan

Current issue:

- `field()` uses `takeField`.
- `payloadField()` uses `readField`.
- `readField` reads only the next field.

Fix:

- Make `payloadField` read path use `takeField(name)`.
- Then `archiveField`, `objectField`, refs, and pointer fields become reorder-safe.

### P3 — Collections Need Item Scope

Current issue:

- `arrayField()` writes count in parent scope.
- Item fields are written directly into parent scope.
- Same item names can collide across arrays.

Fix:

- Keep collection count tagged.
- Wrap each item in a child archive field.
- Example item names: `items.0`, `items.1`, or repeated `item` payloads if `takeField` supports duplicates cleanly.

### P4 — Ref Helpers

Use tagged helpers:

```cpp
ar.buildingAsField("sourceBuilding", &board(), mBuilding);
ar.buildingField("targetBuilding", &board(), mTarget);
ar.characterField("worker", &board(), mWorker);
ar.characterAsField("ox", &board(), mOx);
ar.tileField("targetTile", board(), mTargetTile);
ar.cityField("city", &board(), mCity);
ar.gameEventField("event", &board(), mEvent);
ar.soldierBannerField("banner", &board(), mBanner);
```

Do not call stream `read*` / `write*` directly from save nodes.

### P5 — Object Helpers

Use tagged helpers:

```cpp
ar.objectField("child", child);
ar.archiveField("group", [&](eSaveArchive& groupAr) {
    groupAr.field("enabled", mEnabled, false);
});
```

Do not call `child.serialize(ar)` on a parent archive if the child owns separate framing.

### P6 — Raw API Cleanup

After migration has zero callers:

- Delete deprecated archive raw helpers.
- Remove public raw stream ref APIs where possible.
- Keep raw stream internals only inside archive engine / file stream engine.

## Migration Order

1. Fix archive primitives: version gate, scan-safe payloads, scoped collections.
2. Add virtual field serializer to `eBuilding`, `eCharacter`, `eCharacterAction`, `eGameEvent`.
3. Convert roots and high-risk children first.
4. Remove subclass `read`/`write` only after the virtual serializer path saves child fields.
5. Convert raw refs and objects to tagged helpers.
6. Convert raw arrays and loops to scoped collection helpers.
7. Remove fallback and deprecated raw helpers.
8. Lock raw stream API.

## Area Checklist

### A — Archive Engine

- [x] Add outer save version gate.
- [x] Make `payloadField` use `takeField`.
- [x] Scope `arrayField`.
- [x] Scope `dequeField`.
- [x] Scope `countedArrayField`.
- [x] Verify duplicate field names still work where intended.
- [x] Add debug `printf` for bad version, bad count, and missing required payload.

### B — Board / Campaign Roots

When migrating child classes (`eGameBoard`, episodes, `eSetAside`), make them default-constructible with `setBoard()` / `setWorldBoard()` post-construct wiring. After that, return to `eCampaign::serialize` and strip the remaining `ar.reading()` branches that construct children with ctor args.

- [~] `eCampaign` — done except `reading()` branches for child construction (revisit after children migrated)
- [~] `eWorldBoard` — done except `reading()` for `eWorldCity` construction (revisit after `eWorldCity`)
- [~] `eWorldCity` — done except `reading()` for `mConqueredBy` city ref (revisit when killing reading() branches globally)
- [~] `eGameBoard` — done except `reading()` branches for ctor-required children
- [~] `eBoardCity` — done except `reading()` branches for ctor-required children (plagues/aid/banners/hippodromes)
- [x] `eBoardPlayer`
- [x] `eTile`
- [x] `eAvailableBuildings`
- [x] `eAICityPlan`
- [x] `eAIDistrict`
- [x] `eEmploymentDistributor`
- [x] `ePopulationData` — not a save node (pure runtime, no serialize)
- [x] `eHusbandryData` — not a save node
- [x] `eEmploymentData` — not a save node
- [x] `eCityFinances`
- [~] `eMilitaryAid` — done except `payloadField` for city ref (uses raw stream callback via lambda; OK)
- [x] `ePlague`
- [x] `eReinforcements`
- [~] `eEpisode` — done except `reading()` for `eGameEvent` factory construction
- [x] `eParentCityEpisode` — inherits `eEpisode` serialization
- [x] `eColonyEpisode` — inherits `eEpisode` serialization
- [x] `eEpisodeGoal`
- [x] `eSetAside`
- [x] `eCityRequest`
- [x] `eTributePayment` — not a save node (no read/write)
- [x] `eGodQuest`

### C — Buildings

- [x] `eBuilding`
- [x] `eAestheticsBuilding` — no fields
- [x] `eWaterPark` — mId via serialize
- [x] `eCommemorative` — const mId via ctor branch
- [x] `eGodMonument` — const mGod/mId via ctor branch; mTiles rebuilt via tile backref
- [x] `eGodMonumentTile` — monument backref via writeBuilding/readBuilding
- [x] `eAgoraSpace` — agora ref reattached via eAgoraBase owner
- [x] `eAnimalBuilding` — already migrated (characterField)
- [x] `eAvenue` — no fields
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
- [x] `eHephaestusSanctuary` — inherits, no own read/write
- [x] `ePatrolBuildingBase`
- [x] `eAgoraBase` — no fields (inherits patrol base)
- [x] `ePatrolBuilding` — inherits, no own read/write
- [x] `eStorageBuilding`
- [x] `eTower`
- [x] `eTradePost`
- [x] `eTriremeWharf`
- [x] `eVendor`
- [x] `eHippodromePiece`
- [x] `eHorseRanchEnclosure`
- [x] `eGatehouse` — inherits, no own read/write
- [x] `eHerosHall`
- [x] `eHouseBase`
- [x] `eEliteHousing`
- [x] `eSmallHouse`
- [x] `ePalace`
- [x] `ePalaceTile` — inherits, no own read/write
- [x] `ePark` — inherits, no own read/write
- [x] `ePier` — inherits, no own read/write
- [x] `ePlaceholder` — inherits, no own read/write
- [x] `eResourceBuilding`
- [x] `eRoad`
- [x] `eRuins`
- [x] `eSanctBuilding`
- [x] `ePyramidElement`
- [x] `eTempleAltarBuilding`
- [x] `eTempleBuilding`
- [x] `eTempleMonumentBuilding` — inherits, no own read/write
- [x] `eTempleStatueBuilding` — inherits, no own read/write
- [x] `eTempleTileBuilding` — inherits, no own read/write
- [x] `eWall` — inherits, no own read/write
- [x] `eHippodrome`

### D — Characters

- [x] `eCharacterBase`
- [x] `eCharacter`
- [x] `eAnimal` — no own read/write, inherits eCharacter
- [x] `eArcher` — no own read/write, inherits eCharacter
- [x] `eArtisan` — no own read/write, inherits eCharacter
- [x] `eBasicPatroler` — no own read/write, inherits eCharacter
- [x] `eActor` — inherits eBasicPatroler, no own read/write
- [x] `eAstronomer` — inherits eBasicPatroler, no own read/write
- [x] `eButcher` — inherits eBasicPatroler, no own read/write
- [x] `eCartTransporter`
- [x] `eCompetitor` — inherits eBasicPatroler, no own read/write
- [x] `eCurator` — inherits eBasicPatroler, no own read/write
- [x] `eDonkey` — inherits eBasicPatroler, no own read/write
- [x] `eGymnast` — inherits eBasicPatroler, no own read/write
- [x] `eHealer` — inherits eBasicPatroler, no own read/write
- [x] `eHomeless` — inherits eBasicPatroler, no own read/write
- [x] `eInventor` — inherits eBasicPatroler, no own read/write
- [x] `eOx` — inherits eBasicPatroler, no own read/write
- [x] `ePeddler` — inherits, no own read/write (mAgora ref not saved — pre-existing gap)
- [x] `ePhilosopher` — inherits eBasicPatroler, no own read/write
- [x] `ePorter` — inherits eBasicPatroler, no own read/write
- [x] `eScholar` — inherits eBasicPatroler, no own read/write
- [x] `eSettler`
- [x] `eTaxCollector` — inherits eBasicPatroler, no own read/write
- [x] `eTrader` — inherits, no own read/write (mFollowers donkeys saved as board chars; trader vec rebuilt is pre-existing gap)
- [x] `eWaterDistributor` — inherits eBasicPatroler, no own read/write
- [x] `eBoatBase` — inherits eCharacter, no own read/write
- [x] `eEnemyBoat` — inherits eBoatBase, no own read/write
- [x] `eTradeBoat` — inherits eBoatBase, no own read/write
- [x] `eTrireme` — inherits, no own read/write
- [x] `eCattle`
- [x] `eChariot` — inherits eCharacter, no own read/write
- [x] `eFightingPatroler` — inherits eCharacter, no own read/write
- [x] `eDisgruntled` — inherits eFightingPatroler, no own read/write
- [x] `eEliteCitizen` — inherits eFightingPatroler, no own read/write
- [x] `eSick` — inherits eFightingPatroler, no own read/write
- [x] `eSoldier`
- [x] `eAmazon`
- [x] `eArcherBase` — inherits eSoldier, no own read/write
- [x] `eAresWarrior` — inherits eSoldier, no own read/write
- [x] `eChariotBase` — inherits eSoldier, no own read/write
- [x] `eHopliteBase` — inherits eSoldier, no own read/write
- [x] `eHorsemanBase` — inherits eSoldier, no own read/write
- [x] `eRangeSoldier` — inherits eSoldier, no own read/write
- [x] `eSpearthrowerBase` — inherits eSoldier, no own read/write
- [x] `eWatchman` — inherits eFightingPatroler, no own read/write
- [x] `eFireFighter` — inherits eCharacter, no own read/write
- [x] `eGod`
- [x] `eBasicGod` — inherits eGod, no own read/write
- [x] `eDionysus` — inherits eGod, no own read/write
- [x] `eExtendedGod` — inherits eGod, no own read/write
- [x] `eHermes` — inherits eGod, no own read/write
- [x] `eGrower` — inherits eCharacter, no own read/write (mGrapes/mOlives/mOranges runtime accum, pre-existing gap)
- [x] `eHero` — inherits eCharacter, no own read/write
- [x] `eBasicHero` — inherits eHero, no own read/write
- [x] `eHorse` — inherits eCharacter, no own read/write
- [x] `eMonster` — inherits eCharacter, no own read/write
- [x] `eBasicMonster` — inherits eMonster, no own read/write
- [x] `eWaterMonster` — inherits eMonster, no own read/write
- [x] `eResourceCollectorBase` — inherits eCharacter, no own read/write
- [x] `eFishingBoat` — inherits, no own read/write
- [x] `eResourceCollector` — inherits eResourceCollectorBase, no own read/write
- [x] `eShepherdBase` — inherits eResourceCollectorBase, no own read/write
- [x] `eUrchinGatherer` — inherits eResourceCollectorBase, no own read/write
- [x] `eTrailer`

### E — Character Actions

- [ ] `eCharacterAction`
- [ ] `eBuildAction`
- [ ] `eCollectAction`
- [ ] `eComplexAction`
- [ ] `eActionWithComeback`
- [ ] `eArtisanAction`
- [x] `eCartTransporterAction`
- [x] `eDeliverCartAction`
- [x] `eGetCartAction`
- [x] `eVendorCartAction`
- [ ] `eCollectResourceAction`
- [ ] `eGrowerAction`
- [ ] `eHeroAction`
- [ ] `eHuntAction`
- [ ] `ePatrolAction`
- [ ] `eFireFighterAction`
- [ ] `eReplaceCattleAction`
- [ ] `eSettlerAction`
- [ ] `eShepherdAction`
- [ ] `eSickDisgruntledAction`
- [ ] `eTakeCattleAction`
- [ ] `eTraderAction`
- [ ] `eAnimalAction`
- [ ] `eArcherAction`
- [ ] `eFightingAction`
- [ ] `eSoldierAction`
- [ ] `eTriremeAction`
- [ ] `eGodMonsterAction`
- [ ] `eDefendAttackCityAction`
- [ ] `eAttackCityAction`
- [ ] `eDefendCityAction`
- [ ] `eGodAction`
- [ ] `eAphroditeHelpAction`
- [ ] `eApolloHelpAction`
- [ ] `eAresHelpAction`
- [ ] `eAtlasHelpAction`
- [ ] `eHadesHelpAction`
- [ ] `eHephaestusHelpAction`
- [ ] `eHeraHelpAction`
- [ ] `eHermesHelpAction`
- [ ] `eProvideResourceHelpAction`
- [ ] `eZeusHelpAction`
- [ ] `eGodMonsterActionInd`
- [ ] `eMonsterAction`
- [ ] `eMoveToAction`
- [ ] `eDieAction`
- [ ] `eFightAction`
- [ ] `eMoveAction`
- [ ] `eFollowAction`
- [ ] `eDionysusFollowAction`
- [ ] `eMoveAroundAction`
- [ ] `eMovePathAction`
- [ ] `ePatrolMoveAction`
- [ ] `eWaitAction`
- [ ] `eWalkableObject`
- [ ] `eHasResourceWalkableObject`
- [ ] `eRectWalkableObject`
- [ ] `eHasResourceObject`
- [ ] `eHasNonBusyResourceObject`

### F — Game Events

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

### G — Cleanup

- [ ] Remove subclass `read`/`write` overrides where base virtual serializer covers them.
- [ ] Remove `legacyReadStream()` callers.
- [ ] Remove `readStream()` callers from save nodes.
- [ ] Remove `writeStream()` callers from save nodes.
- [ ] Remove deprecated `ar.tile()`, `ar.character()`, `ar.building()`, `ar.buildingAs()`, `ar.characterAs()`, `ar.object()`, `ar.gameEvent()`, `ar.soldierBanner()`.
- [ ] Lock public raw stream pointer-ref APIs.

## Done Criteria

- [ ] No save node uses raw stream calls.
- [ ] All polymorphic save roots use one archive entry point.
- [ ] All child serializers call parent first.
- [ ] All pointer refs use tagged ref helpers.
- [ ] All collections are tagged and item-scoped.
- [ ] Save load can reject unsupported versions cleanly.
- [ ] New fields load with defaults.
- [ ] Removing fields does not corrupt later fields.
- [ ] Reordering fields does not corrupt later fields.
