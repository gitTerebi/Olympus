
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

eGameBoard::eGameBoard() :
    mThreadPool(*this) {
    const auto types = eResourceTypeHelpers::extractResourceTypes(
                           eResourceType::allBasic);
    for(const auto type : types) {
        mPrices[type] = eResourceTypeHelpers::defaultPrice(type);
    }
    mSupportedResources = eResourceType::allBasic;
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

    mAppealMap.initialize(w, h);

    updateNeighbours();
    updateMarbleTiles();
    scheduleTerrainUpdate();
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

    mAppealMap.initialize(w, h);

    updateNeighbours();
    updateMarbleTiles();
    scheduleTerrainUpdate();
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

void eGameBoard::scheduleAppealMapUpdate() {
    mUpdateAppeal = true;
}

void eGameBoard::updateAppealMapIfNeeded() {
    if(!mUpdateAppeal) return;
    mUpdateAppeal = false;
    const auto finish = [this](eHeatMap& map) {
        std::swap(appealMap(), map);
    };
    const auto task = new eHeatMapTask(eHeatGetters::appeal, finish);
    mThreadPool.queueTask(task);
}

void eGameBoard::enlistForces(const eEnlistedForces& forces) {
    for(const auto& b : forces.fSoldiers) {
        b->goAbroad();
    }
    for(const auto h : forces.fHeroes) {
        const auto hh = heroHall(h);
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

bool eGameBoard::supportsResource(const eResourceType rt) const {
    const auto s = mSupportedResources & rt;
    return static_cast<bool>(s);
}

eResourceType eGameBoard::supportedResources() const {
    return mSupportedResources;
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

void eGameBoard::setFriendlyGods(const eCityId cid,
                                 const std::vector<eGodType>& gods) {
    mFriendlyGods = gods;

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
                       eGameEventBranch::root);
    e->setIsEpisodeEvent(true);
    e->setGameBoard(this);
    eDate date = mDate;
    const int period = eNumbers::sFriendlyGodVisitPeriod;
    date += period;
    e->initializeDate(date, period, 10000);
    e->setTypes(gods);
    addRootGameEvent(e);
}

void eGameBoard::setHostileGods(const std::vector<eGodType>& gods) {
    mHostileGods = gods;

    const auto e = e::make_shared<eGodAttackEvent>(
                       eGameEventBranch::root);
    e->setIsEpisodeEvent(true);
    e->setGameBoard(this);
    eDate date = mDate;
    const int period = 900;
    date += period;
    e->initializeDate(date, period, 10000);
    e->setTypes(gods);
    addRootGameEvent(e);
}

void eGameBoard::setHostileMonsters(const std::vector<eMonsterType>& monsters) {
    mHostileMonsters = monsters;

//    const auto e = e::make_shared<eMonsterAttackEvent>(*this);
//    e->setTypes(monsters);
//    const int period = 150;
//    const auto date = mDate + period;
//    const auto ec = e::make_shared<eGameEventCycle>(
//                        e, date, period, 2, *this);
    //    addGameEvent(ec);
}

void eGameBoard::allowHero(const eCityId cid, const eHeroType heroType,
                           const std::string& reason) {
    const auto hallType = eHerosHall::sHeroTypeToHallType(heroType);
    allow(cid, hallType);
    const auto& inst = eMessages::instance;
    const auto hm = inst.heroMessages(heroType);
    if(!hm) return;
    eEventData ed;
    auto msg = hm->fHallAvailable;
    eStringHelpers::replaceAll(msg.fFull.fText, "[reason_phrase]",
                               reason.empty() ? msg.fNoReason : reason);
    showMessage(ed, msg);
}

void eGameBoard::planInvasion(const eDate& date,
                              const int infantry,
                              const int cavalry,
                              const int archers) {
    const auto e = e::make_shared<eInvasionEvent>(
                       eGameEventBranch::root);
    e->setGameBoard(this);
    e->setWorldBoard(mWorldBoard);
    const auto city = mWorldBoard->cities().front();
    e->initialize(city, infantry, cavalry, archers);
    e->initializeDate(date, 0, 1);
    addRootGameEvent(e);
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

eTile* eGameBoard::monsterTile(const int id) const {
    const auto b = banner(eBannerTypeS::monsterPoint, id);
    return b ? b->tile() : nullptr;
}

eTile* eGameBoard::landInvasionTile(const int id) const {
    const auto b = banner(eBannerTypeS::landInvasion, id);
    return b ? b->tile() : nullptr;
}

eTile* eGameBoard::disasterTile(const int id) const {
    const auto b = banner(eBannerTypeS::disasterPoint, id);
    return b ? b->tile() : nullptr;
}

const eGameBoard::eIV& eGameBoard::invasionHandlers() const {
    return mInvasionHandlers;
}

void eGameBoard::addInvasionHandler(eInvasionHandler* const i) {
    mInvasionHandlers.push_back(i);
    updateMusic();
}

void eGameBoard::removeInvasionHandler(eInvasionHandler* const i) {
    eVectorHelpers::remove(mInvasionHandlers, i);
    updateMusic();
}

bool eGameBoard::hasActiveInvasions() const {
    return !mInvasionHandlers.empty();
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
                       eGameEventBranch::root);
    e->setGameBoard(this);
    e->setWorldBoard(mWorldBoard);
    e->initialize(true, type, count, c);
    const auto date = mDate + delay;
    e->initializeDate(date);
    addRootGameEvent(e);
}

void eGameBoard::request(const stdsptr<eWorldCity>& c,
                         const eResourceType type) {
    const auto e = e::make_shared<eMakeRequestEvent>(
                       eGameEventBranch::root);
    e->setGameBoard(this);
    e->setWorldBoard(mWorldBoard);
    e->initialize(true, type, c);
    const auto date = mDate + 90;
    e->initializeDate(date);
    addRootGameEvent(e);
}

void eGameBoard::requestAid(const stdsptr<eWorldCity>& c) {
    const auto e = e::make_shared<eRequestAidEvent>(
                       eGameEventBranch::root);
    e->setGameBoard(this);
    e->setWorldBoard(mWorldBoard);
    e->setCity(c);
    const auto date = mDate + 30;
    e->initializeDate(date);
    addRootGameEvent(e);
}

void eGameBoard::tributeFrom(const stdsptr<eWorldCity>& c,
                             const bool postpone) {
    const auto type = c->tributeType();
    const auto count = c->tributeCount();

    eEventData ed;
    ed.fType = eMessageEventType::requestTributeGranted;
    ed.fCity = c;
    ed.fSpaceCount = spaceForResource(type);
    ed.fResourceType = c->tributeType();
    ed.fResourceCount = c->tributeCount();
    ed.fA0 = [this, c, type, count]() { // accept
        const int a = addResource(type, count);
        if(a == count) return;
        eEventData ed;
        ed.fType = eMessageEventType::resourceGranted;
        ed.fCity = c;
        ed.fResourceType = type;
        ed.fResourceCount = a;
        event(eEvent::tributeAccepted, ed);
    };
    if(postpone) {
        ed.fA1 = [this, c, type, count]() { // postpone
            eEventData ed;
            ed.fType = eMessageEventType::resourceGranted;
            ed.fCity = c;
            ed.fResourceType = type;
            ed.fResourceCount = count;
            event(eEvent::tributePostponed, ed);

            const auto e = e::make_shared<ePayTributeEvent>(
                               eGameEventBranch::root);
            e->setGameBoard(this);
            e->setWorldBoard(mWorldBoard);
            e->initialize(c);
            const auto date = mDate + 31;
            e->initializeDate(date);
            addRootGameEvent(e);
        };
    }
    ed.fA2 = [this, c, type, count]() { // decline
        eEventData ed;
        ed.fType = eMessageEventType::resourceGranted;
        ed.fCity = c;
        ed.fResourceType = type;
        ed.fResourceCount = count;
        event(eEvent::tributeDeclined, ed);
    };
    event(eEvent::tributePaid, ed);
}

void eGameBoard::giftTo(const stdsptr<eWorldCity>& c,
                        const eResourceType type,
                        const int count) {
    int remC = count;
    const auto cts = playerCities(mPersonPlayer);
    for(const auto cid : cts) {
        const auto c = boardCityWithId(cid);
        if(!c) continue;
        const int t = c->takeResource(type, remC);
        remC -= t;
        if(remC <= 0) break;
    }
    const auto e = e::make_shared<eGiftToEvent>(
                       eGameEventBranch::root);
    e->setGameBoard(this);
    e->setWorldBoard(mWorldBoard);
    e->initialize(c, type, count);
    const auto date = mDate + 90;
    e->initializeDate(date);
    addRootGameEvent(e);
}

void eGameBoard::giftToReceived(const stdsptr<eWorldCity>& c,
                                const eResourceType type,
                                const int count) {
    const bool a = c->acceptsGift(type, count);
    eEventData ed;
    ed.fType = eMessageEventType::resourceGranted;
    ed.fCity = c;
    ed.fResourceType = type;
    ed.fResourceCount = count;
    if(a) {
        const bool b = c->buys(type);
        const bool s = c->sells(type);
        if(type == eResourceType::drachmas) {
            event(eEvent::giftReceivedDrachmas, ed);
            c->incAttitude(10);
        } else if(b) {
            event(eEvent::giftReceivedNeeded, ed);
            c->incAttitude(10);
        } else if(s) {
            event(eEvent::giftReceivedSells, ed);
            c->incAttitude(5);
        } else {
            event(eEvent::giftReceivedNotNeeded, ed);
            c->incAttitude(5);
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

void eGameBoard::consolidateSoldiers(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->consolidateSoldiers();
}

bool eGameBoard::wasHeroSummoned(const eHeroType hero) const {
    return eVectorHelpers::contains(mSummonedHeroes, hero);
}

void eGameBoard::heroSummoned(const eHeroType hero) {
    mSummonedHeroes.push_back(hero);
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

ePlayerId eGameBoard::cityIdToPlayerId(const eCityId cid) const {
    if(cid == eCityId::neutralFriendly) {
        return ePlayerId::neutralFriendly;
    } else if(cid == eCityId::neutralAggresive) {
        return ePlayerId::neutralAggresive;
    }
    const auto it = mCityToPlayer.find(cid);
    if(it == mCityToPlayer.end()) {
        return ePlayerId::neutralFriendly;
    }
    return it->second;
}

eTeamId eGameBoard::cityIdToTeamId(const eCityId cid) const {
    const auto pid = cityIdToPlayerId(cid);
    return playerIdToTeamId(pid);
}

eTeamId eGameBoard::playerIdToTeamId(const ePlayerId pid) const {
    if(pid == ePlayerId::neutralFriendly) {
        return eTeamId::neutralFriendly;
    } else if(pid == ePlayerId::neutralAggresive) {
        return eTeamId::neutralAggresive;
    }
    const auto it = mPlayerToTeam.find(pid);
    if(it == mPlayerToTeam.end()) {
        return eTeamId::neutralFriendly;
    }
    return it->second;
}

void eGameBoard::moveCityToPlayer(const eCityId cid, const ePlayerId pid) {
    mCityToPlayer[cid] = pid;
}

std::vector<eCityId> eGameBoard::playerCities(const ePlayerId pid) const {
    if(pid == ePlayerId::neutralFriendly) {
        return {eCityId::neutralFriendly};
    } else if(pid == ePlayerId::neutralAggresive) {
        return {eCityId::neutralAggresive};
    }
    std::vector<eCityId> result;
    for(const auto it : mCityToPlayer) {
        if(it.second == pid) {
            result.push_back(it.first);
        }
    }
    return result;
}

std::vector<eCityId> eGameBoard::personPlayerCities() const {
    return playerCities(mPersonPlayer);
}

eBoardCity* eGameBoard::boardCityWithId(const eCityId cid) const {
    for(const auto& c : mCitiesOnBoard) {
        if(c->id() == cid) return c.get();
    }
    return nullptr;
}

eBoardPlayer* eGameBoard::boardPlayerWithId(const ePlayerId pid) const {
    for(const auto& p : mPlayersOnBoard) {
        if(p->id() == pid) return p.get();
    }
    return nullptr;
}

void eGameBoard::updateMaxSoldiers(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->updateMaxSoldiers();
}

void eGameBoard::addSoldier(const eCharacterType st,
                            const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->addSoldier(st);
}

void eGameBoard::removeSoldier(const eCharacterType st,
                               const eCityId cid,
                               const bool skipNotHome) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->removeSoldier(st, skipNotHome);
}

void eGameBoard::distributeSoldiers(const eCityId cid) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->distributeSoldiers();
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

eEnlistedForces eGameBoard::getEnlistableForces() const {
    eEnlistedForces result;

    for(const auto& s : mSoldierBanners) {
        if(s->count() <= 0) continue;
        switch(s->type()) {
        case eBannerType::hoplite:
        case eBannerType::horseman:
        case eBannerType::amazon:
        case eBannerType::aresWarrior:
            result.fSoldiers.push_back(s);
            break;
        default:
            break;
        }
    }

    for(const auto& h : mHeroHalls) {
        const auto s = h->stage();
        if(s != eHeroSummoningStage::arrived) continue;
        const auto ht = h->heroType();
        result.fHeroes.push_back(ht);
    }

    const auto& cts = mWorldBoard->cities();
    for(const auto& c : cts) {
        if(!c->active()) continue;
        const auto type = c->type();
        const auto rel = c->relationship();
        const bool e = type == eCityType::colony ||
                       (type == eCityType::foreignCity &&
                        (rel == eForeignCityRelationship::ally ||
                         rel == eForeignCityRelationship::vassal));
        if(e && c->attitude() > 50) {
            result.fAllies.push_back(c);
        }
    }

    return result;
}

void eGameBoard::addGodQuest(eGodQuestEvent* const q) {
    eVectorHelpers::remove(mGodQuests, q);
    mGodQuests.push_back(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::removeGodQuest(eGodQuestEvent* const q) {
    eVectorHelpers::remove(mGodQuests, q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::addCityRequest(eReceiveRequestEvent* const q) {
    mCityRequests.push_back(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::removeCityRequest(eReceiveRequestEvent* const q) {
    eVectorHelpers::remove(mCityRequests, q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::addCityTroopsRequest(eTroopsRequestEvent* const q) {
    mCityTroopsRequests.push_back(q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::removeCityTroopsRequest(eTroopsRequestEvent* const q) {
    eVectorHelpers::remove(mCityTroopsRequests, q);
    if(mRequestUpdateHandler) mRequestUpdateHandler();
}

void eGameBoard::addConquest(ePlayerConquestEventBase* const q) {
    mConquests.push_back(q);
}

void eGameBoard::removeConquest(ePlayerConquestEventBase* const q) {
    eVectorHelpers::remove(mConquests, q);
}

eInvasionEvent* eGameBoard::invasionToDefend() const {
    const auto date = eGameBoard::date();
    for(const auto i : mInvasions) {
        const int ip = i->invasionPoint();
        const auto t = landInvasionTile(ip);
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

void eGameBoard::addArmyEvent(eArmyEventBase* const q) {
    mArmyEvents.push_back(q);
}

void eGameBoard::removeArmyEvent(eArmyEventBase* const q) {
    eVectorHelpers::remove(mArmyEvents, q);
}

void eGameBoard::registerAttackingGod(eCharacter* const c) {
    mAttackingGods.push_back(c);
    updateMusic();
}

void eGameBoard::startPlague(eSmallHouse* const h) {
    const auto plague = std::make_shared<ePlague>(*this);
    plague->spreadFrom(h);
    mPlagues.push_back(plague);
    eEventData ed;
    ed.fTile = h->centerTile();
    event(eEvent::plague, ed);
}

stdsptr<ePlague> eGameBoard::plagueForHouse(eSmallHouse* const h) {
    for(const auto& p : mPlagues) {
        if(p->hasHouse(h)) return p;
    }
    return nullptr;
}

void eGameBoard::healPlague(const stdsptr<ePlague>& p) {
    p->healAll();
    eVectorHelpers::remove(mPlagues, p);
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

stdsptr<ePlague> eGameBoard::nearestPlague(
        const int tx, const int ty, int& dist) const {
    dist = __INT_MAX__/2;
    stdsptr<ePlague> result;
    for(const auto& p : mPlagues) {
        const auto& hs = p->houses();
        for(const auto h : hs) {
            const auto tt = h->centerTile();
            const int ttx = tt->x();
            const int tty = tt->y();
            const int d = sqrt((ttx - tx)*(ttx - tx) +
                               (tty - ty)*(tty - ty));
            if(d < dist) {
                dist = d;
                result = p;
            }
        }
    }
    return result;
}

void eGameBoard::updateMusic() {
    bool monsterActiveAttack = false;
    for(const auto m : mMonsters) {
        const auto a = m->action();
        if(const auto ma = dynamic_cast<eMonsterAction*>(a)) {
            const auto stage = ma->stage();
            if(stage == eMonsterAttackStage::none ||
               stage == eMonsterAttackStage::wait) {
                continue;
            }
            monsterActiveAttack = true;
            break;
        }
    }
    if(!monsterActiveAttack &&
       mInvasionHandlers.empty() &&
       mAttackingGods.empty()) {
        eMusic::playRandomMusic();
    } else {
        eMusic::playRandomBattleMusic();
    }
}

eTile* eGameBoard::entryPoint() const {
    for(const auto b : mBanners) {
        const auto type = b->type();
        if(type == eBannerTypeS::entryPoint) {
            return b->tile();
        }
    }
    return nullptr;
}

eTile* eGameBoard::exitPoint() const {
    for(const auto b : mBanners) {
        const auto type = b->type();
        if(type == eBannerTypeS::exitPoint) {
            return b->tile();
        }
    }
    return nullptr;
}

void eGameBoard::setPoseidonMode(const bool p) {
    mPoseidonMode = p;
    scheduleTerrainUpdate();
}

void eGameBoard::setWorldBoard(eWorldBoard* const wb) {
    mWorldBoard = wb;
    for(const auto& e : mGameEvents) {
        e->setWorldBoard(wb);
    }
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
    e->setGameBoard(this);
    e->setWorldBoard(mWorldBoard);
    mGameEvents.push_back(e);
}

void eGameBoard::removeRootGameEvent(const stdsptr<eGameEvent>& e) {
    eVectorHelpers::remove(mGameEvents, e);
}

void eGameBoard::addGameEvent(eGameEvent* const e) {
    mAllGameEvents.push_back(e);
}

void eGameBoard::removeGameEvent(eGameEvent* const e) {
    eVectorHelpers::remove(mAllGameEvents, e);
}

void eGameBoard::updateCoverage() {
    int totalPeople = 0;
    int commonPeople = 0;
    int sport = 0;
    int phil = 0;
    int drama = 0;
    int taxes = 0;
    double totalUnrest = 0;
    int totalSatisfaction = 0;
    int totalHygiene = 0;
    for(const auto b : mTimedBuildings) {
        if(const auto h = dynamic_cast<eHouseBase*>(b)) {
            const int p = h->people();
            if(h->athletesScholars() > 0) {
                sport += p;
            }
            if(h->philosophersInventors() > 0) {
                phil += p;
            }
            if(h->actorsAstronomers() > 0) {
                drama += p;
            }
            if(h->paidTaxes()) {
                taxes += p;
            }
            if(const auto ch = dynamic_cast<eSmallHouse*>(b)) {
                if(ch->disgruntled()) totalUnrest += p;
                totalSatisfaction += p*ch->satisfaction();
                totalHygiene += p*ch->hygiene();
                commonPeople += p;
            }
            totalPeople += p;
        }
    }
    if(totalPeople <= 0) {
        mAthleticsCoverage = 0;
        mPhilosophyCoverage = 0;
        mDramaCoverage = 0;
        mTaxesCoverage = 0;
    } else {
        mAthleticsCoverage = std::round(100.*sport/totalPeople);
        mPhilosophyCoverage = std::round(100.*phil/totalPeople);
        mDramaCoverage = std::round(100.*drama/totalPeople);
        mTaxesCoverage = std::round(100.*taxes/totalPeople);
    }
    mAllDiscCoverage = (mAthleticsCoverage + mPhilosophyCoverage + mDramaCoverage)/3;

    if(commonPeople == 0) {
        mUnrest = 0;
        mPopularity = 100;
        mHealth = 100;
    } else {
        mUnrest = std::round(100.*totalUnrest/commonPeople);
        mPopularity = std::round(1.*totalSatisfaction/commonPeople);
        mHealth = std::round(1.*totalHygiene/commonPeople);
    }
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

    eEventData ed;
    const auto pcids = playerCities(mPersonPlayer);
    for(const auto& cid : pcids) {
        const auto c = boardCityWithId(cid);
        const double chance = c->winningChance(game);
        ed.fCityId = cid;
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
    const auto ppid = eGameBoard::personPlayer();
    for(const auto& c : mCitiesOnBoard) {
        const auto id = c->id();
        const double chance = c->winningChance(game);
        chances.push_back(eCityChance{id, chance});
    }
    std::random_shuffle(chances.begin(), chances.end());

    eCityId winner = eCityId::neutralFriendly;
    eEventData ed;
    for(const auto& c : chances) {
        const bool won = eRand::rand() % 101 < 100*c.second;
        if(!won) continue;
        const auto cid = c.first;
        const auto city = boardCityWithId(cid);
        const auto player = cityIdToPlayerId(cid);
        const bool personPlayer = ppid == player;
        if(won) {
            winner = cid;
            if(personPlayer) {
                ed.fCityId = cid;
                showMessage(ed, msgs->fWon);
            }
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
    for(const auto& c : mCitiesOnBoard) {
        const auto cid = c->id();
        if(cid == winner) continue;
        const auto player = cityIdToPlayerId(cid);
        if(player != ppid) continue;
        const double chance = c->winningChance(game);
        const bool second = eRand::rand() % 101 < 200*chance;
        ed.fCityId = cid;
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

void eGameBoard::registerCharacter(eCharacter* const c) {
    mCharacters.push_back(c);
}

bool eGameBoard::unregisterCharacter(eCharacter* const c) {
    const bool r = eVectorHelpers::remove(mAttackingGods, c);
    if(r) updateMusic();
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
    if(bt == eBuildingType::commemorative ||
       bt == eBuildingType::godMonument) {
        mCommemorativeBuildings.push_back(b);
    }
    scheduleAppealMapUpdate();

    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    if(!city) return;
    city->registerBuilding(b);
}

bool eGameBoard::unregisterBuilding(eBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    eVectorHelpers::remove(mAllBuildings, b);
    eVectorHelpers::remove(mTimedBuildings, b);
    eVectorHelpers::remove(mCommemorativeBuildings, b);
    scheduleAppealMapUpdate();
    return true;
}

bool eGameBoard::unregisterCommonHouse(eSmallHouse* const ch) {
    const auto p = plagueForHouse(ch);
    if(p) {
        p->removeHouse(ch);
        const int c = p->houseCount();
        if(c <= 0) healPlague(p);
    }
    return true;
}

void eGameBoard::registerEmplBuilding(eEmployingBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return;
    mEmployingBuildings.push_back(b);
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    city->registerEmplBuilding(b);
}

bool eGameBoard::unregisterEmplBuilding(eEmployingBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    const bool rr = eVectorHelpers::remove(mEmployingBuildings, b);
    const auto cid = b->cityId();
    const auto city = boardCityWithId(cid);
    city->unregisterEmplBuilding(b);
    return rr;
}

void eGameBoard::registerTradePost(eTradePost* const b) {
    if(!mRegisterBuildingsEnabled) return;
    mTradePosts.push_back(b);
    if(mButtonVisUpdater) mButtonVisUpdater();
}

bool eGameBoard::unregisterTradePost(eTradePost* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    const bool r = eVectorHelpers::remove(mTradePosts, b);
    if(r && mButtonVisUpdater) mButtonVisUpdater();
    return r;
}

bool eGameBoard::hasTradePost(const eWorldCity& city) {
    for(const auto t : mTradePosts) {
        const bool r = &t->city() == &city;
        if(r) return true;
    }
    return false;
}

void eGameBoard::registerSpawner(eSpawner* const s) {
    mSpawners.push_back(s);
}

bool eGameBoard::unregisterSpawner(eSpawner* const s) {
    return eVectorHelpers::remove(mSpawners, s);;
}

void eGameBoard::registerStadium(eBuilding* const s) {
    if(!mRegisterBuildingsEnabled) return;
    mStadium = s;
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::unregisterStadium() {
    if(!mRegisterBuildingsEnabled) return;
    mStadium = nullptr;
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::registerMuseum(eBuilding* const s) {
    if(!mRegisterBuildingsEnabled) return;
    mMuseum = s;
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::unregisterMuseum() {
    if(!mRegisterBuildingsEnabled) return;
    mMuseum = nullptr;
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::registerStorBuilding(eStorageBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return;
    mStorBuildings.push_back(b);
}

bool eGameBoard::unregisterStorBuilding(eStorageBuilding* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    return eVectorHelpers::remove(mStorBuildings, b);
    return true;
}

void eGameBoard::registerSanctuary(eSanctuary* const b) {
    if(!mRegisterBuildingsEnabled) return;
    mSanctuaries.push_back(b);
}

bool eGameBoard::unregisterSanctuary(eSanctuary* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    return eVectorHelpers::remove(mSanctuaries, b);
    return true;
}

void eGameBoard::registerHeroHall(eHerosHall* const b) {
    if(!mRegisterBuildingsEnabled) return;
    mHeroHalls.push_back(b);
}

bool eGameBoard::unregisterHeroHall(eHerosHall* const b) {
    if(!mRegisterBuildingsEnabled) return false;
    return eVectorHelpers::remove(mHeroHalls, b);
    return true;
}

void eGameBoard::registerMissile(eMissile* const m) {
    mMissiles.push_back(m);
}

bool eGameBoard::unregisterMissile(eMissile* const m) {
    return eVectorHelpers::remove(mMissiles, m);
}

eStadium* eGameBoard::stadium() const {
    return static_cast<eStadium*>(mStadium);
}

eMuseum* eGameBoard::museum() const {
    return static_cast<eMuseum*>(mMuseum);
}

void eGameBoard::registerPalace(ePalace* const p) {
    if(!mRegisterBuildingsEnabled) return;
    mPalace = p;
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::unregisterPalace(const eCityId cid) {
    if(!mRegisterBuildingsEnabled) return;
    mPalace = nullptr;
    updateMaxSoldiers(cid);
    distributeSoldiers(cid);
    consolidateSoldiers(cid);
    if(mButtonVisUpdater) mButtonVisUpdater();
}

void eGameBoard::registerMonster(eMonster* const m) {
    mMonsters.push_back(m);
}

void eGameBoard::unregisterMonster(eMonster* const m) {
    eVectorHelpers::remove(mMonsters, m);
    updateMusic();
}

eBanner* eGameBoard::banner(const eBannerTypeS type, const int id) const {
    for(const auto b : mBanners) {
        const int bid = b->id();
        if(bid != id) continue;
        const auto btype = b->type();
        if(btype != type) continue;
        return b;
    }
    return nullptr;
}

void eGameBoard::registerBanner(eBanner* const b) {
    const int id = b->id();
    const auto type = b->type();
    const auto bb = banner(type, id);
    const auto t = bb ? bb->tile() : nullptr;
    if(t) t->setBanner(nullptr);
    mBanners.push_back(b);
}

void eGameBoard::unregisterBanner(eBanner* const b) {
    eVectorHelpers::remove(mBanners, b);
}

void eGameBoard::registerAllSoldierBanner(eSoldierBanner* const b) {
    mAllSoldierBanners.push_back(b);
}

void eGameBoard::unregisterAllSoldierBanner(eSoldierBanner* const b) {
    eVectorHelpers::remove(mAllSoldierBanners, b);
}

std::vector<eAgoraBase*> eGameBoard::agoras() const {
    std::vector<eAgoraBase*> r;
    for(const auto b : mAllBuildings) {
        const auto a = dynamic_cast<eAgoraBase*>(b);
        if(!a) continue;
        r.push_back(a);
    }
    return r;
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
            const auto cid = c->id();
            updateMaxSoldiers(cid);
            distributeSoldiers(cid);
            consolidateSoldiers(cid);
        }
    }

    for(const auto i : mInvasionHandlers) {
        i->incTime(by);
    }

    for(const auto& c : mCitiesOnBoard) {
        c->incTime(by);
    }

    mTime += by;
    mTotalTime += by;
    bool nextMonth = false;
    bool nextYear = false;
    const int nd = mTime/dayLen;
    mDate.nextDays(nd, nextMonth, nextYear);
    mTime -= nd*dayLen;

    for(int i = 0; i < (int)mGameEvents.size(); i++) {
        const auto& e = mGameEvents[i];
        if(e->finished() && !e->hasActiveConsequences()) {
            eVectorHelpers::remove(mGameEvents, e);
            i--;
        } else {
            e->handleNewDate(mDate);
        }
    }

    if(nextYear) {
        mTaxesPaidLastYear = mTaxesPaidThisYear;
        mTaxesPaidThisYear = 0;
        mPeoplePaidTaxesLastYear = mPeoplePaidTaxesThisYear;
        mPeoplePaidTaxesThisYear = 0;

        for(const auto& c : mDefeatedBy) {
            if(!c->isRival()) continue;
            const auto rr = e::make_shared<eReceiveRequestEvent>(
                                eGameEventBranch::root);
            const auto type = c->recTributeType();
            const int count = c->recTributeCount();
            rr->initialize(0, type, count, c, false);
            rr->setTributeRequest(true);
            rr->initializeDate(mDate);
            rr->setGameBoard(this);
            rr->setWorldBoard(mWorldBoard);
            addRootGameEvent(rr);
        }
    }
    if(nextMonth) {
        for(const auto& c : mCitiesOnBoard) {
            c->nextMonth();
        }
        for(const auto& p : mPlayersOnBoard) {
            p->nextMonth();
        }

        if(!mPoseidonMode) {
            const auto m = mDate.month();
            const int ng = std::abs(mDate.year() % 4);
            const auto game = static_cast<eGames>(ng);
            if(m == eMonth::june) {
                handleGamesBegin(game);
            } else if(m == eMonth::august) {
                handleGamesEnd(game);
            }
        }
    }

    const int ect = 5000;
    mEmploymentCheckTime += by;
    if(mEmploymentCheckTime > ect) {
        mEmploymentCheckTime -= ect;
        const double employable = mEmplData.employable();
        const double jobVacs = mEmplData.totalJobVacancies();
        int emplState = 0;
        if(employable < jobVacs*0.75) {
            emplState = 1;
        } else if(employable > jobVacs*1.25) {
            emplState = -1;
        } else {
            emplState = 0;
        }
        if(mLastEmploymentState != emplState) {
            const auto& inst = eMessages::instance;
            eEventData ed;
            if(emplState == -1) { // unemployed
                showMessage(ed, inst.fUnemployment);
            } else if(emplState == 1) { // employed
                showMessage(ed, inst.fEmployees);
            }
        }
        mLastEmploymentState = emplState;
    }

    if(nextMonth) {
        mWorldBoard->nextMonth(this);
    }
    if(nextYear) {
        mWorldBoard->nextYear();
        const auto cs = mWorldBoard->getTribute();
        for(const auto& c : cs) {
            if(c->conqueredByRival()) continue;
            tributeFrom(c, true);
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

    for(const auto& c : mCitiesOnBoard) {
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

void eGameBoard::payTaxes(const eCityId cid, const int d, const int people) {
    const auto c = boardCityWithId(cid);
    if(!c) return;
    c->payTaxes(d, people);
    const auto pid = cityIdToPlayerId(cid);
    const auto p = boardPlayerWithId(pid);
    if(!p) return;
    p->incDrachmas(d);
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

void eGameBoard::showTip(const std::string& tip) const {
    if(mTipShower) mTipShower(tip);
}

void eGameBoard::setEnlistForcesRequest(const eEnlistRequest& req) {
    mEnlistRequester = req;
}

void eGameBoard::requestForces(const eEnlistAction& action,
                               const std::vector<eResourceType>& plunderResources,
                               const std::vector<stdsptr<eWorldCity>>& exclude) {
    if(mEnlistRequester) {
        auto f = getEnlistableForces();
        std::vector<bool> heroesAbroad;
        for(const auto h : f.fHeroes) {
            const auto hh = heroHall(eCityId::city0, h);
            const bool abroad = !hh ? true : hh->heroOnQuest();
            heroesAbroad.push_back(abroad);
        }
        for(const auto& e : exclude) {
            eVectorHelpers::remove(f.fAllies, e);
        }
        mEnlistRequester(f, heroesAbroad, action, plunderResources);
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

const std::vector<eSanctuary*>* eGameBoard::sanctuaries(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return &c->sanctuaries();
}

eSanctuary* eGameBoard::sanctuary(const eCityId cid, const eGodType god) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->sanctuary(god);
}

const std::vector<eHerosHall*>* eGameBoard::heroHalls(const eCityId cid) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return &c->heroHalls();
}

eHerosHall* eGameBoard::heroHall(const eCityId cid, const eHeroType hero) const {
    const auto c = boardCityWithId(cid);
    if(!c) return nullptr;
    return c->heroHall(hero);
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

void eGameBoard::addFulfilledQuest(const eGodQuest q) {
    mFulfilledQuests.push_back(q);
}

void eGameBoard::addSlayedMonster(const eMonsterType m) {
    mSlayedMonsters.push_back(m);
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
    for(int i = 0; i < static_cast<int>(mGameEvents.size()); i++) {
        const auto& e = mGameEvents[i];
        const auto type = e->type();
        if(type == eGameEventType::godQuest) continue;
        e->startingNewEpisode();
        if(e->finished()) {
            if(!e->hasActiveConsequences()) {
                mGameEvents.erase(mGameEvents.begin() + i);
                i--;
            }
        }
    }
    mGoals.clear();
    mWorldBoard = e->fWorldBoard;
    const auto& date = e->fStartDate;
    setDate(date);
    mDrachmas = e->fDrachmas;
    mWageMultiplier = e->fWageMultiplier;
    mPrices = e->fPrices;
    const auto& es = e->fEvents;
    for(const auto& ee : es) {
        const auto eee = ee->makeCopy();
        eee->setupStartDate(date);
        mGameEvents.push_back(eee);
    }
    mAvailableBuildings.startEpisode(e->fAvailableBuildings);
    setFriendlyGods(e->fFriendlyGods);
    mPoseidonMode = e->fAtlantean;
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

void eGameBoard::musterAllSoldiers() {
    for(const auto& s : mSoldierBanners) {
        s->backFromHome();
    }
}

void eGameBoard::sendAllSoldiersHome() {
    for(const auto& s : mSoldierBanners) {
        s->goHome();
    }
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

eMilitaryAid* eGameBoard::militaryAid(const stdsptr<eWorldCity>& c) const {
    for(const auto& m : mMilitaryAid) {
        if(m->fCity == c) return m.get();
    }
    return nullptr;
}

void eGameBoard::removeMilitaryAid(const stdsptr<eWorldCity>& c) {
    const int iMax = mMilitaryAid.size();
    for(int i = 0; i < iMax; i++) {
        const auto& m = mMilitaryAid[i];
        if(m->fCity != c) continue;
        mMilitaryAid.erase(mMilitaryAid.begin() + i);
        break;
    }
}

void eGameBoard::addMilitaryAid(const stdsptr<eMilitaryAid>& a) {
    mMilitaryAid.push_back(a);
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

void eGameBoard::defeatedBy(const stdsptr<eWorldCity>& c) {
    const bool r = eVectorHelpers::contains(mDefeatedBy, c);
    if(r) {
        setEpisodeLost();
    } else {
        mDefeatedBy.push_back(c);
    }
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
    scheduleTerrainUpdate();
}
