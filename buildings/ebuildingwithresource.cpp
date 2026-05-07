#include "ebuildingwithresource.h"

#include "characters/ecarttransporter.h"
#include "characters/actions/ecarttransporteraction.h"
#include "engine/egameboard.h"
#include "fileIO/esavearchive.h"

int eBuildingWithResource::addProduced(const eResourceType type, const int count) {
    const int c = add(type, count);
    const bool isPp = isPersonPlayer();
    if(isPp) {
        auto& board = getBoard();
        board.incProduced(type, c);
    }
    return c;
}

int eBuildingWithResource::stash(const eResourceType type, const int count) {
    const bool a = static_cast<bool>(type & mStashable);
    if(!a) return 0;
    for(auto& s : mStash) {
        if(s.fType == type) {
            s.fCount += count;
            return count;
        }
    }
    mStash.push_back(eStash{type, count});
    return count;
}

int eBuildingWithResource::stashCount(const eResourceType type) const {
    for(const auto& s : mStash) {
        if(static_cast<bool>(s.fType & type)) {
            return s.fCount;
        }
    }
    return 0;
}

void eBuildingWithResource::setStashable(const eResourceType s) {
    mStashable = s;
}

void eBuildingWithResource::addFromStash() {
    for(int i = 0; i < (int)mStash.size(); i++) {
        auto& s = mStash[i];
        const int a = add(s.fType, s.fCount);
        if(a >= s.fCount) {
            mStash.erase(mStash.begin() + i);
            i--;
        } else {
            s.fCount -= a;
        }
    }
}

int eBuildingWithResource::takeFromStash(const eResourceType t, const int count) {
    for(int i = 0; i < (int)mStash.size(); i++) {
        auto& s = mStash[i];
        if(!static_cast<bool>(t & s.fType)) continue;
        const int take = std::min(s.fCount, count);
        if(take >= s.fCount) {
            mStash.erase(mStash.begin() + i);
            i--;
        } else {
            s.fCount -= take;
        }
        return take;
    }
    return 0;
}

stdptr<eCartTransporter> eBuildingWithResource::spawnCart(
        const eCartActionTypeSupport s) {
    const auto t = centerTile();
    auto& board = getBoard();

    const auto c = e::make_shared<eCartTransporter>(board);
    c->setBothCityIds(cityId());
    c->setAtlantean(atlantean());
    c->changeTile(t);

    const auto a = e::make_shared<eCartTransporterAction>(
                       c.get(), this);
    c->setAction(a);
    c->setSupport(s);
    return c.get();
}

void eBuildingWithResource::read(eReadStream& src) {
    eBuilding::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eBuildingWithResource::write(eWriteStream& dst) const {
    eBuilding::write(dst);
    eSaveArchive ar(dst);
    const_cast<eBuildingWithResource*>(this)->serialize(ar);
}

void eBuildingWithResource::serialize(eSaveArchive& ar) {
    ar.value(mStashable);
    int ns = mStash.size();
    ar.value(ns);
    if(ar.reading()) {
        mStash.clear();
    }
    for(int i = 0; i < ns; i++) {
        eStash s;
        if(ar.writing()) {
            s = mStash[i];
        }
        ar.value(s.fType);
        ar.value(s.fCount);
        if(ar.reading()) {
            mStash.push_back(s);
        }
    }
}
