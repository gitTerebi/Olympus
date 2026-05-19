#include "esoldierbanner.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "engine/etile.h"
#include "engine/e-game-board.h"
#include "engine/eorientation.h"
#include "characters/esoldier.h"
#include "characters/actions/soldier-action.h"
#include "fileIO/esavearchive.h"
#include "eiteratesquare.h"

#include "evectorhelpers.h"
#include "ewalkablehelpers.h"

#include "buildings/epalace.h"
#include "buildings/epalacetile.h"

#include "eiteratesquare.h"
#include "engine/epathfinder.h"

#include "elanguage.h"
#include "estringhelpers.h"

int gNextId = 0;

eSoldierBanner::eSoldierBanner(const eBannerType type,
                                eGameBoard& board) :
    eObject(board),
    mType(type), mId(gNextId++), mBoard(board), mFacing(0) {
    mBoard.registerAllSoldierBanner(this);
    const int nameId = mId % 30;
    auto name = eLanguage::zeusText(138, nameId);
    eStringHelpers::replaceAll(name, "&quot;", "\"");
    setName(name);
}

eSoldierBanner::~eSoldierBanner() {
    killAll();
    mBoard.unregisterAllSoldierBanner(this);
}

eGameBoard& eSoldierBanner::board() const {
    if(mTile) return mTile->board();
    return mBoard;
}

void eSoldierBanner::setFacing(const int facing) {
    mFacing = facing;
    updatePlaces();
    if(!mHome) callSoldiers();
}

void eSoldierBanner::setFacingOnLoad(const int facing) {
    mFacing = facing;
}

eOrientation eSoldierBanner::soldierOrientation() const {
    const int facing = ((mFacing % 360) + 360) % 360;
    if(facing == 45) return eOrientation::topRight;
    if(facing == 90) return eOrientation::right;
    if(facing == 135) return eOrientation::bottomRight;
    if(facing == 180) return eOrientation::bottom;
    if(facing == 225) return eOrientation::bottomLeft;
    if(facing == 270) return eOrientation::left;
    if(facing == 315) return eOrientation::topLeft;
    return eOrientation::top;
}

void eSoldierBanner::commandFormation(const int facing,
                                      const int lineDX,
                                      const int lineDY) {
    if(!mTile) return;
    purgeDead();
    if(mSoldiers.empty()) return;

    mFacing = facing;
    const int sideDX = lineDX == 0 ? 0 : (lineDX > 0 ? 1 : -1);
    const int sideDY = lineDY == 0 ? 0 : (lineDY > 0 ? 1 : -1);
    if(sideDX == 0 && sideDY == 0) {
        updatePlaces();
        if(!mHome) callSoldiers();
        return;
    }

    const auto onCid = onCityId();
    const auto cid = cityId();
    const auto onTid = mBoard.cityIdToTeamId(onCid);
    const auto tid = mBoard.cityIdToTeamId(cid);
    const auto walkable = [&](eTile* const tt) {
        if(!tt) return false;
        if(tt->cityId() != onCid) return false;
        if(onTid == tid) {
            return eWalkableHelpers::sDefaultWalkable(tt);
        } else {
            return eWalkableHelpers::sBuildingsWalkable(tt);
        }
    };

    const int depthDX = -sideDY;
    const int depthDY = sideDX;
    const int slds = mSoldiers.size();
    const bool missile = mType == eBannerType::rockThrower;
    const int files = slds > 1 ? (missile ? 2 : (slds + 1) / 2) : 1;
    const int ranks = (slds + files - 1) / files;
    std::map<eSoldier*, eTile*> places;
    int isld = 0;
    for(int rank = 0; rank < ranks; rank++) {
        for(int file = 0; file < files; file++) {
            if(isld >= slds) break;
            const int side = file - files/2;
            const int dx = side*sideDX + rank*depthDX;
            const int dy = side*sideDY + rank*depthDY;
            const auto tt = mBoard.tile(mTile->x() + dx, mTile->y() + dy);
            if(!walkable(tt)) continue;
            places[mSoldiers[isld++]] = tt;
        }
    }
    if(!places.empty()) {
        mPlaces = places;
    }
    if(!mHome) callSoldiers();
}

