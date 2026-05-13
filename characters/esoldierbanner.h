#ifndef ESOLDIERBANNER_H
#define ESOLDIERBANNER_H

#include <vector>
#include <map>

#include "pointers/eobject.h"
#include "fileIO/estreams.h"
#include "engine/ecityid.h"

class eSoldier;
class eGameBoard;
class eTile;
class eSaveArchive;
enum class eCharacterType;
enum class eOrientation;

enum class eBannerType {
    hoplite,
    horseman,
    rockThrower,
    amazon,
    aresWarrior,
    enemy,
    trireme
};

class eSoldierBanner : public eObject {
public:
    eSoldierBanner(const eBannerType type,
                   eGameBoard& board);
    ~eSoldierBanner();

    eBannerType type() const { return mType; }
    eCharacterType characterType() const;
    stdsptr<eSoldier> createSoldier(eTile* const t);

    int id() const { return mId; }

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    bool militaryAid() const { return mMilitaryAid; }
    void setMilitaryAid(const bool a) { mMilitaryAid = a; }

    void moveTo(const int x, const int y);
    void moveToDefault();
    void goHome();
    void backFromHome();
    void goAbroad();
    void backFromAbroad(int& wait);
    void callSoldiers();

    bool isHome() const { return mHome; }
    bool isGoingHome() const;
    bool isAbroad() const { return mAbroad; }

    void addSoldier(eSoldier* const s);
    void removeSoldier(eSoldier* const s);

    eTile* tile() const { return mTile; }
    eTile* place(eSoldier* const s) const;

    void killAll();
    void killAllWithCorpse();

    void setSelected(const bool s) { mSelected = s; }
    bool selected() const { return mSelected; }

    void setBothCityIds(const eCityId cid);

    void setCityId(const eCityId pid) { mCityId = pid; }
    eCityId cityId() const { return mCityId; }

    void setOnCityId(const eCityId pid) { mOnCityId = pid; }
    eCityId onCityId() const { return mOnCityId; }

    bool atlantean() const;

    ePlayerId playerId() const;
    eTeamId teamId() const;

    int count() const { return mCount; }
    void incCount();
    void decCount();

    int facing() const { return mFacing; }
    eOrientation soldierOrientation() const;
    void setFacing(int facing);
    void setFacingOnLoad(int facing);
    void commandFormation(int facing, int lineDX, int lineDY);

    bool stationary() const;
    bool fighting() const;

    const std::string& name() const { return mName; }
    void setName(const std::string& n) { mName = n; }

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
    void serializeJson(class eJsonArchive& ar);

    bool nearestSoldier(const int fromX, const int fromY,
                        int& toX, int& toY) const;

    bool visibleOnTile() const;

    void teleportSoldiersToPlaces();

    static void sPlaceDefault(std::vector<eSoldierBanner*>& bs,
                              const int ctx, const int cty,
                              eGameBoard& board);
    static void sPlaceNoPathTrace(std::vector<eSoldierBanner*> bs,
                                  const int ctx, const int cty,
                                  eGameBoard& board, const int dist,
                                  const int minDistFromEdge);
    static void sPlace(std::vector<eSoldierBanner*> bs,
                       const int ctx, const int cty,
                       eGameBoard& board, const int dist,
                       const int minDistFromEdge);
    static void sPlaceFacing(std::vector<eSoldierBanner*> bs,
                             const int ctx, const int cty,
                             eGameBoard& board,
                             const int facing,
                             const int lineDX,
                             const int lineDY,
                             const int dist,
                             const int minDistFromEdge);
    static std::vector<eSoldierBanner*> sPlayerBanners(
            const std::vector<eSoldierBanner*>& bs,
            const ePlayerId playerId);
    static void sRotatePlayerBanners(
            const std::vector<eSoldierBanner*>& bs,
            const ePlayerId playerId);
    static void sSetPlayerBannersFacing(
            const std::vector<eSoldierBanner*>& bs,
            const ePlayerId playerId,
            const int facing);
    static void sPlacePlayerBannersFacing(
            const std::vector<eSoldierBanner*>& bs,
            const ePlayerId playerId,
            const int ctx, const int cty,
            eGameBoard& board,
            const int facing,
            const int lineDX,
            const int lineDY,
            const int dist,
            const int minDistFromEdge);
    static std::string sName(const eBannerType type,
                             const bool atlantean);
private:
    void updatePlaces();
    void updateCount();
    void callSoldier(eSoldier* const s);
    void purgeDead();
    void serialize(eSaveArchive& ar);

    const eBannerType mType;
    const int mId;
    std::string mName;

    bool mMilitaryAid = false;

    int mIOID = -1;

    bool mHome = true;
    bool mAbroad = false;
    bool mSelected = false;

    eGameBoard& mBoard;

    eTile* mTile = nullptr;

    int mCount = 0;
    int mFacing = 0; // degrees, 0 = north, 90 = east, etc.

    eCityId mCityId = eCityId::neutralFriendly;
    eCityId mOnCityId = eCityId::neutralFriendly;

    std::map<eSoldier*, eTile*> mPlaces;
    std::vector<eSoldier*> mSoldiers;
};

#endif // ESOLDIERBANNER_H
