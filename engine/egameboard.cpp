
#include "egameboard.h"

#include <random>

#include "buildings/eagorabase.h"
#include "characters/echaracter.h"
#include "buildings/ebuilding.h"
#include "spawners/espawner.h"

#include "buildings/estoragebuilding.h"

#include "characters/esoldier.h"
#include "characters/esoldierbanner.h"

#include "buildings/sanctuaries/esanctbuilding.h"
#include "buildings/etradepost.h"

#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"

#include "buildings/epalace.h"
#include "buildings/epalacetile.h"

#include "buildings/ehorseranch.h"

#include "engine/boardData/eheatmaptask.h"

#include "missiles/emissile.h"

#include "etilehelper.h"

#include "emessages.h"

#include "spawners/emonsterpoint.h"
#include "spawners/elandinvasionpoint.h"

#include "buildings/eheatgetters.h"

#include "fileIO/ebuildingreader.h"
#include "fileIO/ebuildingwriter.h"

#include "eevent.h"
#include "emessageeventtype.h"

#include "gameEvents/egodvisitevent.h"
#include "gameEvents/egodattackevent.h"
#include "gameEvents/emonsterunleashedevent.h"
#include "gameEvents/einvasionevent.h"
#include "gameEvents/epaytributeevent.h"
#include "gameEvents/emakerequestevent.h"
#include "gameEvents/egifttoevent.h"
#include "gameEvents/egiftfromevent.h"
#include "gameEvents/ereceiverequestevent.h"
#include "gameEvents/erequestaidevent.h"
#include "gameEvents/eplayerconquesteventbase.h"
#include "gameEvents/etroopsrequestevent.h"

#include "eeventdata.h"

#include "einvasionhandler.h"
#include "characters/actions/emonsteraction.h"

#include "evectorhelpers.h"
#include "egifthelpers.h"
#include "estringhelpers.h"

#include "buildings/eheroshall.h"
#include "buildings/emuseum.h"
#include "buildings/estadium.h"
#include "buildings/eagoraspace.h"
#include "buildings/evendor.h"
#include "buildings/eroad.h"
#include "buildings/egatehouse.h"
#include "eplague.h"
#include "audio/emusic.h"

#include "ecampaign.h"
#include "eiteratesquare.h"
#include "ecolonymonumentaction.h"
#include "textures/emarbletile.h"
#include "elanguage.h"
#include "enumbers.h"

#include "characters/actions/eanimalaction.h"
#include "buildings/eanimalbuilding.h"

#include "buildings/eplaceholder.h"
#include "buildings/sanctuaries/ezeussanctuary.h"
#include "buildings/sanctuaries/ehephaestussanctuary.h"
#include "buildings/sanctuaries/eartemissanctuary.h"
#include "buildings/sanctuaries/estairsrenderer.h"
#include "buildings/sanctuaries/etempletilebuilding.h"
#include "buildings/sanctuaries/etemplestatuebuilding.h"
#include "buildings/sanctuaries/etemplemonumentbuilding.h"
#include "buildings/sanctuaries/etemplealtarbuilding.h"
#include "buildings/sanctuaries/etemplebuilding.h"

eGameBoard::eGameBoard() :
    mThreadPool(*this) {
    const auto types = eResourceTypeHelpers::extractResourceTypes(
                           eResourceType::allBasic);
    for(const auto type : types) {
        mPrices[type] = eResourceTypeHelpers::defaultPrice(type);
    }
}

eGameBoard::~eGameBoard() {
    mRegisterBuildingsEnabled = false;
    clear();
}

void eGameBoard::initialize(const int w, const int h) {
    waitUntilFinished();
    mThreadPool.initialize(w, h);

    clear();
    mTiles.reserve(w);
    for(int x = 0; x < w; x++) {
        auto& yArr = mTiles.emplace_back();
        yArr.reserve(h);
        for(int y = 0; y < h; y++) {
            int tx;
            int ty;
            eTileHelper::dtileIdToTileId(x, y, tx, ty);
            const auto tile = new eTile(tx, ty, x, y);
            yArr.push_back(tile);
        }
    }
    mWidth = w;
    mHeight = h;

    mAppealMap.initialize(0, 0, w, h);

    updateNeighbours();
//    updateMarbleTiles();
    scheduleTerrainUpdate();
    for(const auto& c : mCitiesOnBoard) {
        c->clearTiles();
    }
}

void eGameBoard::resize(const int w, const int h) {
    waitUntilFinished();
    mThreadPool.initialize(w, h);

    mTiles.reserve(w);

    for(int x = w; x < mWidth; x++) {
        mTiles.pop_back();
    }
    for(int x = 0; x < w; x++) {
        auto& yArr = mTiles[x];
        for(int y = h; y < mHeight; y++) {
            yArr.pop_back();
        }
    }

    for(int x = 0; x < w; x++) {
        auto& yArr = x < mWidth ? mTiles[x] : mTiles.emplace_back();
        yArr.reserve(h);
        for(int y = 0; y < h; y++) {
            if(x < mWidth && y < mHeight) continue;
            int tx;
            int ty;
            eTileHelper::dtileIdToTileId(x, y, tx, ty);
            const auto tile = new eTile(tx, ty, x, y);
            yArr.push_back(tile);
        }
    }
    mWidth = w;
    mHeight = h;

    mAppealMap.initialize(0, 0, w, h);

    updateNeighbours();
//    updateMarbleTiles();
    scheduleTerrainUpdate();
    updateTerritoryBorders();
}

void eGameBoard::clear() {
    for(const auto c : mCharacters) {
        c->kill();
    }
    emptyRubbish();
    for(const auto& x : mTiles) {
        for(const auto y : x) {
            delete y;
        }
    }
    mTiles.clear();
    mWidth = 0;
    mHeight = 0;
    emptyRubbish();
    mCitiesOnBoard.clear();
    mPlayersOnBoard.clear();
    mDefeatedBy.clear();
    mEarthquakes.clear();
    mGoals.clear();
    emptyRubbish();
}

void eGameBoard::setWorldDirection(const eWorldDirection dir) {
    mDirection = dir;
    scheduleTerrainUpdate();
}

eTile* eGameBoard::rotateddtile(const int x, const int y) const {
    int rx;
    int ry;
    eTileHelper::rotatedDTileIdToDTileId(
            x, y, rx, ry, mDirection, mWidth, mHeight);
    return dtile(rx, ry);
}

int eGameBoard::rotatedWidth() const {
    if(mDirection == eWorldDirection::N) {
        return mWidth;
    } else if(mDirection == eWorldDirection::E) {
        return mHeight/2;
    } else if(mDirection == eWorldDirection::S) {
        return mWidth;
    } else { // if(mDirection == eWorldDirection::W) {
        return mHeight/2;
    }}

int eGameBoard::rotatedHeight() const {
    if(mDirection == eWorldDirection::N) {
        return mHeight;
    } else if(mDirection == eWorldDirection::E) {
        return 2*mWidth;
    } else if(mDirection == eWorldDirection::S) {
        return mHeight;
    } else { // if(mDirection == eWorldDirection::W) {
        return 2*mWidth;
    }
}

void eGameBoard::iterateOverAllTiles(const eTileAction& a) {
    const int height = rotatedHeight();
    const int width = rotatedWidth();
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            const auto t = rotateddtile(x, y);
            if(!t) continue;
            a(t);
        }
    }
}

void eGameBoard::scheduleAppealMapUpdate(const eCityId cid) {
    if(mEditorMode) return;
    mUpdateAppeal[cid].fV = true;
}

void eGameBoard::updateAppealMapIfNeeded() {
    for(auto& c : mUpdateAppeal) {
        bool& fV = c.second.fV;
        if(!fV) continue;
        fV = false;
        const auto cid = c.first;
        const auto finish = [this, cid](eHeatMap& map) {
            const auto c = boardCityWithId(cid);
            const auto& tiles = c->tiles();
            for(const auto t : tiles) {
                const int dx = t->dx();
                const int dy = t->dy();
                const bool e = map.enabled(dx, dy);
                const double h = map.heat(dx, dy);
                mAppealMap.set(dx, dy, e, h);
            }
        };
        const auto cc = boardCityWithId(cid);
        const auto rect = cc->tileBRect();
        const auto task = new eHeatMapTask(cid, rect,
                                           eHeatGetters::appeal, finish);
        mThreadPool.queueTask(task);
    }
}

void eGameBoard::enlistForces(const eEnlistedForces& forces) {
    for(const auto& b : forces.fSoldiers) {
        b->goAbroad();
    }
    const auto cids = citiesOnBoard();
    for(const auto h : forces.fHeroes) {
        eHerosHall* hh = nullptr;
        for(const auto cid : cids) {
            hh = heroHall(cid, h);
            if(hh) break;
        }
        if(!hh) continue;
        hh->sendHeroOnQuest();
    }
    for(const auto& a : forces.fAllies) {
        a->setAbroad(true);
    }
}

void eGameBoard::clearBannerSelection() {
    for(const auto s : mSelectedBanners) {
        s->setSelected(false);
    }
    mSelectedBanners.clear();
}

void eGameBoard::deselectBanner(eSoldierBanner* const c) {
    eVectorHelpers::remove(mSelectedBanners, c);
    c->setSelected(false);
}

void eGameBoard::selectBanner(eSoldierBanner* const c) {
    mSelectedBanners.push_back(c);
    c->setSelected(true);
}

void eGameBoard::bannersGoHome() {
    for(const auto b : mSelectedBanners) {
        b->goHome();
    }
}

void eGameBoard::bannersBackFromHome() {
    for(const auto b : mSelectedBanners) {
        b->backFromHome();
    }
}

void eGameBoard::setRegisterBuildingsEnabled(const bool e) {
    mRegisterBuildingsEnabled = e;
}

void eGameBoard::setButtonsVisUpdater(const eAction& u) {
    mButtonVisUpdater = u;
}

eBuilding* eGameBoard::buildingAt(const int x, const int y) const {
    const auto t = tile(x, y);
    if(!t) return nullptr;
    return t->underBuilding();
}

bool eGameBoard::isShutDown(const eCityId cid,
                            const eResourceType type) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->isShutDown(type);
}

bool eGameBoard::isShutDown(const eCityId cid,
                            const eBuildingType type) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->isShutDown(type);
}

std::vector<eBuilding*> eGameBoard::buildings(
        const eCityId cid, const eBuildingValidator& v) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->buildings(v);
}

std::vector<eBuilding*> eGameBoard::buildings(
        const eCityId cid, const eBuildingType type) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->buildings(type);
}

int eGameBoard::countBuildings(
        const eCityId cid, const eBuildingValidator& v) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->countBuildings(v);
}

int eGameBoard::countBuildings(
        const eCityId cid, const eBuildingType t) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->countBuildings(t);
}

int eGameBoard::hasBuilding(
        const eCityId cid, const eBuildingType t) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->hasBuilding(t);
}

int eGameBoard::countAllowed(
        const eCityId cid, const eBuildingType t) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->countAllowed(t);
}

eBuilding* eGameBoard::randomBuilding(
        const eCityId cid, const eBuildingValidator& v) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->randomBuilding(v);
}

std::vector<eBuilding*> eGameBoard::commemorativeBuildings(
        const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->commemorativeBuildings();
}

bool eGameBoard::supportsBuilding(const eCityId cid,
                                  const eBuildingMode mode) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->supportsBuilding(mode);
}

bool eGameBoard::availableBuilding(const eCityId cid,
                                   const eBuildingType type,
                                   const int id) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->availableBuilding(type, id);
}

void eGameBoard::built(const eCityId cid,
                       const eBuildingType type,
                       const int id) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->built(type, id);
}

void eGameBoard::destroyed(const eCityId cid,
                           const eBuildingType type,
                           const int id) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->destroyed(type, id);
}

void eGameBoard::allow(const eCityId cid,
                       const eBuildingType type,
                       const int id) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->allow(type, id);
}

void eGameBoard::disallow(const eCityId cid,
                          const eBuildingType type,
                          const int id) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->disallow(type, id);
}

void eGameBoard::updateButtonsVisibility() {
    if(mButtonVisUpdater) mButtonVisUpdater();
}

