#ifndef EBOARDCITY_H
#define EBOARDCITY_H

#include "ecityid.h"
#include "ewagerate.h"
#include "etaxrate.h"
#include "edate.h"
#include "eresourcetype.h"
#include "boardData/epopulationdata.h"
#include "boardData/ehusbandrydata.h"
#include "boardData/eemploymentdata.h"
#include "eemploymentdistributor.h"
#include "buildings/eavailablebuildings.h"
#include "characters/eenlistedforces.h"
#include "ai/eaicityplan.h"
#include "egameevents.h"

class ePalace;
class eSanctuary;
class eHerosHall;
class eStorageBuilding;
class eEmployingBuilding;
class eTradePost;
class eSpawner;
class eMonster;
struct eMilitaryAid;
class eInvasionEvent;
class ePlague;
class eSmallHouse;
class eBoardPlayer;
class eAgoraBase;
class eInvasionHandler;
class eStadium;
class eMuseum;
class eHouseBase;
enum class eCharacterType;
enum class eBuildingMode;
enum struct eHeroType;
enum class eBannerTypeS;
enum class eMonsterType;
struct eEpisode;

enum class eImmigrationLimitedBy {
    none,
    lackOfVacancies,
    lowWages,
    unemployment,
    lackOfFood,
    highTaxes,
    prolongedDebt,
    excessiveMilitaryService
};

enum class eGames {
    isthmian,
    nemean,
    pythian,
    olympian
};

class eBoardCity {
    friend class eGameBoard;
public:
    eBoardCity(const eCityId cid, eGameBoard& board);

    eCityId id() const { return mId; }
//    void setId(const eCityId id) { mId = id; }

    const std::vector<eTile*>& tiles() const { return mTiles; }
    void updateTiles();
    void clearTiles();
    const SDL_Rect& tileBRect() const { return mTileBRect; }

    void incTime(const int by);

    void acquired();

    bool atlantean() const { return mAtlantean; }
    void setAtlantean(const bool a) { mAtlantean = a; }

    int basePrice() const { return mBasePrice; }

    void updateCoverage();

    void payTaxes(const int d, const int people);

    void nextYear();
    void nextMonth();

    double taxRateF() const;
    eTaxRate taxRate() const { return mTaxRate; }
    eWageRate wageRate() const { return mWageRate; }

    void setTaxRate(const eTaxRate tr);
    void setWageRate(const eWageRate wr);

    int taxesPaidThisYear() const { return mTaxesPaidThisYear; }
    int taxesPaidLastYear() const { return mTaxesPaidLastYear; }
    int peoplePaidTaxesThisYear() const { return mPeoplePaidTaxesThisYear; }
    int peoplePaidTaxesLastYear() const { return mPeoplePaidTaxesLastYear; }

    void saveEditorCityPlan();
    void editorClearBuildings();
    void editorDisplayBuildings();

    void buildScheduled();
    bool previousDistrictFulfilled();
    void buildNextDistrict(const int drachmas);
    void setCurrentDistrictId(const int id) { mCurrentDistrictId = id; }
    using eCondition = eDistrictReadyCondition;
    std::vector<eCondition> getDistrictReadyConditions();
    void addDistrictReadyCondition(const eCondition& c);
    void removeDistrictReadyCondition(const int id);
    void setDistrictReadyCondition(const int id, const eCondition& c);

    void registerBuilding(eBuilding* const b);
    bool unregisterBuilding(eBuilding* const b);

    void registerEmplBuilding(eEmployingBuilding* const b);
    bool unregisterEmplBuilding(eEmployingBuilding* const b);

    bool supportsBuilding(const eBuildingMode mode) const;
    bool availableBuilding(const eBuildingType type,
                           const int id = -1) const;
    void built(const eBuildingType type,
               const int id = -1);
    void destroyed(const eBuildingType type,
                   const int id = -1);
    void allow(const eBuildingType type,
               const int id = -1);
    void disallow(const eBuildingType type,
                  const int id = -1);

    bool manTowers() const { return mManTowers; }
    void setManTowers(const bool m);

    void distributeEmployees(const eSector s);
    void distributeEmployees();

    bool isShutDown(const eResourceType type) const;
    bool isShutDown(const eBuildingType type) const;

    void incDistributeEmployees(const int by);
    void incPopulation(const int by);

    ePopulationData& populationData() { return mPopData; }
    eHusbandryData& husbandryData() { return mHusbData; }
    eEmploymentData& employmentData() { return mEmplData; }
    eEmploymentDistributor& employmentDistributor()
    { return mEmplDistributor; }

    void addShutDown(const eResourceType type);
    void removeShutDown(const eResourceType type);
    int industryJobVacancies(const eResourceType type) const;

