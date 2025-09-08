#include "eworldboard.h"

#include "evectorhelpers.h"

eWorldBoard::eWorldBoard() {}

void eWorldBoard::nextMonth(eGameBoard* const board) {
    for(const auto& c : mCities) {
        c->nextMonth(board);
    }
}

void eWorldBoard::nextYear() {
    for(const auto& c : mCities) {
        c->nextYear();
    }
}

std::vector<stdsptr<eWorldCity>> eWorldBoard::getTribute() const {
    std::vector<stdsptr<eWorldCity>> r;
    for(const auto& c : mCities) {
        if(c->isCurrentCity()) continue;
        if(!c->active()) continue;
        if(!c->visible()) continue;
        if(c->isOnBoard()) continue;
        const auto type = c->type();
        const auto rel = c->relationship();
        const bool e = type == eCityType::colony ||
                       (type == eCityType::foreignCity &&
                        rel == eForeignCityRelationship::vassal);
        if(e) r.push_back(c);
    }
    return r;
}

void eWorldBoard::addCity(const stdsptr<eWorldCity>& c) {
    mCities.push_back(c);
}

void eWorldBoard::addRegion(const eWorldRegion& region) {
    mRegions.push_back(region);
}

stdsptr<eWorldCity> eWorldBoard::currentCity() const {
    for(const auto& c : mCities) {
        const bool cc = c->isCurrentCity();
        if(cc) return c;
    }
    return nullptr;
}

eCityId eWorldBoard::firstFreeCityId() const {
    std::vector<eCityId> used;
    for(const auto& c : mCities) {
        const auto cid = c->cityId();
        used.push_back(cid);
    }
    for(int i = 0;; i++) {
        const auto cid = static_cast<eCityId>(i);
        const bool r = eVectorHelpers::contains(used, cid);
        if(!r) return cid;
    }
    return eCityId::neutralFriendly;
}

ePlayerId eWorldBoard::firstFreePlayerId() const {
    std::vector<ePlayerId> used;
    for(const auto& c : mCityToPlayer) {
        const auto pid = c.second;
        used.push_back(pid);
    }
    for(int i = 0;; i++) {
        const auto pid = static_cast<ePlayerId>(i);
        const bool r = eVectorHelpers::contains(used, pid);
        if(!r) return pid;
    }
    return ePlayerId::neutralFriendly;
}

stdsptr<eWorldCity> eWorldBoard::cityWithId(const eCityId cid) const {
    const int id = static_cast<int>(cid);
    if(id < 0) return nullptr;
    const int iMax = mCities.size() - 1;
    if(id > iMax) return nullptr;
    return mCities[id];
}

std::string eWorldBoard::cityName(const eCityId cid) const {
    const auto c = cityWithId(cid);
    if(!c) return "Invalid";
    return c->name();
}

stdsptr<eWorldCity> eWorldBoard::cityWithIOID(const int id) const {
    for(const auto& c : mCities) {
        const int i = c->ioID();
        if(id == i) return c;
    }
    return nullptr;
}

void eWorldBoard::setIOIDs() const {
    int id = 0;
    for(const auto& c : mCities) {
        c->setIOID(id++);
    }
}

void eWorldBoard::write(eWriteStream& dst) const {
    setIOIDs();

    dst << mMap;

    dst << mRegions.size();
    for(const auto& r : mRegions) {
        r.write(dst);
    }

    dst << mCities.size();
    for(const auto& c : mCities) {
        c->write(dst);
    }

    {
        dst << mCityToPlayer.size();
        for(const auto ctp : mCityToPlayer) {
            dst << ctp.first;
            dst << ctp.second;
        }
    }

    dst << mPersonPlayer;

    {
        dst << mPlayerToTeam.size();
        for(const auto ptt : mPlayerToTeam) {
            dst << ptt.first;
            dst << ptt.second;
        }
    }
}

void eWorldBoard::read(eReadStream& src) {
    src >> mMap;

    int nr;
    src >> nr;
    for(int i = 0; i < nr; i++) {
        auto& r = mRegions.emplace_back();
        r.read(src);
    }

    int nc;
    src >> nc;
    for(int i = 0; i < nc; i++) {
        const auto c = std::make_shared<eWorldCity>();
        c->read(src, this);
        addCity(c);
    }

    {
        int nc;
        src >> nc;
        for(int i = 0; i < nc; i++) {
            eCityId cid;
            src >> cid;
            ePlayerId pid;
            src >> pid;
            mCityToPlayer[cid] = pid;
        }
    }

    src >> mPersonPlayer;

    {
        int np;
        src >> np;
        for(int i = 0; i < np; i++) {
            ePlayerId pid;
            src >> pid;
            eTeamId tid;
            src >> tid;
            mPlayerToTeam[pid] = tid;
        }
    }
}

