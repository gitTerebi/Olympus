#include "world-board.h"

#include "evectorhelpers.h"
#include "fileIO/esavearchive.h"

#include <iterator>

WorldBoard::WorldBoard() {}

void WorldBoard::nextMonth(GameBoard *const board)
{
    for (const auto &c : mCities)
    {
        c->nextMonth(board);
    }
}

void WorldBoard::nextYear()
{
    for (const auto &c : mCities)
    {
        c->nextYear();
    }
}

std::vector<stdsptr<WorldCity>> WorldBoard::receiveTribute() const
{
    std::vector<stdsptr<WorldCity>> r;
    for (const auto &c : mCities)
    {
        if (c->isCurrentCity())
            continue;
        if (!c->active())
            continue;
        if (!c->visible())
            continue;
        if (c->isOnBoard())
            continue;
        const auto type = c->type();
        const auto rel = c->relationship();
        const bool e = type == eCityType::colony ||
                       (type == eCityType::foreignCity &&
                        rel == eForeignCityRelationship::vassal);
        if (e)
            r.push_back(c);
    }
    return r;
}

void WorldBoard::addCity(const stdsptr<WorldCity> &c)
{
    mCities.push_back(c);
}

void WorldBoard::addRegion(const eWorldRegion &region)
{
    mRegions.push_back(region);
}

stdsptr<WorldCity> WorldBoard::currentCity() const
{
    for (const auto &c : mCities)
    {
        const bool cc = c->isCurrentCity();
        if (cc)
            return c;
    }
    return nullptr;
}

eCityId WorldBoard::firstFreeCityId() const
{
    std::vector<eCityId> used;
    for (const auto &c : mCities)
    {
        const auto cid = c->cityId();
        used.push_back(cid);
    }
    for (int i = 0;; i++)
    {
        const auto cid = static_cast<eCityId>(i);
        const bool r = eVectorHelpers::contains(used, cid);
        if (!r)
            return cid;
    }
    return eCityId::neutralFriendly;
}

ePlayerId WorldBoard::firstFreePlayerId() const
{
    std::vector<ePlayerId> used;
    for (const auto &c : mCityToPlayer)
    {
        const auto pid = c.second;
        used.push_back(pid);
    }
    for (int i = 0;; i++)
    {
        const auto pid = static_cast<ePlayerId>(i);
        const bool r = eVectorHelpers::contains(used, pid);
        if (!r)
            return pid;
    }
    return ePlayerId::neutralFriendly;
}

stdsptr<WorldCity> WorldBoard::cityWithId(const eCityId cid) const
{
    const int id = static_cast<int>(cid);
    if (id < 0)
        return nullptr;
    const int iMax = mCities.size() - 1;
    if (id > iMax)
        return nullptr;
    return mCities[id];
}

std::string WorldBoard::cityName(const eCityId cid) const
{
    const auto c = cityWithId(cid);
    if (!c)
        return "Invalid";
    return c->name();
}

stdsptr<WorldCity> WorldBoard::cityWithIOID(const int id) const
{
    for (const auto &c : mCities)
    {
        const int i = c->ioID();
        if (id == i)
            return c;
    }
    return nullptr;
}

void WorldBoard::setIOIDs() const
{
    int id = 0;
    for (const auto &c : mCities)
    {
        c->setIOID(id++);
    }
}

void WorldBoard::serialize(eSaveArchive &ar)
{
    if (ar.writing()) setIOIDs();

    ar.field("map", mMap);

    ar.arrayField("regions", mRegions,
        [](eSaveArchive& itemAr, eWorldRegion& r) {
            r.serialize(itemAr);
        });

    // cities — ctor-free, serialize takes board ptr
    {
        int nc = static_cast<int>(mCities.size());
        ar.field("cities.count", nc);
        if (ar.reading()) mCities.clear();
        for (int i = 0; i < nc; i++) {
            if (ar.reading()) {
                const auto c = std::make_shared<WorldCity>();
                ar.archiveField(("city." + std::to_string(i)).c_str(),
                    [this, &c](eSaveArchive& cAr) { c->serialize(cAr, this); });
                addCity(c);
            } else {
                ar.archiveField(("city." + std::to_string(i)).c_str(),
                    [this, i](eSaveArchive& cAr) { mCities[i]->serialize(cAr, this); });
            }
        }
        if (ar.reading()) setIOIDs();
    }

    // cityToPlayer map
    {
        int nc = static_cast<int>(mCityToPlayer.size());
        ar.field("cityToPlayer.count", nc);
        if (ar.reading()) {
            mCityToPlayer.clear();
            for (int i = 0; i < nc; i++) {
                eCityId cid; ePlayerId pid;
                ar.archiveField(("cityToPlayer." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& itemAr) {
                        itemAr.field("cityId", cid);
                        itemAr.field("playerId", pid);
                    });
                mCityToPlayer[cid] = pid;
            }
        } else {
            int i = 0;
            for (auto& kv : mCityToPlayer) {
                eCityId cid = kv.first;
                ePlayerId pid = kv.second;
                ar.archiveField(("cityToPlayer." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& itemAr) {
                        itemAr.field("cityId", cid);
                        itemAr.field("playerId", pid);
                    });
            }
        }
    }

    ar.field("personPlayer", mPersonPlayer);

    // playerToTeam map
    {
        int np = static_cast<int>(mPlayerToTeam.size());
        ar.field("playerToTeam.count", np);
        if (ar.reading()) {
            mPlayerToTeam.clear();
            for (int i = 0; i < np; i++) {
                ePlayerId pid; eTeamId tid;
                ar.archiveField(("playerToTeam." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& itemAr) {
                        itemAr.field("playerId", pid);
                        itemAr.field("teamId", tid);
                    });
                mPlayerToTeam[pid] = tid;
            }
        } else {
            int i = 0;
            for (auto& kv : mPlayerToTeam) {
                ePlayerId pid = kv.first;
                eTeamId tid = kv.second;
                ar.archiveField(("playerToTeam." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& itemAr) {
                        itemAr.field("playerId", pid);
                        itemAr.field("teamId", tid);
                    });
            }
        }
    }
}

