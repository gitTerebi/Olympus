#ifndef SOLDIER_BANNER_H
#define SOLDIER_BANNER_H

#include <vector>
#include <map>

#include "pointers/eobject.h"
#include "pointers/estdpointer.h"
#include "fileIO/estreams.h"
#include "engine/ecityid.h"

class eSoldier;
class eCharacter;
class eBuilding;
class GameBoard;
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

enum class eBannerFormationRole {
    melee,
    missile,
    cavalry,
    other
};

class SoldierBanner : public eObject {
public:
    struct CombatAssignment {
        enum class Intent {
            hold,
            moveToSlot,
            clearObstacle
        };

        eSoldier* soldier = nullptr;
        Intent intent = Intent::hold;
        eCharacter* target = nullptr;
        stdptr<eBuilding> targetBuilding = nullptr;
        eTile* standTile = nullptr;
    };

    SoldierBanner(const eBannerType type,
                   GameBoard& board);
    ~SoldierBanner();

    eBannerType type() const { return mType; }
    eCharacterType characterType() const;
    stdsptr<eSoldier> createSoldier(eTile* const t);

    int id() const { return mId; }

    void setIOID(const int id) { mIOID = id; }
    int ioID() const { return mIOID; }

    bool militaryAid() const { return mMilitaryAid; }
    void setMilitaryAid(const bool a) { mMilitaryAid = a; }

    void moveTo(const int x, const int y);
    void detachFromTile();
    void moveToPalace();
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
    eBannerFormationRole formationRole() const;
    void setFormationRole(const eBannerFormationRole role) {
        mFormationRole = role;
    }

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

    // Retaliation brain for enemy banners: only moves the whole banner toward a
    // defender after this banner was attacked. Strategic movement stays with
    // InvasionGeneral.
    void updateRetaliation(const int by);
    void signalRetaliationTarget(const int tx, const int ty);
    bool needsHelp() const;
    bool attackEnemyNearRetaliationPoint();
    bool combatAssignment(eSoldier* const s,
                          CombatAssignment& a) const;
    void setCombatBlockage(eSoldier* const s, eBuilding* const b);

    const std::string& name() const { return mName; }
    void setName(const std::string& n) { mName = n; }

    void serialize(eSaveArchive& ar);

    bool nearestSoldier(const int fromX, const int fromY,
                        int& toX, int& toY) const;

    bool visibleOnTile() const;

    void teleportSoldiersToPlaces();

    static void sPlaceDefault(std::vector<SoldierBanner*>& bs,
                              const int ctx, const int cty,
                              GameBoard& board);
    static void sPlaceNoPathTrace(std::vector<SoldierBanner*> bs,
                                  const int ctx, const int cty,
                                  GameBoard& board, const int dist,
                                  const int minDistFromEdge);
    static void sPlace(std::vector<SoldierBanner*> bs,
                       const int ctx, const int cty,
                       GameBoard& board, const int dist,
                       const int minDistFromEdge);
    static void sPlaceFacing(std::vector<SoldierBanner*> bs,
                             const int ctx, const int cty,
                             GameBoard& board,
                             const int facing,
                             const int lineDX,
                             const int lineDY,
                             const int dist,
                             const int minDistFromEdge);

    struct sFormationSlot { SoldierBanner* banner; int tx; int ty; };
    static std::vector<sFormationSlot> sFormationPositions(
                             std::vector<SoldierBanner*> bs,
                             const int ctx, const int cty,
                             const int lineDX, const int lineDY,
                             const int dist);

    static std::vector<SoldierBanner*> sPlayerBanners(
            const std::vector<SoldierBanner*>& bs,
            const ePlayerId playerId);
    static void sRotatePlayerBanners(
            const std::vector<SoldierBanner*>& bs,
            const ePlayerId playerId);
    static void sSetPlayerBannersFacing(
            const std::vector<SoldierBanner*>& bs,
            const ePlayerId playerId,
            const int facing);
    static void sPlacePlayerBannersFacing(
            const std::vector<SoldierBanner*>& bs,
            const ePlayerId playerId,
            const int ctx, const int cty,
            GameBoard& board,
            const int facing,
            const int lineDX,
            const int lineDY,
            const int dist,
            const int minDistFromEdge);
    static std::string sName(const eBannerType type,
                             const bool atlantean);
protected:
    void serializeFields(eSaveArchive& ar);
private:
    GameBoard& board() const;
    void updatePlaces();
    void updateCount();
    void callSoldier(eSoldier* const s);
    void purgeDead();
    void updateCombatAssignments();

    const eBannerType mType;
    const int mId;
    std::string mName;

    bool mMilitaryAid = false;

    int mIOID = -1;

    bool mHome = true;
    bool mAbroad = false;
    bool mSelected = false;

    GameBoard& mBoard;

    eTile* mTile = nullptr;

    int mCount = 0;
    int mFacing = 0; // degrees, 0 = north, 90 = east, etc.

    eBannerFormationRole mFormationRole = eBannerFormationRole::other;

    // Throttle for retaliation so the brain doesn't re-issue moveTo every frame.
    // Runtime-only (combat re-derives next tick); never serialized.
    int mCombatRetargetCountdown = 0;
    int mRetaliationX = 0;
    int mRetaliationY = 0;
    int mRetaliationTime = 0;

    eCityId mCityId = eCityId::neutralFriendly;
    eCityId mOnCityId = eCityId::neutralFriendly;

    std::map<eSoldier*, eTile*> mPlaces;
    std::vector<eSoldier*> mSoldiers;
    std::map<eSoldier*, CombatAssignment> mCombatAssignments;
    std::map<eSoldier*, stdptr<eBuilding>> mCombatBlockages;
};

#endif // SOLDIER_BANNER_H
