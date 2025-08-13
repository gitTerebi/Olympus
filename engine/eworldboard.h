#ifndef EWORLDBOARD_H
#define EWORLDBOARD_H

#include <vector>

#include "eworldcity.h"
#include "pointers/estdselfref.h"
#include "eworldregion.h"

enum class eWorldMap {
    greece1,
    greece2,
    greece3,
    greece4,
    greece5,
    greece6,
    greece7,
    greece8,

    poseidon1,
    poseidon2,
    poseidon3,
    poseidon4
};

class eWorldBoard {
public:
    eWorldBoard();

    void nextMonth(eGameBoard* const board);
    void nextYear();

    std::vector<stdsptr<eWorldCity>> getTribute(
            const ePlayerId pid) const;

    void setHomeCity(const stdsptr<eWorldCity>& hc);
    void addCity(const stdsptr<eWorldCity>& c);

    void addRegion(const eWorldRegion& region);
    const std::vector<eWorldRegion>& regions() const
    { return mRegions; }

    stdsptr<eWorldCity> currentCity() const;
    const std::vector<stdsptr<eWorldCity>>& cities() const
    { return mCities; }

    eWorldMap map() const { return mMap; }
    void setMap(const eWorldMap m) { mMap = m; }

    eCityId firstFreeCityId() const;
    ePlayerId firstFreePlayerId() const;

    stdsptr<eWorldCity> cityWithId(const eCityId cid) const;
    std::string cityName(const eCityId cid) const;
    stdsptr<eWorldCity> cityWithIOID(const int id) const;
    void setIOIDs() const;

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);

    void attackedAlly(const ePlayerId pid);

    stdsptr<eWorldCity> colonyWithId(const int id) const;
    void activateColony(const int id);

    void setCitiesOnBoard(const std::vector<eCityId>& cids);
    void setColonyAsCurrentCity(const int id);
    void setParentAsCurrentCity();

    bool editorMode() const { return mEditorMode; }
    void setEditorMode(const bool m) { mEditorMode = m; }

    ePlayerId cityIdToPlayerId(const eCityId cid) const;
    eTeamId cityIdToTeamId(const eCityId cid) const;
    eTeamId playerIdToTeamId(const ePlayerId pid) const;
    void moveCityToPlayer(const eCityId cid, const ePlayerId pid);
    void setPlayerTeam(const ePlayerId pid, const eTeamId tid);
    std::vector<eCityId> playerCities(const ePlayerId pid) const;
    eCityId playerCapital(const ePlayerId pid) const;
    std::vector<eCityId> personPlayerCities() const;
    void setPersonPlayer(const ePlayerId pid) { mPersonPlayer = pid; }
    ePlayerId personPlayer() const { return mPersonPlayer; }
private:
    bool mEditorMode = false;
    eWorldMap mMap{eWorldMap::greece8};
    std::vector<stdsptr<eWorldCity>> mCities;
    std::vector<eWorldRegion> mRegions;

    std::map<eCityId, ePlayerId> mCityToPlayer;
    ePlayerId mPersonPlayer = ePlayerId::player0;
    std::map<ePlayerId, eTeamId> mPlayerToTeam;
};

#endif // EWORLDBOARD_H