bool eGameBoard::supportsResource(
        const eCityId cid, const eResourceType rt) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->supportsResource(rt);
}

eResourceType eGameBoard::supportedResources(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return eResourceType::none;
    return c->supportedResources();
}

int eGameBoard::wonGames(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->wonGames();
}

int eGameBoard::horses() const {
    int h = 0;
    for(const auto b : mTimedBuildings) {
        if(b->type() != eBuildingType::horseRanch) continue;
        const auto r = static_cast<eHorseRanch*>(b);
        h += r->horseCount();
    }
    return h;
}

void eGameBoard::planAction(ePlannedAction* const a) {
    mPlannedActions.emplace_back(a);
}

void eGameBoard::restockMarbleTiles() {
    for(const auto& mt : mMarbleTiles) {
        mt.restock();
    }
}

bool eGameBoard::eMarbleTiles::contains(eTile* const tile) const {
    return eVectorHelpers::contains(fTiles, tile);
}

void eGameBoard::eMarbleTiles::add(eTile* const tile) {
    fTiles.push_back(tile);
}

void eGameBoard::eMarbleTiles::addWithNeighbours(eTile* const tile) {
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            const auto n = tile->tileRel<eTile>(x, y);
            const auto terr = n->terrain();
            if(terr != eTerrain::marble) return;
            const bool c = contains(n);
            if(c) continue;
            add(n);
            addWithNeighbours(n);
        }
    }
}

void eGameBoard::eMarbleTiles::restock() const {
    int maxLevel = 0;
    for(const auto t : fTiles) {
        if(t->resource() > 0) return;
        const int l = t->marbleLevel();
        if(l > maxLevel) maxLevel = l;
    }
    for(const auto t : fTiles) {
        const int l = t->marbleLevel();
        if(l == maxLevel) {
            if(maxLevel == 2) {
                t->setResource(99999);
            } else {
                const bool e = eMarbleTile::edge(t);
                if(!e) t->setResource(1);
            }
        }
    }
}

void eGameBoard::updateMarbleTiles() {
    mMarbleTiles.clear();
    iterateOverAllTiles([&](eTile* const t) {
        const auto terr = t->terrain();
        if(terr != eTerrain::marble) return;
        for(const auto& mt : mMarbleTiles) {
            const bool c = mt.contains(t);
            if(c) return;
        }
        auto& mt = mMarbleTiles.emplace_back();
        mt.addWithNeighbours(t);
    });
}

void eGameBoard::restockBlackMarbleTiles() {
    for(const auto& mt : mMarbleTiles) {
        mt.restock();
    }
}

void eGameBoard::updateBlackMarbleTiles() {
    mBlackMarbleTiles.clear();
    iterateOverAllTiles([&](eTile* const t) {
        const auto terr = t->terrain();
        if(terr != eTerrain::blackMarble) return;
        for(const auto& mt : mBlackMarbleTiles) {
            const bool c = mt.contains(t);
            if(c) return;
        }
        auto& mt = mBlackMarbleTiles.emplace_back();
        mt.addWithNeighbours(t);
    });
}

void eGameBoard::setFriendlyGods(const eCityId cid,
                                 const std::vector<eGodType>& gods) {
    for(const auto g : gods) {
        eBuildingType bt;
        switch(g) {
        case eGodType::aphrodite:
            bt = eBuildingType::templeAphrodite;
            break;
        case eGodType::apollo:
            bt = eBuildingType::templeApollo;
            break;
        case eGodType::ares:
            bt = eBuildingType::templeAres;
            break;
        case eGodType::artemis:
            bt = eBuildingType::templeArtemis;
            break;
        case eGodType::athena:
            bt = eBuildingType::templeAthena;
            break;
        case eGodType::atlas:
            bt = eBuildingType::templeAtlas;
            break;
        case eGodType::demeter:
            bt = eBuildingType::templeDemeter;
            break;
        case eGodType::dionysus:
            bt = eBuildingType::templeDionysus;
            break;
        case eGodType::hades:
            bt = eBuildingType::templeHades;
            break;
        case eGodType::hephaestus:
            bt = eBuildingType::templeHephaestus;
            break;
        case eGodType::hera:
            bt = eBuildingType::templeHera;
            break;
        case eGodType::hermes:
            bt = eBuildingType::templeHermes;
            break;
        case eGodType::poseidon:
            bt = eBuildingType::templePoseidon;
            break;
        case eGodType::zeus:
            bt = eBuildingType::templeZeus;
            break;
        default:
            bt = eBuildingType::none;
            break;
        }

        allow(cid, bt);
    }

    const auto e = e::make_shared<eGodVisitEvent>(
                       cid, eGameEventBranch::root, *this);
    e->setIsEpisodeEvent(true);
    eDate date = mDate;
    const int period = eNumbers::sFriendlyGodVisitPeriod;
    date += period;
    date += eRand::rand() % 60;
    e->initializeDate(date, period, 10000);
    e->setTypes(gods);
    addRootGameEvent(e);
}

void eGameBoard::allowHero(const eCityId cid, const eHeroType heroType,
                           const std::string& reason) {
    const auto hallType = eHerosHall::sHeroTypeToHallType(heroType);
    allow(cid, hallType);
    const auto& inst = eMessages::instance;
    const auto hm = inst.heroMessages(heroType);
    if(!hm) return;
    eEventData ed(cid);
    auto msg = hm->fHallAvailable;
    eStringHelpers::replaceAll(msg.fFull.fText, "[reason_phrase]",
                               reason.empty() ? msg.fNoReason : reason);
    showMessage(ed, msg);
}

eBuilding* eGameBoard::buildingWithIOID(const int id) const {
    if(id == -1) return nullptr;
    for(const auto b : mAllBuildings) {
        const int bio = b->ioID();
        if(bio == id) return b;
    }
    return nullptr;
}

eCharacter* eGameBoard::characterWithIOID(const int id) const {
    if(id == -1) return nullptr;
    for(const auto c : mCharacters) {
        const int bio = c->ioID();
        if(bio == id) return c;
    }
    return nullptr;
}

eCharacterAction* eGameBoard::characterActionWithIOID(const int id) const {
    if(id == -1) return nullptr;
    for(const auto ca : mCharacterActions) {
        const int bio = ca->ioID();
        if(bio == id) return ca;
    }
    return nullptr;
}

eBanner* eGameBoard::bannerWithIOID(const int id) const {
    if(id == -1) return nullptr;
    for(const auto b : mBanners) {
        const int bio = b->ioID();
        if(bio == id) return b;
    }
    return nullptr;
}

eSoldierBanner* eGameBoard::soldierBannerWithIOID(const int id) const {
    if(id == -1) return nullptr;
    for(const auto b : mAllSoldierBanners) {
        const int bio = b->ioID();
        if(bio == id) return b;
    }
    return nullptr;
}

eGameEvent* eGameBoard::eventWithIOID(const int id) const {
    if(id == -1) return nullptr;
    for(const auto e : mAllGameEvents) {
        const int eio = e->ioID();
        if(eio == id) return e;
    }
    return nullptr;
}

eTile* eGameBoard::monsterTile(const eCityId cid, const int id) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->monsterTile(id);
}

eTile* eGameBoard::landInvasionTile(const eCityId cid, const int id) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->landInvasionTile(id);
}

eTile* eGameBoard::disasterTile(const eCityId cid, const int id) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->disasterTile(id);
}

std::vector<eInvasionHandler*> eGameBoard::invasionHandlers(
        const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->invasionHandlers();
}

void eGameBoard::addInvasionHandler(const eCityId cid,
                                    eInvasionHandler* const i) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->addInvasionHandler(i);
    updateMusic();
}

void eGameBoard::removeInvasionHandler(const eCityId cid,
                                       eInvasionHandler* const i) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->removeInvasionHandler(i);
    updateMusic();
}

bool eGameBoard::hasActiveInvasions(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->hasActiveInvasions();
}

int eGameBoard::addResource(const eCityId cid,
                            const eResourceType type,
                            const int count) {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->addResource(type, count);
}

int eGameBoard::spaceForResource(const eCityId cid,
                                 const eResourceType type) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->spaceForResource(type);
}

int eGameBoard::maxSingleSpaceForResource(
        const eCityId cid,
        const eResourceType type,
        eStorageBuilding** b) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->maxSingleSpaceForResource(type, b);
}

int eGameBoard::maxSanctuarySpaceForResource(
        const eCityId cid,
        eSanctuary** b) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->maxSanctuarySpaceForResource(b);
}

void eGameBoard::planGiftFrom(const stdsptr<eWorldCity>& c,
                              const eResourceType type,
                              const int count,
                              const int delay) {
    const auto e = e::make_shared<eGiftFromEvent>(
                       personPlayerCapital(),
                       eGameEventBranch::root, *this);
    e->initialize(true, type, count, c);
    const auto date = mDate + delay;
    e->initializeDate(date);
    addRootGameEvent(e);
}

void eGameBoard::request(const stdsptr<eWorldCity>& c,
                         const eResourceType type,
                         const eCityId cid) {
    const auto e = e::make_shared<eMakeRequestEvent>(
                       cid, eGameEventBranch::root, *this);
    e->initialize(true, type, c);
    const auto date = mDate + 90;
    e->initializeDate(date);
    addRootGameEvent(e);

    const auto pid = cityIdToPlayerId(cid);
    const auto& cts = mWorldBoard->cities();
    for(const auto& ct : cts) {
        if(ct->isCurrentCity()) continue;
        ct->incAttitude(-10, pid);
    }
    c->incAttitude(-10, pid);
}

void eGameBoard::requestAid(const stdsptr<eWorldCity>& c) {
    const auto e = e::make_shared<eRequestAidEvent>(
                       personPlayerCapital(),
                       eGameEventBranch::root, *this);
    e->setCity(c);
    const auto date = mDate + 30;
    e->initializeDate(date);
    addRootGameEvent(e);
}

void eGameBoard::tributeFrom(const ePlayerId pid,
                             const stdsptr<eWorldCity>& c,
                             const bool postpone) {
    const auto type = c->tributeType();
    const int count = c->tributeCount();

    eEventData ed(pid);
    ed.fType = eMessageEventType::requestTributeGranted;
    ed.fCity = c;
    if(type == eResourceType::drachmas) {
        ed.fA0 = [this, c, count, pid]() { // accept
            const auto p = boardPlayerWithId(pid);
            if(p) p->incDrachmas(count);
            return count;
        };
    } else {
        const auto cids = playerCitiesOnBoard(pid);
        for(const auto cid : cids) {
            ed.fCSpaceCount[cid] = spaceForResource(cid, type);
            ed.fCityNames[cid] = cityName(cid);
            ed.fCCA0[cid] = [this, cid, c, type, count, pid]() { // accept
                const int a = addResource(cid, type, count);
                if(a == count) return;
                eEventData ed(pid);
                ed.fType = eMessageEventType::resourceGranted;
                ed.fCity = c;
                ed.fResourceType = type;
                ed.fResourceCount = a;
                event(eEvent::tributeAccepted, ed);
            };
        }
    }
    ed.fResourceType = type;
    ed.fResourceCount = count;
    if(postpone) {
        ed.fA1 = [this, c, type, count, pid]() { // postpone
            eEventData ed(pid);
            ed.fType = eMessageEventType::resourceGranted;
            ed.fCity = c;
            ed.fResourceType = type;
            ed.fResourceCount = count;
            event(eEvent::tributePostponed, ed);

            const auto e = e::make_shared<ePayTributeEvent>(
                               personPlayerCapital(),
                               eGameEventBranch::root, *this);
            e->initialize(c);
            const auto date = mDate + 31;
            e->initializeDate(date);
            addRootGameEvent(e);
        };
    }
    ed.fA2 = [this, c, type, count, pid]() { // decline
        eEventData ed(pid);
        ed.fType = eMessageEventType::resourceGranted;
        ed.fCity = c;
        ed.fResourceType = type;
        ed.fResourceCount = count;
        event(eEvent::tributeDeclined, ed);
    };
    event(eEvent::tributePaid, ed);
}

