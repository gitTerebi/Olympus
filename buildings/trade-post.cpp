#include "trade-post.h"
#include "fileIO/esavearchive.h"

#include "textures/egametextures.h"

#include "engine/e-game-board.h"
#include "etilehelper.h"

#include "characters/trader.h"
#include "characters/actions/trader-action.h"
#include "characters/etradeboat.h"
#include "enumbers.h"

#include <algorithm>
#include <vector>

namespace {

struct TradePostSlot {
    int fX = 0;
    int fY = 0;
};

struct TradePostDrawSlot {
    TradePostSlot fSlot;
    int fStorageId = -1;
    bool fDoor = false;
};

TradePostSlot sTradePostHomeDoorSlot() {
    return {0, 0};
}

std::vector<TradePostSlot> sTradePostStorageRealSlots() {
    return {{0, 1},
            {0, 2},
            {0, 3},
            {1, 0},
            {1, 1},
            {1, 2},
            {1, 3},
            {2, 0},
            {2, 1},
            {2, 2},
            {2, 3},
            {3, 0},
            {3, 1},
            {3, 2},
            {3, 3}};
}

std::pair<double, double> sTradePostSlotXY(const TradePostSlot& slot) {
    return {double(slot.fX) - 1.5, double(slot.fY) - 4.5};
}

std::pair<double, double> sTradePostSlotShiftFromHome(
        const TradePostSlot& slot) {
    const auto homeSlot = sTradePostHomeDoorSlot();
    return {double(slot.fX - homeSlot.fX),
            double(slot.fY - homeSlot.fY)};
}

TradePostSlot sTradePostSlotFromLocalTile(const int x, const int y) {
    return {x, y};
}

TradePostSlot sTradePostSlotFromRealTile(const SDL_Rect& rect,
                                          const eWorldDirection dir,
                                          const int boardW,
                                          const int boardH,
                                          SDL_Rect& rotatedRect) {
    const SDL_Point realDoor{rect.x, rect.y};
    SDL_Point rotatedDoor;
    rotatedRect = eTileHelper::toRotatedRect(rect, dir, boardW, boardH);
    eTileHelper::tileIdToRotatedTileId(realDoor.x, realDoor.y,
                                       rotatedDoor.x, rotatedDoor.y,
                                       dir, boardW, boardH);
    return sTradePostSlotFromLocalTile(rotatedDoor.x - rotatedRect.x,
                                       rotatedDoor.y - rotatedRect.y);
}

TradePostSlot sTradePostRotatedSlotFromRealSlot(
        const SDL_Rect& rect,
        const SDL_Rect& rotatedRect,
        const eWorldDirection dir,
        const int boardW,
        const int boardH,
        const TradePostSlot& slot) {
    const int tileX = rect.x + slot.fX;
    const int tileY = rect.y + slot.fY;
    int rotatedX;
    int rotatedY;
    eTileHelper::tileIdToRotatedTileId(tileX, tileY,
                                       rotatedX, rotatedY,
                                       dir, boardW, boardH);
    return sTradePostSlotFromLocalTile(rotatedX - rotatedRect.x,
                                       rotatedY - rotatedRect.y);
}

std::vector<TradePostDrawSlot> sTradePostStorageSlots(
        const SDL_Rect& rect,
        const SDL_Rect& rotatedRect,
        const eWorldDirection dir,
        const int boardW,
        const int boardH) {
    std::vector<TradePostDrawSlot> result;
    int storageId = 0;
    for(const auto& realSlot : sTradePostStorageRealSlots()) {
        auto& slot = result.emplace_back();
        slot.fSlot = sTradePostRotatedSlotFromRealSlot(rect, rotatedRect,
                                                       dir, boardW, boardH,
                                                       realSlot);
        slot.fStorageId = storageId++;
    }
    return result;
}

void sSortTradePostDrawSlots(std::vector<TradePostDrawSlot>& slots) {
    std::sort(slots.begin(), slots.end(),
              [](const TradePostDrawSlot& a,
                 const TradePostDrawSlot& b) {
        if(a.fSlot.fY != b.fSlot.fY) {
            return a.fSlot.fY < b.fSlot.fY;
        }
        return a.fSlot.fX < b.fSlot.fX;
    });
}

} // namespace