eCharacterType eSoldierBanner::characterType() const {
    if(atlantean()) {
        switch(mType) {
        case eBannerType::hoplite:
            return eCharacterType::hoplitePoseidon;
        case eBannerType::horseman:
            return eCharacterType::chariotPoseidon;
        case eBannerType::rockThrower:
            return eCharacterType::archerPoseidon;
        case eBannerType::amazon:
            return eCharacterType::amazon;
        case eBannerType::aresWarrior:
            return eCharacterType::aresWarrior;
        case eBannerType::enemy:
        case eBannerType::trireme:
            return eCharacterType::none;
        }
    } else {
        switch(mType) {
        case eBannerType::hoplite:
            return eCharacterType::hoplite;
        case eBannerType::horseman:
            return eCharacterType::horseman;
        case eBannerType::rockThrower:
            return eCharacterType::rockThrower;
        case eBannerType::amazon:
            return eCharacterType::amazon;
        case eBannerType::aresWarrior:
            return eCharacterType::aresWarrior;
        case eBannerType::enemy:
        case eBannerType::trireme:
            return eCharacterType::none;
        }
    }
    return eCharacterType::none;
}

stdsptr<eSoldier> eSoldierBanner::createSoldier(eTile* const t) {
    const auto ct = characterType();
    const auto c = eCharacter::sCreate(ct, mBoard);
    c->setCityId(cityId());
    c->setOnCityId(onCityId());
    const auto s = c->ref<eSoldier>();
    s->setBanner(this);
    const auto a = e::make_shared<eSoldierAction>(s.get());
    c->setAction(a);
    c->changeTile(t);
    c->setActionType(eCharacterActionType::stand);
    if(mHome) a->goHome();
    else a->goBackToBanner(soldierOrientation());
    return s;
}

void eSoldierBanner::moveTo(const int x, const int y) {
    const auto t = mBoard.tile(x, y);
    if(!t || t == mTile) return;

    const bool visible = visibleOnTile();

    if(visible && mTile) {
        mTile->setSoldierBanner(nullptr);
    }
    if(visible && t) {
        t->setSoldierBanner(this);
    }
    mTile = t;

    updatePlaces();
    if(!mHome) callSoldiers();
}

void eSoldierBanner::moveToPalace() {
    const auto onCid = onCityId();
    const auto cid = cityId();
    if(onCid != cid) return;
    switch(mType) {
    case eBannerType::rockThrower:
    case eBannerType::hoplite:
    case eBannerType::horseman: {
        const auto palace = mBoard.palace(cid);
        if(!palace) return;
        const auto ts = palace->tiles();
        for(const auto t : ts) {
            if(t->other()) continue;
            const auto tt = t->centerTile();
            if(!tt) continue;
            const auto bb = tt->soldierBanner();
            if(bb) continue;
            moveTo(tt->x(), tt->y());
            break;
        }
    } break;
    case eBannerType::amazon:
    case eBannerType::aresWarrior: {
        const auto s = mType == eBannerType::amazon ?
                            mBoard.sanctuary(cid, eGodType::artemis) :
                            mBoard.sanctuary(cid, eGodType::ares);
        if(!s) return;
        const auto ts = s->warriorTiles();
        for(const auto t : ts) {
            const auto bb = t->soldierBanner();
            if(bb) continue;
            moveTo(t->x(), t->y());
            break;
        }
    } break;
    case eBannerType::enemy:
    case eBannerType::trireme:
        break;
    }
}

void eSoldierBanner::goHome() {
    if(mAbroad && mMilitaryAid) {
        const auto onCid = onCityId();
        const auto c = mBoard.boardCityWithId(onCid);
        if(c) {
            const auto ptr = ref<eSoldierBanner>();
            c->reinforcementsGoHome(ptr);
        }
        goAbroad();
        mMilitaryAid = false;
        return;
    }
    if(mAbroad) return;
    if(mMilitaryAid) return;
    const auto onCid = onCityId();
    const auto cid = cityId();
    if(onCid != cid) return;
    if(mHome) return;
    mHome = true;
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto a = s->soldierAction();
        if(a) a->goHome();
    }
}

void eSoldierBanner::goAbroad() {
    if(mAbroad && !mMilitaryAid) return;
    if(mSelected) mBoard.deselectBanner(this);
    if(mHome) backFromHome();
    mAbroad = true;
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto a = s->soldierAction();
        if(a) a->goAbroad();
    }
    if(mTile) {
        mTile->setSoldierBanner(nullptr);
        mTile = nullptr;
    }
}

