#include "soldier-banner.h"

#include "formation-facing.h"

#include <algorithm>
#include <memory>
#include <vector>

#include "engine/etile.h"
#include "engine/game-board.h"
#include "engine/board-city.h"
#include "engine/eworlddirection.h"
#include "engine/eorientation.h"
#include "characters/esoldier.h"
#include "characters/actions/soldier-action.h"
#include "fileIO/esavearchive.h"
#include "eiteratesquare.h"

#include "evectorhelpers.h"
#include "ewalkablehelpers.h"

#include "buildings/epalace.h"
#include "buildings/epalacetile.h"
#include "buildings/ebuilding.h"

#include "eiteratesquare.h"
#include "engine/epathfinder.h"

#include "elanguage.h"
#include "estringhelpers.h"
#include "enumbers.h"

#include "gameEvents/invasions/invasion-targeting.h"

int gNextId = 0;

namespace {
bool canAttackCharacter(const eCharacter* const c) {
    if(c->isSoldier()) return true;
    const auto type = c->type();
    return type == eCharacterType::wolf ||
           type == eCharacterType::enemyBoat ||
           type == eCharacterType::trireme ||
           c->isImmortal();
}

bool canStandOn(eTile* const t) {
    if(!t) return false;
    const auto ubt = t->underBuildingType();
    if(ubt == eBuildingType::none) return true;
    return eBuilding::sWalkableBuilding(ubt);
}

bool hasLiveCombatUnit(eTile* const t, eCharacter* const except) {
    if(!t) return false;
    for(const auto& c : t->characters()) {
        if(c.get() == except) continue;
        if(c->dead()) continue;
        if(c->isSoldier() || c->isImmortal() ||
           c->type() == eCharacterType::wolf) {
            return true;
        }
    }
    return false;
}

eTile* findRangedBannerTile(GameBoard& board,
                            eTile* const from,
                            const int tx,
                            const int ty,
                            const int range) {
    if(!from) return nullptr;
    eTile* best = nullptr;
    int bestDist = 0;
    const int scan = std::max(range, 2);
    for(int i = -scan; i <= scan; i++) {
        for(int j = -scan; j <= scan; j++) {
            const int distToTarget = std::max(abs(i), abs(j));
            if(distToTarget > range || distToTarget < 2) continue;
            const auto t = board.tile(tx + i, ty + j);
            if(!canStandOn(t)) continue;
            const int dist = std::max(abs(t->x() - from->x()),
                                      abs(t->y() - from->y()));
            if(!best || dist < bestDist) {
                best = t;
                bestDist = dist;
            }
        }
    }
    return best;
}

void formationDepthAxis(const int facing,
                        const int lineDX,
                        const int lineDY,
                        int& depthDX,
                        int& depthDY) {
    depthDX = -lineDY;
    depthDY =  lineDX;

    int frontX = 0;
    int frontY = 0;
    eFormationFacing::facingFrontVector(facing, frontX, frontY);
    if(depthDX*frontX + depthDY*frontY > 0) {
        depthDX = -depthDX;
        depthDY = -depthDY;
    }
}

}

SoldierBanner::SoldierBanner(const eBannerType type,
                                GameBoard& board) :
    eObject(board),
    mType(type), mId(gNextId++), mBoard(board), mFacing(0) {
    mBoard.registerAllSoldierBanner(this);
    const int nameId = mId % 30;
    auto name = eLanguage::zeusText(138, nameId);
    eStringHelpers::replaceAll(name, "&quot;", "\"");
    setName(name);
}

SoldierBanner::~SoldierBanner() {
    killAll();
    mBoard.unregisterAllSoldierBanner(this);
}

GameBoard& SoldierBanner::board() const {
    if(mTile) return mTile->board();
    return mBoard;
}

void SoldierBanner::setFacing(const int facing) {
    mFacing = facing;
    updatePlaces();
    if(!mHome) callSoldiers();
}

void SoldierBanner::setFacingOnLoad(const int facing) {
    mFacing = facing;
}