TradePost::TradePost(GameBoard& board, eWorldCity& city,
                       const eCityId cid,
                       const eTradePostType type) :
    WarehouseBase(board, eBuildingType::tradePost, 4, 4, 24,
                   eResourceType::tradePost, cid, 15),
    mCity(city), mType(type) {
    eGameTextures::loadTradingPost();
    if(type == eTradePostType::pier) eGameTextures::loadPier();
    setOverlayEnabledFunc([]() { return true; });
    setOrders(eResourceType::none, eResourceType::none);
    getBoard().registerTradePost(this);

    switch(type) {
    case eTradePostType::pier: {
        setCharacterCreator([](eTile* const tile, GameBoard& board) {
            const auto r = e::make_shared<eTradeBoat>(board);
            r->changeTile(tile);
            return r;
        });
        setWalkable(eWalkableObject::sCreate(eWalkableObjectType::deepWater));;
    } break;
    case eTradePostType::post: {
        setCharacterCreator([](eTile* const tile, GameBoard& board) {
            const auto r = e::make_shared<Trader>(board);
            r->changeTile(tile);
            return r;
        });
    } break;
    }
}

TradePost::~TradePost() {
    ownerBoard().unregisterTradePost(this);
}

std::shared_ptr<eTexture> TradePost::getTexture(const eTileSize size) const {
    (void)size;
    return nullptr;

}

eTextureSpace TradePost::getTextureSpace(const int tx, const int ty,
                                          const eTileSize size) const {
    auto result = eBuilding::getTextureSpace(tx, ty, size);
    result.fClamp = false;
    return result;
}

std::vector<eOverlay> TradePost::getOverlays(const eTileSize size) const {
    std::vector<eOverlay> os;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& texs = blds[sizeId];
    const eWorldDirection dir = getBoard().direction();
    const auto rect = tileRect();
    const int boardW = getBoard().width();
    const int boardH = getBoard().height();
    SDL_Rect rotatedRect;
    const auto doorSlot = sTradePostSlotFromRealTile(rect, dir, boardW,
                                                     boardH, rotatedRect);
    const auto doorSlotShift = sTradePostSlotShiftFromHome(doorSlot);
    auto drawSlots = sTradePostStorageSlots(rect, rotatedRect, dir,
                                            boardW, boardH);
    auto& doorSlotInfo = drawSlots.emplace_back();
    doorSlotInfo.fSlot = doorSlot;
    doorSlotInfo.fDoor = true;
    sSortTradePostDrawSlots(drawSlots);

    for(const auto& slot : drawSlots) {
        if(slot.fDoor) {
            auto& door = os.emplace_back();
            door.fTex = mType == eTradePostType::post ?
                        texs.fTradingPost : texs.fPier2;
            door.fX = doorSlotShift.first;
            door.fY = doorSlotShift.second;
            door.fAlignTop = true;
            if(enabled() && mType == eTradePostType::post) {
                const auto& coll = texs.fTradingPostOverlay;
                const int texId = textureTime() % coll.size();
                auto& man = os.emplace_back();
                man.fTex = coll.getTexture(texId);
                man.fX = doorSlotShift.first - 3.1;
                man.fY = doorSlotShift.second - 7.2;
            }
        } else {
            getSpaceOverlay(size, os, sTradePostSlotXY(slot.fSlot),
                            slot.fStorageId);
        }
    }

    return os;
}

void TradePost::timeChanged(const int by) {
    mRouteTimer += by;
    if(mRouteTimer > eNumbers::sTraderSpawnPeriod) {
        mRouteTimer = 0;
        if(trades()) spawnTrader();
    }
    WarehouseBase::timeChanged(by);
}

void TradePost::erase() {
    if(mUnpackBuilding) {
        mUnpackBuilding->eBuilding::erase();
    }
    eBuilding::erase();
}

void TradePost::setOrders(const eResourceType imports,
                           const eResourceType exports,
                           const eResourceType empty,
                           const eResourceType cartGet,
                           const eResourceType cartAccept,
                           const eResourceType cartDontAccept) {
    mImports = imports;
    mExports = exports;
    mCartEmpty = empty;
    mCartGet = cartGet;
    mCartDontAccept = cartDontAccept;
    mCartAccept = cartAccept | (exports & ~(empty | cartGet | cartDontAccept));

    eStorageBuilding::setOrders(mCartGet, mCartEmpty, mCartAccept);
}

