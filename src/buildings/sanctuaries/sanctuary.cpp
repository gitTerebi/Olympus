#include "sanctuary.h"

#include "engine/game-board.h"
#include "characters/gods/god.h"
#include "characters/gods/actions/god-worshipped-action.h"
#include "characters/monsters/emonster.h"
#include "characters/actions/monster-action.h"
#include "characters/gods/actions/god-minion-action.h"
#include "etilehelper.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "gameEvents/conquest/player-conquest-event-base.h"

#include "characters/actions/godHelp/eaphroditehelpaction.h"
#include "characters/actions/godHelp/eapollohelpaction.h"
#include "characters/actions/godHelp/eareshelpaction.h"
#include "characters/actions/godHelp/eartemishelpaction.h"
#include "characters/actions/godHelp/eathenahelpaction.h"
#include "characters/actions/godHelp/eatlashelpaction.h"
#include "characters/actions/godHelp/edemeterhelpaction.h"
#include "characters/actions/godHelp/edionysushelpaction.h"
#include "characters/actions/godHelp/ehadeshelpaction.h"
#include "characters/actions/godHelp/eherahelpaction.h"
#include "characters/actions/godHelp/ehephaestushelpaction.h"
#include "characters/actions/godHelp/ehermeshelpaction.h"
#include "characters/actions/godHelp/eposeidonhelpaction.h"
#include "characters/actions/godHelp/ezeushelpaction.h"

#include "gameEvents/gods/egodattackevent.h"
#include "characters/actions/ekillcharacterfinishfail.h"

#include "buildings/eresourcebuilding.h"
#include "buildings/eplaceholder.h"

#include "eartemissanctuary.h"
#include "ehephaestussanctuary.h"
#include "ezeussanctuary.h"
#include "fileIO/esavearchive.h"
#include "etemplealtarbuilding.h"
#include "enumbers.h"

#include "etemplealtarbuilding.h"
#include "enumbers.h"

eSanctuary::eSanctuary(GameBoard& board,
                       const eBuildingType type,
                       const int sw, const int sh,
                       const int maxEmployees,
                       const eCityId cid) :
    eMonument(board, type, sw, sh,
              maxEmployees, cid) {
    GameTextures::loadSanctuary();
    sLoadMonumentTextures(godType());
    board.registerSanctuary(this);
}

eSanctuary::~eSanctuary() {
    auto& board = ownerBoard();
    board.unregisterSanctuary(this);
    board.destroyed(cityId(), type());
}

void eSanctuary::erase() {
    if(mMinion) mMinion->kill();
    const auto& board = getBoard();
    for(const auto s : mSpecialTiles) {
        const auto ub = s->underBuilding();
        if(!ub) continue;
        const auto ubt = ub->type();
        switch(ubt) {
        case eBuildingType::placeholder: {
            const auto p = static_cast<ePlaceholder*>(ub);
            p->sanctuaryErase();
        } break;
        case eBuildingType::oliveTree:
        case eBuildingType::vine:
        case eBuildingType::orangeTree: {
            const auto r = static_cast<eResourceBuilding*>(ub);
            r->sanctuaryErase();
        } break;
        default:
            break;
        }
    }
    const auto g = godType();
    if(g == GodType::hephaestus ||
       g == GodType::hades ||
       g == GodType::demeter) {
        const auto cid = cityId();
        const auto c = board.boardCityWithId(cid);
        if(c) c->incTerrainState();
    }
    eMonument::erase();
}

GodType eSanctuary::godType() const {
    const auto bt = type();
    switch(bt) {
    case eBuildingType::templeAphrodite: return GodType::aphrodite;
    case eBuildingType::templeApollo: return GodType::apollo;
    case eBuildingType::templeAres: return GodType::ares;
    case eBuildingType::templeArtemis: return GodType::artemis;
    case eBuildingType::templeAthena: return GodType::athena;
    case eBuildingType::templeAtlas: return GodType::atlas;
    case eBuildingType::templeDemeter: return GodType::demeter;
    case eBuildingType::templeDionysus: return GodType::dionysus;
    case eBuildingType::templeHades: return GodType::hades;
    case eBuildingType::templeHephaestus: return GodType::hephaestus;
    case eBuildingType::templeHera: return GodType::hera;
    case eBuildingType::templeHermes: return GodType::hermes;
    case eBuildingType::templePoseidon: return GodType::poseidon;
    case eBuildingType::templeZeus: return GodType::zeus;
    default: return GodType::aphrodite;
    }
}