    using eBuildingValidator = std::function<bool(eBuilding*)>;
    std::vector<eBuilding*> buildings(const eBuildingValidator& v) const;
    std::vector<eBuilding*> buildings(const eBuildingType type) const;
    int countBuildings(const eBuildingValidator& v) const;
    int countBuildings(const eBuildingType t) const;
    int hasBuilding(const eBuildingType t) const;
    int countAllowed(const eBuildingType t) const;
    eBuilding* randomBuilding(const eBuildingValidator& v) const;
    eTile* randomTile() const;
    const std::vector<eBuilding*>& commemorativeBuildings() const
    { return mCommemorativeBuildings; }

    int philosophyResearchCoverage() const { return mPhilosophyCoverage; }
    int athleticsLearningCoverage() const { return mAthleticsCoverage; }
    int dramaAstronomyCoverage() const { return mDramaCoverage; }
    int allCultureScienceCoverage() const { return mAllDiscCoverage; }
    int taxesCoverage() const { return mTaxesCoverage; }
    int unrest() const { return mUnrest; }
    int popularity() const { return mPopularity; }
    int health() const { return mHealth; }

    int wonGames() const { return mWonGames; }
    void incWonGames() { mWonGames++; }

    double coverageMultiplier() const;
    double winningChance(const eGames game) const;

    void updateResources();
    using eResources = std::vector<std::pair<eResourceType, int>>;
    const eResources& resources() const { return mResources; }
    int resourceCount(const eResourceType type) const;
    int takeResource(const eResourceType type, const int count);
    int addResource(const eResourceType type, const int count);
    int spaceForResource(const eResourceType type) const;
    int maxSanctuarySpaceForResource(eSanctuary** b) const;
    int maxSingleSpaceForResource(const eResourceType type,
                                  eStorageBuilding** b) const;

    void killCommonFolks(int toKill);
    void walkerKilled();
    void rockThrowerKilled();
    void hopliteKilled();
    void horsemanKilled();

    int missingArmorFromEliteHouses() const;

    static const int sPalaceTiles = 20;
    int maxPalaceBannerCount() const;
    void soldierBannersUpdate();
    void updateMaxSoldiers();
    void distributeSoldiers();
    void consolidateSoldiers();
    void addSoldier(const eCharacterType st);
    void removeSoldier(const eCharacterType st,
                       const bool skipNotHome = true);
    void registerSoldierBanner(const stdsptr<eSoldierBanner>& b);
    bool unregisterSoldierBanner(const stdsptr<eSoldierBanner>& b);

    bool personPlayerOwner() const;
    ePlayerId owningPlayerId() const;
    eBoardPlayer* owningPlayer() const;

    bool supportsResource(const eResourceType rt) const;
    eResourceType supportedResources() const { return mSupportedResources; }

    std::vector<eAgoraBase*> agoras() const;

    void registerTradePost(eTradePost* const b);
    bool unregisterTradePost(eTradePost* const b);
    bool hasTradePost(const eWorldCity& city);

    bool landTradeShutdown() const { return mShutdownLandTrade; }
    void setLandTradeShutdown(const bool s) { mShutdownLandTrade = s; }

    bool seaTradeShutdown() const { return mShutdownSeaTrade; }
    void setSeaTradeShutdown(const bool s) { mShutdownSeaTrade = s; }

    void registerSpawner(eSpawner* const s);
    bool unregisterSpawner(eSpawner* const s);

    void registerStadium(eStadium* const s);
    void unregisterStadium();

    void registerMuseum(eMuseum* const s);
    void unregisterMuseum();

    void registerStorBuilding(eStorageBuilding* const b);
    bool unregisterStorBuilding(eStorageBuilding* const b);

    void registerSanctuary(eSanctuary* const b);
    bool unregisterSanctuary(eSanctuary* const b);

    void registerHeroHall(eHerosHall* const b);
    bool unregisterHeroHall(eHerosHall* const b);

    bool unregisterCommonHouse(eSmallHouse* const ch);

    bool hasStadium() const { return mStadium; }
    bool hasMuseum() const { return mMuseum; }
    eStadium* stadium() const { return mStadium; }
    eMuseum* museum() const { return mMuseum; }

    void registerPalace(ePalace* const p);
    void unregisterPalace();

    ePalace* palace() const { return mPalace; }
    bool hasPalace() const { return mPalace; }

    int population() const { return mPopData.population(); }

    int eliteHouses() const;
    eSanctuary* sanctuary(const eGodType god) const;
    eHerosHall* heroHall(const eHeroType hero) const;
    int countBanners(const eBannerType bt) const;
    int countSoldiers(const eBannerType bt) const;
    const std::vector<eSanctuary*>& sanctuaries() const
    { return mSanctuaries; }
    const std::vector<eHerosHall*>& heroHalls() const
    { return mHeroHalls; }