bool eGameBoard::giftTo(const stdsptr<eWorldCity>& c,
                        const eResourceType type,
                        const int count,
                        const eCityId cid) {
    const auto cc = boardCityWithId(cid);
    const int has = cc->resourceCount(type);
    if(has < count) return false;
    cc->takeResource(type, count);
    const auto e = e::make_shared<eGiftToEvent>(
                       cid, eGameEventBranch::root, *this);
    e->initialize(c, type, count);
    const auto date = mDate + 90;
    e->initializeDate(date);
    addRootGameEvent(e);
    return true;
}

void eGameBoard::giftToReceived(const stdsptr<eWorldCity>& c,
                                const eResourceType type,
                                const int count,
                                const ePlayerId pid) {
    const bool a = c->acceptsGift(type, count);
    eEventData ed(pid);
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = c;
    ed.fResourceType = type;
    ed.fResourceCount = count;
    if(a) {
        const int mult = count/eGiftHelpers::giftCount(type);
        const bool b = c->buys(type);
        const bool s = c->sells(type);
        if(type == eResourceType::drachmas) {
            event(eEvent::giftReceivedDrachmas, ed);
            c->incAttitude(3*mult, pid);
        } else if(b) {
            event(eEvent::giftReceivedNeeded, ed);
            c->incAttitude(3*mult, pid);
        } else if(s) {
            event(eEvent::giftReceivedSells, ed);
            c->incAttitude(1.5*mult, pid);
        } else {
            event(eEvent::giftReceivedNotNeeded, ed);
            c->incAttitude(1.5*mult, pid);
        }
        c->gifted(type, count);
    } else {
        event(eEvent::giftReceivedRefuse, ed);
    }
}

void eGameBoard::waitUntilFinished() {
    while(!mThreadPool.finished()) {
        mThreadPool.waitFinished();
        mThreadPool.handleFinished();
    }
}

void eGameBoard::addFulfilledQuest(const ePlayerId pid, const eGodQuest q) {
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->addFulfilledQuest(q);
}

void eGameBoard::addSlayedMonster(const ePlayerId pid, const eMonsterType m) {
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->addSlayedMonster(m);
}

std::vector<eGodQuest> eGameBoard::fulfilledQuests(const ePlayerId pid) const {
    const auto p = boardPlayerWithId(pid);
    if(!p) return {};
    return p->fulfilledQuests();
}

std::vector<eMonsterType> eGameBoard::slayedMonsters(const ePlayerId pid) const {
    const auto p = boardPlayerWithId(pid);
    if(!p) return {};
    return p->slayedMonsters();
}

bool eGameBoard::wasHeroSummoned(const eCityId cid, const eHeroType hero) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->wasHeroSummoned(hero);
}

void eGameBoard::heroSummoned(const eCityId cid, const eHeroType hero) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->heroSummoned(hero);
}

int eGameBoard::price(const eResourceType type) const {
    const auto it = mPrices.find(type);
    if(it == mPrices.end()) return 0;
    return it->second;
}

void eGameBoard::incPrice(const eResourceType type, const int by) {
    mPrices[type] += by;
}

void eGameBoard::changeWage(const int per) {
    const double mult = 1 + per/100.;
    mWageMultiplier *= mult;
}

void eGameBoard::updateTerritoryBorders() {
    iterateOverAllTiles([](eTile* const tile) {
        tile->updateTerritoryBorder();
    });
    for(const auto& c : mCitiesOnBoard) {
        c->updateTiles();
    }
}

void eGameBoard::assignAllTerritory(const eCityId cid) {
    iterateOverAllTiles([cid](eTile* const tile) {
        tile->setCityId(cid);
    });
    updateTerritoryBorders();
}

std::vector<eCityId> eGameBoard::personPlayerCitiesOnBoard() const {
    return playerCitiesOnBoard(personPlayer());
}

ePlayerId eGameBoard::cityIdToPlayerId(const eCityId cid) const {
    return mWorldBoard->cityIdToPlayerId(cid);
}

eTeamId eGameBoard::cityIdToTeamId(const eCityId cid) const {
    const auto pid = cityIdToPlayerId(cid);
    return playerIdToTeamId(pid);
}

eTeamId eGameBoard::playerIdToTeamId(const ePlayerId pid) const {
    return mWorldBoard->playerIdToTeamId(pid);
}

void eGameBoard::moveCityToPlayer(const eCityId cid, const ePlayerId pid) {
    const auto oldPid = cityIdToPlayerId(cid);
    if(oldPid == ePlayerId::neutralAggresive ||
       oldPid == ePlayerId::neutralFriendly) {
        const auto c = boardCityWithId(cid);
        mActiveCitiesOnBoard.push_back(c);
        c->acquired();
    }
    return mWorldBoard->moveCityToPlayer(cid, pid);
}

std::vector<eCityId> eGameBoard::playerCities(const ePlayerId pid) const {
    return mWorldBoard->playerCities(pid);
}

eCityId eGameBoard::playerCapital(const ePlayerId pid) const {
    return mWorldBoard->playerCapital(pid);
}

std::vector<eCityId> eGameBoard::playerCitiesOnBoard(const ePlayerId pid) const {
    std::vector<eCityId> result;
    for(const auto& c : mCitiesOnBoard) {
        const auto cid = c->id();
        const auto ppid = cityIdToPlayerId(cid);
        if(pid == ppid) {
            result.push_back(cid);
        }
    }
    return result;
}

ePlayerId eGameBoard::personPlayer() const {
    return mWorldBoard->personPlayer();
}

eCityId eGameBoard::personPlayerCapital() const {
    const auto ppid = personPlayer();
    return playerCapital(ppid);
}

eBoardCity* eGameBoard::boardCityWithId(const eCityId cid) const {
    for(const auto& c : mCitiesOnBoard) {
        if(c->id() == cid) return c.get();
    }
    return nullptr;
}

SDL_Rect eGameBoard::boardCityTileBRect(const eCityId cid) const {
    for(const auto& c : mCitiesOnBoard) {
        if(c->id() == cid) return c->tileBRect();
    }
    return SDL_Rect{0, 0, 0, 0};
}

eBoardPlayer* eGameBoard::boardPlayerWithId(const ePlayerId pid) const {
    for(const auto& p : mPlayersOnBoard) {
        if(p->id() == pid) return p.get();
    }
    return nullptr;
}

std::vector<eCityId> eGameBoard::citiesOnBoard() const {
    std::vector<eCityId> result;
    for(const auto& c : mCitiesOnBoard) {
        result.push_back(c->id());
    }
    return result;
}

std::vector<ePlayerId> eGameBoard::playersOnBoard() const {
    std::vector<ePlayerId> result;
    for(const auto& c : mCitiesOnBoard) {
        const auto cid = c->id();
        const auto pid = cityIdToPlayerId(cid);
        if(pid == ePlayerId::neutralFriendly ||
           pid == ePlayerId::neutralAggresive) continue;
        if(eVectorHelpers::contains(result, pid)) continue;
        result.push_back(pid);
    }
    return result;
}

std::string eGameBoard::cityName(const eCityId cid) const {
    return mWorldBoard->cityName(cid);
}

std::vector<eCityId> eGameBoard::allyCidsNotOnBoard(const ePlayerId pid) const {
    std::vector<eCityId> result;
    const auto tid = playerIdToTeamId(pid);
    const auto& cities = mWorldBoard->cities();
    for(const auto& c : cities) {
        const bool onBoard = c->isOnBoard();
        if(onBoard) continue;
        const auto cid = c->cityId();
        const auto cpid = cityIdToPlayerId(cid);
        if(cpid == pid) continue;
        const auto ctid = playerIdToTeamId(cpid);
        if(tid != ctid) continue;
        result.push_back(cid);
    }
    return result;
}

std::vector<eCityId> eGameBoard::enemyCidsOnBoard(const eTeamId ptid) const {
    std::vector<eCityId> result;
    for(const auto c : mActiveCitiesOnBoard) {
        const auto cid = c->id();
        const auto ctid = cityIdToTeamId(cid);
        if(ctid == eTeamId::neutralFriendly) continue;
        if(ctid == eTeamId::neutralAggresive) continue;
        if(ctid == ptid) continue;
        result.push_back(cid);
    }
    return result;
}

void eGameBoard::updatePlayersOnBoard() {
    const auto ps = mPlayersOnBoard;
    for(const auto& p : ps) {
        const auto pid = p->id();
        const auto cids = playerCitiesOnBoard(pid);
        if(cids.empty()) removePlayerFromBoard(pid);
    }
    for(const auto& c : mCitiesOnBoard) {
        const auto cid = c->id();
        const auto pid = cityIdToPlayerId(cid);
        if(pid == ePlayerId::neutralAggresive ||
           pid == ePlayerId::neutralFriendly) continue;
        const auto p = boardPlayerWithId(pid);
        if(p) continue;
        addPlayerToBoard(pid);
    }
}

eBoardPlayer* eGameBoard::addPlayerToBoard(const ePlayerId pid) {
    const auto p = std::make_shared<eBoardPlayer>(pid, *this);
    mPlayersOnBoard.push_back(p);
    return p.get();
}

void eGameBoard::removePlayerFromBoard(const ePlayerId pid) {
    for(auto it = mPlayersOnBoard.begin(); it < mPlayersOnBoard.end(); it++) {
        const auto p = it->get();
        if(p->id() != pid) continue;
        mPlayersOnBoard.erase(it);
        return;
    }
}

eBoardCity* eGameBoard::addCityToBoard(const eCityId cid) {
    const auto c = std::make_shared<eBoardCity>(cid, *this);
    mCitiesOnBoard.push_back(c);
    const auto pid = cityIdToPlayerId(cid);
    if(pid != ePlayerId::neutralFriendly &&
       pid != ePlayerId::neutralAggresive) {
        mActiveCitiesOnBoard.push_back(c.get());
    }
    return c.get();
}

void eGameBoard::removeCityFromBoard(const eCityId cid) {
    for(auto it = mActiveCitiesOnBoard.begin(); it < mActiveCitiesOnBoard.end(); it++) {
        const auto c = *it;
        if(c->id() != cid) continue;
        mActiveCitiesOnBoard.erase(it);
        break;
    }
    for(auto it = mCitiesOnBoard.begin(); it < mCitiesOnBoard.end(); it++) {
        const auto c = it->get();
        if(c->id() != cid) continue;
        mCitiesOnBoard.erase(it);
        return;
    }
}

void eGameBoard::killCommonFolks(const eCityId cid, int toKill) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->killCommonFolks(toKill);
}

void eGameBoard::walkerKilled(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->walkerKilled();
}

void eGameBoard::rockThrowerKilled(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->rockThrowerKilled();
}

void eGameBoard::hopliteKilled(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->hopliteKilled();
}

void eGameBoard::horsemanKilled(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->horsemanKilled();
}

eEnlistedForces eGameBoard::getEnlistableForces(const ePlayerId pid) const {
    eEnlistedForces result;

    const auto cids = playerCitiesOnBoard(pid);
    for(const auto cid : cids) {
        const auto c = boardCityWithId(cid);
        const auto e = c->getEnlistableForces();
        result.add(e);
    }

    const auto& cts = mWorldBoard->cities();
    for(const auto& c : cts) {
        if(!c->active()) continue;
        const auto cpid = c->playerId();
        if(c->isOnBoard() && cpid == pid) continue;
        const auto type = c->type();
        const auto rel = c->relationship();
        const bool e = type == eCityType::colony ||
                       (type == eCityType::foreignCity &&
                        (rel == eForeignCityRelationship::ally ||
                         rel == eForeignCityRelationship::vassal));
        if(e && c->attitude(pid) > 50) {
            result.fAllies.push_back(c);
        }
    }

    return result;
}

eGameBoard::eQuests eGameBoard::godQuests(const ePlayerId pid) const {
    const auto p = boardPlayerWithId(pid);
    if(!p) return {};
    return p->godQuests();
}