void eSoldierBanner::backFromAbroad(int& wait) {
    if(!mAbroad) return;
    if(mCount <= 0) {
        const auto tptr = ref<eSoldierBanner>();
        mBoard.unregisterSoldierBanner(tptr);
        return;
    }
    mAbroad = false;
    moveToPalace();
    const auto cid = cityId();
    const auto entryPoint = mBoard.entryPoint(cid);
    if(entryPoint) {
        while((int)mSoldiers.size() < mCount) {
            const auto s = createSoldier(entryPoint);
            const auto a = s->soldierAction();
            if(!a) continue;
            a->waitAndGoHome(wait);
            wait += 150;
        }
        mHome = true;
    } else {
        goHome();
    }
}

void eSoldierBanner::backFromHome() {
    if(mAbroad && !mMilitaryAid) return;
    if(!mHome) return;
    mHome = false;
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto a = s->soldierAction();
        if(a) a->goBackToBanner(soldierOrientation());
    }
    updateCount();
}

void eSoldierBanner::callSoldiers() {
    for(const auto s : mSoldiers) {
        callSoldier(s);
    }
}

bool eSoldierBanner::isGoingHome() const {
    if(!mHome) return false;
    return !mSoldiers.empty();
}

void eSoldierBanner::addSoldier(eSoldier* const s) {
    mSoldiers.push_back(s);
    updatePlaces();
    if(!mHome) callSoldier(s);
}

void eSoldierBanner::removeSoldier(eSoldier* const s) {
    mPlaces.erase(s);
    const bool r = eVectorHelpers::remove(mSoldiers, s);
    if(r) updatePlaces();
}

eTile* eSoldierBanner::place(eSoldier* const s) const {
    const auto it = mPlaces.find(s);
    if(it == mPlaces.end()) return nullptr;
    return it->second;
}

void eSoldierBanner::killAll() {
    const auto soldiers = mSoldiers;
    for(const auto s : soldiers) {
        if(!s) continue;
        s->kill();
        if(s->banner() == this) s->setBanner(nullptr);
    }
    mSoldiers.clear();
    mPlaces.clear();
}

void eSoldierBanner::killAllWithCorpse() {
    const auto soldiers = mSoldiers;
    for(const auto s : soldiers) {
        if(!s) continue;
        s->killWithCorpse();
    }
    mSoldiers.clear();
    mPlaces.clear();
}

void eSoldierBanner::setBothCityIds(const eCityId cid) {
    mCityId = cid;
    mOnCityId = cid;
}

bool eSoldierBanner::atlantean() const {
    return mBoard.atlantean(mCityId);
}

ePlayerId eSoldierBanner::playerId() const {
    const auto cid = cityId();
    auto& board = getBoard();
    return board.cityIdToPlayerId(cid);
}

eTeamId eSoldierBanner::teamId() const {
    const auto pid = playerId();
    auto& board = getBoard();
    return board.playerIdToTeamId(pid);
}

void eSoldierBanner::incCount() {
    mCount++;
    updateCount();
}

void eSoldierBanner::decCount() {
    if(mCount > 0) mCount--;
    updateCount();
}

bool eSoldierBanner::stationary() const {
    for(const auto s : mSoldiers) {
        const auto at = s->actionType();
        if(at != eCharacterActionType::stand) return false;
    }
    return true;
}

bool eSoldierBanner::fighting() const {
    for(const auto s : mSoldiers) {
        const auto at = s->actionType();
        if(at == eCharacterActionType::fight ||
           at == eCharacterActionType::fight2) return true;
    }
    return false;
}