eOrientation SoldierBanner::soldierOrientation() const {
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

void SoldierBanner::commandFormation(const int facing,
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
    const auto walkable = [&](eTile* const tt) {
        if(!tt) return false;
        if(tt->cityId() != onCid) return false;
        // Slots must land on a tile NOT occupied by a razable building, even for
        // enemy banners. sDefaultWalkable rejects building tiles; this forces the
        // formation adjacent to a building parked on, so soldiers stop at the wall
        // and the fighting-action building pass bulldozes inward toward the banner
        // instead of phasing through the structure.
        return eWalkableHelpers::sDefaultWalkable(tt);
    };

    int depthDX = 0;
    int depthDY = 0;
    formationDepthAxis(facing, sideDX, sideDY, depthDX, depthDY);
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

eCharacterType SoldierBanner::characterType() const {
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

stdsptr<eSoldier> SoldierBanner::createSoldier(eTile* const t) {
    const auto ct = characterType();
    const auto c = eCharacter::sCreate(ct, mBoard);
    c->setCityId(cityId());
    c->setOnCityId(onCityId());
    const auto s = c->ref<eSoldier>();
    s->setBanner(this);
    const auto a = e::make_shared<SoldierAction>(s.get());
    c->setAction(a);
    c->changeTile(t);
    c->setActionType(eCharacterActionType::stand);
    if(mHome) a->goHome();
    else a->goBackToBanner(soldierOrientation());
    return s;
}

void SoldierBanner::moveTo(const int x, const int y) {
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

void SoldierBanner::detachFromTile() {
    if(mTile) {
        if(mTile->soldierBanner() == this) {
            mTile->setSoldierBanner(nullptr);
        }
        mTile = nullptr;
    }
}

void SoldierBanner::moveToPalace() {
    const auto onCid = onCityId();
    const auto cid = cityId();
    if(onCid != cid) return;
    switch(mType) {
    case eBannerType::rockThrower:
    case eBannerType::hoplite:
    case eBannerType::horseman: {
        const auto palace = mBoard.palace(cid);
        if(!palace) return;
        const auto slots = sFixedPalaceBannerPathTiles(*palace);
        if(slots.empty()) return;

        const auto bs = sSortedPalaceBannersByUnitType(mBoard.banners(cid));

        int slot = -1;
        for(int i = 0; i < static_cast<int>(bs.size()); i++) {
            if(bs[i].get() == this) {
                slot = i;
                break;
            }
        }
        if(slot >= 0 && slot < static_cast<int>(slots.size())) {
            const auto tt = slots[slot];
            const auto bb = tt->soldierBanner();
            if(!bb || bb == this) moveTo(tt->x(), tt->y());
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

void SoldierBanner::goHome() {
    if(mAbroad && mMilitaryAid) {
        const auto onCid = onCityId();
        const auto c = mBoard.boardCityWithId(onCid);
        if(c) {
            const auto ptr = ref<SoldierBanner>();
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
    const auto c = mBoard.boardCityWithId(cid);
    if(c) c->repackPalaceBanners();
}

void SoldierBanner::goAbroad() {
    if(mAbroad && !mMilitaryAid) return;
    if(mSelected) mBoard.deselectBanner(this);
    if(mHome) backFromHome();
    mAbroad = true;
    int idx = 0;
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto a = s->soldierAction();
        if(a) {
            a->setDepartDelay(idx * 100);
            a->goAbroad();
        }
        idx++;
    }
    if(mTile) {
        mTile->setSoldierBanner(nullptr);
        mTile = nullptr;
    }
}

void SoldierBanner::backFromAbroad(int& wait) {
    if(!mAbroad) return;
    if(mCount <= 0) {
        const auto tptr = ref<SoldierBanner>();
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

void SoldierBanner::backFromHome() {
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

void SoldierBanner::callSoldiers() {
    for(const auto s : mSoldiers) {
        callSoldier(s);
    }
}

bool SoldierBanner::isGoingHome() const {
    if(!mHome) return false;
    return !mSoldiers.empty();
}

void SoldierBanner::addSoldier(eSoldier* const s) {
    mSoldiers.push_back(s);
    updatePlaces();
    if(!mHome) callSoldier(s);
}

void SoldierBanner::removeSoldier(eSoldier* const s) {
    mPlaces.erase(s);
    mCombatAssignments.erase(s);
    mCombatBlockages.erase(s);
    const bool r = eVectorHelpers::remove(mSoldiers, s);
    if(r) updatePlaces();
}

eTile* SoldierBanner::place(eSoldier* const s) const {
    const auto it = mPlaces.find(s);
    if(it == mPlaces.end()) return nullptr;
    return it->second;
}

void SoldierBanner::killAll() {
    const auto soldiers = mSoldiers;
    for(const auto s : soldiers) {
        if(!s) continue;
        s->kill();
        if(s->banner() == this) s->setBanner(nullptr);
    }
    mSoldiers.clear();
    mPlaces.clear();
}

void SoldierBanner::killAllWithCorpse() {
    const auto soldiers = mSoldiers;
    for(const auto s : soldiers) {
        if(!s) continue;
        s->killWithCorpse();
    }
    mSoldiers.clear();
    mPlaces.clear();
}

void SoldierBanner::setBothCityIds(const eCityId cid) {
    mCityId = cid;
    mOnCityId = cid;
}

bool SoldierBanner::atlantean() const {
    return mBoard.atlantean(mCityId);
}

ePlayerId SoldierBanner::playerId() const {
    const auto cid = cityId();
    auto& board = getBoard();
    return board.cityIdToPlayerId(cid);
}

eTeamId SoldierBanner::teamId() const {
    const auto pid = playerId();
    auto& board = getBoard();
    return board.playerIdToTeamId(pid);
}

eBannerFormationRole SoldierBanner::formationRole() const {
    if(mFormationRole != eBannerFormationRole::other)
        return mFormationRole;
    switch(mType) {
    case eBannerType::horseman:
        return eBannerFormationRole::cavalry;
    case eBannerType::rockThrower:
        return eBannerFormationRole::missile;
    case eBannerType::hoplite:
    case eBannerType::amazon:
    case eBannerType::aresWarrior:
        return eBannerFormationRole::melee;
    case eBannerType::enemy:
    case eBannerType::trireme:
        break;
    }
    return eBannerFormationRole::other;
}

int SoldierBanner::sPalaceUnitSortRank(const eBannerType type) {
    switch(type) {
    case eBannerType::horseman:
        return 0;
    case eBannerType::hoplite:
        return 1;
    case eBannerType::rockThrower:
        return 2;
    default:
        return 3;
    }
}

std::vector<stdsptr<SoldierBanner>> SoldierBanner::sSortedPalaceBannersByUnitType(
        std::vector<stdsptr<SoldierBanner>> banners) {
    banners.erase(std::remove_if(banners.begin(), banners.end(),
        [](const stdsptr<SoldierBanner>& b) {
            if(!b) return true;
            if(b->isAbroad()) return true;
            switch(b->type()) {
            case eBannerType::rockThrower:
            case eBannerType::hoplite:
            case eBannerType::horseman:
                return false;
            default:
                return true;
            }
        }), banners.end());
    std::sort(banners.begin(), banners.end(),
        [](const stdsptr<SoldierBanner>& a,
           const stdsptr<SoldierBanner>& b) {
            const int ar = SoldierBanner::sPalaceUnitSortRank(a->type());
            const int br = SoldierBanner::sPalaceUnitSortRank(b->type());
            if(ar != br) return ar < br;
            return a->id() < b->id();
        });
    return banners;
}

std::vector<eTile*> SoldierBanner::sFixedPalaceBannerPathTiles(
        const ePalace& palace) {
    std::vector<eTile*> pathTiles;
    const auto& palaceTiles = palace.tiles();
    if(palaceTiles.empty()) return pathTiles;

    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;
    bool first = true;
    std::vector<eTile*> perimeterTiles;
    perimeterTiles.reserve(palaceTiles.size());
    for(const auto palaceTile : palaceTiles) {
        if(!palaceTile) continue;
        const auto tile = palaceTile->centerTile();
        if(!tile) continue;
        if(first) {
            minX = maxX = tile->x();
            minY = maxY = tile->y();
            first = false;
        } else {
            minX = std::min(minX, tile->x());
            maxX = std::max(maxX, tile->x());
            minY = std::min(minY, tile->y());
            maxY = std::max(maxY, tile->y());
        }
        perimeterTiles.push_back(tile);
    }
    if(perimeterTiles.empty()) return pathTiles;

    const auto addTile = [&](const int x, const int y) {
        for(const auto tile : perimeterTiles) {
            if(tile->x() == x && tile->y() == y) {
                pathTiles.push_back(tile);
                return;
            }
        }
    };

    // Fixed palace home slots use world tile coords, not current camera view.
    // Include palace corner tiles; they are part of the visible banner path.
    if(palace.rotated()) {
        for(int y = maxY; y >= minY; y--) addTile(maxX, y);
        for(int x = maxX - 1; x >= minX; x--) addTile(x, minY);
        for(int y = minY + 1; y <= maxY; y++) addTile(minX, y);
        for(int x = minX + 1; x < maxX; x++) addTile(x, maxY);
    } else {
        for(int x = maxX; x >= minX; x--) addTile(x, maxY);
        for(int y = maxY - 1; y >= minY; y--) addTile(minX, y);
        for(int x = minX + 1; x <= maxX; x++) addTile(x, minY);
        for(int y = minY + 1; y < maxY; y++) addTile(maxX, y);
    }
    return pathTiles;
}

void SoldierBanner::incCount() {
    mCount++;
    updateCount();
}

void SoldierBanner::decCount() {
    if(mCount > 0) mCount--;
    updateCount();
}

bool SoldierBanner::soldiersOnMap() const {
    for(const auto s : mSoldiers) {
        if(s->tile()) return true;
    }
    return false;
}

bool SoldierBanner::stationary() const {
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto at = s->actionType();
        if(at != eCharacterActionType::stand) return false;
    }
    return true;
}

bool SoldierBanner::fighting() const {
    for(const auto s : mSoldiers) {
        const auto at = s->actionType();
        if(at == eCharacterActionType::fight ||
           at == eCharacterActionType::fight2) return true;
    }
    return false;
}

void SoldierBanner::cancelSoldiersAttack() {
    for(const auto s : mSoldiers) {
        const auto a = s->soldierAction();
        if(a) a->cancelAndClearAction();
    }
}

void SoldierBanner::cancelSoldierActions() {
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto a = s->soldierAction();
        if(a) a->cancelAndClearAction();
    }
}

void SoldierBanner::noteAttackFrom(const int tx, const int ty) {
    mLastAttackTile = mBoard.tile(tx, ty);
}

int SoldierBanner::soldierRange() const {
    for(const auto s : mSoldiers) {
        if(!s->dead()) return s->range();
    }
    return 0;
}

void SoldierBanner::serializeFields(eSaveArchive& ar) {
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
        const stdptr<SoldierBanner> tptr(this);
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
        }, "SoldierBanner::places");
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
        const stdptr<SoldierBanner> tptr(this);
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
        }, "SoldierBanner::soldiers");
    } else {
        std::vector<eSoldier*> soldiers = mSoldiers;
        ar.arrayField("soldiers", soldiers,
            [this](eSaveArchive& itemAr, eSoldier*& s) {
                itemAr.characterField("soldier", &mBoard, s);
            });
    }
    ar.field("formationRole", mFormationRole, eBannerFormationRole::other);
}

void SoldierBanner::serialize(eSaveArchive& ar) {
    serializeFields(ar);
    if(ar.reading()) {
        const stdptr<SoldierBanner> tptr(this);
        ar.addPostFunc([tptr]() {
            if(!tptr) return;
            if(tptr->visibleOnTile() && tptr->mTile) {
                tptr->mTile->setSoldierBanner(tptr.get());
            }
            tptr->updatePlaces();
            if(!tptr->mHome) tptr->callSoldiers();
        }, "SoldierBanner::postLoad");
    }
}

void SoldierBanner::sSendPalaceBannersHomeAndRepack(
        std::vector<SoldierBanner*>& bs,
        const eCityId cid,
        GameBoard& board) {
    bool changed = false;
    for(int i = 0; i < (int)bs.size(); i++) {
        const auto bb = bs[i];
        const auto bbt = bb->type();
        if(bbt == eBannerType::hoplite ||
           bbt == eBannerType::rockThrower ||
           bbt == eBannerType::horseman) {
            bb->goHome();
            eVectorHelpers::remove(bs, bb);
            changed = true;
            i--;
        }
    }
    if(changed) {
        const auto c = board.boardCityWithId(cid);
        if(c) c->repackPalaceBanners();
    }
}

void SoldierBanner::sHandleHomeBuildingPlacement(
        std::vector<SoldierBanner*>& bs,
        const int ctx,
        const int cty,
        GameBoard& board) {
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
                // Right-clicking selected banners onto the palace sends them
                // home, then repacks all palace banners into fixed slots.
                sSendPalaceBannersHomeAndRepack(bs, cid, board);
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
                        bb->goHome();
                        eVectorHelpers::remove(bs, bb);
                        i--;
                    }
                }
            }
        }
    }
}

