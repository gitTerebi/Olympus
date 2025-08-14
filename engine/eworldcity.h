#ifndef EWORLDCITY_H
#define EWORLDCITY_H

#include <string>
#include <set>

#include "eresourcetype.h"
#include "pointers/estdselfref.h"
#include "fileIO/estreams.h"
#include "engine/ecityid.h"

enum class eNationality {
    greek,
    trojan,
    persian,
    centaur,
    amazon,

    egyptian,
    mayan,
    phoenician,
    oceanid,
    atlantean,
};

enum class eDistantDirection {
    none,
    N,
    NE,
    E,
    SE,
    S,
    SW,
    W,
    NW
};

enum class eCityType {
    parentCity,
    colony,
    foreignCity,
    distantCity,
    enchantedPlace,
    destroyedCity
};

enum class eForeignCityRelationship {
    vassal,
    ally,
    rival
};

enum class eCityAttitude {
    // ally, colony, vassal
    philanthropic, // - recently pleased
    resentful, // - recently displeased

    // ally
    helpful,
    congenial,
    sympathetic,
    apatheticA,
    annoyed,

    // colony, vassal
    devoted,
    dedicated,
    loyal,
    bitter,
    angry,

    // rival

    docile, // - recently pleased
    hostile, // - recently displeased

    admiring,
    respectful,
    apatheticR,
    displeased,
    furious,

    insubordinate
};

enum class eCityState {
    active,
    inactive
};

enum class eNamePlace {
    left,
    top,
    right,
    bottom
};

struct eResourceTrade {
    eResourceType fType;
    std::map<ePlayerId, int> fUsed;
    int fMax;

    int used(const ePlayerId pid) const {
        const auto it = fUsed.find(pid);
        if(it == fUsed.end()) return 0;
        return it->second;
    }

    void incUsed(const ePlayerId pid, const int by) {
        const auto it = fUsed.find(pid);
        if(it == fUsed.end()) {
            fUsed[pid] = by;
        } else {
            fUsed[pid] += by;
        }
    }

    void zeroUsed() {
        for(const auto& u : fUsed) {
            fUsed[u.first] = 0;
        }
    }

    void write(eWriteStream& dst) const {
        dst << fType;

        dst << fUsed.size();
        for(const auto& u : fUsed) {
            dst << u.first;
            dst << u.second;
        }

        dst << fMax;
    }

    void read(eReadStream& src) {
        src >> fType;

        int nu;
        src >> nu;
        for(int i = 0; i < nu; i++) {
            ePlayerId pid;
            src >> pid;
            int c;
            src >> c;
            fUsed[pid] = c;
        }

        src >> fMax;
    }
};

class eWorldCity {
public:
    eWorldCity() {}
    eWorldCity(const eCityType type,
               const eCityId id,
               const std::string& name,
               const double x, const double y);

    bool rebellion() const { return mRebellion; }
    void setRebellion(const bool r) { mRebellion = r; }

    bool tradeShutdown() const { return mTradeShutdown; }
    void setTradeShutdown(const bool s) { mTradeShutdown = s; }

    static std::string sTypeName(const eCityType type);
    void setType(const eCityType type) { mType = type; }
    eCityType type() const { return mType; }

    eCityId cityId() const { return mCityId; }
    void setCityId(const eCityId cid) { mCityId = cid; }

    bool isDistant() const;
    bool isVassal() const;
    bool isColony() const;
    bool isParentCity() const;
    bool isRival() const;
    bool isAlly() const;
    bool isOnBoardColony() const;
    bool isOnBoardNeutral() const;
    bool isOnBoardEnemy() const;

    static std::string sNationalityName(const eNationality type);
    void setNationality(const eNationality n) { mNationality = n; }
    eNationality nationality() const { return mNationality; }

    static std::string sDirectionName(const eDistantDirection d);
    void setDirection(const eDistantDirection n) { mDirection = n; }
    eDistantDirection direction() const { return mDirection; }

    void move(const double x, const double y);

    double x() const { return mX; }
    double y() const { return mY; }

    bool isCurrentCity() const { return mIsCurrentCity; }
    void setIsCurrentCity(const bool c) { mIsCurrentCity = c; }

    bool isOnBoard() const { return mIsOnBoard; }
    void setIsOnBoard(const bool is) { mIsOnBoard = is; }

    static std::string sRelationshipName(
            const eForeignCityRelationship r);
    eForeignCityRelationship relationship() const { return mRel; }
    void setRelationship(const eForeignCityRelationship r) { mRel = r; }

    static std::string sStateName(const eCityState s);
    void setState(const eCityState state) { mState = state; }
    eCityState state() const { return mState; };
    bool active() const { return mType != eCityType::colony ||
                                 mState == eCityState::active; }

    bool visible() const { return mVisible; }
    void setVisible(const bool v) { mVisible = v; }

    static std::string sAttitudeName(const eCityAttitude at);
    eCityAttitude attitudeClass(const ePlayerId pid) const;
    double attitude(const ePlayerId pid) const;
    void setAttitude(const double a, const ePlayerId pid);
    void incAttitude(const double a, const ePlayerId pid);

    eNamePlace namePlace() const { return mNamePlace; }
    void setNamePlace(const eNamePlace p) { mNamePlace = p; }