void eWorldBoard::attackedAlly(const ePlayerId pid) {
    for(const auto& c : mCities) {
        const auto type = c->type();
        const auto rel = c->relationship();
        if(type == eCityType::foreignCity &&
           rel == eForeignCityRelationship::ally) {
            c->incAttitude(-25, pid);
        }
    }
}

stdsptr<eWorldCity> eWorldBoard::colonyWithId(const int id) const {
    int i = 0;
    for(const auto& c : mCities) {
        const auto type = c->type();
        if(type != eCityType::colony) continue;
        if(i == id) return c;
        i++;
    }
    return nullptr;
}

void eWorldBoard::activateColony(const int id) {
    const auto c = colonyWithId(id);
    if(c) c->setState(eCityState::active);
}

void eWorldBoard::setCitiesOnBoard(const std::vector<eCityId>& cids) {
    for(const auto& c : mCities) {
        const auto cid = c->cityId();
        const bool is = eVectorHelpers::contains(cids, cid);
        c->setIsOnBoard(is);
    }
}

void eWorldBoard::setColonyAsCurrentCity(const int id) {
    for(const auto& c : mCities) {
        c->setIsCurrentCity(false);
    }
    const auto c = colonyWithId(id);
    if(c) {
        c->setIsCurrentCity(true);
        const auto cid = c->cityId();
        const auto pid = cityIdToPlayerId(cid);
        setPersonPlayer(pid);
    }
}

void eWorldBoard::setParentAsCurrentCity() {
    for(const auto& c : mCities) {
        const auto type = c->type();
        if(type == eCityType::parentCity) {
            c->setIsCurrentCity(true);
            const auto cid = c->cityId();
            const auto pid = cityIdToPlayerId(cid);
            setPersonPlayer(pid);
        } else {
            c->setIsCurrentCity(false);
        }
    }
}

ePlayerId eWorldBoard::cityIdToPlayerId(const eCityId cid) const {
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

eTeamId eWorldBoard::cityIdToTeamId(const eCityId cid) const {
    const auto pid = cityIdToPlayerId(cid);
    return playerIdToTeamId(pid);
}

eTeamId eWorldBoard::playerIdToTeamId(const ePlayerId pid) const {
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

void eWorldBoard::moveCityToPlayer(const eCityId cid, const ePlayerId pid) {
    const auto c = cityWithId(cid);
    c->setPlayerId(pid);
    mCityToPlayer[cid] = pid;
}

void eWorldBoard::setPlayerTeam(const ePlayerId pid, const eTeamId tid) {
    mPlayerToTeam[pid] = tid;
}

std::vector<eCityId> eWorldBoard::playerCities(const ePlayerId pid) const {
    if(pid == ePlayerId::neutralFriendly) {
        return {eCityId::neutralFriendly};
    } else if(pid == ePlayerId::neutralAggresive) {
        return {eCityId::neutralAggresive};
    }
    std::vector<eCityId> result;
    for(const auto it : mCityToPlayer) {
        if(it.second == pid) {
            const auto cid = it.first;
            const auto c = cityWithId(cid);
            if(!c->visible()) continue;
            if(!c->active()) continue;
            result.push_back(cid);
        }
    }
    return result;
}

eCityId eWorldBoard::playerCapital(const ePlayerId pid) const {
    if(pid == ePlayerId::neutralFriendly) {
        return eCityId::neutralFriendly;
    } else if(pid == ePlayerId::neutralAggresive) {
        return eCityId::neutralAggresive;
    }
    for(const auto it : mCityToPlayer) {
        if(it.second == pid) {
            const auto cid = it.first;
            const auto c = cityWithId(cid);
            const bool isc = c->isCapitalOf(pid);
            if(isc) return cid;
        }
    }
    return eCityId::neutralFriendly;
}

std::vector<eCityId> eWorldBoard::personPlayerCities() const {
    return playerCities(mPersonPlayer);
}

eCityId eWorldBoard::currentCityId() const {
    const auto c = currentCity();
    if(!c) return eCityId::neutralFriendly;
    return c->cityId();
}