    void startPlague(eSmallHouse* const h);
    stdsptr<ePlague> plagueForHouse(eSmallHouse* const h);
    void healPlague(const stdsptr<ePlague>& p);
    using ePlagues = std::vector<stdsptr<ePlague>>;
    const ePlagues& plagues() const;
    stdsptr<ePlague> nearestPlague(const int tx, const int ty,
                                   int& dist) const;

    eInvasionEvent* invasionToDefend() const;

    void musterAllSoldiers();
    void sendAllSoldiersHome();

    eEnlistedForces getEnlistableForces() const;

    eImmigrationLimitedBy immigrationLimit() const
    { return mImmigrationLimit; }

    const std::vector<eInvasionHandler*>& invasionHandlers() const {
        return mInvasionHandlers;
    }

    void addInvasionHandler(eInvasionHandler* const i);
    void removeInvasionHandler(eInvasionHandler* const i);
    bool hasActiveInvasions() const;

    const std::vector<stdsptr<eSoldierBanner>>& banners() const
    { return mSoldierBanners; }
    eBanner* banner(const eBannerTypeS type, const int id = 0) const;
    eTile* entryPoint() const;
    eTile* exitPoint() const;
    void registerBanner(eBanner* const b);
    void unregisterBanner(eBanner* const b);

    eTile* monsterTile(const int id) const;
    eTile* landInvasionTile(const int id) const;
    eTile* disasterTile(const int id) const;

    const std::vector<eMonster*>& monsters() const;
    void registerMonster(eMonster* const m);
    void unregisterMonster(eMonster* const m);
    using eChars = std::vector<eCharacter*>;
    const eChars& attackingGods() const;
    void registerAttackingGod(eCharacter* const c);
    bool unregisterAttackingGod(eCharacter* const c);

    bool nearestEnemySoldier(const eTeamId tid,
                             const int tx, const int ty,
                             int& nX, int& nY) const;

    eMilitaryAid* militaryAid(const stdsptr<eWorldCity>& c) const;
    void removeMilitaryAid(const stdsptr<eWorldCity>& c);
    void addMilitaryAid(const stdsptr<eMilitaryAid>& a);

    bool wasHeroSummoned(const eHeroType hero) const;
    void heroSummoned(const eHeroType hero);

    void clearAfterLastEpisode();
    void startEpisode(eEpisode* const e);

    void addRootGameEvent(const stdsptr<eGameEvent>& e);
    void removeRootGameEvent(const stdsptr<eGameEvent>& e);

    void addExported(const eCityId cid,
                     const eResourceType type,
                     const int count);
    void removeExported(const eCityId cid,
                        const eResourceType type,
                        const int count);
    int exported(const eCityId cid, const eResourceType type);
    std::map<eResourceType, int> exported(const eCityId cid) const;

    int allBuildingsState() const { return mAllBuildingsState; }

    int terrainState() const { return mTerrainState; }
    void incTerrainState();

    void incForestsState();

    int sanctuariesState() const { return mSanctuariesState; }

    const std::vector<eTile*>& animalBuildingsTiles();

    const std::vector<eBuilding*>& buildingsWithResource() const
    { return mBuildingsWithResource; }
    const std::vector<eHouseBase*>& houses() const
    { return mHouses; }
    const std::vector<eBuilding*>& treesAndVines() const
    { return mTreesAndVines; }
    const std::vector<eTile*>& resourceTiles();
    const std::vector<eTile*>& forestTiles();
    const std::vector<eBuilding*>& sanctBuildings() const
    { return mSanctBuildings; }

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
private:
    void payPensions();
    bool replace3By3AestheticByCommemorative();
    void updateResourceTiles();
    void updateForestTiles();

    eGameBoard& mBoard;

    eCityId mId;
    int mBasePrice = 5000;
    std::vector<eTile*> mTiles;
    SDL_Rect mTileBRect{0, 0, 0, 0};
    bool mAtlantean = false;

    eGameEvents mCityEvents;

    std::map<int, std::vector<eDistrictReadyCondition>> mEditorDistrictConditions;
    int mCurrentDistrictId = -1; // -1 when not editing
    eAICityPlan mCityPlan;
    struct eDistrictCost {
        int fDrachmas = -1;
        int fMarble = -1;
    };

    eDistrictCost mNextDistrictCost;

    eWageRate mWageRate{eWageRate::normal};
    eTaxRate mTaxRate{eTaxRate::normal};

    using eILB = eImmigrationLimitedBy;
    eILB mImmigrationLimit{eILB::lackOfVacancies};
    bool mNoFood = false;
    eDate mNoFoodSince;

    int mTaxesPaidLastYear = 0;
    int mTaxesPaidThisYear = 0;

    int mPeoplePaidTaxesLastYear = 0;
    int mPeoplePaidTaxesThisYear = 0;

