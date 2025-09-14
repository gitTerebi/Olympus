#include "etradepost.h"

#include "textures/egametextures.h"

#include "engine/egameboard.h"

#include "characters/etrader.h"
#include "characters/actions/etraderaction.h"
#include "characters/etradeboat.h"
#include "enumbers.h"

eTradePost::eTradePost(eGameBoard& board, eWorldCity& city,
                       const eCityId cid,
                       const eTradePostType type) :
    eWarehouseBase(board, eBuildingType::tradePost, 4, 4, 24,
                   eResourceType::tradePost, cid, 15),
    mCity(city), mType(type) {
    eGameTextures::loadTradingPost();
    setOverlayEnabledFunc([]() { return true; });
    setOrders(eResourceType::none, eResourceType::none);
    getBoard().registerTradePost(this);

    switch(type) {
    case eTradePostType::pier: {
        setCharacterCreator([](eTile* const tile, eGameBoard& board) {
            const auto r = e::make_shared<eTradeBoat>(board);
            r->changeTile(tile);
            return r;
        });
        setWalkable(eWalkableObject::sCreate(eWalkableObjectType::deepWater));;
    } break;
    case eTradePostType::post: {
        setCharacterCreator([](eTile* const tile, eGameBoard& board) {
            const auto r = e::make_shared<eTrader>(board);
            r->changeTile(tile);
            r->createFollowers();
            return r;
        });
    } break;
    }
}

eTradePost::~eTradePost() {
    getBoard().unregisterTradePost(this);
}

std::shared_ptr<eTexture> eTradePost::getTexture(const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& coll = blds[sizeId];
    switch(mType) {
    case eTradePostType::post:
        return coll.fTradingPost;
    default:
        return coll.fPier2;
    }

}

std::vector<eOverlay> eTradePost::getOverlays(const eTileSize size) const {
    std::vector<eOverlay> os;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& texs = blds[sizeId];
    if(enabled()) {
        const auto& coll = texs.fTradingPostOverlay;
        const int texId = textureTime() % coll.size();
        auto& o = os.emplace_back();
        o.fTex = coll.getTexture(texId);
        switch(mType) {
        case eTradePostType::post:
            o.fX = -3.1;
            o.fY = -7.2;
            break;
        default:
            o.fX = -1.75;
            o.fY = -5.85;
            break;
        }
    }
    const eXY xy = {{-1.5, -3.5},
                    {-1.5, -2.5},
                    {-1.5, -1.5},

                    {-0.5, -4.5},
                    {-0.5, -3.5},
                    {-0.5, -2.5},
                    {-0.5, -1.5},

                    {0.5, -4.5},
                    {0.5, -3.5},
                    {0.5, -2.5},
                    {0.5, -1.5},

                    {1.5, -4.5},
                    {1.5, -3.5},
                    {1.5, -2.5},
                    {1.5, -1.5}};

    getSpaceOverlays(size, os, xy);

    return os;
}

void eTradePost::timeChanged(const int by) {
    mRouteTimer += by;
    if(mRouteTimer > eNumbers::sTraderSpawnPeriod) {
        mRouteTimer = 0;
        if(trades()) spawnTrader();
    }
    eWarehouseBase::timeChanged(by);
}

void eTradePost::erase() {
    if(mUnpackBuilding) {
        mUnpackBuilding->eBuilding::erase();
    }
    eBuilding::erase();
}

void eTradePost::setOrders(const eResourceType imports,
                           const eResourceType exports) {
    mImports = imports;
    mExports = exports;

    eStorageBuilding::setOrders(mExports, mImports, eResourceType::none);
}

void eTradePost::getOrders(eResourceType& imports,
                           eResourceType& exports) const {
    imports = mImports;
    exports = mExports;
}

eTile* eTradePost::entryPoint() const {
    auto& board = getBoard();
    const auto cid = cityId();
    switch(mType) {
    case eTradePostType::post:
        return board.entryPoint(cid);
    case eTradePostType::pier:
        return board.riverEntryPoint(cid);
    default:
        return nullptr;
    }
}

void eTradePost::spawnTrader() {
    const auto entryPoint = eTradePost::entryPoint();
    auto& board = getBoard();

    const auto r = mCharGen(entryPoint, board);
    r->setBothCityIds(cityId());

    const auto ta = e::make_shared<eTraderAction>(r.get());
    ta->setFinishOnComeback(true);
    ta->setTradePost(this);
    ta->setUnpackBuilding(mUnpackBuilding);
    ta->setWalkable(eWalkableObject::sCreateRect(
                        mUnpackBuilding, mWalkable));
    r->setAction(ta);
}