void eGameBoard::addGodQuest(eGodQuestEvent* const q) {
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->addGodQuest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::removeGodQuest(eGodQuestEvent* const q) {
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->removeGodQuest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

eGameBoard::eRequests eGameBoard::cityRequests(const ePlayerId pid) const {
    const auto p = boardPlayerWithId(pid);
    if(!p) return {};
    return p->cityRequests();
}

void eGameBoard::addCityRequest(eReceiveRequestEvent* const q) {
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->addCityRequest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::removeCityRequest(eReceiveRequestEvent* const q) {
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->removeCityRequest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

eGameBoard::eTroopsRequests eGameBoard::cityTroopsRequests(const ePlayerId pid) const {
    const auto p = boardPlayerWithId(pid);
    if(!p) return {};
    return p->cityTroopsRequests();
}

void eGameBoard::addCityTroopsRequest(eTroopsRequestEvent* const q) {
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->addCityTroopsRequest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::removeCityTroopsRequest(eTroopsRequestEvent* const q) {
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->removeCityTroopsRequest(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

eGameBoard::eConquests eGameBoard::conquests(const ePlayerId pid) const {
    const auto p = boardPlayerWithId(pid);
    if(!p) return {};
    return p->conquests();
}

void eGameBoard::addConquest(ePlayerConquestEventBase* const q) {
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->addConquest(q);
}

void eGameBoard::removeConquest(ePlayerConquestEventBase* const q) {
    const auto cid = q->cityId();
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->removeConquest(q);
}

eInvasionEvent* eGameBoard::invasionToDefend(const eCityId cid) const {
    const auto date = eGameBoard::date();
    for(const auto i : mInvasions) {
        const auto icid = i->cityId();
        if(icid != cid) continue;
        const int ip = i->invasionPoint();
        const auto t = landInvasionTile(cid, ip);
        if(!t) continue;
        const auto sDate = i->nextDate();
        if(sDate - date < 120) {
            return i;
        }
    }
    return nullptr;
}

void eGameBoard::addInvasion(eInvasionEvent* const i) {
    mInvasions.push_back(i);
}

void eGameBoard::removeInvasion(eInvasionEvent* const i) {
    eVectorHelpers::remove(mInvasions, i);
}

eGameBoard::eArmyEvents eGameBoard::armyEvents() const {
    return mArmyEvents;
}

void eGameBoard::addArmyEvent(eArmyEventBase* const q) {
    mArmyEvents.push_back(q);
}

void eGameBoard::removeArmyEvent(eArmyEventBase* const q) {
    eVectorHelpers::remove(mArmyEvents, q);
}

std::vector<eMonster*> eGameBoard::monsters(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->monsters();
}

eGameBoard::eChars eGameBoard::attackingGods(
        const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->attackingGods();
}

void eGameBoard::registerAttackingGod(const eCityId cid,
                                      eCharacter* const c) {
    const auto cc = boardCityWithId(cid);
    if(!c) return;
    cc->registerAttackingGod(c);
    updateMusic();
}

void eGameBoard::startPlague(eSmallHouse* const h) {
    if(!h) return;
    const auto cid = h->cityId();
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->startPlague(h);
    eEventData ed(cid);
    ed.fTile = h->centerTile();
    event(eEvent::plague, ed);
}

stdsptr<ePlague> eGameBoard::plagueForHouse(eSmallHouse* const h) {
    if(!h) return nullptr;
    const auto cid = h->cityId();
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->plagueForHouse(h);
}

void eGameBoard::healPlague(const stdsptr<ePlague>& p) {
    if(!p) return;
    const auto cid = p->cityId();
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->healPlague(p);
}

void eGameBoard::healHouse(eSmallHouse* const h) {
    const auto p = plagueForHouse(h);
    if(p) {
        if(p->houseCount() == 1) healPlague(p);
        else p->healHouse(h);
    } else {
        h->setPlague(false);
    }
}

eGameBoard::ePlagues eGameBoard::plagues(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->plagues();
}

stdsptr<ePlague> eGameBoard::nearestPlague(
        const eCityId cid,
        const int tx, const int ty,
        int& dist) const {
    dist = __INT_MAX__/2;
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->nearestPlague(tx, ty, dist);
}

void eGameBoard::updateMusic() {
    bool battle = false;
    const auto cids = personPlayerCitiesOnBoard();
    for(const auto cid : cids) {
        const auto c = boardCityWithId(cid);
        if(!c) continue;
        const bool i = c->hasActiveInvasions();
        if(i) {
            battle = true;
            break;
        }
        const bool a = !c->attackingGods().empty();
        if(a) {
            battle = true;
            break;
        }

        for(const auto m : c->monsters()) {
            const auto a = m->action();
            if(const auto ma = dynamic_cast<eMonsterAction*>(a)) {
                const auto stage = ma->stage();
                if(stage == eMonsterAttackStage::none ||
                   stage == eMonsterAttackStage::wait) {
                    continue;
                }
                battle = true;
                break;
            }
        }
        if(battle) break;
    }
    if(battle) {
        eMusic::playRandomBattleMusic();
    } else {
        eMusic::playRandomMusic();
    }
}

eTile* eGameBoard::entryPoint(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->entryPoint();
}

eTile* eGameBoard::exitPoint(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->exitPoint();
}

void eGameBoard::editorClearBuildings() {
    for(const auto& c : mCitiesOnBoard) {
        c->editorClearBuildings();
    }
}

void eGameBoard::editorDisplayBuildings() {
    for(const auto& c : mCitiesOnBoard) {
        c->editorDisplayBuildings();
    }
}

void eGameBoard::saveEditorCityPlan() {
    for(const auto& c : mCitiesOnBoard) {
        c->saveEditorCityPlan();
    }
}

void eGameBoard::setCurrentDistrictId(const int id) {
    mCurrentDistrictId = id;
    for(const auto& c : mCitiesOnBoard) {
        c->setCurrentDistrictId(id);
    }
}

bool eGameBoard::atlantean(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->atlantean();
}

bool eGameBoard::setAtlantean(const eCityId cid, const bool a) {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    c->setAtlantean(a);
    return true;
}

void eGameBoard::setWorldBoard(eWorldBoard* const wb) {
    mWorldBoard = wb;
}

void eGameBoard::registerSoldierBanner(const stdsptr<eSoldierBanner>& b) {
    if(b->militaryAid()) return;
    const auto cid = b->cityId();
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->registerSoldierBanner(b);
}

bool eGameBoard::unregisterSoldierBanner(const stdsptr<eSoldierBanner>& b) {
    eVectorHelpers::remove(mSelectedBanners, b.get());
    const auto cid = b->cityId();
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->unregisterSoldierBanner(b);
}

void eGameBoard::addRootGameEvent(const stdsptr<eGameEvent>& e) {
    const auto cid = e->cityId();
    const auto c = boardCityWithId(cid);
    c->addRootGameEvent(e);
}

void eGameBoard::removeRootGameEvent(const stdsptr<eGameEvent>& e) {
    const auto cid = e->cityId();
    const auto c = boardCityWithId(cid);
    c->removeRootGameEvent(e);
}

void eGameBoard::addGameEvent(eGameEvent* const e) {
    mAllGameEvents.push_back(e);
}

void eGameBoard::removeGameEvent(eGameEvent* const e) {
    eVectorHelpers::remove(mAllGameEvents, e);
}

void eGameBoard::handleGamesBegin(const eGames game) {
    eGameMessages* msgs = nullptr;
    switch(game) {
    case eGames::isthmian:
        msgs = &eMessages::instance.fIsthmianGames;
        break;
    case eGames::nemean:
        msgs = &eMessages::instance.fNemeanGames;
        break;
    case eGames::pythian:
        msgs = &eMessages::instance.fPythianGames;
        break;
    case eGames::olympian:
        msgs = &eMessages::instance.fOlympianGames;
        break;
    }

    const auto pcids = personPlayerCitiesOnBoard();
    for(const auto cid : pcids) {
        const auto c = boardCityWithId(cid);
        if(c->atlantean()) continue;
        const double chance = c->winningChance(game);
        eEventData ed(cid);
        if(chance > 0) {
            showMessage(ed, msgs->fBegin);
        } else {
            showMessage(ed, msgs->fNoPart);
        }
    }
}

void eGameBoard::handleGamesEnd(const eGames game) {
    eGameMessages* msgs = nullptr;
    switch(game) {
    case eGames::isthmian:
        msgs = &eMessages::instance.fIsthmianGames;
        break;
    case eGames::nemean:
        msgs = &eMessages::instance.fNemeanGames;
        break;
    case eGames::pythian:
        msgs = &eMessages::instance.fPythianGames;
        break;
    case eGames::olympian:
        msgs = &eMessages::instance.fOlympianGames;
        break;
    }

    using eCityChance = std::pair<eCityId, double>;
    std::vector<eCityChance> chances;
    for(const auto c : mActiveCitiesOnBoard) {
        if(c->atlantean()) continue;
        const auto id = c->id();
        const double chance = c->winningChance(game);
        chances.push_back(eCityChance{id, chance});
    }
    if(chances.empty()) return;
    std::random_shuffle(chances.begin(), chances.end());

    eCityId winner = eCityId::neutralFriendly;
    for(const auto& c : chances) {
        const bool won = eRand::rand() % 101 < 100*c.second;
        if(!won) continue;
        const auto cid = c.first;
        const auto city = boardCityWithId(cid);
        if(won) {
            winner = cid;
            eEventData ed(cid);
            showMessage(ed, msgs->fWon);
            city->incWonGames();
            int id = 0;
            switch(game) {
            case eGames::isthmian:
                id = 8;
                break;
            case eGames::nemean:
                id = 3;
                break;
            case eGames::pythian:
                id = 8;
                break;
            case eGames::olympian:
                id = 8;
                break;
            }
            allow(c.first, eBuildingType::commemorative, id);
        }
        break;
    }
    eCityId secondCid = eCityId::neutralFriendly;
    for(const auto& c : chances) {
        const auto cid = c.first;
        if(cid == winner) continue;
        const bool second = eRand::rand() % 101 < 200*c.second;
        eEventData ed(cid);
        if(second && secondCid == eCityId::neutralFriendly) {
            secondCid = cid;
            showMessage(ed, msgs->fSecond);
        } else {
            showMessage(ed, msgs->fLost);
        }
    }
}

eTile* eGameBoard::tile(const int x, const int y) const {
    int dtx;
    int dty;
    eTileHelper::tileIdToDTileId(x, y, dtx, dty);
    return dtile(dtx, dty);
}

eTile* eGameBoard::dtile(const int x, const int y) const {
    if(x < 0 || x >= mWidth) return nullptr;
    if(y < 0 || y >= mHeight) return nullptr;
    return mTiles[x][y];
}

double eGameBoard::taxRateF(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 1.;
    return city->taxRateF();
}

eTaxRate eGameBoard::taxRate(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return eTaxRate::normal;
    return city->taxRate();
}

eWageRate eGameBoard::wageRate(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return eWageRate::normal;
    return city->wageRate();
}

void eGameBoard::setTaxRate(const eCityId cid, const eTaxRate tr) {
    const auto city = boardCityWithId(cid);
    if(!city) return;
    return city->setTaxRate(tr);
}

void eGameBoard::setWageRate(const eCityId cid, const eWageRate wr) {
    const auto city = boardCityWithId(cid);
    if(!city) return;
    return city->setWageRate(wr);
}

int eGameBoard::taxesPaidThisYear(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->taxesPaidThisYear();
}

int eGameBoard::taxesPaidLastYear(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->taxesPaidThisYear();
}

int eGameBoard::peoplePaidTaxesThisYear(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->taxesPaidThisYear();
}

int eGameBoard::peoplePaidTaxesLastYear(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->taxesPaidThisYear();
}

int eGameBoard::philosophyResearchCoverage(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->philosophyResearchCoverage();
}

int eGameBoard::athleticsLearningCoverage(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->athleticsLearningCoverage();
}

int eGameBoard::dramaAstronomyCoverage(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->dramaAstronomyCoverage();
}

int eGameBoard::allCultureScienceCoverage(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->allCultureScienceCoverage();
}

int eGameBoard::taxesCoverage(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->philosophyResearchCoverage();
}

int eGameBoard::unrest(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->unrest();
}

int eGameBoard::popularity(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->popularity();
}

int eGameBoard::health(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return 0;
    return city->health();
}

int eGameBoard::drachmas(const ePlayerId pid) const {
    const auto player = boardPlayerWithId(pid);
    if(!player) return 0;
    return player->drachmas();
}

void eGameBoard::incDrachmas(const ePlayerId pid, const int by) {
    const auto player = boardPlayerWithId(pid);
    if(!player) return;
    return player->incDrachmas(by);
}

void eGameBoard::setDrachmas(const ePlayerId pid, const int to) {
    const auto player = boardPlayerWithId(pid);
    if(!player) return;
    return player->setDrachmas(to);
}

void eGameBoard::registerCharacter(eCharacter* const c) {
    mCharacters.push_back(c);
}

bool eGameBoard::unregisterCharacter(eCharacter* const c) {
    bool updateMusic = false;
    for(const auto& cc : mCitiesOnBoard) {
        const bool r = cc->unregisterAttackingGod(c);
        updateMusic = updateMusic || r;
    }
    if(updateMusic) this->updateMusic();
    return eVectorHelpers::remove(mCharacters, c);
}

void eGameBoard::registerCharacterAction(eCharacterAction* const ca) {
    mCharacterActions.push_back(ca);
}

bool eGameBoard::unregisterCharacterAction(eCharacterAction* const ca) {
    return eVectorHelpers::remove(mCharacterActions, ca);
}

void eGameBoard::registerSoldier(eSoldier* const c) {
    mSoldiers.push_back(c);
}

bool eGameBoard::unregisterSoldier(eSoldier* const c) {
    return eVectorHelpers::remove(mSoldiers, c);
}

void eGameBoard::registerBuilding(eBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return;
    mAllBuildings.push_back(b);
    const auto bt = b->type();
    if(eBuilding::sTimedBuilding(bt)) {
        mTimedBuildings.push_back(b);
    }
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    if(!city) return;
    city->registerBuilding(b);
    scheduleAppealMapUpdate(cid);
}

bool eGameBoard::unregisterBuilding(eBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    eVectorHelpers::remove(mAllBuildings, b);
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    if(!city) return false;
    city->unregisterBuilding(b);
    eVectorHelpers::remove(mTimedBuildings, b);
    scheduleAppealMapUpdate(cid);
    return true;
}

bool eGameBoard::unregisterCommonHouse(eSmallHouse* const ch) {
    if(!mRegisterBuildingsEnabled) return false;
    const auto cid = ch->cityId();
    const auto city = boardCityWithId(cid);
    return city->unregisterCommonHouse(ch);
}

void eGameBoard::registerEmplBuilding(eEmployingBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    city->registerEmplBuilding(b);
}

bool eGameBoard::unregisterEmplBuilding(eEmployingBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    return city->unregisterEmplBuilding(b);
}

void eGameBoard::registerTradePost(eTradePost* const b) {
    if(!mRegisterBuildingsEnabled) return;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    city->registerTradePost(b);
    if(mButtonVisUpdater) mButtonVisUpdater();
}

bool eGameBoard::unregisterTradePost(eTradePost* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    const bool r = city->unregisterTradePost(b);
    if(r && mButtonVisUpdater) mButtonVisUpdater();
    return r;
}

bool eGameBoard::hasTradePost(const eCityId cid, const eWorldCity& city) {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->hasTradePost(city);
}

void eGameBoard::registerSpawner(eSpawner* const s) {
    mSpawners.push_back(s);
}

bool eGameBoard::unregisterSpawner(eSpawner* const s) {
    return eVectorHelpers::remove(mSpawners, s);;
}

void eGameBoard::registerStadium(eStadium* const s) {
    if(!mRegisterBuildingsEnabled) return;
    const auto cid = s->cityId();
    const auto city = boardCityWithId(cid);
    city->registerStadium(s);
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::unregisterStadium(const eCityId cid) {
    if(!mRegisterBuildingsEnabled) return;
    const auto city = boardCityWithId(cid);
    city->unregisterStadium();
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::registerMuseum(eMuseum* const s) {
    if(!mRegisterBuildingsEnabled) return;
    const auto cid = s->cityId();
    const auto city = boardCityWithId(cid);
    city->registerMuseum(s);
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::unregisterMuseum(const eCityId cid) {
    if(!mRegisterBuildingsEnabled) return;
    const auto city = boardCityWithId(cid);
    city->unregisterMuseum();
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::registerStorBuilding(eStorageBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    city->registerStorBuilding(b);
}

bool eGameBoard::unregisterStorBuilding(eStorageBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    return city->unregisterStorBuilding(b);
}

void eGameBoard::registerSanctuary(eSanctuary* const b) {
    if(!mRegisterBuildingsEnabled) return;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    city->registerSanctuary(b);
    if(mButtonVisUpdater) mButtonVisUpdater();
}

bool eGameBoard::unregisterSanctuary(eSanctuary* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    const bool r = city->unregisterSanctuary(b);
    if(r && mButtonVisUpdater) mButtonVisUpdater();
    return r;
}

void eGameBoard::registerHeroHall(eHerosHall* const b) {
    if(!mRegisterBuildingsEnabled) return;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    city->registerHeroHall(b);
}

bool eGameBoard::unregisterHeroHall(eHerosHall* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    const bool r = city->unregisterHeroHall(b);
    if(r && mButtonVisUpdater) mButtonVisUpdater();
    return r;
}

void eGameBoard::registerMissile(eMissile* const m) {
    mMissiles.push_back(m);
}

bool eGameBoard::unregisterMissile(eMissile* const m) {
    return eVectorHelpers::remove(mMissiles, m);
}

bool eGameBoard::hasStadium(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return false;
    return city->hasStadium();
}

bool eGameBoard::hasMuseum(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return false;
    return city->hasMuseum();
}

eStadium* eGameBoard::stadium(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return nullptr;
    return city->stadium();
}

eMuseum* eGameBoard::museum(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return nullptr;
    return city->museum();
}

void eGameBoard::registerPalace(ePalace* const p) {
    if(!mRegisterBuildingsEnabled) return;
    const auto cid = p->cityId();
    const auto city = boardCityWithId(cid);
    if(!city) return;
    city->registerPalace(p);
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::unregisterPalace(const eCityId cid) {
    if(!mRegisterBuildingsEnabled) return;
    const auto city = boardCityWithId(cid);
    if(!city) return;
    city->unregisterPalace();
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::registerMonster(const eCityId cid, eMonster* const m) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->registerMonster(m);
}

void eGameBoard::unregisterMonster(const eCityId cid, eMonster* const m) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->unregisterMonster(m);
    updateMusic();
}

eBanner* eGameBoard::banner(const eCityId cid,
                            const eBannerTypeS type,
                            const int id) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->banner(type, id);
}

void eGameBoard::registerBanner(eBanner* const b) {
    const auto t = b->tile();
    const auto cid = t->cityId();
    const auto c = boardCityWithId(cid);
    if(!c) return;
    mBanners.push_back(b);
    return c->registerBanner(b);
}

void eGameBoard::unregisterBanner(eBanner* const b) {
    const auto t = b->tile();
    const auto cid = t->cityId();
    const auto c = boardCityWithId(cid);
    if(!c) return;
    eVectorHelpers::remove(mBanners, b);
    return c->unregisterBanner(b);
}

void eGameBoard::registerAllSoldierBanner(eSoldierBanner* const b) {
    mAllSoldierBanners.push_back(b);
}

void eGameBoard::unregisterAllSoldierBanner(eSoldierBanner* const b) {
    eVectorHelpers::remove(mAllSoldierBanners, b);
}

bool eGameBoard::manTowers(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return false;
    return city->manTowers();
}

void eGameBoard::setManTowers(const eCityId cid, const bool m) {
    const auto city = boardCityWithId(cid);
    if(!city) return;
    city->setManTowers(m);
}

std::vector<eAgoraBase*> eGameBoard::agoras(const eCityId cid) const {
    const auto city = boardCityWithId(cid);
    if(!city) return {};
    return city->agoras();
}

void eGameBoard::incTime(const int by) {
    if(mEpisodeLost) return;
    const int dayLen = eNumbers::sDayLength;
    { // autosave
        const int time = mTime + by;
        bool nextMonth = false;
        bool nextYear = false;
        const int nd = time/dayLen;
        auto date = mDate;
        date.nextDays(nd, nextMonth, nextYear);
        if(nextYear) {
            if(mAutosaver) mAutosaver();
        }
    }

    const int iMax = mPlannedActions.size() - 1;
    for(int i = iMax; i >= 0; i--) {
        const auto a = mPlannedActions[i];
        a->incTime(by, *this);
        if(a->finished()) {
            mPlannedActions.erase(mPlannedActions.begin() + i);
            delete a;
        }
    }

//    if(mTotalTime == 0) {
////        receiveRequest(mWorldBoard->cities()[0],
////                            eResourceType::fleece, 9, 0);
//        planGiftFrom(mWorldBoard->cities()[0],
//                     eResourceType::fleece, 16);
//    }

//    for(const auto& p : mPlagues) {
//        const int r = eRand::rand() % 5000;
//        const bool spread = r/by == 0;
//        if(spread) p->randomSpread();
//    }

    mProgressEarthquakes += by;
    const int earthquakeWait = eNumbers::sEarthquakeProgressPeriod;
    if(mProgressEarthquakes > earthquakeWait) {
        mProgressEarthquakes = 0;
        progressEarthquakes();
    }

    mSoldiersUpdate += by;
    const int sup = 1000;
    if(mSoldiersUpdate > sup) {
        mSoldiersUpdate -= sup;
        for(const auto& c : mCitiesOnBoard) {
            c->soldierBannersUpdate();
        }
    }

    for(const auto& c : mActiveCitiesOnBoard) {
        c->incTime(by);
    }

    for(const auto& p : mPlayersOnBoard) {
        p->incTime(by);
        if(mTotalTime == 0) { // start building AI cities
            p->nextMonth();
        }
    }
    mTime += by;
    mTotalTime += by;
    bool nextMonth = false;
    bool nextYear = false;
    const int nd = mTime/dayLen;
    mDate.nextDays(nd, nextMonth, nextYear);
    mTime -= nd*dayLen;

    if(nextYear) {
        for(auto& y : mYearlyProduction) {
            auto& p = y.second;
            p.fLastYear = p.fThisYear;
            p.fThisYear = 0;
            if(p.fLastYear > p.fBest) p.fBest = p.fLastYear;
        }

        for(const auto& c : mActiveCitiesOnBoard) {
            c->nextYear();
        }

        const auto ppcs = personPlayerCitiesOnBoard();
        for(const auto cid : ppcs) {
            auto& defs = mDefeatedBy[cid];
            for(const auto& cc : defs) {
                if(!cc->isRival()) continue;
                const auto rr = e::make_shared<eReceiveRequestEvent>(
                                    personPlayerCapital(),
                                    eGameEventBranch::root, *this);
                const auto type = cc->recTributeType();
                const int count = cc->recTributeCount();
                rr->initialize(0, type, count, cc, false);
                rr->setTributeRequest(true);
                rr->initializeDate(mDate);
                addRootGameEvent(rr);
            }
        }
    }
    if(nextMonth) {
        for(const auto& c : mActiveCitiesOnBoard) {
            c->nextMonth();
        }
        for(const auto& p : mPlayersOnBoard) {
            p->nextMonth();
        }

        const auto m = mDate.month();
        const int ng = std::abs(mDate.year() % 4);
        const auto game = static_cast<eGames>(ng);
        if(m == eMonth::june) {
            handleGamesBegin(game);
        } else if(m == eMonth::august) {
            handleGamesEnd(game);
        }
    }

    const int ect = 5000;
    mEmploymentCheckTime += by;
    if(mEmploymentCheckTime > ect) {
        mEmploymentCheckTime -= ect;
        const auto ppcs = personPlayerCitiesOnBoard();
        for(const auto cid : ppcs) {
            const auto c = boardCityWithId(cid);
            const auto& emplData = c->employmentData();
            const double employable = emplData.employable();
            const double jobVacs = emplData.totalJobVacancies();
            int emplState = 0;
            if(employable < jobVacs*0.75) {
                emplState = 1;
            } else if(employable > jobVacs*1.25) {
                emplState = -1;
            } else {
                emplState = 0;
            }
            auto& les = mLastEmploymentState[cid];
            if(les.fV != emplState) {
                const auto& inst = eMessages::instance;
                eEventData ed(cid);
                if(emplState == -1) { // unemployed
                    showMessage(ed, inst.fUnemployment);
                } else if(emplState == 1) { // employed
                    showMessage(ed, inst.fEmployees);
                }
            }
            les.fV = emplState;
        }
    }

    if(nextMonth) {
        mWorldBoard->nextMonth(this);
    }
    if(nextYear) {
        mWorldBoard->nextYear();
        const auto ppid = personPlayer();
        const auto cs = mWorldBoard->getTribute(ppid);
        for(const auto& c : cs) {
            if(c->conqueredByRival()) continue;
            tributeFrom(ppid, c, true);
        }
    }
    const auto chars = mCharacters;
    for(const auto c : chars) {
        if(c->isSoldier()) continue;
        c->incTime(by);
    }
    const auto solds = mSoldiers;
    for(const auto c : solds) {
        c->incTime(by);
    }
    const auto build = mTimedBuildings;
    for(const auto b : build) {
        b->incTime(by);
        if(nextMonth) b->nextMonth();
    }
    for(const auto s : mSpawners) {
        s->incTime(by);
    }

    const auto missiles = mMissiles;
    for(const auto m : missiles) {
        m->incTime(by);
    }

    const int goalsCheckWait = 5050;
    mGoalsCheckTime += by;
    if(mGoalsCheckTime > goalsCheckWait) {
        mGoalsCheckTime -= goalsCheckWait;
        const bool f = checkGoalsFulfilled();
        if(f) {
            if(mEpisodeFinishedHandler) {
                mEpisodeFinishedHandler();
            }
        }
    }

    for(const auto& c : mActiveCitiesOnBoard) {
        c->incDistributeEmployees(by);
    }
}

void eGameBoard::incFrame() {
    mFrame++;
}

void eGameBoard::handleFinishedTasks() {
    mThreadPool.handleFinished();
}

void eGameBoard::scheduleDataUpdate() {
    mThreadPool.scheduleDataUpdate();
}

int eGameBoard::population(const ePlayerId pid) const {
    int result = 0;
    const auto cids = playerCitiesOnBoard(pid);
    for(const auto cid : cids) {
        result += population(cid);
    }
    return result;
}

int eGameBoard::population(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->population();
}

ePopulationData* eGameBoard::populationData(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return &c->populationData();
}

eHusbandryData* eGameBoard::husbandryData(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return &c->husbandryData();
}

eEmploymentData* eGameBoard::employmentData(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return &c->employmentData();
}

eEmploymentDistributor* eGameBoard::employmentDistributor(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return &c->employmentDistributor();
}

void eGameBoard::addShutDown(const eCityId cid, const eResourceType type) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->addShutDown(type);
}

void eGameBoard::removeShutDown(const eCityId cid, const eResourceType type) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->removeShutDown(type);
}

int eGameBoard::industryJobVacancies(const eCityId cid, const eResourceType type) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->industryJobVacancies(type);
}

void eGameBoard::distributeEmployees(
        const eCityId cid, const eSector s) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->distributeEmployees(s);
}

void eGameBoard::distributeEmployees(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->distributeEmployees();
}

void eGameBoard::payTaxes(const eCityId cid, const int d, const int people) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->payTaxes(d, people);
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->incDrachmas(d);
}

void eGameBoard::setDifficulty(const eDifficulty d) {
    std::map<eDifficulty, eDifficulty> rivalDiff =
        {{eDifficulty::beginner, eDifficulty::olympian},
         {eDifficulty::mortal, eDifficulty::titan},
         {eDifficulty::hero, eDifficulty::hero},
         {eDifficulty::titan, eDifficulty::mortal},
         {eDifficulty::olympian, eDifficulty::beginner}};
    std::map<eDifficulty, eDifficulty> allyDiff =
        {{eDifficulty::beginner, eDifficulty::beginner},
         {eDifficulty::mortal, eDifficulty::mortal},
         {eDifficulty::hero, eDifficulty::hero},
         {eDifficulty::titan, eDifficulty::titan},
         {eDifficulty::olympian, eDifficulty::olympian}};
    const auto ppid = personPlayer();
    const auto ptid = playerIdToTeamId(ppid);
    for(const auto& p : mPlayersOnBoard) {
        const auto pid = p->id();
        eDifficulty pd;
        if(pid == ppid) {
            pd = d;
        } else {
            const auto tid = playerIdToTeamId(pid);
            if(tid == ptid) {
                pd = allyDiff[d];
            } else {
                pd = rivalDiff[d];
            }
        }
        setDifficulty(pid, pd);
    }
}

void eGameBoard::setDifficulty(const ePlayerId pid, const eDifficulty d) {
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->setDifficulty(d);
}

eDifficulty eGameBoard::personPlayerDifficulty() const {
    return difficulty(personPlayer());
}

eDifficulty eGameBoard::difficulty(const ePlayerId pid) const {
    const auto p = boardPlayerWithId(pid);
    if(!p) return eDifficulty::beginner;
    return p->difficulty();
}

void eGameBoard::setDate(const eDate& d) {
    mDate = d;
}

double eGameBoard::appeal(const int tx, const int ty) const {
    return mAppealMap.heat(tx, ty);
}

void eGameBoard::addRubbish(const stdsptr<eObject>& o) {
    mRubbish.push_back(o);
}

void eGameBoard::emptyRubbish() {
    while(!mRubbish.empty()) {
        std::vector<stdsptr<eObject>> r;
        std::swap(mRubbish, r);
    }
}

void eGameBoard::setRequestUpdateHandler(const eAction& ru) {
    mRequestUpdateHandler = ru;
}

void eGameBoard::setEventHandler(const eEventHandler& eh) {
    mEventHandler = eh;
}

void eGameBoard::event(const eEvent e, eEventData& ed) {
    if(mEventHandler) mEventHandler(e, ed);
}

void eGameBoard::setEpisodeFinishedHandler(const eAction& a) {
    mEpisodeFinishedHandler = a;
}

void eGameBoard::setAutosaver(const eAction& a) {
    mAutosaver = a;
}

void eGameBoard::setVisibilityChecker(const eVisibilityChecker& vc) {
    mVisibilityChecker = vc;
}

void eGameBoard::setTipShower(const eTipShower& ts) {
    mTipShower = ts;
}

void eGameBoard::showTip(const ePlayerCityTarget& target,
                         const std::string& tip) const {
    if(mTipShower) mTipShower(target, tip);
}

void eGameBoard::setEnlistForcesRequest(const eEnlistRequest& req) {
    mEnlistRequester = req;
}

void eGameBoard::requestForces(const eEnlistAction& action,
                               const std::vector<eResourceType>& plunderResources,
                               const std::vector<stdsptr<eWorldCity>>& exclude) {
    if(mEnlistRequester) {
        const auto ppid = personPlayer();
        const auto cids = playerCitiesOnBoard(ppid);
        std::vector<std::string> cnames;
        for(const auto cid : cids) {
            const auto n = cityName(cid);
            cnames.push_back(n);
        }
        auto f = getEnlistableForces(ppid);
        std::vector<eHeroType> heroesAbroad;
        std::map<eHeroType, eCityId> heroesCity;
        for(const auto h : f.fHeroes) {
            eCityId hcid = eCityId::neutralFriendly;
            eHerosHall* hh = nullptr;
            for(const auto cid : cids) {
                hh = heroHall(cid, h);
                if(hh) {
                    hcid = cid;
                    break;
                }
            }
            heroesCity[h] = hcid;
            const bool abroad = !hh ? true : hh->heroOnQuest();
            if(abroad) heroesAbroad.push_back(h);
        }
        for(const auto& e : exclude) {
            eVectorHelpers::remove(f.fAllies, e);
        }
        mEnlistRequester(f, cids, cnames, heroesAbroad, heroesCity, action, plunderResources);
    }
}

bool eGameBoard::ifVisible(eTile* const tile, const eAction& func) const {
    if(!tile) return false;
    if(!mVisibilityChecker) return false;
    const bool r = mVisibilityChecker(tile);
    if(r) func();
    return r;
}

void eGameBoard::setMessageShower(const eMessageShower& msg) {
    mMsgShower = msg;
}

void eGameBoard::showMessage(eEventData& ed,
                             const eMessageType& msg) {
    mMsgShower(ed, msg);
}

void eGameBoard::updateNeighbours() {
    for(int x = 0; x < mWidth; x++) {
        for(int y = 0; y < mHeight; y++) {
            const auto t = mTiles[x][y];
            {
                const int dx = y % 2 == 0 ? -1 : 0;
                t->setTopLeft(dtile(x + dx, y - 1));
                t->setBottomLeft(dtile(x + dx, y + 1));
            }
            {
                const int dx = y % 2 == 0 ? 0 : 1;
                t->setTopRight(dtile(x + dx, y - 1));
                t->setBottomRight(dtile(x + dx, y + 1));
            }
        }
    }
}

void eGameBoard::updateResources(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->updateResources();
}

const eGameBoard::eResources* eGameBoard::resources(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return &c->resources();
}

ePalace* eGameBoard::palace(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->palace();
}

bool eGameBoard::hasPalace(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->hasPalace();
}

int eGameBoard::resourceCount(const eCityId cid,
                              const eResourceType type) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->resourceCount(type);
}

int eGameBoard::takeResource(const eCityId cid,
                             const eResourceType type,
                             const int count) {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->takeResource(type, count);
}

int eGameBoard::eliteHouses(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->eliteHouses();
}

std::vector<eSanctuary*> eGameBoard::sanctuaries(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->sanctuaries();
}

eSanctuary* eGameBoard::sanctuary(const eCityId cid, const eGodType god) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->sanctuary(god);
}

std::vector<eHerosHall*> eGameBoard::heroHalls(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->heroHalls();
}

eHerosHall* eGameBoard::heroHall(const eCityId cid, const eHeroType hero) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->heroHall(hero);
}

std::vector<stdsptr<eSoldierBanner>> eGameBoard::banners(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return {};
    return c->banners();
}

int eGameBoard::countBanners(const eBannerType bt,
                             const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->countBanners(bt);
}

int eGameBoard::countSoldiers(const eBannerType bt,
                              const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return 0;
    return c->countSoldiers(bt);
}

void eGameBoard::startEpisode(eEpisode* const e,
                              const eWC& lastPlayedColony) {
    if(mTimedBuildings.empty()) { // first episode
        for(const auto s : mSpawners) {
            const auto type = s->type();
            if(type == eBannerTypeS::boar ||
               type == eBannerTypeS::deer) {
                s->spawnMax();
            }
        }
    }
    mDefeatedBy.clear();

    for(const auto& c : mCitiesOnBoard) {
        c->clearAfterLastEpisode();
    }

    mGoals.clear();
    mWorldBoard = e->fWorldBoard;
    const auto& date = e->fStartDate;
    setDate(date);
    for(const auto& d : e->fDrachmas) {
        setDrachmas(d.first, d.second);
    }
    mWageMultiplier = e->fWageMultiplier;
    mPrices = e->fPrices;

    for(const auto& c : mCitiesOnBoard) {
        c->startEpisode(e);
        const auto cid = c->id();
        setFriendlyGods(cid, e->fFriendlyGods[cid]);
    }
    const auto& gs = e->fGoals;
    for(const auto& g : gs) {
        const auto gg = g->makeCopy();
        gg->initializeDate(this);
        gg->update(this);
        mGoals.push_back(gg);
    }
    if(lastPlayedColony) {
        const auto a = new eColonyMonumentAction(lastPlayedColony);
        planAction(a);
    }
}

bool eGameBoard::checkGoalsFulfilled() const {
    if(mGoals.empty()) return false;
    bool result = true;
    for(const auto& g : mGoals) {
        g->update(this);
        const bool m = g->met();
        if(!m) result = false;
    }
    return result;
}

void eGameBoard::musterAllSoldiers(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->musterAllSoldiers();
}

void eGameBoard::sendAllSoldiersHome(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->sendAllSoldiersHome();
}

void eGameBoard::incPopulation(const eCityId cid, const int by) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->incPopulation(by);
}

void eGameBoard::topElevationExtremas(int& min, int& max) const {
    min = 10000;
    max = -10000;
    for(int x = 0; x < mWidth; x++) {
        const auto t = dtile(x, 0);
        const int a = t->altitude();
        min = std::min(min, a);
        max = std::max(max, a);
    }
}

void eGameBoard::rightElevationExtremas(int& min, int& max) const {
    min = 10000;
    max = -10000;
    for(int y = 0; y < mHeight; y++) {
        const auto t = dtile(mWidth - 1, y);
        const int a = t->altitude();
        min = std::min(min, a);
        max = std::max(max, a);
    }
}

void eGameBoard::bottomElevationExtremas(int& min, int& max) const {
    min = 10000;
    max = -10000;
    for(int x = 0; x < mWidth; x++) {
        const auto t = dtile(x, mHeight - 1);
        const int a = t->altitude();
        min = std::min(min, a);
        max = std::max(max, a);
    }
}

void eGameBoard::leftElevationExtremas(int& min, int& max) const {
    min = 10000;
    max = -10000;
    for(int y = 0; y < mHeight; y++) {
        const auto t = dtile(0, y);
        const int a = t->altitude();
        min = std::min(min, a);
        max = std::max(max, a);
    }
}

void eGameBoard::minMaxAltitude(int& min, int& max) const {
    min = 10000;
    max = -10000;
    for(int x = 0; x < mWidth; x++) {
        for(int y = 0; y < mHeight; y++) {
            const auto t = dtile(x, y);
            const int a = t->altitude();
            if(a > max) max = a;
            if(a < min) min = a;
        }
    }
}

bool eGameBoard::landTradeShutdown(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->landTradeShutdown();
}

void eGameBoard::setLandTradeShutdown(
        const eCityId cid, const bool s) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->setLandTradeShutdown(s);
}