void eSoldierBanner::serializeFields(eSaveArchive& ar) {
    ar.field("mIOID", mIOID);
    ar.field("mMilitaryAid", mMilitaryAid);
    ar.field("mHome", mHome);
    ar.field("mAbroad", mAbroad);
    ar.tile(mTile, mBoard);
    ar.field("mCount", mCount);
    ar.field("mCityId", mCityId);
    ar.field("mOnCityId", mOnCityId);
    if(ar.reading()) {
        int facing = 0;
        ar.field("mFacing", facing);
        setFacingOnLoad(facing);
    } else {
        ar.field("mFacing", mFacing);
    }

    if(ar.reading()) {
        const stdptr<eSoldierBanner> tptr(this);
        auto places = std::make_shared<std::vector<std::pair<eSoldier*, eTile*>>>();
        ar.arrayField("places", *places,
            [this](eSaveArchive& itemAr, std::pair<eSoldier*, eTile*>& p) {
                itemAr.tileField("tile", mBoard, p.second);
                itemAr.characterField("soldier", &mBoard, p.first);
            });
        ar.addPostFunc([tptr, places]() {
            if(!tptr) return;
            tptr->mPlaces.clear();
            for(const auto& p : *places) {
                if(p.first) tptr->mPlaces[p.first] = p.second;
            }
        }, "eSoldierBanner::places");
    } else {
        std::vector<std::pair<eSoldier*, eTile*>> places;
        places.reserve(mPlaces.size());
        for(const auto& p : mPlaces) places.emplace_back(p.first, p.second);
        ar.arrayField("places", places,
            [this](eSaveArchive& itemAr, std::pair<eSoldier*, eTile*>& p) {
                itemAr.tileField("tile", mBoard, p.second);
                itemAr.characterField("soldier", &mBoard, p.first);
            });
    }
    if(ar.reading()) {
        const stdptr<eSoldierBanner> tptr(this);
        auto soldiers = std::make_shared<std::vector<eSoldier*>>();
        ar.arrayField("soldiers", *soldiers,
            [this](eSaveArchive& itemAr, eSoldier*& s) {
                itemAr.characterField("soldier", &mBoard, s);
            });
        ar.addPostFunc([tptr, soldiers]() {
            if(!tptr) return;
            tptr->mSoldiers.clear();
            for(const auto s : *soldiers) {
                if(s) tptr->mSoldiers.push_back(s);
            }
        }, "eSoldierBanner::soldiers");
    } else {
        std::vector<eSoldier*> soldiers = mSoldiers;
        ar.arrayField("soldiers", soldiers,
            [this](eSaveArchive& itemAr, eSoldier*& s) {
                itemAr.characterField("soldier", &mBoard, s);
            });
    }
}

void eSoldierBanner::read(eReadStream& src) {
    eSaveArchive ar(src);
    serializeFields(ar);

    const stdptr<eSoldierBanner> tptr(this);
    ar.addPostFunc([tptr]() {
        if(!tptr) return;
        if(tptr->visibleOnTile() && tptr->mTile) {
            tptr->mTile->setSoldierBanner(tptr.get());
        }
        tptr->updatePlaces();
        if(!tptr->mHome) tptr->callSoldiers();
    }, "eSoldierBanner::postLoad");
}

void eSoldierBanner::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eSoldierBanner*>(this)->serializeFields(ar);
}

void eSoldierBanner::sPlaceDefault(std::vector<eSoldierBanner*>& bs,
                                   const int ctx, const int cty,
                                   eGameBoard& board) {
    if(bs.empty()) return;
    const auto bsFirst = bs[0];
    const auto cid = bsFirst->cityId();
    const auto onCid = bsFirst->onCityId();
    const auto tt = board.tile(ctx, cty);
    const auto ttCid = tt->cityId();
    if(tt && ttCid == onCid && ttCid == cid) {
        const auto b = tt->underBuilding();
        if(b) {
            const auto bt = b->type();
            if(bt == eBuildingType::palace ||
               bt == eBuildingType::palaceTile) {
                for(int i = 0; i < (int)bs.size(); i++) {
                    const auto bb = bs[i];
                    const auto bbt = bb->type();
                    if(bbt == eBannerType::hoplite ||
                       bbt == eBannerType::rockThrower ||
                       bbt == eBannerType::horseman) {
                        bb->moveToPalace();
                        eVectorHelpers::remove(bs, bb);
                        i--;
                    }
                }
            } else if(const auto sb = dynamic_cast<eSanctBuilding*>(b)) {
                const auto s = dynamic_cast<eSanctuary*>(sb->monument());
                if(!s) return;
                const auto gt = s->godType();
                for(int i = 0; i < (int)bs.size(); i++) {
                    const auto bb = bs[i];
                    const auto bbt = bb->type();
                    if((bbt == eBannerType::amazon && gt == eGodType::artemis) ||
                       (bbt == eBannerType::aresWarrior && gt == eGodType::ares)) {
                        bb->moveToPalace();
                        eVectorHelpers::remove(bs, bb);
                        i--;
                    }
                }
            }
        }
    }
}