stdsptr<WorldCity> WorldBoard::colonyWithId(const int id) const
{
    int i = 0;
    for (const auto &c : mCities)
    {
        const auto type = c->type();
        if (type != eCityType::colony)
            continue;
        if (i == id)
            return c;
        i++;
    }
    return nullptr;
}

void WorldBoard::activateColony(const int id)
{
    const auto c = colonyWithId(id);
    if (c)
        c->setState(eCityState::active);
}

void WorldBoard::setCitiesOnBoard(const std::vector<eCityId> &cids)
{
    for (const auto &c : mCities)
    {
        const auto cid = c->cityId();
        const bool is = eVectorHelpers::contains(cids, cid);
        c->setIsOnBoard(is);
    }
}

void WorldBoard::setColonyAsCurrentCity(const int id)
{
    for (const auto &c : mCities)
    {
        c->setIsCurrentCity(false);
    }
    const auto c = colonyWithId(id);
    if (c)
    {
        c->setIsCurrentCity(true);
        const auto cid = c->cityId();
        const auto pid = cityIdToPlayerId(cid);
        setPersonPlayer(pid);
    }
}

void WorldBoard::setParentAsCurrentCity()
{
    for (const auto &c : mCities)
    {
        const auto type = c->type();
        if (type == eCityType::parentCity)
        {
            c->setIsCurrentCity(true);
            const auto cid = c->cityId();
            const auto pid = cityIdToPlayerId(cid);
            setPersonPlayer(pid);
        }
        else
        {
            c->setIsCurrentCity(false);
        }
    }
}

ePlayerId WorldBoard::cityIdToPlayerId(const eCityId cid) const
{
    if (cid == eCityId::neutralFriendly)
    {
        return ePlayerId::neutralFriendly;
    }
    else if (cid == eCityId::neutralAggresive)
    {
        return ePlayerId::neutralAggresive;
    }
    const auto it = mCityToPlayer.find(cid);
    if (it == mCityToPlayer.end())
    {
        return ePlayerId::neutralFriendly;
    }
    return it->second;
}

eTeamId WorldBoard::cityIdToTeamId(const eCityId cid) const
{
    const auto pid = cityIdToPlayerId(cid);
    return playerIdToTeamId(pid);
}

eTeamId WorldBoard::playerIdToTeamId(const ePlayerId pid) const
{
    if (pid == ePlayerId::neutralFriendly)
    {
        return eTeamId::neutralFriendly;
    }
    else if (pid == ePlayerId::neutralAggresive)
    {
        return eTeamId::neutralAggresive;
    }
    const auto it = mPlayerToTeam.find(pid);
    if (it == mPlayerToTeam.end())
    {
        return eTeamId::neutralFriendly;
    }
    return it->second;
}

void WorldBoard::moveCityToPlayer(const eCityId cid, const ePlayerId pid)
{
    const auto c = cityWithId(cid);
    c->setPlayerId(pid);
    mCityToPlayer[cid] = pid;
}

void WorldBoard::setPlayerTeam(const ePlayerId pid, const eTeamId tid)
{
    mPlayerToTeam[pid] = tid;
}

std::vector<eCityId> WorldBoard::playerCities(const ePlayerId pid) const
{
    if (pid == ePlayerId::neutralFriendly)
    {
        return {eCityId::neutralFriendly};
    }
    else if (pid == ePlayerId::neutralAggresive)
    {
        return {eCityId::neutralAggresive};
    }
    std::vector<eCityId> result;
    for (const auto it : mCityToPlayer)
    {
        if (it.second == pid)
        {
            const auto cid = it.first;
            const auto c = cityWithId(cid);
            if (!c->visible())
                continue;
            if (!c->active())
                continue;
            result.push_back(cid);
        }
    }
    return result;
}

eCityId WorldBoard::playerCapital(const ePlayerId pid) const
{
    if (pid == ePlayerId::neutralFriendly)
    {
        return eCityId::neutralFriendly;
    }
    else if (pid == ePlayerId::neutralAggresive)
    {
        return eCityId::neutralAggresive;
    }
    for (const auto it : mCityToPlayer)
    {
        if (it.second == pid)
        {
            const auto cid = it.first;
            const auto c = cityWithId(cid);
            const bool isc = c->isCapitalOf(pid);
            if (isc)
                return cid;
        }
    }
    return eCityId::neutralFriendly;
}

std::vector<eCityId> WorldBoard::personPlayerCities() const
{
    return playerCities(mPersonPlayer);
}

eCityId WorldBoard::currentCityId() const
{
    const auto c = currentCity();
    if (!c)
        return eCityId::neutralFriendly;
    return c->cityId();
}