bool eGameBoard::seaTradeShutdown(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return false;
    return c->seaTradeShutdown();
}

void eGameBoard::setSeaTradeShutdown(
        const eCityId cid, const bool s) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    return c->setSeaTradeShutdown(s);
}

eMilitaryAid* eGameBoard::militaryAid(const eCityId cid,
                                      const stdsptr<eWorldCity>& c) const {
    const auto cc = boardCityWithId(cid);
    if(!cc) return nullptr;
    return cc->militaryAid(c);
}

void eGameBoard::removeMilitaryAid(const eCityId cid,
                                   const stdsptr<eWorldCity>& c) {
    const auto cc = boardCityWithId(cid);
    if(!cc) return;
    cc->removeMilitaryAid(c);
}

void eGameBoard::addMilitaryAid(const eCityId cid,
                                const stdsptr<eMilitaryAid>& a) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->addMilitaryAid(a);
}

void eGameBoard::setEpisodeLost() const {
    mEpisodeLost = true;
}

int eGameBoard::tradingPartners() const {
    int n = 0;
    for(const auto t : mTradePosts) {
        const bool trades = t->trades();
        if(!trades) continue;
        n++;
    }
    return n;
}

eOrientation randomOrientation() {
    std::vector<eOrientation> os{eOrientation::topRight,
                                 eOrientation::bottomRight,
                                 eOrientation::bottomLeft,
                                 eOrientation::topLeft};
    return os[eRand::rand() % os.size()];
}