void eSoldierBanner::sPlaceNoPathTrace(std::vector<eSoldierBanner*> bs,
                                       const int ctx, const int cty,
                                       eGameBoard& board, const int dist,
                                       const int minDistFromEdge) {
    sPlaceDefault(bs, ctx, cty, board);
    if(bs.empty()) return;
    const int bannerDist = bs.size() > 1 ? dist + 1 : dist;

    const auto bsFirst = bs[0];
    const auto onCid = bsFirst->onCityId();
    const auto cid = bsFirst->cityId();
    const auto onTid = board.cityIdToTeamId(onCid);
    const auto tid = board.cityIdToTeamId(cid);
    const bool isEnemy = onTid != tid;

    int isld = 0;
    const int slds = bs.size();

    const int bw = board.width();
    const int bh = board.height();

    const auto prcsTile = [&](const int i, const int j) {
        if(isld >= slds) return false;
        const int tx = ctx + i;
        const int ty = cty + j;
        const auto tt = board.tile(tx, ty);
        if(!tt) return false;
        const auto ttCid = tt->cityId();
        if(ttCid != onCid) return false;
        const int dtx = tt->dx();
        const int dty = tt->dy();
        if(dtx < minDistFromEdge) return false;
        if(dty < 2*minDistFromEdge) return false;
        if(dtx > bw - minDistFromEdge) return false;
        if(dty > bh - 2*minDistFromEdge) return false;
        if(!isEnemy && !tt->walkable()) return false;
        if(isEnemy && !tt->walkableTerrain()) return false;
        if(!isEnemy && tt->soldierBanner()) return false;

        const auto s = bs[isld++];
        s->moveTo(tx, ty);
        return false;
    };

    const int kinc = slds == 1 ? 1 : bannerDist;
    for(int k = 0; isld < slds; k += kinc) {
        (void)isld;
        eIterateSquare::iterateSquare(k, prcsTile, bannerDist);
    }
}

void eSoldierBanner::sPlace(std::vector<eSoldierBanner*> bs,
                            const int ctx, const int cty,
                            eGameBoard& board, const int dist,
                            const int minDistFromEdge) {
    sPlaceDefault(bs, ctx, cty, board);
    if(bs.empty()) return;
    const int bannerDist = bs.size() > 1 ? dist + 1 : dist;

    eTile* startTile = nullptr;

    const auto bsFirst = bs[0];
    const auto onCid = bsFirst->onCityId();
    const auto cid = bsFirst->cityId();
    const auto onTid = board.cityIdToTeamId(onCid);
    const auto tid = board.cityIdToTeamId(cid);
    const bool isEnemy = onTid != tid;

    const int bw = board.width();
    const int bh = board.height();

    const auto prcsTile = [&](const int i, const int j) {
        const int tx = ctx + i;
        const int ty = cty + j;
        const auto tt = board.tile(tx, ty);
        if(!tt) return false;
        const auto ttCid = tt->cityId();
        if(ttCid != onCid) return false;
        const int dtx = tt->dx();
        const int dty = tt->dy();
        if(dtx < minDistFromEdge) return false;
        if(dty < 2*minDistFromEdge) return false;
        if(dtx > bw - minDistFromEdge) return false;
        if(dty > bh - 2*minDistFromEdge) return false;
        if(!isEnemy && !tt->walkable()) return false;
        if(isEnemy && !tt->walkableTerrain()) return false;
        if(!isEnemy && tt->soldierBanner()) return false;
        startTile = tt;
        return true;
    };

    for(int k = 0; k < 9; k++) {
        eIterateSquare::iterateSquare(k, prcsTile, bannerDist);
        if(startTile) break;
    }

    if(!startTile) return;

    const auto rect = board.boardCityTileBRect(onCid);
    std::vector<SDL_Point> placed;
    for(const auto b : bs) {
        eTile* final = nullptr;
        ePathFinder p([&](eTileBase* const t) {
            if(isEnemy) return t->walkableTerrain();
            else return t->walkable();
        }, [&](eTileBase* const t) {
            const bool walkable = isEnemy ? t->walkableTerrain() :
                                            t->walkable();
            if(!walkable) return false;

            const auto tt = static_cast<eTile*>(t);
            const int tx = tt->x();
            const int ty = tt->y();
            for(const auto& p : placed) {
                const double d = std::sqrt((tx - p.x)*(tx - p.x) +
                                           (ty - p.y)*(ty - p.y));
                if(d < bannerDist) return false;
            }
            final = tt;
            return true;
        });
        const int w = board.width();
        const int h = board.height();
        const bool r = p.findPath(rect, startTile, 100, true, w, h);
        if(!r) break;
        const int tx = final->x();
        const int ty = final->y();
        placed.push_back({tx, ty});
        b->moveTo(tx, ty);
    }
}