God* eSanctuary::spawnGod() {
    auto& board = getBoard();
    const auto c = God::sCreateGod(godType(), board);
    c->setBothCityIds(cityId());
    c->setAttitude(GodAttitude::worshipped);
    mGod = c.get();
    const auto ct = centerTile();
    const int tx = ct->x();
    const int ty = ct->y();
    const auto cr = eTileHelper::closestRoad(tx, ty, board);
    if(!cr) return nullptr;
    mGod->changeTile(cr);
    return c.get();
}

void eSanctuary::spawnPatrolingGod() {
    const auto c = spawnGod();
    if(!c) return;
    const auto ha = e::make_shared<GodWorshippedAction>(c);
    mGod->setAction(ha);
    mSpawnWait = 5000;
}

void eSanctuary::spawnDefenderMinion() {
    auto& board = getBoard();
    const auto mt = eMonster::sGodsMinion(godType());
    const auto m = eMonster::sCreateMonster(mt, board);
    if(!m) return;
    m->setBothCityIds(cityId());
    const auto ct = centerTile();
    const auto cr = eTileHelper::closestRoad(ct->x(), ct->y(), board);
    if(!cr) return;
    m->changeTile(cr);
    const auto a = e::make_shared<eGodMinionAction>(m.get());
    m->setAction(a);
    mMinion = m.get();
    mMinionSpawnWait = 5000;
}

void eSanctuary::buildingProgressed() {
    const bool f = finished();
    if(f) {
        const auto cid = cityId();
        auto& board = getBoard();
        const auto c = board.boardCityWithId(cid);
        if(c) c->monumentFinished();
        const auto g = godType();
        for(const auto s : mSpecialTiles) {
            const auto ub = s->underBuilding();
            if(ub) ub->erase();
            const auto build = [&](const eResourceBuildingType type) {
                const auto b = e::make_shared<eResourceBuilding>(
                            board, type, cid);
                b->setSanctuary(true);
                b->setCenterTile(s);
                b->setTileRect({s->x(), s->y(), 1, 1});
                s->setUnderBuilding(b);
                b->addUnderBuilding(s);
            };
            switch(g) {
            case GodType::hephaestus:
                s->setTerrain(eTerrain::copper);
                break;
            case GodType::hades:
                s->setTerrain(eTerrain::silver);
                break;
            case GodType::athena:
                build(eResourceBuildingType::oliveTree);
                break;
            case GodType::dionysus:
                build(eResourceBuildingType::vine);
                break;
            case GodType::hera:
                build(eResourceBuildingType::orangeTree);
                break;
            default:
                break;
            }
        }

        if(g == GodType::demeter) {
            const auto& sanctRect = tileRect();
            const int xMin = sanctRect.x - 3;
            const int yMin = sanctRect.y - 3;
            const int xMax = sanctRect.x + sanctRect.w + 3;
            const int yMax = sanctRect.y + sanctRect.h + 3;
            for(int x = xMin; x < xMax; x++) {
                for(int y = yMin; y < yMax; y++) {
                    const SDL_Point pt{x, y};
                    const bool in = SDL_PointInRect(&pt, &sanctRect);
                    if(in) continue;
                    const auto tile = board.tile(x, y);
                    if(!tile) continue;
                    const auto terr = tile->terrain();
                    if(terr == eTerrain::dry ||
                        terr == eTerrain::forest ||
                        terr == eTerrain::choppedForest) {
                        tile->setTerrain(eTerrain::fertile);
                    }
                }
            }
        }

        if(g == GodType::hephaestus ||
           g == GodType::hades ||
           g == GodType::demeter) {
            const auto c = board.boardCityWithId(cid);
            if(c) c->incTerrainState();
        }

        eEventData ed(cid);
        ed.fGod = g;
        ed.fTile = centerTile();
        board.event(eEvent::sanctuaryComplete, ed);

        if(!mMinion) spawnDefenderMinion();
    }
}

void eSanctuary::godComeback() {
    mGodAbroad = false;
}

void eSanctuary::sendAresAbroad() {
    mGodAbroad = true;
    if(mGod) {
        const auto a = e::make_shared<eAresHelpAction>(mGod.get());
        mGod->setAction(a);
    }
}

double eSanctuary::helpTimeFraction() const {
    return double(mHelpTimer)/eNumbers::sGodHelpPeriod;
}