    static std::vector<std::string> sNames();
    void setName(const std::string& name);
    const std::string& name() const { return mName; }

    static std::vector<std::string> sLeaders();
    void setLeader(const std::string& name);
    const std::string& leader() const { return mLeader; }

    std::string anArmy() const;

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    void nextMonth(eGameBoard* const board);
    void nextYear();

    void setTroops(const int t) { mTroops = t; }
    int troops() const { return mTroops; }
    void troopsByType(int& infantry,
                      int& cavalry,
                      int& archers) const;

    int shields() const;
    int militaryStrength() const { return mMilitaryStrength; }
    void setMilitaryStrength(const int s);

    int wealth() const { return mWealth; }
    void setWealth(const int w) { mWealth = w; }

    void setWaterTrade(const bool w, const eCityId cid);
    bool waterTrade(const eCityId cid) const;
    const std::set<eCityId>& waterTrades() const
    { return mWaterTrade; }

    bool trades() const;

    void setAbroad(const bool a) { mAbroad = a; }
    bool abroad() const { return mAbroad; }

    const std::vector<eResourceTrade>& buys() const
    { return mBuys; }
    std::vector<eResourceTrade>& buys()
    { return mBuys; }
    bool buys(const eResourceType type) const;
    void addBuys(const eResourceTrade& b);

    void changeDemand(const eResourceType res, const int by);
    void changeSupply(const eResourceType res, const int by);

    const std::vector<eResourceTrade>& sells() const
    { return mSells; }
    std::vector<eResourceTrade>& sells()
    { return mSells; }
    bool sells(const eResourceType type) const;
    void addSells(const eResourceTrade& s);

    void setTributeType(const eResourceType r) { mTributeType = r; }
    void setTributeCount(const int c) { mTributeCount = c; }

    eResourceType tributeType() const { return mTributeType; }
    int tributeCount() const { return mTributeCount; }

    void setRecTributeType(const eResourceType r) { mRecTributeType = r; }
    void setRecTributeCount(const int c) { mRecTributeCount = c; }

    eResourceType recTributeType() const { return mRecTributeType; }
    int recTributeCount() const { return mRecTributeCount; }

    void write(eWriteStream& dst) const;
    void read(eReadStream& src, eWorldBoard* const board);

    void gifted(const eResourceType type, const int count);
    bool acceptsGift(const eResourceType type, const int count) const;

    bool conqueredByRival() const { return mConqueredBy.get(); }
    void setConqueredBy(const stdsptr<eWorldCity>& c) { mConqueredBy = c; }

    void setPlayerId(const ePlayerId pid);
    ePlayerId playerId() const { return mPlayerId; }

    void setCapitalOf(const ePlayerId pid);
    bool isCapitalOf(const ePlayerId pid) const;

    static stdsptr<eWorldCity> sCreateAthens(const eCityId cid);
    static stdsptr<eWorldCity> sCreateSparta(const eCityId cid);
    static stdsptr<eWorldCity> sCreateKnossos(const eCityId cid);
    static stdsptr<eWorldCity> sCreateCorinth(const eCityId cid);
    static stdsptr<eWorldCity> sCreateOlympia(const eCityId cid);

    static stdsptr<eWorldCity> sCreateEgypt(const eCityId cid);
    static stdsptr<eWorldCity> sCreateCyprus(const eCityId cid);

    static stdsptr<eWorldCity> sCreateTroy(const eCityId cid);

    static stdsptr<eWorldCity> sCreateMtPelion(const eCityId cid);

    static stdsptr<eWorldCity> sCreateSardis(const eCityId cid);
    static stdsptr<eWorldCity> sCreateHattusas(const eCityId cid);
private:
    int mIOID = -1;

    stdsptr<eWorldCity> mConqueredBy;

    ePlayerId mPlayerId = ePlayerId::neutralFriendly;
    ePlayerId mCapitalOf = ePlayerId::neutralFriendly;
    bool mIsCurrentCity = false;
    bool mIsOnBoard = false;
    eCityType mType{eCityType::foreignCity};

    eNationality mNationality{eNationality::greek};
    eDistantDirection mDirection{eDistantDirection::none};
    eCityState mState{eCityState::inactive};

    eCityId mCityId;

    eNamePlace mNamePlace = eNamePlace::bottom;

    std::string mName;
    int mNameString = -1;

    std::string mLeader;
    int mLeaderString = -1;

    double mX = 0.5;
    double mY = 0.5;

    bool mTradeShutdown = false;

    bool mRebellion = false;

    eForeignCityRelationship mRel{eForeignCityRelationship::ally};
    std::map<ePlayerId, double> mAtt;

    bool mAbroad = false;

    int mMilitaryStrength = 1; // 1-5
    int mTroops = 0;
    int mYearsElapsed = 0;
    int mWealth = 1; // 1-5

    std::set<eCityId> mWaterTrade;
    bool mVisible = true;

    std::vector<eResourceTrade> mBuys;
    std::vector<eResourceTrade> mSells;

    std::map<eResourceType, int> mReceived;

    eResourceType mTributeType = eResourceType::drachmas;
    int mTributeCount = 500;

    eResourceType mRecTributeType = eResourceType::drachmas;
    int mRecTributeCount = 100;
};

#endif // EWORLDCITY_H