std::vector<eSoldierBanner::sFormationSlot>
eSoldierBanner::sFormationPositions(
        std::vector<eSoldierBanner*> bs,
        const int ctx, const int cty,
        const int lineDX, const int lineDY,
        const int dist) {
    // hoplites rear (depth=0), missiles front (max depth)
    std::stable_sort(bs.begin(), bs.end(), [](const eSoldierBanner* a, const eSoldierBanner* b) {
        auto order = [](eBannerType t) {
            switch(t) {
            case eBannerType::rockThrower: return 0;
            case eBannerType::horseman:    return 1;
            case eBannerType::hoplite:     return 2;
            default:                       return 3;
            }
        };
        return order(a->type()) > order(b->type());
    });

    const int depthDX = -lineDY;
    const int depthDY =  lineDX;

    const bool cardinal = (lineDX == 0 || lineDY == 0);
    const int missileSlot = cardinal ? 2 : 1;
    const int otherSlot   = cardinal ? 4 : 2;
    const int groupGap    = cardinal ? dist + 1 : dist;

    std::vector<sFormationSlot> result;
    result.reserve(bs.size());
    int depth = 0;
    int gi = 0;
    while(gi < (int)bs.size()) {
        const eBannerType gtype = bs[gi]->type();
        int gend = gi;
        while(gend < (int)bs.size() && bs[gend]->type() == gtype) gend++;
        const int count = gend - gi;
        const int slotW = gtype == eBannerType::rockThrower ? missileSlot : otherSlot;
        const int total = count * slotW;
        int cur = -(total / 2);
        for(int i = 0; i < count; i++) {
            const int side = cur + slotW / 2;
            cur += slotW;
            result.push_back({bs[gi + i],
                              ctx + side*lineDX + depth*depthDX,
                              cty + side*lineDY + depth*depthDY});
        }
        depth += groupGap;
        gi = gend;
    }
    return result;
}

void eSoldierBanner::sPlaceFacing(std::vector<eSoldierBanner*> bs,
                                  const int ctx, const int cty,
                                  eGameBoard& board,
                                  const int facing,
                                  const int lineDX,
                                  const int lineDY,
                                  const int dist,
                                  const int minDistFromEdge) {
    if(bs.empty()) return;
    sPlaceDefault(bs, ctx, cty, board);
    if(bs.empty()) return;

    const auto slots = sFormationPositions(bs, ctx, cty, lineDX, lineDY, dist);

    if(bs.size() == 1) {
        sPlace(bs, ctx, cty, board, dist, minDistFromEdge);
    } else {
        std::vector<eSoldierBanner*> fallback;
        for(const auto& slot : slots) {
            const auto tile = board.tile(slot.tx, slot.ty);
            if(tile && tile->cityId() == slot.banner->onCityId() &&
               tile->walkable() && !tile->soldierBanner()) {
                slot.banner->moveTo(slot.tx, slot.ty);
            } else {
                fallback.push_back(slot.banner);
            }
        }
        if(!fallback.empty()) {
            sPlace(fallback, ctx, cty, board, dist, minDistFromEdge);
        }
    }

    for(const auto& slot : slots) {
        slot.banner->commandFormation(facing, lineDX, lineDY);
    }
}

std::vector<eSoldierBanner*> eSoldierBanner::sPlayerBanners(
        const std::vector<eSoldierBanner*>& bs,
        const ePlayerId playerId) {
    std::vector<eSoldierBanner*> result;
    for(const auto b : bs) {
        if(!b) continue;
        if(b->playerId() != playerId) continue;
        result.push_back(b);
    }
    return result;
}