double eSanctuary::helpAttackTimeFraction() const {
    auto& board = getBoard();
    const auto pid = playerId();
    const auto p = board.boardPlayerWithId(pid);
    const int pTimer = p->godAttackTimer();
    const double v1 = double(mHelpTimer)/eNumbers::sGodHelpAttackPeriod;
    const double v2 = double(pTimer)/eNumbers::sGodHelpAttackPlayerPeriod;
    return std::min(v1, v2);
}

stdsptr<eSanctuary> eSanctuary::sCreate(
        const eBuildingType type,
        const int sw, const int sh,
        GameBoard& board,
        const eCityId cid) {
    switch(type) {
    case eBuildingType::templeAphrodite:
        return e::make_shared<eAphroditeSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeApollo:
        return e::make_shared<eApolloSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeAres:
        return e::make_shared<eAresSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeArtemis:
        return e::make_shared<eArtemisSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeAthena:
        return e::make_shared<eAthenaSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeAtlas:
        return e::make_shared<eAtlasSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeDemeter:
        return e::make_shared<eDemeterSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeDionysus:
        return e::make_shared<eDionysusSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeHades:
        return e::make_shared<eHadesSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeHephaestus:
        return e::make_shared<eHephaestusSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeHera:
        return e::make_shared<eHeraSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeHermes:
        return e::make_shared<eHermesSanctuary>(sw, sh, board, cid);
    case eBuildingType::templePoseidon:
        return e::make_shared<ePoseidonSanctuary>(sw, sh, board, cid);
    case eBuildingType::templeZeus:
        return e::make_shared<eZeusSanctuary>(sw, sh, board, cid);
    default:
        return nullptr;
    }
}

void eSanctuary::sLoadMonumentTextures(const GodType type) {
    switch(type) {
    case GodType::aphrodite:
        return GameTextures::loadAphroditeMonuments();
    case GodType::apollo:
        return GameTextures::loadApolloMonuments();
    case GodType::ares:
        return GameTextures::loadAresMonuments();
    case GodType::artemis:
        return GameTextures::loadArtemisMonuments();
    case GodType::athena:
        return GameTextures::loadAthenaMonuments();
    case GodType::atlas:
        return GameTextures::loadAtlasMonuments();
    case GodType::demeter:
        return GameTextures::loadDemeterMonuments();
    case GodType::dionysus:
        return GameTextures::loadDionysusMonuments();
    case GodType::hades:
        return GameTextures::loadHadesMonuments();
    case GodType::hephaestus:
        return GameTextures::loadHephaestusMonuments();
    case GodType::hera:
        return GameTextures::loadHeraMonuments();
    case GodType::hermes:
        return GameTextures::loadHermesMonuments();
    case GodType::poseidon:
        return GameTextures::loadPoseidonMonuments();
    case GodType::zeus:
        return GameTextures::loadZeusMonuments();
    }
}

void eSanctuary::setSpawnWait(const int w) {
    mSpawnWait = w;
}

bool eSanctuary::sacrificing() const {
    for(const auto& e : mElements) {
        const auto type = e->type();
        if(type != eBuildingType::templeAltar) continue;
        const auto altar = static_cast<eTempleAltarBuilding*>(e.get());
        if(altar->sacrificing()) return true;
    }
    return false;
}

bool eSanctuary::priestOut() const {
    for(const auto& e : mElements) {
        if(e->type() != eBuildingType::templeAltar) continue;
        const auto altar = static_cast<eTempleAltarBuilding*>(e.get());
        if(altar->priestOut()) return true;
    }
    return false;
}

int eSanctuary::sacrificeDaysLeft() const {
    int best = 0;
    for(const auto& e : mElements) {
        if(e->type() != eBuildingType::templeAltar) continue;
        const auto altar = static_cast<eTempleAltarBuilding*>(e.get());
        best = std::max(best, altar->sacrificeDaysLeft());
    }
    return best;
}

void eSanctuary::registerElement(const stdsptr<eSanctBuilding>& e) {
    eMonument::registerElement(e);
    if(e->type() == eBuildingType::templeAltar) {
        const auto altar = static_cast<eTempleAltarBuilding*>(e.get());
        altar->setOnSacrificeComplete([this]() { boostHelpTimer(); });
    }
}

void eSanctuary::boostHelpTimer() {
    // each completed sacrifice adds 15 days worth of prayer progress
    mHelpTimer += 15 * eNumbers::sDayLength;
}

