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

class eBoardCity {
public:
    eBoardCity(eGameBoard& board);

    eCityId id() const { return mId; }
    void setId(const eCityId id) { mId = id; }

    void payTaxes(const int d, const int people);

    void nextMonth();

    double taxRateF() const;
    eTaxRate taxRate() const { return mTaxRate; }
    eWageRate wageRate() const { return mWageRate; }

    double wageMultiplier() const { return mWageMultiplier; }

    void setTaxRate(const eTaxRate tr);
    void setWageRate(const eWageRate wr);

    int taxesPaidThisYear() const { return mTaxesPaidThisYear; }
    int taxesPaidLastYear() const { return mTaxesPaidLastYear; }
    int peoplePaidTaxesThisYear() const { return mPeoplePaidTaxesThisYear; }
    int peoplePaidTaxesLastYear() const { return mPeoplePaidTaxesLastYear; }

    void updateResources();
    using eResources = std::vector<std::pair<eResourceType, int>>;
    const eResources& resources() const { return mResources; }

    void registerBuilding(eBuilding* const b);

    void registerEmplBuilding(eEmployingBuilding* const b);
    bool unregisterEmplBuilding(eEmployingBuilding* const b);

    bool manTowers() const { return mManTowers; }
    void setManTowers(const bool m);

    void distributeEmployees(const eSector s);
    void distributeEmployees();

    bool isShutDown(const eResourceType type) const;
    bool isShutDown(const eBuildingType type) const;

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

    int takeResource(const eResourceType type, const int count);

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
private:
    void payPensions();

    eGameBoard& mBoard;

    eCityId mId;
    std::vector<eTile*> mTiles;

    double mWageMultiplier = 1.;
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

    eBuilding* mStadium = nullptr;
    eBuilding* mMuseum = nullptr;
    ePalace* mPalace = nullptr;

    ePopulationData mPopData;
    eHusbandryData mHusbData;
    eEmploymentData mEmplData;

    eEmploymentDistributor mEmplDistributor;
    std::vector<eResourceType> mShutDown;
    std::map<eSector, std::vector<eEmployingBuilding*>> mSectorBuildings;
};

#endif // EBOARDCITY_H