void SoldierBanner::sPlaceNoPathTrace(std::vector<SoldierBanner*> bs,
                                       const int ctx, const int cty,
                                       GameBoard& board, const int dist,
                                       const int minDistFromEdge) {
    sHandleHomeBuildingPlacement(bs, ctx, cty, board);
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

void SoldierBanner::sPlace(std::vector<SoldierBanner*> bs,
                            const int ctx, const int cty,
                            GameBoard& board, const int dist,
                            const int minDistFromEdge) {
    sHandleHomeBuildingPlacement(bs, ctx, cty, board);
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

std::vector<SoldierBanner::sFormationSlot>
SoldierBanner::sFormationPositions(
        std::vector<SoldierBanner*> bs,
        const int ctx, const int cty,
        const int facing,
        const int lineDX, const int lineDY,
        const int dist) {
    std::stable_sort(bs.begin(), bs.end(), [](const SoldierBanner* a, const SoldierBanner* b) {
        auto order = [](const SoldierBanner* banner) {
            const auto role = banner->formationRole();
            if(role == eBannerFormationRole::missile) return 0;
            if(role == eBannerFormationRole::cavalry) return 1;
            if(role == eBannerFormationRole::melee) return 2;
            return 3;
        };
        return order(a) < order(b);
    });

    // Depth must grow behind the facing direction so the leading group (melee)
    // sits at the front and missiles fall to the rear, regardless of facing.
    int depthDX = 0;
    int depthDY = 0;
    formationDepthAxis(facing, lineDX, lineDY, depthDX, depthDY);

    const bool cardinal = (lineDX == 0 || lineDY == 0);
    const int missileSlot = cardinal ? 2 : 1;
    const int otherSlot   = cardinal ? 4 : 2;
    const int groupGap    = cardinal ? dist + 1 : dist;

    std::vector<sFormationSlot> result;
    result.reserve(bs.size());
    int depth = 0;
    int gi = 0;
    while(gi < (int)bs.size()) {
        const auto gtype = bs[gi]->formationRole();
        int gend = gi;
        while(gend < (int)bs.size() && bs[gend]->formationRole() == gtype) gend++;
        const int count = gend - gi;
        const int slotW = gtype == eBannerFormationRole::missile ?
                          missileSlot : otherSlot;
        const int total = count * slotW;
        int cur = -(total / 2);
        for(int i = 0; i < count; i++) {
            const int side = cur + slotW / 2;
            cur += slotW;
            int sideOffset = side;
            int depthOffset = depth;
            if(gtype == eBannerFormationRole::cavalry) {
                const int flank = i % 2 == 0 ? -1 : 1;
                const int rank = i / 2;
                sideOffset = flank*(total/2 + otherSlot + rank*otherSlot);
                depthOffset = 0;
            }
            result.push_back({bs[gi + i],
                              ctx + sideOffset*lineDX + depthOffset*depthDX,
                              cty + sideOffset*lineDY + depthOffset*depthDY});
        }
        if(gtype != eBannerFormationRole::cavalry) {
            depth += groupGap;
        }
        gi = gend;
    }
    return result;
}

void SoldierBanner::sPlaceFacing(std::vector<SoldierBanner*> bs,
                                  const int ctx, const int cty,
                                  GameBoard& board,
                                  const int facing,
                                  const int lineDX,
                                  const int lineDY,
                                  const int dist,
                                  const int minDistFromEdge) {
    if(bs.empty()) return;
    sHandleHomeBuildingPlacement(bs, ctx, cty, board);
    if(bs.empty()) return;

    const auto slots = sFormationPositions(bs, ctx, cty, facing, lineDX, lineDY, dist);

    if(bs.size() == 1) {
        sPlace(bs, ctx, cty, board, dist, minDistFromEdge);
    } else {
        std::vector<SoldierBanner*> fallback;
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

std::vector<SoldierBanner*> SoldierBanner::sPlayerBanners(
        const std::vector<SoldierBanner*>& bs,
        const ePlayerId playerId) {
    std::vector<SoldierBanner*> result;
    for(const auto b : bs) {
        if(!b) continue;
        if(b->playerId() != playerId) continue;
        result.push_back(b);
    }
    return result;
}

void SoldierBanner::sRotatePlayerBanners(
        const std::vector<SoldierBanner*>& bs,
        const ePlayerId playerId) {
    const auto banners = sPlayerBanners(bs, playerId);
    for(const auto b : banners) {
        const int newFacing = (b->facing() + 90) % 360;
        b->setFacing(newFacing);
    }
}

void SoldierBanner::sSetPlayerBannersFacing(
        const std::vector<SoldierBanner*>& bs,
        const ePlayerId playerId,
        const int facing) {
    const auto banners = sPlayerBanners(bs, playerId);
    for(const auto b : banners) {
        b->setFacing(facing);
    }
}

void SoldierBanner::sPlacePlayerBannersFacing(
        const std::vector<SoldierBanner*>& bs,
        const ePlayerId playerId,
        const int ctx, const int cty,
        GameBoard& board,
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

std::string SoldierBanner::sName(
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

void SoldierBanner::updatePlaces() {
    if(!mTile) return;
    purgeDead();
    if(mSoldiers.empty()) return;

    const int slds = mSoldiers.size();

    const auto onCid = onCityId();

    const auto walkable = [&](eTile* const tt) {
        if(!tt) return false;
        const auto cid = tt->cityId();
        if(cid != onCid) return false;
        // Reject building-occupied tiles (incl. enemy banners). Keeps formation
        // slots adjacent to a building parked on so soldiers bulldoze in from the
        // wall rather than phasing into the structure. See commandFormation.
        return eWalkableHelpers::sDefaultWalkable(tt);
    };

    const auto rotateOffset = [this](const int side, const int depth,
                                     int& i, int& j) {
        const int facing = ((mFacing % 360) + 360) % 360;
        if(facing == 90) {
            i = -depth;
            j = side;
        } else if(facing == 180) {
            i = -side;
            j = -depth;
        } else if(facing == 270) {
            i = depth;
            j = -side;
        } else {
            i = side;
            j = depth;
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

void SoldierBanner::updateCount() {
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
            const auto home = SoldierAction::sFindHome(cht, cid, mBoard);
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
    const auto tptr = ref<SoldierBanner>();
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

void SoldierBanner::callSoldier(eSoldier* const s) {
    if(s->dead()) return;
    const auto a = s->soldierAction();
    if(a) a->goBackToBanner(soldierOrientation());
}

void SoldierBanner::purgeDead() {
    for(int i = 0; i < (int)mSoldiers.size(); i++) {
        const auto s = mSoldiers[i];
        const bool dead = s->dead();
        if(dead) {
            removeSoldier(s);
            i--;
        }
    }
}

bool SoldierBanner::enemyNear(const int by) {
    mEnemyNearCountdown -= by;
    if(mEnemyNearCountdown > 0) return mEnemyNear;
    mEnemyNearCountdown = 250;

    if(mHome || mAbroad || !mTile) {
        mEnemyNear = false;
        return false;
    }

    const int hrange = eNumbers::sInvasionEngageDefenderRange;
    const int tx = mTile->x();
    const int ty = mTile->y();
    const auto tid = teamId();
    const bool playerBanner = (mType != eBannerType::enemy);
    for(int i = -hrange; i <= hrange; i++) {
        for(int j = -hrange; j <= hrange; j++) {
            const auto t = mBoard.tile(tx + i, ty + j);
            if(!t) continue;
            for(const auto& cc : t->characters()) {
                if(cc->dead()) continue;
                if(!eTeamIdHelpers::isEnemy(cc->teamId(), tid)) continue;
                if(!cc->isSoldier() && cc->type() != eCharacterType::wolf &&
                   !cc->isImmortal()) continue;
                mEnemyNear = true;
                return true;
            }
        }
    }
    mEnemyNear = false;
    return false;
}

void SoldierBanner::tickCombat(const int by) {
    if(mType != eBannerType::enemy) return;
    if(mHome || mAbroad || !mTile) {
        mCombatAssignments.clear();
        return;
    }
    enemyNear(by); // advance shared cache for soldiers
    mCombatRetargetCountdown -= by;
    if(mCombatRetargetCountdown > 0) return;
    mCombatRetargetCountdown = 500;
    updateCombatAssignments();
}

void SoldierBanner::clearCombatBlockages() {
    mCombatBlockages.clear();
}

bool SoldierBanner::combatAssignment(eSoldier* const s,
                                      CombatAssignment& a) const {
    const auto it = mCombatAssignments.find(s);
    if(it == mCombatAssignments.end()) return false;
    a = it->second;
    return a.soldier && a.standTile;
}

void SoldierBanner::setCombatBlockage(eSoldier* const s, eBuilding* const b) {
    if(!s) return;
    if(b) mCombatBlockages[s] = b;
    else mCombatBlockages.erase(s);
}

void SoldierBanner::updateCombatAssignments() {
    mCombatAssignments.clear();
    purgeDead();
    if(mSoldiers.empty()) return;

    const auto tid = teamId();
    const int range = eNumbers::sInvasionEngageDefenderRange;
    std::vector<eCharacter*> enemies;
    // Scan from the banner tile only — scanning from each soldier lets wandering
    // soldiers pull in enemies far from the formation, causing chase oscillation.
    if(mTile) {
        const int bx = mTile->x();
        const int by = mTile->y();
        for(int i = -range; i <= range; i++) {
            for(int j = -range; j <= range; j++) {
                const auto t = mBoard.tile(bx + i, by + j);
                if(!t) continue;
                for(const auto& c : t->characters()) {
                    if(c->dead()) continue;
                    if(!eTeamIdHelpers::isEnemy(c->teamId(), tid)) continue;
                    if(!canAttackCharacter(c.get())) continue;
                    if(std::find(enemies.begin(), enemies.end(), c.get()) ==
                       enemies.end()) {
                        enemies.push_back(c.get());
                    }
                }
            }
        }
    }
    if(enemies.empty()) return;

    std::vector<eTile*> usedTiles;
    for(const auto s : mSoldiers) {
        if(s->dead()) continue;
        const auto st = s->tile();
        if(!st) continue;
        // Already fighting — keep existing assignment, don't reassign mid-fight.
        const auto sat = s->actionType();
        if(sat == eCharacterActionType::fight ||
           sat == eCharacterActionType::fight2) {
            const auto it = mCombatAssignments.find(s);
            if(it != mCombatAssignments.end() && it->second.standTile)
                usedTiles.push_back(it->second.standTile);
            continue;
        }

        if(s->range() > 0) {
            const int sr = s->range();
            CombatAssignment best;
            int bestDist = 0;
            for(const auto enemy : enemies) {
                const auto et = enemy->tile();
                if(!et) continue;
                eTile* bestTile = nullptr;
                int bestTileDist = 0;
                for(int ai = -sr; ai <= sr; ai++) {
                    for(int aj = -sr; aj <= sr; aj++) {
                        const int d = std::max(abs(ai), abs(aj));
                        if(d != sr) continue;
                        const auto at = mBoard.tile(et->x() + ai, et->y() + aj);
                        if(!canStandOn(at)) continue;
                        if(hasLiveCombatUnit(at, s)) continue;
                        if(std::find(usedTiles.begin(), usedTiles.end(), at) !=
                           usedTiles.end()) continue;
                        const int dist = std::max(abs(at->x() - st->x()),
                                                  abs(at->y() - st->y()));
                        if(!bestTile || dist < bestTileDist) {
                            bestTile = at;
                            bestTileDist = dist;
                        }
                    }
                }
                if(!bestTile) continue;
                if(!best.target || bestTileDist < bestDist) {
                    best = {s, CombatAssignment::Intent::moveToSlot,
                            enemy, nullptr, bestTile};
                    bestDist = bestTileDist;
                }
            }
            if(best.target && best.standTile) {
                mCombatAssignments[s] = best;
                usedTiles.push_back(best.standTile);
            } else {
                const auto pt = place(s);
                if(pt) {
                    mCombatAssignments[s] = {
                        s, CombatAssignment::Intent::moveToSlot,
                        nullptr, nullptr, pt
                    };
                }
            }
            continue;
        }

        const auto blocked = mCombatBlockages.find(s);
        if(blocked != mCombatBlockages.end()) {
            const auto b = blocked->second;
            if(b && eTeamIdHelpers::isEnemy(b->teamId(), tid) &&
               eBuilding::sAttackable(b->type())) {
                mCombatAssignments[s] = {
                    s, CombatAssignment::Intent::clearObstacle,
                    nullptr, b, st
                };
                continue;
            }
            mCombatBlockages.erase(blocked);
        }

        CombatAssignment best;
        int bestDist = 0;
        for(const auto enemy : enemies) {
            const auto et = enemy->tile();
            if(!et) continue;
            eTile* bestTile = nullptr;
            int bestTileDist = 0;
            for(int ai = -1; ai <= 1; ai++) {
                for(int aj = -1; aj <= 1; aj++) {
                    if(!ai && !aj) continue;
                    const auto at = mBoard.tile(et->x() + ai, et->y() + aj);
                    if(!canStandOn(at)) continue;
                    if(hasLiveCombatUnit(at, s)) continue;
                    if(std::find(usedTiles.begin(), usedTiles.end(), at) !=
                       usedTiles.end()) continue;
                    const int d = std::max(abs(at->x() - st->x()),
                                           abs(at->y() - st->y()));
                    if(!bestTile || d < bestTileDist) {
                        bestTile = at;
                        bestTileDist = d;
                    }
                }
            }
            if(!bestTile) continue;
            if(!best.target || bestTileDist < bestDist ||
               (bestTileDist == bestDist &&
                enemy->targetedByCount() < best.target->targetedByCount())) {
                best = {
                    s, CombatAssignment::Intent::moveToSlot,
                    enemy, nullptr, bestTile
                };
                bestDist = bestTileDist;
            }
        }
        if(best.target && best.standTile) {
            mCombatAssignments[s] = best;
            usedTiles.push_back(best.standTile);
        } else {
            const auto pt = place(s);
            if(pt) {
                mCombatAssignments[s] = {
                    s, CombatAssignment::Intent::moveToSlot,
                    nullptr, nullptr, pt
                };
            }
        }
    }
}

bool SoldierBanner::nearestSoldier(const int fromX, const int fromY,
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

bool SoldierBanner::visibleOnTile() const {
    const auto onCid = onCityId();
    const auto onPid = mBoard.cityIdToPlayerId(onCid);
    const auto ppid = mBoard.personPlayer();
    if(onPid != ppid) return false;
    if(mType == eBannerType::enemy) return true;
    const auto pid = playerId();
    if(pid == ppid) return true;
    return mMilitaryAid;
}

void SoldierBanner::teleportSoldiersToPlaces() {
    for(const auto s : mSoldiers) {
        const auto tile = place(s);
        if(!tile) continue;
        s->changeTile(tile);
        s->setOrientation(soldierOrientation());
        const auto a = s->soldierAction();
        if(a) a->setCurrentAction(nullptr);
    }
}