void eSanctuary::timeChanged(const int by) {
    mHelpTimer += by;
    eMonument::timeChanged(by);

    if(!mGod && !mGodAbroad && finished()) {
        mSpawnWait -= by;
        if(mSpawnWait <= 0) {
            spawnPatrolingGod();
        }
    }

    if(!mMinion && finished()) {
        mMinionSpawnWait -= by;
        if(mMinionSpawnWait <= 0) {
            spawnDefenderMinion();
        }
    }

    if(mAskedForHelp) {
        const int checkInterval = 1000;
        mCheckHelpNeeded += by;
        if(mCheckHelpNeeded > checkInterval) {
            mCheckHelpNeeded -= checkInterval;
            eHelpDenialReason r;
            askForHelp(r);
        }
    }
}

void eSanctuary::nextMonth() {
    eEmployingBuilding::nextMonth();
    const bool person = isPersonPlayer();
    if(person) return;
    if(!finished()) return;
    eHelpDenialReason reason;
    if(eRand::rand() % 5 == 0) {
        const bool r = askForHelp(reason);
        mAskedForHelp = false;
        if(r) return;
    }
    const auto& board = getBoard();
    auto cids = board.citiesOnBoard();
    std::random_shuffle(cids.begin(), cids.end());
    const auto thisTid = teamId();
    for(const auto cid : cids) {
        const auto tid = board.cityIdToTeamId(cid);
        if(tid == eTeamId::neutralFriendly) continue;
        if(tid == eTeamId::neutralAggresive) continue;
        if(tid == thisTid) continue;
        const auto c = board.boardCityWithId(cid);
        const auto& gods = c->attackingGods();
        if(!gods.empty()) continue;
        askForAttack(cid, reason);
        break;
    }
}

void eSanctuary::serializeFields(eSaveArchive& ar) {
    eMonument::serializeFields(ar);
    auto& board = getBoard();
    ar.characterAsField("god", &board, mGod);
    ar.characterAsField("minion", &board, mMinion);
    ar.field("spawnWait", mSpawnWait);
    ar.field("minionSpawnWait", mMinionSpawnWait);
    ar.field("godAbroad", mGodAbroad);

    ar.field("askedForHelp", mAskedForHelp);
    ar.field("checkHelpNeeded", mCheckHelpNeeded);
    ar.field("helpTimer", mHelpTimer);
    ar.field("aresBuffReady", mAresBuffReady, false);

    const int nw = ar.writing() ? static_cast<int>(mWarriorTiles.size()) : 0;
    if(ar.reading()) mWarriorTiles.clear();
    ar.countedArrayField("warriorTiles", nw,
        [this, &board](eSaveArchive& itemAr, const int i) {
            eTile* t = itemAr.writing() ? mWarriorTiles[i] : nullptr;
            itemAr.tileField("tile", board, t);
            if(itemAr.reading()) mWarriorTiles.push_back(t);
        });

    const int ns = ar.writing() ? static_cast<int>(mSpecialTiles.size()) : 0;
    if(ar.reading()) mSpecialTiles.clear();
    ar.countedArrayField("specialTiles", ns,
        [this, &board](eSaveArchive& itemAr, const int i) {
            eTile* t = itemAr.writing() ? mSpecialTiles[i] : nullptr;
            itemAr.tileField("tile", board, t);
            if(itemAr.reading()) mSpecialTiles.push_back(t);
        });
}

std::vector<eTile*> eSanctuary::warriorTiles() const {
    return mWarriorTiles;
}

void eSanctuary::addWarriorTile(eTile* const t) {
    mWarriorTiles.push_back(t);
}

void eSanctuary::addSpecialTile(eTile* const t) {
    mSpecialTiles.push_back(t);
}

bool eSanctuary::askForAttack(const eCityId cid, eHelpDenialReason& reason) {
    auto& board = getBoard();
    const auto pid = playerId();
    const auto p = board.boardPlayerWithId(pid);
    const int pTimer = p->godAttackTimer();
    if(mGodAbroad || mHelpTimer < eNumbers::sGodHelpAttackPeriod ||
       pTimer < eNumbers::sGodHelpAttackPlayerPeriod) {
        reason = eHelpDenialReason::tooSoon;
        return false;
    }
    mHelpTimer = 0;
    p->resetGodAttackTimer();
    const auto ee = eGameEvent::sCreate(cid,
                                        eGameEventType::godAttack,
                                        eGameEventBranch::root,
                                        board);

    const auto e = ee->ref<eGodAttackEvent>();
    e->setSanctuary(this);
    e->setTypes({godType()});
    e->setRepeat(0);
    e->trigger();
    const auto c = board.boardCityWithId(cid);
    c->addRootGameEvent(e);
    mGodAbroad = true;
    if(mGod) {
        const auto a = mGod->action();
        if(const auto gma = dynamic_cast<GodMonsterAction*>(a)) {
            using eKill = eKillCharacterFinishFail;
            const auto finish = std::make_shared<eKill>(board, mGod.get());
            gma->disappear(false, finish);
        } else {
            mGod->kill();
        }
    }
    if(isPersonPlayer()) {
        eSounds::playGodSound(godType(), eGodSound::invade);
    }
    return true;
}