void eSoldierBanner::sRotatePlayerBanners(
        const std::vector<eSoldierBanner*>& bs,
        const ePlayerId playerId) {
    const auto banners = sPlayerBanners(bs, playerId);
    for(const auto b : banners) {
        const int newFacing = (b->facing() + 90) % 360;
        b->setFacing(newFacing);
    }
}

void eSoldierBanner::sSetPlayerBannersFacing(
        const std::vector<eSoldierBanner*>& bs,
        const ePlayerId playerId,
        const int facing) {
    const auto banners = sPlayerBanners(bs, playerId);
    for(const auto b : banners) {
        b->setFacing(facing);
    }
}

void eSoldierBanner::sPlacePlayerBannersFacing(
        const std::vector<eSoldierBanner*>& bs,
        const ePlayerId playerId,
        const int ctx, const int cty,
        eGameBoard& board,
        const int facing,
        const int lineDX,
        const int lineDY,
        const int dist,
        const int minDistFromEdge) {
    const auto banners = sPlayerBanners(bs, playerId);
    if(banners.empty()) return;
    sPlaceFacing(banners, ctx, cty, board,
                 facing, lineDX, lineDY, dist, minDistFromEdge);
}

std::string eSoldierBanner::sName(
        const eBannerType type,
        const bool atlantean) {
    if(atlantean) {
        switch(type) {
        case eBannerType::hoplite:
            return eLanguage::zeusText(138, 79);
        case eBannerType::rockThrower:
            return eLanguage::zeusText(138, 77);
        case eBannerType::horseman:
            return eLanguage::zeusText(138, 80);
        case eBannerType::trireme:
            return eLanguage::zeusText(138, 76);
        default:
            break;
        }
    } else {
        switch(type) {
        case eBannerType::hoplite:
            return eLanguage::zeusText(138, 72);
        case eBannerType::rockThrower:
            return eLanguage::zeusText(138, 74);
        case eBannerType::horseman:
            return eLanguage::zeusText(138, 71);
        case eBannerType::trireme:
            return eLanguage::zeusText(138, 73);
        default:
            break;
        }
    }
    return "";
}

void eSoldierBanner::updatePlaces() {
    if(!mTile) return;
    purgeDead();
    if(mSoldiers.empty()) return;

    const int slds = mSoldiers.size();

    const auto onCid = onCityId();
    const auto cid = cityId();
    const auto onTid = mBoard.cityIdToTeamId(onCid);
    const auto tid = mBoard.cityIdToTeamId(cid);

    const auto walkable = [&](eTile* const tt) {
        if(!tt) return false;
        const auto cid = tt->cityId();
        if(cid != onCid) return false;
        if(onTid == tid) {
            return eWalkableHelpers::sDefaultWalkable(tt);
        } else {
            return eWalkableHelpers::sBuildingsWalkable(tt);
        }
    };

    const auto rotateOffset = [this](const int side, const int depth,
                                     int& i, int& j) {
        const int facing = ((mFacing % 360) + 360) % 360;
        if(facing == 90) {
            i = depth;
            j = side;
        } else if(facing == 180) {
            i = -side;
            j = depth;
        } else if(facing == 270) {
            i = -depth;
            j = -side;
        } else {
            i = side;
            j = -depth;
        }
    };

    const bool missile = mType == eBannerType::rockThrower;
    const int files = slds > 1 ? (missile ? 2 : (slds + 1) / 2) : 1;
    const int ranks = (slds + files - 1) / files;
    std::vector<SDL_Point> offsets;
    offsets.reserve(slds);
    for(int rank = 0; rank < ranks; rank++) {
        for(int file = 0; file < files; file++) {
            if((int)offsets.size() >= slds) break;
            const int side = file - files/2;
            const int depth = rank;
            int i = 0;
            int j = 0;
            rotateOffset(side, depth, i, j);
            offsets.push_back({i, j});
        }
    }

    int isld = 0;
    std::map<eSoldier*, eTile*> places;
    const auto alreadyUsed = [&places](eTile* const tile) {
        for(const auto& p : places) {
            if(p.second == tile) return true;
        }
        return false;
    };
    for(const auto& offset : offsets) {
        if(isld >= slds) break;
        const int tx = mTile->x();
        const int ty = mTile->y();
        const auto tt = mBoard.tile(tx + offset.x, ty + offset.y);
        if(!walkable(tt)) continue;

        const auto s = mSoldiers[isld++];
        places[s] = tt;
    }
    const int maxDist = std::max(mBoard.width(), mBoard.height());
    for(int k = 1; isld < slds && k <= maxDist; k++) {
        const auto prcsTile = [&](const int i, const int j) {
            if(isld >= slds) return true;
            const int tx = mTile->x();
            const int ty = mTile->y();
            const auto tt = mBoard.tile(tx + i, ty + j);
            if(!walkable(tt)) return false;
            if(alreadyUsed(tt)) return false;

            const auto s = mSoldiers[isld++];
            places[s] = tt;
            return false;
        };
        eIterateSquare::iterateSquare(k, prcsTile);
    }
    if(!places.empty()) {
        mPlaces = places;
    }
}

