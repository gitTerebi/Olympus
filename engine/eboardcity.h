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

class ePalace;
class eSanctuary;
class eHerosHall;
class eStorageBuilding;
class eEmployingBuilding;
class eTradePost;
class eSpawner;
class eMonster;
struct eMilitaryAid;
class eBoardPlayer;
enum class eCharacterType;
enum class eBuildingMode;
enum struct eHeroType;

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
public:
    eBoardCity(eGameBoard& board);

    eCityId id() const { return mId; }
    void setId(const eCityId id) { mId = id; }

    void incTime(const int by);

    void updateCoverage();

    void payTaxes(const int d, const int people);

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

    void registerBuilding(eBuilding* const b);

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

    void updateMaxSoldiers();
    void distributeSoldiers();
    void consolidateSoldiers();
    void addSoldier(const eCharacterType st);
    void removeSoldier(const eCharacterType st,
                       const bool skipNotHome = true);
    void registerSoldierBanner(const stdsptr<eSoldierBanner>& b);
    bool unregisterSoldierBanner(const stdsptr<eSoldierBanner>& b);

    eBoardPlayer* owningPlayer() const;

    int eliteHouses() const;
    eSanctuary* sanctuary(const eGodType god) const;
    eHerosHall* heroHall(const eHeroType hero) const;
    int countBanners(const eBannerType bt) const;
    int countSoldiers(const eBannerType bt) const;
    const std::vector<eSanctuary*>& sanctuaries() const
    { return mSanctuaries; }
    const std::vector<eHerosHall*>& heroHalls() const
    { return mHeroHalls; }
private:
    void payPensions();

    eGameBoard& mBoard;

    eCityId mId;
    std::vector<eTile*> mTiles;
    bool mAtlantean = false;

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

    std::vector<eSanctuary*> mSanctuaries;
    std::vector<eHerosHall*> mHeroHalls;
    std::vector<eStorageBuilding*> mStorBuildings;
    std::vector<eCharacter*> mCharacters;
    std::vector<eCharacterAction*> mCharacterActions;
    std::vector<eBuilding*> mTimedBuildings;
    std::vector<eEmployingBuilding*> mEmployingBuildings;
    std::vector<eBuilding*> mAllBuildings;
    std::vector<eBuilding*> mCommemorativeBuildings;
    std::vector<eTradePost*> mTradePosts;
    std::vector<eSpawner*> mSpawners;
    std::vector<eMonster*> mMonsters;
    std::vector<eBanner*> mBanners;

    std::vector<eSoldierBanner*> mAllSoldierBanners;
    std::vector<stdsptr<eSoldierBanner>> mSoldierBanners;
    std::vector<stdsptr<eSoldierBanner>> mPalaceSoldierBanners;

    std::vector<stdsptr<eMilitaryAid>> mMilitaryAid;

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

    eBuilding* mStadium = nullptr;
    eBuilding* mMuseum = nullptr;
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
    eResourceType mSupportedResources;

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
};

#endif // EBOARDCITY_H