bool eSanctuary::askForHelp(eHelpDenialReason& reason) {
    if(mGodAbroad || mHelpTimer < eNumbers::sGodHelpPeriod) {
        reason = eHelpDenialReason::tooSoon;
        return false;
    }
    auto& board = getBoard();
    const auto type = godType();
    const auto cid = cityId();
    bool r = false;
    switch(type) {
    case GodType::aphrodite: {
        r = eAphroditeHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::apollo: {
        r = eApolloHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::ares: {
        // Ares prayer always succeeds — buff stored, consumed on conquest dispatch
        mHelpTimer = 0;
        mAresBuffReady = true;
        mAskedForHelp = false;
        return true;
    }
    case GodType::artemis: {
        r = eArtemisHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::athena: {
        r = eAthenaHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::atlas: {
        r = eAtlasHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::demeter: {
        r = eDemeterHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::dionysus: {
        r = eDionysusHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::hades: {
        r = eHadesHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::hera: {
        r = eHeraHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::hephaestus: {
        r = eHephaestusHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::hermes: {
        const auto pid = board.cityIdToPlayerId(cid);
        r = eHermesHelpAction::sHelpNeeded(pid, board);
    } break;
    case GodType::poseidon: {
        r = ePoseidonHelpAction::sHelpNeeded(cid, board);
    } break;
    case GodType::zeus: {
        r = eZeusHelpAction::sHelpNeeded(cid, board);
    } break;
    }
    if(!r) {
        mAskedForHelp = true;
        reason = eHelpDenialReason::noTarget;
        return false;
    }
    stdsptr<eCharacterAction> a;
    eCharacter* c = nullptr;
    if(const auto g = god()) {
        const auto tile = g->tile();
        if(tile) {
            c = g;
        } else {
            c = spawnGod();
        }
    } else {
        c = spawnGod();
    }
    if(!c) {
        reason = eHelpDenialReason::error;
        return false;
    }
    switch(type) {
    case GodType::aphrodite:
        a = e::make_shared<eAphroditeHelpAction>(c);
        break;
    case GodType::apollo:
        a = e::make_shared<eApolloHelpAction>(c);
        break;
    case GodType::ares:
        a = e::make_shared<eAresHelpAction>(c);
        break;
    case GodType::artemis:
        a = e::make_shared<eArtemisHelpAction>(c);
        break;
    case GodType::athena:
        a = e::make_shared<eAthenaHelpAction>(c);
        break;
    case GodType::atlas:
        a = e::make_shared<eAtlasHelpAction>(c);
        break;
    case GodType::demeter:
        a = e::make_shared<eDemeterHelpAction>(c);
        break;
    case GodType::dionysus:
        a = e::make_shared<eDionysusHelpAction>(c);
        break;
    case GodType::hades:
        a = e::make_shared<eHadesHelpAction>(c);
        break;
    case GodType::hera:
        a = e::make_shared<eHeraHelpAction>(c);
        break;
    case GodType::hephaestus:
        a = e::make_shared<eHephaestusHelpAction>(c);
        break;
    case GodType::hermes:
        a = e::make_shared<eHermesHelpAction>(c);
        break;
    case GodType::poseidon:
        a = e::make_shared<ePoseidonHelpAction>(c);
        break;
    case GodType::zeus:
        a = e::make_shared<eZeusHelpAction>(c);
        break;
    }
    if(!a) {
        reason = eHelpDenialReason::error;
        return false;
    }
    mHelpTimer = 0;
    mAskedForHelp = false;
    c->setAction(a);
    eEventData ed(cityId());
    ed.fGod = type;
    ed.fChar = c;
    ed.fTile = c->tile();
    board.event(eEvent::godHelp, ed);
    if(type == GodType::ares) {
        mAresBuffReady = true;
    }
    return true;
}
