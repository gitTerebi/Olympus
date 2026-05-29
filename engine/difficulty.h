#ifndef EDIFFICULTY_H
#define EDIFFICULTY_H

#include "buildings/ebuilding.h"

#include "etaxrate.h"
#include "ewagerate.h"

enum class eCharacterType;

enum class Difficulty {
    beginner, mortal, hero, titan, olympian
};

namespace DifficultyHelpers {
    std::string name(const Difficulty diff);

    int soliderBribe(const Difficulty diff,
                     const eCharacterType type);

    int buildingCost(const Difficulty diff,
                     const eBuildingType type);

    int buildingCost(const Difficulty diff,
                     const eBuildingType type);

    int fireRisk(const Difficulty diff,
                 const eBuildingType type);

    int houseDiseaseRisk(const Difficulty diff, const int level, const bool elite);
    int houseCrimeRisk(const Difficulty diff, const int level, const bool elite);

    int damageRisk(const Difficulty diff,
                   const eBuildingType type);

    int taxMultiplier(const Difficulty diff,
                      const eBuildingType type,
                      const int level);
    int taxSentiment(const Difficulty diff,
                     const eTaxRate taxRate);

    double workerFrac(const Difficulty diff,
                      const eWageRate wageRate);

    double costMultiplier(const Difficulty diff);

    struct eHouseLevelReq {
        int fAppD;
        int fAppE;
        int fEnt;
    };
    eHouseLevelReq houseLevelReq(const Difficulty diff,
                                 const bool elite,
                                 const int level);
}

#endif // EDIFFICULTY_H
