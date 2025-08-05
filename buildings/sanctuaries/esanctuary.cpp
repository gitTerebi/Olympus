#include "esanctuary.h"

#include "characters/ecarttransporter.h"
#include "engine/egameboard.h"
#include "characters/gods/egod.h"
#include "characters/actions/egodworshippedaction.h"
#include "etilehelper.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "gameEvents/eplayerconquesteventbase.h"

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

#include "gameEvents/egodattackevent.h"
#include "characters/actions/ekillcharacterfinishfail.h"

#include "buildings/eresourcebuilding.h"
#include "buildings/eplaceholder.h"

#include "eartemissanctuary.h"
#include "ehephaestussanctuary.h"
#include "ezeussanctuary.h"

#include "etemplealtarbuilding.h"
#include "enumbers.h"

eSanctuary::eSanctuary(eGameBoard& board,
                       const eBuildingType type,
                       const int sw, const int sh,
                       const int maxEmployees,
                       const eCityId cid) :
    eMonument(board, type, sw, sh,
              maxEmployees, cid) {
    eGameTextures::loadSanctuary();
    switch(godType()) {
    case eGodType::aphrodite:
        eGameTextures::loadAphroditeMonuments();
        break;
    case eGodType::apollo:
        eGameTextures::loadApolloMonuments();
        break;
    case eGodType::ares:
        eGameTextures::loadAresMonuments();
        break;
    case eGodType::artemis:
        eGameTextures::loadArtemisMonuments();
        break;
    case eGodType::athena:
        eGameTextures::loadAthenaMonuments();
        break;
    case eGodType::atlas:
        eGameTextures::loadAtlasMonuments();
        break;
    case eGodType::demeter:
        eGameTextures::loadDemeterMonuments();
        break;
    case eGodType::dionysus:
        eGameTextures::loadDionysusMonuments();
        break;
    case eGodType::hades:
        eGameTextures::loadHadesMonuments();
        break;
    case eGodType::hephaestus:
        eGameTextures::loadHephaestusMonuments();
        break;
    case eGodType::hera:
        eGameTextures::loadHeraMonuments();
        break;
    case eGodType::hermes:
        eGameTextures::loadHermesMonuments();
        break;
    case eGodType::poseidon:
        eGameTextures::loadPoseidonMonuments();
        break;
    case eGodType::zeus:
        eGameTextures::loadZeusMonuments();
        break;
    }
    board.registerSanctuary(this);
}

eSanctuary::~eSanctuary() {
    auto& board = getBoard();
    board.unregisterSanctuary(this);
}

void eSanctuary::erase() {
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
    if(g == eGodType::hephaestus ||
       g == eGodType::hades ||
       g == eGodType::demeter) {
        const auto cid = cityId();
        const auto c = board.boardCityWithId(cid);
        if(c) c->incTerrainState();
    }
    eMonument::erase();
}

eGodType eSanctuary::godType() const {
    const auto bt = type();
    switch(bt) {
    case eBuildingType::templeAphrodite: return eGodType::aphrodite;
    case eBuildingType::templeApollo: return eGodType::apollo;
    case eBuildingType::templeAres: return eGodType::ares;
    case eBuildingType::templeArtemis: return eGodType::artemis;
    case eBuildingType::templeAthena: return eGodType::athena;
    case eBuildingType::templeAtlas: return eGodType::atlas;
    case eBuildingType::templeDemeter: return eGodType::demeter;
    case eBuildingType::templeDionysus: return eGodType::dionysus;
    case eBuildingType::templeHades: return eGodType::hades;
    case eBuildingType::templeHephaestus: return eGodType::hephaestus;
    case eBuildingType::templeHera: return eGodType::hera;
    case eBuildingType::templeHermes: return eGodType::hermes;
    case eBuildingType::templePoseidon: return eGodType::poseidon;
    case eBuildingType::templeZeus: return eGodType::zeus;
    default: return eGodType::aphrodite;
    }
}

eGod* eSanctuary::spawnGod() {
    auto& board = getBoard();
    const auto c = eGod::sCreateGod(godType(), board);
    c->setBothCityIds(cityId());
    c->setAttitude(eGodAttitude::worshipped);
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
    const auto ha = e::make_shared<eGodWorshippedAction>(c);
    mGod->setAction(ha);
    mSpawnWait = 5000;
}

void eSanctuary::buildingProgressed() {
    const bool f = finished();
    if(f) {
        auto& board = getBoard();
        const auto g = godType();
        for(const auto s : mSpecialTiles) {
            const auto ub = s->underBuilding();
            if(ub) ub->erase();
            const auto build = [&](const eResourceBuildingType type) {
                const auto b = e::make_shared<eResourceBuilding>(
                            board, type, cityId());
                b->setSanctuary(true);
                b->setCenterTile(s);
                b->setTileRect({s->x(), s->y(), 1, 1});
                s->setUnderBuilding(b);
                b->addUnderBuilding(s);
            };
            switch(g) {
            case eGodType::hephaestus:
                s->setTerrain(eTerrain::copper);
                break;
            case eGodType::hades:
                s->setTerrain(eTerrain::silver);
                break;
            case eGodType::athena:
                build(eResourceBuildingType::oliveTree);
                break;
            case eGodType::dionysus:
                build(eResourceBuildingType::vine);
                break;
            case eGodType::hera:
                build(eResourceBuildingType::orangeTree);
                break;
            case eGodType::demeter: {
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
            } break;
            default:
                break;
            }
        }

        if(g == eGodType::hephaestus ||
           g == eGodType::hades ||
           g == eGodType::demeter) {
            const auto cid = cityId();
            const auto c = board.boardCityWithId(cid);
            if(c) c->incTerrainState();
        }

        eEventData ed(cityId());
        ed.fGod = g;
        board.event(eEvent::sanctuaryComplete, ed);
    }
}

