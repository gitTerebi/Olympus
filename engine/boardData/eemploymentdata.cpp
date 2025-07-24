#include "eemploymentdata.h"

#include "epopulationdata.h"
#include "engine/edifficulty.h"
#include "engine/egameboard.h"

#include <math.h>

eEmploymentData::eEmploymentData(const ePopulationData& popData,
                                 const eBoardCity& city,
                                 const eGameBoard& board) :
    mPopData(popData), mCity(city), mBoard(board) {

}

void eEmploymentData::incTotalJobVacancies(const int v) {
    mTotalJobVacs += v;
}

int eEmploymentData::employable() const {
    const auto cid = mCity.id();
    const auto pid = mBoard.cityIdToPlayerId(cid);
    const auto diff = mBoard.difficulty(pid);
    const auto wageRate = mCity.wageRate();
    const double frac = eDifficultyHelpers::workerFrac(
                            diff, wageRate);
    return frac*mPopData.population();
}

int eEmploymentData::employed() const {
    return std::min(employable(), totalJobVacancies());
}

int eEmploymentData::pensions() const {
    const auto wageRate = mCity.wageRate();
    const auto wageMult = mBoard.wageMultiplier();
    const double frac = eWageRateHelpers::wageMultiplier(wageRate);
    return wageMult*frac*employed();
}

int eEmploymentData::unemployed() const {
    return employable() - employed();
}

int eEmploymentData::freeJobVacancies() const {
    return totalJobVacancies() - employed();
}

int eEmploymentData::totalJobVacancies() const {
    return mTotalJobVacs;
}

double eEmploymentData::vacanciesFilledFraction() const {
    if(mTotalJobVacs == 0) return 1;
    const double e = employed();
    return e/mTotalJobVacs;
}

double eEmploymentData::employedFraction() const {
    const int e = employable();
    if(e == 0) return 1.;
    const double ee = employed();
    return ee/e;
}