void eGameBoard::earthquake(eTile* const startTile, const int size) {
    struct eQuakeEnd {
        eTile* fTile = nullptr;
        eOrientation fLastO = randomOrientation();
    };

    const auto quake = std::make_shared<eEarthquake>();
    mEarthquakes.push_back(quake);

    std::queue<eQuakeEnd> ends;
    quake->fStartTile = startTile;
    auto& tiles = quake->fTiles;
    tiles.push_back(startTile);
    ends.push({startTile});
//    std::vector<eOrientation> os{eOrientation::topRight,
//                                 eOrientation::bottomRight,
//                                 eOrientation::bottomLeft,
//                                 eOrientation::topLeft};
//    std::random_shuffle(os.begin(), os.end());
//    for(int i = 0; i < 2; i++) {
//        const auto o = os[i];
//        const auto tt = startTile->neighbour<eTile>(o);
//        ends.push({tt});
//        mEarthquake.push_back(tt);
//    }
    while((int)tiles.size() < size && !ends.empty()) {
        const auto t = ends.front();
        ends.pop();
        std::vector<eOrientation> os{eOrientation::topRight,
                                     eOrientation::bottomRight,
                                     eOrientation::bottomLeft,
                                     eOrientation::topLeft};
        std::random_shuffle(os.begin(), os.end());
        if(eRand::rand() % 7) {
            os.insert(os.begin(), t.fLastO);
        }
        for(const auto o : os) {
            const auto tt = t.fTile->neighbour<eTile>(o);
            if(!tt) continue;
            const auto terr = tt->terrain();
            if(terr == eTerrain::dry ||
               terr == eTerrain::fertile ||
               terr == eTerrain::forest ||
               terr == eTerrain::choppedForest) {
                ends.push({tt, o});
                tiles.push_back(tt);
                if(eRand::rand() % 5 == 0) {
                    ends.push({tt});
                }
                break;
            }
        }
    }
}