void eSoldierBanner::updateCount() {
    if(mMilitaryAid) return;
    const auto onCid = onCityId();
    const auto cid = cityId();
    if(onCid != cid) return;
    purgeDead();
    const int n = mSoldiers.size();
    if(!mHome && !mAbroad) {
        for(int i = n; i < mCount; i++) {
            eCharacterType cht;
            switch(mType) {
            case eBannerType::rockThrower:
                cht = eCharacterType::rockThrower;
                break;
            case eBannerType::hoplite:
                cht = eCharacterType::hoplite;
                break;
            case eBannerType::horseman:
                cht = eCharacterType::horseman;
                break;
            case eBannerType::amazon:
                cht = eCharacterType::amazon;
                break;
            case eBannerType::aresWarrior:
                cht = eCharacterType::aresWarrior;
                break;
            case eBannerType::enemy:
            case eBannerType::trireme:
                return;
            }
            const auto home = eSoldierAction::sFindHome(cht, cid, mBoard);
            if(!home) break;
            createSoldier(home->centerTile());
        }
    }

    while((int)mSoldiers.size() > std::max(0, mCount)) {
        const auto s = mSoldiers.back();
        const auto a = s->soldierAction();
        if(a) a->goHome();
        s->setBanner(nullptr);
    }
    const auto tptr = ref<eSoldierBanner>();
    if(mCount <= 0 && mSoldiers.size() == 0) {
        switch(mType) {
        case eBannerType::rockThrower:
        case eBannerType::hoplite:
        case eBannerType::horseman:
            mBoard.unregisterSoldierBanner(tptr);
            break;
        case eBannerType::amazon:
        case eBannerType::aresWarrior:
            break;
        case eBannerType::enemy:
        case eBannerType::trireme:
            break;
        }
        return;
    }
    updatePlaces();
}

void eSoldierBanner::callSoldier(eSoldier* const s) {
    if(s->dead()) return;
    const auto tt = place(s);
    if(!tt) return;

    const auto a = s->soldierAction();
    if(a) {
        a->goBackToBanner(soldierOrientation());
    }
}

void eSoldierBanner::purgeDead() {
    for(int i = 0; i < (int)mSoldiers.size(); i++) {
        const auto s = mSoldiers[i];
        const bool dead = s->dead();
        if(dead) {
            removeSoldier(s);
            i--;
        }
    }
}

bool eSoldierBanner::nearestSoldier(const int fromX, const int fromY,
                                    int& toX, int& toY) const {
    bool found = false;
    int minDist = 99999;
    for(const auto& s : mSoldiers) {
        const auto t = s->tile();
        if(!t) continue;
        const int tx = t->x();
        const int ty = t->y();
        const int dx = fromX - tx;
        const int dy = fromY - ty;
        const int dist = sqrt(dx*dx + dy*dy);
        if(dist > minDist) continue;
        found = true;
        toX = tx;
        toY = ty;
        minDist = dist;
    }
    return found;
}

bool eSoldierBanner::visibleOnTile() const {
    const auto onCid = onCityId();
    const auto onPid = mBoard.cityIdToPlayerId(onCid);
    const auto ppid = mBoard.personPlayer();
    if(onPid != ppid) return false;
    const auto pid = playerId();
    if(pid == ppid) return true;
    return mMilitaryAid;
}

void eSoldierBanner::teleportSoldiersToPlaces() {
    for(const auto s : mSoldiers) {
        const auto tile = place(s);
        if(!tile) continue;
        s->changeTile(tile);
        s->setOrientation(soldierOrientation());
        const auto a = s->soldierAction();
        if(a) a->setCurrentAction(nullptr);
    }
}
