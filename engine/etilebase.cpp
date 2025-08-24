#include "etilebase.h"

#include "buildings/ebuilding.h"
#include "spawners/ebanner.h"

#include <algorithm>

void eTileBase::setWalkableElev(const bool w) {
    mWalkableElev = w;
}

void eTileBase::setElevation(const bool e) {
    mElevation = e;
}

void eTileBase::decResource(const int by) {
    setResource(mResource - by);
}

void eTileBase::setResource(const int r) {
    mResource = std::clamp(r, 0, 100);
}

void eTileBase::setBusy(const bool b) {
    mBusy = b;
}

void eTileBase::setMarbleLevel(const int l) {
    mMarbleLevel = l;
}

bool eTileBase::hasTerrain(const eTerrain terr) const {
    return terrain() == terr;
}

bool eTileBase::hasBridge() const {
    return hasRoad() && (hasWater() || hasQuake());
}

bool eTileBase::hasWater() const {
    return terrain() == eTerrain::water;
}

bool eTileBase::hasDeepWater() const {
    return hasWater() && !isShoreTile();
}

bool eTileBase::hasQuake() const {
    return terrain() == eTerrain::quake;
}

bool eTileBase::walkableTerrain() const {
    const auto t = underBuildingType();
    if(t == eBuildingType::road) return true;
    const auto terr = terrain() & eTerrain::walkable;
    if(!static_cast<bool>(terr)) return false;
    if(!mWalkableElev && mElevation) return false;
    return true;
}

bool eTileBase::walkable() const {
    const auto t = underBuildingType();
    if(t == eBuildingType::road) return true;
    const auto terr = terrain() & eTerrain::walkable;
    if(!static_cast<bool>(terr)) return false;
    if(!mWalkableElev && mElevation) return false;
    return eBuilding::sWalkableBuilding(t);
}

bool eTileBase::isShoreTile(const eTerrain terr) const {
    if(mTerr != terr) return false;
    for(int x = -1; x < 2; x++) {
        for(int y = -1; y < 2; y++) {
            if(x == 0 && y == 0) continue;
            const auto t = tileRel(x, y);
            if(!t) continue;
            if(t->terrain() != terr) return true;
        }
    }
    return false;
}

bool eTileBase::isShoreTile() const {
    return isShoreTile(eTerrain::water);
}

bool eTileBase::isQuakeShoreTile() const {
    return isShoreTile(eTerrain::quake);
}

double eTileBase::sDistance(eTileBase* const t1,
                            eTileBase* const t2) {
    const int dx = t1->x() - t2->x();
    const int dy = t1->y() - t2->y();
    return sqrt(dx*dx + dy*dy);
}

void eTileBase::setSeed(const int s) {
    mSeed = s;
}

void eTileBase::setDX(const int x) {
    mDX = x;
}

void eTileBase::setDY(const int y) {
    mDY = y;
}

void eTileBase::setX(const int x) {
    mX = x;
}

void eTileBase::setY(const int y) {
    mY = y;
}

bool eTileBase::isCityEdge() const {
    if(!mTopRight || mTopRight->cityId() != cityId()) return true;
    if(!mBottomRight || mBottomRight->cityId() != cityId()) return true;
    if(!mBottomLeft || mBottomLeft->cityId() != cityId()) return true;
    if(!mTopLeft || mTopLeft->cityId() != cityId()) return true;
    return false;
}

std::vector<eTileBase::eTO> eTileBase::neighbours(const eTileVerifier& v) const {
    std::vector<eTileBase::eTO> result;
    for(int i = 0; i < 8; i++) {
        const auto o = static_cast<eOrientation>(i);
        const auto t = neighbour(o);
        if(!t) continue;
        if(!v || v(t)) result.push_back({o, t});
    }
    return result;
}

eTileBase::eTO eTileBase::randomNeighbour(const eTileVerifier& v) const {
    const auto ts = neighbours(v);
    if(ts.empty()) return {eOrientation::top, nullptr};
    return ts[eRand::rand() % ts.size()];
}

std::vector<eTileBase::eTO> eTileBase::diagonalNeighbours(const eTileVerifier& v) const {
    std::vector<eTileBase::eTO> result;
    for(int i = 0; i < 8; i += 2) {
        const auto o = static_cast<eOrientation>(i);
        const auto t = neighbour(o);
        if(!t) continue;
        if(!v || v(t)) result.push_back({o, t});
    }
    return result;
}

eTileBase::eTO eTileBase::randomDiagonalNeighbour(const eTileVerifier& v) const {
    const auto ts = diagonalNeighbours(v);
    if(ts.empty()) return {eOrientation::top, nullptr};
    return ts[eRand::rand() % ts.size()];
}

void eTileBase::setTerrain(const eTerrain terr) {
    mTerr = terr;
}

void eTileBase::setTopLeft(eTileBase* const tl) {
    mTopLeft = tl;
}

void eTileBase::setTopRight(eTileBase* const tr) {
    mTopRight = tr;
}

void eTileBase::setBottomRight(eTileBase* const br) {
    mBottomRight = br;
}

void eTileBase::setBottomLeft(eTileBase* const bl) {
    mBottomLeft = bl;
}

void eTileBase::read(eReadStream& src) {
    src >> mSeed;
    src >> mTerr;
    src >> mMarbleLevel;
    src >> mResource;
    src >> mElevation;
    src >> mWalkableElev;
    src >> mHasUrchin;
    src >> mHasFish;
    src >> mRoadblock;
    src >> mCityId;
}

void eTileBase::write(eWriteStream& dst) const {
    dst << mSeed;
    dst << mTerr;
    dst << mMarbleLevel;
    dst << mResource;
    dst << mElevation;
    dst << mWalkableElev;
    dst << mHasUrchin;
    dst << mHasFish;
    dst << mRoadblock;
    dst << mCityId;
}