bool eGameBoard::duringEarthquake() const {
    return !mEarthquakes.empty();
}

void eGameBoard::defeatedBy(const eCityId defeated,
                            const stdsptr<eWorldCity>& by) {
    auto& defs = mDefeatedBy[defeated];
    const bool r = eVectorHelpers::contains(defs, by);
    if(r) {
        const auto ccid = personPlayerCapital();
        if(defeated == ccid) setEpisodeLost();
    } else {
        defs.push_back(by);
    }
}

eImmigrationLimitedBy eGameBoard::immigrationLimit(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return eImmigrationLimitedBy::none;
    return c->immigrationLimit();
}

void eGameBoard::progressEarthquakes() {
    if(mEarthquakes.empty()) return;
    eSounds::playEarthquakeSound();
    for(int i = 0; i < (int)mEarthquakes.size(); i++) {
        const auto& e = mEarthquakes[i];
        const auto st = e->fStartTile;
        const int stx = st->x();
        const int sty = st->y();
        const auto prcs = [this, e, stx, sty](const int x, const int y) {
            const int tx = stx + x;
            const int ty = sty + y;
            const auto t = tile(tx, ty);
            if(!t) return false;
            const bool r = eVectorHelpers::contains(e->fTiles, t);
            if(!r) return false;
            t->setTerrain(eTerrain::quake);
            for(int xx = -1; xx <= 1; xx++) {
                for(int yy = -1; yy <= 1; yy++) {
                    if(xx == 0 && yy == 0) continue;
                    const auto tt = t->tileRel<eTile>(xx, yy);
                    tt->scheduleTerrainUpdate();
                }
            }
            const auto cid = t->cityId();
            const auto c = boardCityWithId(cid);
            if(c) c->incTerrainState();
            if(const auto ub = t->underBuilding()) {
                const auto type = ub->type();
                const bool s = eBuilding::sSanctuaryBuilding(type);
                if(s) {
                } else if(type == eBuildingType::ruins) {
                    ub->erase();
                } else {
                    if(const auto as = dynamic_cast<eAgoraSpace*>(ub)) {
                        const auto a = as->agora();
                        a->collapse();
                    } else if(const auto v = dynamic_cast<eVendor*>(ub)) {
                        const auto a = v->agora();
                        a->collapse();
                    } else if(const auto r = dynamic_cast<eRoad*>(ub)) {
                        const auto a = r->underAgora();
                        if(a) a->collapse();
                        const auto g = r->underGatehouse();
                        if(g) g->collapse();
                        r->eBuilding::erase();
                    } else {
                        if(type == eBuildingType::park) {
                            scheduleTerrainUpdate();
                        }
                        ub->collapse();
                    }
                    eSounds::playCollapseSound();
                }
            }
            eVectorHelpers::removeAll(e->fTiles, t);
            return false;
        };
        eIterateSquare::iterateSquare(e->fLastDim, prcs);
        e->fLastDim++;
        if(e->fTiles.empty() || e->fLastDim > 50) {
            eVectorHelpers::remove(mEarthquakes, e);
            i--;
        }
    }
}

void centerTile(const int minX, const int minY,
                const int sw, const int sh,
                int& tx, int& ty) {
    tx = minX;
    ty = minY;

    if(sw == 2 && sh == 2) {
        ty += 1;
    } else if(sw == 3 && sh == 3) {
        tx += 1;
        ty += 1;
    } else if(sw == 4 || sh == 4) {
        tx += 1;
        ty += 2;
    } else if(sw == 5 || sh == 5) {
        tx += 2;
        ty += 2;
    } else if(sw == 6 || sh == 6) {
        tx += 2;
        ty += 2;
    }
}

void buildTiles(int& minX, int& minY,
                int& maxX, int& maxY,
                const int tx, const int ty,
                const int sw, const int sh) {
    minX = tx;
    minY = ty;

    if(sw == 2 && sh == 2) {
        minY -= 1;
    } else if(sw == 3 && sh == 3) {
        minX -= 1;
        minY -= 1;
    } else if(sw == 4 || sh == 4) {
        minX -= 1;
        minY -= 2;
    } else if(sw == 5 || sh == 5) {
        minX -= 2;
        minY -= 2;
    } else if(sw == 6 || sh == 6) {
        minX -= 2;
        minY -= 2;
    }

    maxX = minX + sw;
    maxY = minY + sh;
}

bool eGameBoard::canBuildAvenue(eTile* const t, const eCityId cid,
                                const ePlayerId pid,
                                const bool forestAllowed) const {
    const int tx = t->x();
    const int ty = t->y();
    const bool cb = canBuildBase(tx, tx + 1, ty, ty + 1, forestAllowed, cid, pid);
    if(!cb) return false;
    const auto tr = t->topRight<eTile>();
    const auto br = t->bottomRight<eTile>();
    const auto bl = t->bottomLeft<eTile>();
    const auto tl = t->topLeft<eTile>();
    const auto tt = t->top<eTile>();
    const auto r = t->right<eTile>();
    const auto b = t->bottom<eTile>();
    const auto l = t->left<eTile>();
    const bool hr = (tr && tr->hasRoad()) ||
                    (br && br->hasRoad()) ||
                    (bl && bl->hasRoad()) ||
                    (tl && tl->hasRoad()) ||
                    (tt && tt->hasRoad()) ||
                    (r && r->hasRoad()) ||
                    (b && b->hasRoad()) ||
                    (l && l->hasRoad());
    return hr;
}

bool eGameBoard::canBuildBase(const int minX, const int maxX,
                              const int minY, const int maxY,
                              const bool forestAllowed,
                              const eCityId cid,
                              const ePlayerId pid,
                              const bool fertile,
                              const bool flat) const {
    if(pid != cityIdToPlayerId(cid) && !mEditorMode) return false;
    bool fertileFound = false;
    for(int x = minX; x < maxX; x++) {
        for(int y = minY; y < maxY; y++) {
            const auto t = tile(x, y);
            if(!t) return false;
            if(t->cityId() != cid) return false;
            if(t->underBuilding()) return false;
            const auto banner = t->banner();
            if(banner && !banner->buildable()) return false;
            const auto ttt = t->terrain();
            if(fertile && ttt == eTerrain::fertile) {
                fertileFound = true;
            }

            const auto ttta = forestAllowed ?
                                  ttt & eTerrain::buildableAfterClear :
                                  ttt & eTerrain::buildable;
            if(!static_cast<bool>(ttta)) return false;

            if(!t->walkableElev() && t->isElevationTile()) return false;

            if(!flat) {
                const auto& chars = t->characters();
                if(!chars.empty()) return false;
            }
        }
    }
    if(fertile) return fertileFound;
    return true;
}