void TradePost::getOrders(eResourceType& imports,
                           eResourceType& exports,
                           eResourceType& empty,
                           eResourceType& cartGet,
                           eResourceType& cartAccept,
                           eResourceType& cartDontAccept) const {
    imports = mImports;
    exports = mExports;
    empty = mCartEmpty;
    cartGet = mCartGet;
    cartAccept = mCartAccept;
    cartDontAccept = mCartDontAccept;
}

eTile* TradePost::entryPoint() const {
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

void TradePost::spawnTrader() {
    const auto entryPoint = TradePost::entryPoint();
    auto& board = getBoard();

    const auto r = mCharGen(entryPoint, board);
    r->setOnCityId(cityId());
    r->setCityId(cityId());

    if(const auto trader = dynamic_cast<Trader*>(r.get())) {
        trader->createFollowers();
    }

    const auto ta = e::make_shared<TraderAction>(r.get());
    ta->setFinishOnComeback(true);
    ta->setTradePost(this);
    ta->setUnpackBuilding(mUnpackBuilding);
    ta->setWalkable(eWalkableObject::sCreateRect(
                        mUnpackBuilding, mWalkable));
    r->setAction(ta);
}

bool TradePost::playerTwoWay() const {
    auto& brd = getBoard();
    const auto targetCid = mCity.cityId();
    const auto targetPid = brd.cityIdToPlayerId(targetCid);
    const bool targetOnBoard = brd.boardCityWithId(targetCid);
    const auto thisPid = playerId();
    return targetOnBoard && targetPid == thisPid;
}

int TradePost::buy(const int cash) {
    std::map<eResourceType, int> bought;
    return buy(cash, bought);
}

int TradePost::sell(const int items) {
    std::map<eResourceType, int> sold;
    return sell(items, sold);
}

int TradePost::buy(const int cash, std::map<eResourceType, int>& bought) {
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
            bought[e] += 1;
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
            bought[b.fType] += 1;
        }
        const auto pid = playerId();
        brd.incDrachmas(pid, spent, eFinanceTarget::exports);
    }
    return spent;
}

int TradePost::sell(const int items, std::map<eResourceType, int>& sold) {
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
            sold[e.first] += 1;
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
            sold[b.fType] += 1;
            if(const auto thisC = brd.boardCityWithId(thisCid))
                thisC->addImported(srcCid, b.fType, 1);
        }
        const auto pid = playerId();
        brd.incDrachmas(pid, -earned, eFinanceTarget::importCosts);
    }
    return earned;
}

void TradePost::setWalkable(const stdsptr<eWalkableObject>& w) {
    mWalkable = w;
}

void TradePost::setUnpackBuilding(eBuilding* const b) {
    mUnpackBuilding = b;
}

void TradePost::setOrientation(const eDiagonalOrientation o) {
    mO = o;
}

eDiagonalOrientation TradePost::orientation() const {
    return mO;
}

void TradePost::setCharacterCreator(const eCharacterCreator& c) {
    mCharGen = c;
}

void TradePost::serializeFields(eSaveArchive& ar) {
    eStorageBuilding::serializeFields(ar);
    ar.field("imports", mImports, eResourceType::none);
    ar.field("exports", mExports, eResourceType::none);
    ar.field("cartEmpty", mCartEmpty, eResourceType::none);
    ar.field("cartGet", mCartGet, eResourceType::none);
    ar.field("cartAccept", mCartAccept, eResourceType::none);
    ar.field("cartDontAccept", mCartDontAccept, eResourceType::none);
    ar.field("routeTimer", mRouteTimer, 0);
    if(mType == eTradePostType::pier) {
        ar.buildingAsField("unpackBuilding", &getBoard(), mUnpackBuilding);
    }
    const stdptr<TradePost> tptr(this);
    ar.addPostFunc([tptr]() {
        if(!tptr) return;
        tptr->setOrders(tptr->mImports, tptr->mExports, tptr->mCartEmpty,
                        tptr->mCartGet, tptr->mCartAccept, tptr->mCartDontAccept);
    }, "TradePost::setOrders");
}

bool TradePost::trades() const {
    if(!mCity.trades()) return false;
    auto& board = getBoard();
    const auto cid = cityId();
    if(mType == eTradePostType::pier) {
        return !board.seaTradeShutdown(cid);
    } else {
        return !board.landTradeShutdown(cid);
    }
}