bool eTradePost::playerTwoWay() const {
    auto& brd = getBoard();
    const auto targetCid = mCity.cityId();
    const auto targetPid = brd.cityIdToPlayerId(targetCid);
    const bool targetOnBoard = brd.boardCityWithId(targetCid);
    const auto thisPid = playerId();
    return targetOnBoard && targetPid == thisPid;
}

int eTradePost::buy(const int cash) {
    if(!trades()) return 0;
    int spent = 0;
    auto& brd = getBoard();
    const auto targetCid = mCity.cityId();
    const auto targetPid = brd.cityIdToPlayerId(targetCid);
    const bool targetOnBoard = brd.boardCityWithId(targetCid);
    const auto thisCid = cityId();
    const auto thisPid = playerId();
    if(targetOnBoard && targetPid == thisPid) {
        const auto thisC = brd.boardCityWithId(thisCid);
        const auto es = eResourceTypeHelpers::extractResourceTypes(mExports);
        for(const auto e : es) {
            const int price = brd.price(e);
            if(price > cash) continue;
            const int c = count(e);
            if(c <= 0) continue;
            const int max = e == eResourceType::sculpture ? eNumbers::sTwoWayTradeMax :
                                                            4*eNumbers::sTwoWayTradeMax;
            if(thisC->exported(targetCid, e) > max) continue;
            take(e, 1);
            thisC->addExported(targetCid, e, 1);
            spent += price;
        }
    } else {
        for(auto& b : mCity.buys()) {
            const auto expt = mExports & b.fType;
            const int price = brd.price(b.fType);
            const bool exp = static_cast<bool>(expt);
            if(!exp) continue;
            if(b.used(thisPid) >= b.fMax) continue;
            if(price > cash) continue;
            const int c = count(b.fType);
            if(c <= 0) continue;
            take(b.fType, 1);
            b.incUsed(thisPid, 1);
            spent += price;
        }
        const auto pid = playerId();
        brd.incDrachmas(pid, spent, eFinanceTarget::exports);
    }
    return spent;
}

int eTradePost::sell(const int items) {
    if(!trades()) return 0;
    int earned = 0;
    auto& brd = getBoard();
    const auto srcCid = mCity.cityId();
    const auto srcPid = brd.cityIdToPlayerId(srcCid);
    const bool targetOnBoard = brd.boardCityWithId(srcCid);
    const auto thisCid = cityId();
    const auto thisPid = playerId();
    if(targetOnBoard && srcPid == thisPid) {
        const auto srcC = brd.boardCityWithId(srcCid);
        const auto es = srcC->exported(thisCid);
        for(const auto& e : es) {
            if(e.second < 1) continue;
            const int price = brd.price(e.first);
            if(price > items) continue;
            const int c = spaceLeftDontAccept(e.first);
            if(c <= 0) continue;
            addNotAccept(e.first, 1);
            srcC->removeExported(thisCid, e.first, 1);
            earned += price;
        }
    } else {
        for(auto& b : mCity.sells()) {
            const auto impt = mImports & b.fType;
            const bool imp = static_cast<bool>(impt);
            const int price = brd.price(b.fType);
            if(!imp) continue;
            if(b.used(thisPid) >= b.fMax) continue;
            if(price > items) continue;
            const int c = spaceLeftDontAccept(b.fType);
            if(c <= 0) continue;
            addNotAccept(b.fType, 1);
            b.incUsed(thisPid, 1);
            earned += price;
        }
        const auto pid = playerId();
        brd.incDrachmas(pid, -earned, eFinanceTarget::importCosts);
    }
    return earned;
}

void eTradePost::setWalkable(const stdsptr<eWalkableObject>& w) {
    mWalkable = w;
}

void eTradePost::setUnpackBuilding(eBuilding* const b) {
    mUnpackBuilding = b;
}

void eTradePost::setOrientation(const eDiagonalOrientation o) {
    mO = o;
}

eDiagonalOrientation eTradePost::orientation() const {
    return mO;
}

void eTradePost::setCharacterCreator(const eCharacterCreator& c) {
    mCharGen = c;
}

void eTradePost::read(eReadStream& src) {
    eStorageBuilding::read(src);

    src >> mImports;
    src >> mExports;
    src >> mRouteTimer;
}

void eTradePost::write(eWriteStream& dst) const {
    eStorageBuilding::write(dst);

    dst << mImports;
    dst << mExports;
    dst << mRouteTimer;
}

bool eTradePost::trades() const {
    if(!mCity.trades()) return false;
    auto& board = getBoard();
    const auto cid = cityId();
    if(mType == eTradePostType::pier) {
        return !board.seaTradeShutdown(cid);
    } else {
        return !board.landTradeShutdown(cid);
    }
}