bool eGameBoard::canBuild(const int tx, const int ty,
                          const int sw, const int sh,
                          const bool forestAllowed,
                          const eCityId cid,
                          const ePlayerId pid,
                          const bool fertile,
                          const bool flat) const {
    int minX;
    int minY;
    int maxX;
    int maxY;
    buildTiles(minX, minY, maxX, maxY, tx, ty, sw, sh);
    return canBuildBase(minX, maxX, minY, maxY,
                        forestAllowed, cid, pid,
                        fertile, flat);
}


bool eGameBoard::buildBase(const int minX, const int minY,
                           const int maxX, const int maxY,
                           const eBuildingCreator& bc,
                           const ePlayerId pid,
                           const eCityId cid,
                           const bool editorDisplay,
                           const bool fertile,
                           const bool flat) {
    const int sw = maxX - minX + 1;
    const int sh = maxY - minY + 1;
    const bool cb = canBuildBase(minX, maxX + 1, minY, maxY + 1,
                                 editorDisplay,
                                 cid, pid, fertile, flat);
    if(!cb) return false;
    if(!bc) return false;
    const auto b = bc();
    if(!b) return false;
    const bool isRoad = b->type() == eBuildingType::road;
    if(!isRoad) {
        for(int x = minX; x <= maxX; x++) {
            for(int y = minY; y <= maxY; y++) {
                const auto t = tile(x, y);
                if(!t) return false;
                if(t->isElevationTile()) return false;
            }
        }
    }
    int tx;
    int ty;
    centerTile(minX, minY, sw, sh, tx, ty);
    const auto tile = this->tile(tx, ty);
    if(!tile) return false;
    b->setCenterTile(tile);
    b->setTileRect({minX, minY, sw, sh});
    for(int x = minX; x <= maxX; x++) {
        for(int y = minY; y <= maxY; y++) {
            const auto t = this->tile(x, y);
            if(t) {
                t->setUnderBuilding(b);
                b->addUnderBuilding(t);
            }
        }
    }

    if(!editorDisplay) {
        const auto diff = difficulty(pid);
        const int cost = eDifficultyHelpers::buildingCost(diff, b->type());
        incDrachmas(pid, -cost);
    }
    return true;
}

bool eGameBoard::build(const int tx, const int ty,
                       const int sw, const int sh,
                       const eCityId cid,
                       const ePlayerId pid,
                       const bool editorDisplay,
                       const eBuildingCreator& bc,
                       const bool fertile,
                       const bool flat) {
    const auto tile = this->tile(tx, ty);
    if(!tile) return false;
    int minX;
    int minY;
    int maxX;
    int maxY;
    buildTiles(minX, minY, maxX, maxY,
               tx, ty, sw, sh);
    return buildBase(minX, minY, maxX - 1, maxY - 1,
                     bc, pid, cid, editorDisplay, fertile, flat);
}

bool eGameBoard::buildAnimal(eTile* const tile,
                             const eBuildingType type,
                             const eAnimalCreator& creator,
                             const eCityId cid,
                             const ePlayerId pid,
                             const bool editorDisplay) {
    const int tx = tile->x();
    const int ty = tile->y();
    const bool cb = canBuild(tx, ty, 1, 2, editorDisplay, cid, pid, true, true);
    if(!cb) return false;
    const auto sh = creator(*this);
    sh->changeTile(tile);
    const auto o = static_cast<eOrientation>(eRand::rand() % 8);
    sh->setOrientation(o);
    const auto w = eWalkableObject::sCreateFertile();
    const auto a = e::make_shared<eAnimalAction>(sh.get(), tx, ty, w);
    sh->setAction(a);

    return build(tx, ty, 1, 2, cid, pid, editorDisplay, [this, sh, type, cid]() {
        return e::make_shared<eAnimalBuilding>(
                    *this, sh.get(), type, cid);
    }, true, true);
}

void eGameBoard::removeAllBuildings() {
    for(const auto& b : mAllBuildings) {
        b->erase();
    }
}

bool eGameBoard::buildSanctuary(const int minX, const int maxX,
                                const int minY, const int maxY,
                                const eBuildingType bt,
                                const bool rotate,
                                const eCityId cid,
                                const ePlayerId pid,
                                const bool editorDisplay) {
    const bool cb = canBuildBase(minX, maxX, minY, maxY,
                                 editorDisplay, cid, pid);
    if(!cb) return false;

    const auto ppid = cityIdToPlayerId(cid);

    const auto h = eSanctBlueprints::sSanctuaryBlueprint(bt, rotate);

    const int sw = h->fW;
    const int sh = h->fH;

    const auto b = eSanctuary::sCreate(bt, sw, sh, *this, cid);
    b->setRotated(rotate);
    const auto god = b->godType();

    if(!editorDisplay) {
        const auto diff = difficulty(ppid);
        const int cost = eDifficultyHelpers::buildingCost(diff, bt);
        incDrachmas(ppid, -cost);
        const int m = eBuilding::sInitialMarbleCost(bt);
        takeResource(cid, eResourceType::marble, m);
    }

    built(cid, bt);

    const auto mint = this->tile(minX, minY);
    const int a = mint->altitude();
    b->setAltitude(a);

    const SDL_Rect sanctRect{minX, minY, sw, sh};
    b->setTileRect(sanctRect);
    const auto ct = this->tile((minX + maxX)/2, (minY + maxY)/2);
    b->setCenterTile(ct);

    if(god == eGodType::demeter) {
        const int xMin = sanctRect.x - 3;
        const int yMin = sanctRect.y - 3;
        const int xMax = sanctRect.x + sanctRect.w + 3;
        const int yMax = sanctRect.y + sanctRect.h + 3;
        for(int x = xMin; x < xMax; x++) {
            for(int y = yMin; y < yMax; y++) {
                const SDL_Point pt{x, y};
                const bool in = SDL_PointInRect(&pt, &sanctRect);
                if(in) continue;
                const auto tile = this->tile(x, y);
                if(!tile) continue;
                const auto terr = tile->terrain();
                if(terr == eTerrain::dry) {
                    tile->setTerrain(eTerrain::fertile);
                }
            }
        }
        const auto c = boardCityWithId(cid);
        if(c) c->incTerrainState();
    }

    for(const auto& tv : h->fTiles) {
        for(const auto& t : tv) {
            const int tx = minX + t.fX;
            const int ty = minY + t.fY;
            const auto tile = this->tile(tx, ty);
            eGodType statueType;
            switch(t.fType) {
            case eSanctEleType::aphroditeStatue:
                statueType = eGodType::aphrodite;
                break;
            case eSanctEleType::apolloStatue:
                statueType = eGodType::apollo;
                break;
            case eSanctEleType::aresStatue:
                statueType = eGodType::ares;
                break;
            case eSanctEleType::artemisStatue:
                statueType = eGodType::artemis;
                break;
            case eSanctEleType::athenaStatue:
                statueType = eGodType::athena;
                break;
            case eSanctEleType::atlasStatue:
                statueType = eGodType::atlas;
                break;
            case eSanctEleType::demeterStatue:
                statueType = eGodType::demeter;
                break;
            case eSanctEleType::dionysusStatue:
                statueType = eGodType::dionysus;
                break;
            case eSanctEleType::hadesStatue:
                statueType = eGodType::hades;
                break;
            case eSanctEleType::hephaestusStatue:
                statueType = eGodType::hephaestus;
                break;
            case eSanctEleType::heraStatue:
                statueType = eGodType::hera;
                break;
            case eSanctEleType::hermesStatue:
                statueType = eGodType::hermes;
                break;
            case eSanctEleType::poseidonStatue:
                statueType = eGodType::poseidon;
                break;
            case eSanctEleType::zeusStatue:
                statueType = eGodType::zeus;
                break;
            default:
                statueType = god;
            }

            switch(t.fType) {
            case eSanctEleType::copper:
            case eSanctEleType::silver:
            case eSanctEleType::oliveTree:
            case eSanctEleType::vine:
            case eSanctEleType::orangeTree: {
                build(tile->x(), tile->y(), 1, 1, cid, pid, editorDisplay,
                      [this, cid]() { return e::make_shared<ePlaceholder>(*this, cid); });
                b->addSpecialTile(tile);
            } break;
            case eSanctEleType::defaultStatue:
            case eSanctEleType::aphroditeStatue:
            case eSanctEleType::apolloStatue:
            case eSanctEleType::aresStatue:
            case eSanctEleType::artemisStatue:
            case eSanctEleType::athenaStatue:
            case eSanctEleType::atlasStatue:
            case eSanctEleType::demeterStatue:
            case eSanctEleType::dionysusStatue:
            case eSanctEleType::hadesStatue:
            case eSanctEleType::hephaestusStatue:
            case eSanctEleType::heraStatue:
            case eSanctEleType::hermesStatue:
            case eSanctEleType::poseidonStatue:
            case eSanctEleType::zeusStatue: {
                const auto tt = e::make_shared<eTempleStatueBuilding>(
                                   statueType, t.fId, *this, cid);
                tt->setSanctuary(b.get());
                this->build(tx, ty, 1, 1, cid, pid, editorDisplay, [tt]() { return tt; });
                b->registerElement(tt);
            } break;
            case eSanctEleType::monument: {
                const auto tt = e::make_shared<eTempleMonumentBuilding>(
                                    god, t.fId, *this, cid);
                tt->setSanctuary(b.get());
                const int d = rotate ? 1 : 0;
                this->build(tx - d, ty + d, 2, 2, cid, pid, editorDisplay, [tt]() { return tt; });
                b->registerElement(tt);
            } break;
            case eSanctEleType::altar: {
                const auto tt = e::make_shared<eTempleAltarBuilding>(
                                    *this, cid);
                tt->setSanctuary(b.get());
                const int d = rotate ? 1 : 0;
                this->build(tx - d, ty + d, 2, 2, cid, pid, editorDisplay, [tt]() { return tt; });
                b->registerElement(tt);
            } break;
            case eSanctEleType::sanctuary: {
                const auto tb = e::make_shared<eTempleBuilding>(
                            t.fId, *this, cid);
                tb->setSanctuary(b.get());
                b->registerElement(tb);
                if(rotate) {
                    this->build(tx - 2, ty + 2, 4, 4, cid, pid, editorDisplay, [tb]() { return tb; });
                } else {
                    this->build(tx + 1, ty - 1, 4, 4, cid, pid, editorDisplay, [tb]() { return tb; });
                }
            } break;
            case eSanctEleType::tile: {
                const auto tt = e::make_shared<eTempleTileBuilding>(
                                    t.fId, *this, cid);
                tt->setSanctuary(b.get());
                this->build(tx, ty, 1, 1, cid, pid, editorDisplay, [tt]() { return tt; });
                b->registerElement(tt);
                if(t.fWarrior) b->addWarriorTile(tile);
            } break;
            case eSanctEleType::stairs: {
                tile->setSeed(t.fId);
                tile->setWalkableElev(true);
            } break;
            case eSanctEleType::none:
                break;
            }
        }
    }
    for(const auto& tv : h->fTiles) {
        for(const auto& t : tv) {
            const int tx = minX + t.fX;
            const int ty = minY + t.fY;
            const auto tile = this->tile(tx, ty);
            tile->setAltitude(tile->altitude() + t.fA);
            const auto trr = tile->terrain();
            const bool bldbl = static_cast<bool>(
                                   trr & eTerrain::buildable);
            if(!tile->underBuilding() && bldbl) {
                tile->setUnderBuilding(b);
                b->addUnderBuilding(tile);
            }
        }
    }

    b->buildingProgressed();

    return true;
}

int eGameBoard::bestYearlyProduction(const eResourceType type) const {
    const auto it = mYearlyProduction.find(type);
    if(it == mYearlyProduction.end()) return 0;
    return it->second.fBest;
}

void eGameBoard::incProduced(const eResourceType type,
                             const int by) {
    mYearlyProduction[type].fThisYear += by;
}

eDistrictIdTmp::eDistrictIdTmp(eGameBoard& board) :
    mBoard(board), mTmpId(board.currentDistrictId()) {}

eDistrictIdTmp::~eDistrictIdTmp() {
    mBoard.setCurrentDistrictId(mTmpId);
}