void eSanctuary::godComeback() {
    mGodAbroad = false;
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
        eGameBoard& board,
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

void eSanctuary::timeChanged(const int by) {
    mHelpTimer += by;
    eMonument::timeChanged(by);

    if(!mGod && !mGodAbroad && finished()) {
        mSpawnWait -= by;
        if(mSpawnWait <= 0) {
            spawnPatrolingGod();
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

void eSanctuary::read(eReadStream& src) {
    eMonument::read(src);

    auto& board = getBoard();

    src.readCharacter(&board, [this](eCharacter* const c) {
        mGod = static_cast<eGod*>(c);
    });
    src >> mSpawnWait;
    src >> mGodAbroad;

    src >> mAskedForHelp;
    src >> mCheckHelpNeeded;
    src >> mHelpTimer;

    int nw;
    src >> nw;
    for(int i = 0; i < nw; i++) {
        const auto t = src.readTile(board);
        mWarriorTiles.push_back(t);
    }

    int ns;
    src >> ns;
    for(int i = 0; i < ns; i++) {
        const auto t = src.readTile(board);
        mSpecialTiles.push_back(t);
    }
}

void eSanctuary::write(eWriteStream& dst) const {
    eMonument::write(dst);

    dst.writeCharacter(mGod);
    dst << mSpawnWait;
    dst << mGodAbroad;

    dst << mAskedForHelp;
    dst << mCheckHelpNeeded;
    dst << mHelpTimer;

    dst << mWarriorTiles.size();
    for(const auto t : mWarriorTiles) {
        dst.writeTile(t);
    }

    dst << mSpecialTiles.size();
    for(const auto t : mSpecialTiles) {
        dst.writeTile(t);
    }
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
    const int delay = 5;
    e->setDatePlusDays(delay);
    e->setDatePlusMonths(0);
    e->setDatePlusYears(0);
    auto date = board.date();
    date += delay;
    e->initializeDate(date);
    const auto c = board.boardCityWithId(cid);
    c->addRootGameEvent(e);
    mGodAbroad = true;
    if(mGod) {
        const auto a = mGod->action();
        if(const auto gma = dynamic_cast<eGodMonsterAction*>(a)) {
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
    case eGodType::aphrodite: {
        r = eAphroditeHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::apollo: {
        r = eApolloHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::ares: {
        const auto pid = board.cityIdToPlayerId(cid);
        r = eAresHelpAction::sHelpNeeded(pid, board);
    } break;
    case eGodType::artemis: {
        r = eArtemisHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::athena: {
        r = eAthenaHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::atlas: {
        r = eAtlasHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::demeter: {
        r = eDemeterHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::dionysus: {
        r = eDionysusHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::hades: {
        r = eHadesHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::hera: {
        r = eHeraHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::hephaestus: {
        r = eHephaestusHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::hermes: {
        const auto pid = board.cityIdToPlayerId(cid);
        r = eHermesHelpAction::sHelpNeeded(pid, board);
    } break;
    case eGodType::poseidon: {
        r = ePoseidonHelpAction::sHelpNeeded(cid, board);
    } break;
    case eGodType::zeus: {
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
    case eGodType::aphrodite:
        a = e::make_shared<eAphroditeHelpAction>(c);
        break;
    case eGodType::apollo:
        a = e::make_shared<eApolloHelpAction>(c);
        break;
    case eGodType::ares:
        a = e::make_shared<eAresHelpAction>(c);
        break;
    case eGodType::artemis:
        a = e::make_shared<eArtemisHelpAction>(c);
        break;
    case eGodType::athena:
        a = e::make_shared<eAthenaHelpAction>(c);
        break;
    case eGodType::atlas:
        a = e::make_shared<eAtlasHelpAction>(c);
        break;
    case eGodType::demeter:
        a = e::make_shared<eDemeterHelpAction>(c);
        break;
    case eGodType::dionysus:
        a = e::make_shared<eDionysusHelpAction>(c);
        break;
    case eGodType::hades:
        a = e::make_shared<eHadesHelpAction>(c);
        break;
    case eGodType::hera:
        a = e::make_shared<eHeraHelpAction>(c);
        break;
    case eGodType::hephaestus:
        a = e::make_shared<eHephaestusHelpAction>(c);
        break;
    case eGodType::hermes:
        a = e::make_shared<eHermesHelpAction>(c);
        break;
    case eGodType::poseidon:
        a = e::make_shared<ePoseidonHelpAction>(c);
        break;
    case eGodType::zeus:
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
    if(type == eGodType::ares) {
        const auto cid = cityId();
        const auto pid = board.cityIdToPlayerId(cid);
        const auto& cs = board.conquests(pid);
        if(cs.empty()) return true;
        cs[0]->addAres();
        mGodAbroad = true;
    }
    return true;
}