    eResources mResources = {{eResourceType::urchin, 0},
                             {eResourceType::fish, 0},
                             {eResourceType::meat, 0},
                             {eResourceType::cheese, 0},
                             {eResourceType::carrots, 0},
                             {eResourceType::onions, 0},
                             {eResourceType::wheat, 0},
                             {eResourceType::oranges, 0},

                             {eResourceType::grapes, 0},
                             {eResourceType::olives, 0},
                             {eResourceType::wine, 0},
                             {eResourceType::oliveOil, 0},
                             {eResourceType::fleece, 0},

                             {eResourceType::wood, 0},
                             {eResourceType::bronze, 0},
                             {eResourceType::marble, 0},

                             {eResourceType::armor, 0},
                             {eResourceType::sculpture, 0}};

    std::map<eCityId, std::map<eResourceType, int>> mExported;

    std::vector<eSanctuary*> mSanctuaries;
    std::vector<eHerosHall*> mHeroHalls;
    std::vector<eStorageBuilding*> mStorBuildings;
    std::vector<eCharacter*> mCharacters;
    std::vector<eCharacterAction*> mCharacterActions;
    std::vector<eBuilding*> mTimedBuildings;
    std::vector<eEmployingBuilding*> mEmployingBuildings;
    std::vector<eBuilding*> mAllBuildings;
    std::vector<eBuilding*> mBuildingsWithResource;
    std::vector<eHouseBase*> mHouses;
    std::vector<eBuilding*> m3x3AestheticBuildings;
    std::vector<eBuilding*> mCommemorativeBuildings;
    std::vector<eBuilding*> mTreesAndVines;
    std::vector<eBuilding*> mAnimalBuildings;
    bool mAnimalBuildingsSurroundingUpdate = false;
    std::vector<eTile*> mAnimalBuildingsSurrounding;
    std::vector<eTile*> mResourceTiles;
    bool mResourceTilesUpdate = true;
    std::vector<eTile*> mForestTiles;
    bool mForestTilesUpdate = true;
    std::vector<eBuilding*> mSanctBuildings;
    std::vector<eTradePost*> mTradePosts;
    std::vector<eSpawner*> mSpawners;
    std::vector<eMonster*> mMonsters;
    std::vector<eBanner*> mBanners;

    std::vector<eSoldierBanner*> mAllSoldierBanners;
    std::vector<stdsptr<eSoldierBanner>> mSoldierBanners;
    std::vector<stdsptr<eSoldierBanner>> mPalaceSoldierBanners;

    std::vector<stdsptr<eMilitaryAid>> mMilitaryAid;

    std::vector<eInvasionEvent*> mInvasions;
    std::vector<eInvasionHandler*> mInvasionHandlers;

    std::vector<eHeroType> mSummonedHeroes;

    std::vector<eCharacter*> mAttackingGods;

    std::vector<stdsptr<ePlague>> mPlagues;

    bool mManTowers = true;

    bool mShutdownLandTrade = false;
    bool mShutdownSeaTrade = false;
    int mMaxRabble = 0;
    int mMaxHoplites = 0;
    int mMaxHorsemen = 0;

    int mAthleticsCoverage = 0;
    int mPhilosophyCoverage = 0;
    int mDramaCoverage = 0;
    int mAllDiscCoverage = 0;
    int mTaxesCoverage = 0;
    int mUnrest = 0; // percent
    int mPopularity = 0;
    int mHealth = 0;
    int mExcessiveMilitaryServiceCount = 0;
    int mMonthsOfMilitaryService = 0;

    int mWonGames = 0;

    eStadium* mStadium = nullptr;
    eMuseum* mMuseum = nullptr;
    ePalace* mPalace = nullptr;

    ePopulationData mPopData;
    eHusbandryData mHusbData;
    eEmploymentData mEmplData;

    int mEmploymentUpdateWait = __INT_MAX__/10;
    bool mEmploymentUpdateScheduled = true;

    eEmploymentDistributor mEmplDistributor;
    std::vector<eResourceType> mShutDown;
    std::map<eSector, std::vector<eEmployingBuilding*>> mSectorBuildings;

    eAvailableBuildings mAvailableBuildings;
    eResourceType mSupportedResources = eResourceType::allBasic;

    bool mPop100 = false;
    bool mPop500 = false;
    bool mPop1000 = false;
    bool mPop2000 = false;
    bool mPop3000 = false;
    bool mPop5000 = false;
    bool mPop10000 = false;
    bool mPop15000 = false;
    bool mPop20000 = false;
    bool mPop25000 = false;

    int mUpdateResources = 999999;
    int mCoverageUpdate = 10000;

    int mAllBuildingsState = 0;
    int mTerrainState = 0;
    int mForestsState = 0;
    int mSanctuariesState = 0;
};

#endif // EBOARDCITY_H
