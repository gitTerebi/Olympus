#include "eboardcity.h"

#include "buildings/ebuilding.h"
#include "buildings/eemployingbuilding.h"
#include "buildings/estoragebuilding.h"
#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"
#include "buildings/eheroshall.h"
#include "evectorhelpers.h"

#include "elanguage.h"
#include "emessages.h"
#include "eeventdata.h"

#include "egameboard.h"

eBoardCity::eBoardCity(eGameBoard& board) :
    mBoard(board),
    mHusbData(mPopData, board),
    mEmplData(mPopData, *this, board),
    mEmplDistributor(mEmplData) {}

void eBoardCity::incTime(const int by) {
    mCoverageUpdate += by;
    const int cup = 2000;
    if(mCoverageUpdate > cup) {
        mCoverageUpdate -= cup;
        updateCoverage();
    }

    const auto& msgs = &eMessages::instance;
    eEventData ed;
    const int pop = mPopData.population();
    if(pop >= 100 && !mPop100) {
        mBoard.showMessage(ed, msgs->fPop100);
        mPop100 = true;
    } else if(pop >= 500 && !mPop500) {
        mBoard.showMessage(ed, msgs->fPop500);
        mPop500 = true;
    } else if(pop >= 1000 && !mPop1000) {
        mBoard.showMessage(ed, msgs->fPop1000);
        allow(eBuildingType::commemorative, 0);
        mPop1000 = true;
    } else if(pop >= 2000 && !mPop2000) {
        mBoard.showMessage(ed, msgs->fPop2000);
        allow(eBuildingType::commemorative, 0);
        mPop2000 = true;
    } else if(pop >= 3000 && !mPop3000) {
        mBoard.showMessage(ed, msgs->fPop3000);
        mPop3000 = true;
    } else if(pop >= 5000 && !mPop5000) {
        mBoard.showMessage(ed, msgs->fPop5000);
        allow(eBuildingType::commemorative, 0);
        mPop5000 = true;
    } else if(pop >= 10000 && !mPop10000) {
        mBoard.showMessage(ed, msgs->fPop10000);
        allow(eBuildingType::commemorative, 0);
        mPop10000 = true;
    } else if(pop >= 15000 && !mPop15000) {
        mBoard.showMessage(ed, msgs->fPop15000);
        allow(eBuildingType::commemorative, 0);
        mPop15000 = true;
    } else if(pop >= 20000 && !mPop20000) {
        mBoard.showMessage(ed, msgs->fPop20000);
        allow(eBuildingType::commemorative, 0);
        mPop20000 = true;
    } else if(pop >= 25000 && !mPop25000) {
        mBoard.showMessage(ed, msgs->fPop25000);
        allow(eBuildingType::commemorative, 0);
        mPop25000 = true;
    }

    const int updateResWait = 1000;
    mUpdateResources += by;
    if(mUpdateResources > updateResWait) {
        mUpdateResources -= updateResWait;
        updateResources();
    }

    const int food = resourceCount(eResourceType::food);
    bool prolongedNoFood = false;
    const auto date = mBoard.date();
    if(mNoFood && food < 1 && pop > 10) {
        prolongedNoFood = date.year() - mNoFoodSince.year() > 1;
    } else if(food < 1 && pop > 10) {
        mNoFood = true;
        mNoFoodSince = date;
    } else {
        mNoFood = false;
    }
    const int u = mEmplData.unemployed();
    const int e = mEmplData.employable();
    const int v = mPopData.vacancies();
    const auto oldLimit = mImmigrationLimit;
    const auto p = owningPlayer();
    int drachmas = 0;
    eDate inDebtSince = date;
    if(p) {
        drachmas = p->drachmas();
        inDebtSince = p->inDebtSince();
    }
    if(prolongedNoFood) {
        mImmigrationLimit = eILB::lackOfFood;
    } else if(drachmas < 0 && date.year() - inDebtSince.year() > 1) {
        mImmigrationLimit = eILB::prolongedDebt;
    } else if(static_cast<int>(mWageRate) < static_cast<int>(eWageRate::low)) {
        mImmigrationLimit = eILB::lowWages;
    } else if(e < 10 ? false : (1.*u/e > 0.25 && u > 50)) {
        mImmigrationLimit = eILB::unemployment;
    } else if(static_cast<int>(mTaxRate) > static_cast<int>(eTaxRate::high)) {
        mImmigrationLimit = eILB::highTaxes;
    } else if(mExcessiveMilitaryServiceCount > 0) {
        mImmigrationLimit = eILB::excessiveMilitaryService;
    } else if(v <= 0) {
        mImmigrationLimit = eILB::lackOfVacancies;
    } else {
        mImmigrationLimit = eILB::none;
    }
    if(oldLimit != mImmigrationLimit) {
        switch(mImmigrationLimit) {
        case eILB::lackOfFood:
            mBoard.showTip(eLanguage::zeusText(19, 112));
            break;
        case eILB::prolongedDebt:
            mBoard.showTip(eLanguage::zeusText(19, 116));
            break;
        case eILB::lowWages:
            mBoard.showTip(eLanguage::zeusText(19, 115));
            break;
        case eILB::unemployment:
            mBoard.showTip(eLanguage::zeusText(19, 113));
            break;
        case eILB::highTaxes:
            mBoard.showTip(eLanguage::zeusText(19, 114));
            break;
        case eILB::excessiveMilitaryService:
            mBoard.showTip(eLanguage::zeusText(19, 117));
            break;
        case eILB::lackOfVacancies:
            mBoard.showTip(eLanguage::zeusText(19, 111));
            break;
        case eILB::none:
            mBoard.showTip(eLanguage::zeusText(19, 124));
        }
    }

}

void eBoardCity::updateCoverage() {
    int totalPeople = 0;
    int commonPeople = 0;
    int sport = 0;
    int phil = 0;
    int drama = 0;
    int taxes = 0;
    double totalUnrest = 0;
    int totalSatisfaction = 0;
    int totalHygiene = 0;
    for(const auto b : mTimedBuildings) {
        if(const auto h = dynamic_cast<eHouseBase*>(b)) {
            const int p = h->people();
            if(h->athletesScholars() > 0) {
                sport += p;
            }
            if(h->philosophersInventors() > 0) {
                phil += p;
            }
            if(h->actorsAstronomers() > 0) {
                drama += p;
            }
            if(h->paidTaxes()) {
                taxes += p;
            }
            if(const auto ch = dynamic_cast<eSmallHouse*>(b)) {
                if(ch->disgruntled()) totalUnrest += p;
                totalSatisfaction += p*ch->satisfaction();
                totalHygiene += p*ch->hygiene();
                commonPeople += p;
            }
            totalPeople += p;
        }
    }
    if(totalPeople <= 0) {
        mAthleticsCoverage = 0;
        mPhilosophyCoverage = 0;
        mDramaCoverage = 0;
        mTaxesCoverage = 0;
    } else {
        mAthleticsCoverage = std::round(100.*sport/totalPeople);
        mPhilosophyCoverage = std::round(100.*phil/totalPeople);
        mDramaCoverage = std::round(100.*drama/totalPeople);
        mTaxesCoverage = std::round(100.*taxes/totalPeople);
    }
    mAllDiscCoverage = (mAthleticsCoverage + mPhilosophyCoverage + mDramaCoverage)/3;

    if(commonPeople == 0) {
        mUnrest = 0;
        mPopularity = 100;
        mHealth = 100;
    } else {
        mUnrest = std::round(100.*totalUnrest/commonPeople);
        mPopularity = std::round(1.*totalSatisfaction/commonPeople);
        mHealth = std::round(1.*totalHygiene/commonPeople);
    }
}

void eBoardCity::payTaxes(const int d, const int people) {
    mTaxesPaidThisYear += d;
    mPeoplePaidTaxesThisYear += people;
}

void eBoardCity::payPensions() {
    const auto p = owningPlayer();
    const int d = std::ceil(mEmplData.pensions()/12.);
    if(p) p->incDrachmas(-d);
}

void eBoardCity::nextMonth() {
    payPensions();

    {
        int nTot = 0;
        int nCalled = 0;
        for(const auto& s : mPalaceSoldierBanners) {
            const bool a = s->isAbroad();
            if(a) continue;
            const int c = s->count();
            nTot += c;
            const bool h = s->isHome();
            if(h) continue;
            nCalled += c;
        }
        if(nTot == 0 || static_cast<double>(nCalled)/nTot < 0.25) {
            mExcessiveMilitaryServiceCount--;
            mMonthsOfMilitaryService = 0;
        } else {
            mMonthsOfMilitaryService++;
            if(mMonthsOfMilitaryService > 6) {
                mExcessiveMilitaryServiceCount = 2;
            }
        }
    }

    mPopData.nextMonth();
}

double eBoardCity::taxRateF() const {
    return eTaxRateHelpers::getRate(mTaxRate);
}

void eBoardCity::setTaxRate(const eTaxRate tr) {
    mTaxRate = tr;
}

void eBoardCity::setWageRate(const eWageRate wr) {
    mWageRate = wr;
    distributeEmployees();
}

void eBoardCity::registerBuilding(eBuilding* const b) {
    mAllBuildings.push_back(b);
    const auto bt = b->type();
    if(eBuilding::sTimedBuilding(bt)) {
        mTimedBuildings.push_back(b);
    }
    if(bt == eBuildingType::commemorative ||
       bt == eBuildingType::godMonument) {
        mCommemorativeBuildings.push_back(b);
    }
}

void eBoardCity::registerEmplBuilding(eEmployingBuilding* const b) {
    mEmployingBuildings.push_back(b);
    const auto type = b->type();
    const bool sd = isShutDown(type);
    if(sd) b->setShutDown(true);
    eSector s;
    const bool r = eSectorHelpers::sBuildingSector(type, s);
    if(r) {
        mSectorBuildings[s].push_back(b);
    }
    if(!sd) {
        const int me = b->maxEmployees();
        mEmplData.incTotalJobVacancies(me);
        if(r) mEmplDistributor.incMaxEmployees(s, me);
    }
    distributeEmployees();
}

bool eBoardCity::unregisterEmplBuilding(eEmployingBuilding* const b) {
    const bool rr = eVectorHelpers::remove(mEmployingBuildings, b);
    const auto type = b->type();
    eSector s;
    const bool r = eSectorHelpers::sBuildingSector(type, s);
    if(r) {
        eVectorHelpers::remove(mSectorBuildings[s], b);
    }
    if(rr) {
        const bool sd = isShutDown(type);
        if(!sd) {
            const int me = b->maxEmployees();
            mEmplData.incTotalJobVacancies(-me);
            if(r) mEmplDistributor.incMaxEmployees(s, -me);
        }
    }
    distributeEmployees();
    return rr;
}

bool eBoardCity::supportsBuilding(const eBuildingMode mode) const {
    const auto type = eBuildingModeHelpers::toBuildingType(mode);
    int id = -1;
    switch(mode) {
    case eBuildingMode::populationMonument:
    case eBuildingMode::victoryMonument:
    case eBuildingMode::colonyMonument:
    case eBuildingMode::athleteMonument:
    case eBuildingMode::conquestMonument:
    case eBuildingMode::happinessMonument:
    case eBuildingMode::heroicFigureMonument:
    case eBuildingMode::diplomacyMonument:
    case eBuildingMode::scholarMonument:
        id = eBuildingModeHelpers::toCommemorativeId(mode);
        break;
    case eBuildingMode::aphroditeMonument:
    case eBuildingMode::apolloMonument:
    case eBuildingMode::aresMonument:
    case eBuildingMode::artemisMonument:
    case eBuildingMode::athenaMonument:
    case eBuildingMode::atlasMonument:
    case eBuildingMode::demeterMonument:
    case eBuildingMode::dionysusMonument:
    case eBuildingMode::hadesMonument:
    case eBuildingMode::hephaestusMonument:
    case eBuildingMode::heraMonument:
    case eBuildingMode::hermesMonument:
    case eBuildingMode::poseidonMonument:
    case eBuildingMode::zeusMonument: {
        const auto gt = eBuildingModeHelpers::toGodType(mode);
        id = static_cast<int>(gt);
    } break;
    default:
        break;
    }

    return availableBuilding(type, id);
}

bool eBoardCity::availableBuilding(const eBuildingType type,
                                   const int id) const {
    switch(type) {
    case eBuildingType::chariotVendor:
    case eBuildingType::chariotFactory: {
        if(!mAtlantean) return false;
    } break;
    case eBuildingType::horseTrainer: {
        if(mAtlantean) return false;
    } break;
    default:
        break;
    }

    return mAvailableBuildings.available(type, id);
}

void eBoardCity::built(const eBuildingType type,
                       const int id) {
    mAvailableBuildings.built(type, id);
    mBoard.updateButtonsVisibility();
}

void eBoardCity::destroyed(const eBuildingType type,
                           const int id) {
    if(!mBoard.registerBuildingsEnabled()) return;
    mAvailableBuildings.destroyed(type, id);
    mBoard.updateButtonsVisibility();
}

void eBoardCity::allow(const eBuildingType type,
                       const int id) {
    mAvailableBuildings.allow(type, id);
    mBoard.updateButtonsVisibility();
}

void eBoardCity::disallow(const eBuildingType type,
                          const int id) {
    mAvailableBuildings.disallow(type, id);
    mBoard.updateButtonsVisibility();
}

void eBoardCity::setManTowers(const bool m) {
    mManTowers = m;
}

void eBoardCity::distributeEmployees(const eSector s) {
    int e = mEmplDistributor.employees(s);
    const int maxE = mEmplDistributor.maxEmployees(s);
    const double frac = e/static_cast<double>(maxE);
    const auto& sb = mSectorBuildings[s];
    struct eSectorReminder {
        double fRem;
        eEmployingBuilding* fB;
    };

    std::vector<eSectorReminder> reminders;
    for(const auto b : sb) {
        const auto type = b->type();
        const bool sd = isShutDown(type);
        if(sd) {
            b->setShutDown(true);
            continue;
        } else {
            b->setShutDown(false);
        }
        const int me = b->maxEmployees();
        const double eeF = frac*me;
        const int ee = std::floor(eeF);
        b->setEmployed(ee);
        e -= ee;
        reminders.push_back({(eeF - ee)/me, b});
    }

    const auto comp = [](const eSectorReminder& r1, const eSectorReminder& r2) {
        return r1.fRem > r2.fRem;
    };
    std::sort(reminders.begin(), reminders.end(), comp);

    while(e > 0) {
        bool changed = false;
        for(auto& r : reminders) {
            if(e <= 0) break;
            const auto b = r.fB;
            const int me = b->maxEmployees();
            const int ee = b->employed();
            if(ee >= me) continue;
            b->setEmployed(ee + 1);
            e--;
            changed = true;
        }
        if(!changed) break;
    }
}

void eBoardCity::distributeEmployees() {
    for(const auto& s : mSectorBuildings) {
        distributeEmployees(s.first);
    }
}

bool eBoardCity::isShutDown(const eResourceType type) const {
    return eVectorHelpers::contains(mShutDown, type);
}

bool eBoardCity::isShutDown(const eBuildingType type) const {
    if(type == eBuildingType::tower) return !mManTowers;
    const auto is = eIndustryHelpers::sIndustries(type);
    if(is.empty()) return false;
    for(const auto i : is) {
        const bool sd = isShutDown(i);
        if(!sd) return false;
    }
    return true;
}

void eBoardCity::incDistributeEmployees(const int by) {
    const int employmentUpdateWait = 5678;
    mEmploymentUpdateWait += by;
    if(mEmploymentUpdateWait > employmentUpdateWait) {
        if(mEmploymentUpdateScheduled) {
            mEmploymentUpdateWait = 0;
            distributeEmployees();
        }
    }
}

void eBoardCity::incPopulation(const int by) {
    mPopData.incPopulation(by);
    mEmploymentUpdateScheduled = true;
}

void eBoardCity::addShutDown(const eResourceType type) {
    mShutDown.push_back(type);
    for(const auto b : mEmployingBuildings) {
        const auto bt = b->type();
        const auto is = eIndustryHelpers::sIndustries(bt);
        if(is.empty()) continue;
        const bool r = eVectorHelpers::contains(is, type);
        if(!r) continue;
        const bool sd = is.size() == 1 ? true : isShutDown(bt);
        if(!sd) continue;
        b->setShutDown(true);
        const int maxE = b->maxEmployees();
        mEmplData.incTotalJobVacancies(-maxE);
        eSector s;
        const bool ss = eSectorHelpers::sBuildingSector(bt, s);
        if(!ss) continue;
        mEmplDistributor.incMaxEmployees(s, -maxE);
    }
    distributeEmployees();
}

void eBoardCity::removeShutDown(const eResourceType type) {
    const auto oldShutDown = mShutDown;
    const auto wasShutDown = [&](const eBuildingType bt) {
        const auto is = eIndustryHelpers::sIndustries(bt);
        if(is.empty()) return false;
        for(const auto i : is) {
            const bool sd = eVectorHelpers::contains(oldShutDown, i);
            if(!sd) return false;
        }
        return true;
    };
    const bool r = eVectorHelpers::remove(mShutDown, type);
    if(!r) return;
    for(const auto b : mEmployingBuildings) {
        const auto bt = b->type();
        const auto is = eIndustryHelpers::sIndustries(bt);
        if(is.empty()) continue;
        const bool r = eVectorHelpers::contains(is, type);
        if(!r) continue;
        const bool sd = is.size() == 1 ? false : isShutDown(bt);
        if(sd) continue;
        const bool wasSD = wasShutDown(bt);
        if(!wasSD) continue;
        b->setShutDown(false);
        const int maxE = b->maxEmployees();
        mEmplData.incTotalJobVacancies(maxE);
        eSector s;
        const bool ss = eSectorHelpers::sBuildingSector(bt, s);
        if(!ss) continue;
        mEmplDistributor.incMaxEmployees(s, maxE);
    }
    distributeEmployees();
}

int eBoardCity::industryJobVacancies(const eResourceType type) const {
    const auto bs = eIndustryHelpers::sBuildings(type);
    int result = 0;
    for(const auto b : bs) {
        result += countBuildings(b);
    }
    return result;
}

std::vector<eBuilding*> eBoardCity::buildings(const eBuildingValidator& v) const {
    std::vector<eBuilding*> result;
    for(const auto b : mTimedBuildings) {
        const bool r = v(b);
        if(r) result.push_back(b);
    }
    return result;
}

std::vector<eBuilding*> eBoardCity::buildings(const eBuildingType type) const {
    return buildings([type](eBuilding* const b) {
        const auto bt = b->type();
        return type == bt;
    });
}

int eBoardCity::countBuildings(const eBuildingValidator& v) const {
    const auto vv = buildings(v);
    return vv.size();
}

int eBoardCity::countBuildings(const eBuildingType t) const {
    return countBuildings([t](eBuilding* const b) {
        const auto bt = b->type();
        return bt == t;
    });
}

int eBoardCity::hasBuilding(const eBuildingType t) const {
    for(const auto b : mTimedBuildings) {
        const bool r = t == b->type();
        if(r) return true;
    }
    return false;
}

int eBoardCity::countAllowed(const eBuildingType t) const {
    eBuildingType parent;

    if(t == eBuildingType::sheep) {
        parent = eBuildingType::cardingShed;
    } else if(t == eBuildingType::goat) {
        parent = eBuildingType::dairy;
    } else if(t == eBuildingType::cattle) {
        parent = eBuildingType::corral;
    } else {
        return 0;
    }
    const int already = countBuildings(t);

    return 8*countBuildings(parent) - already;
}

eBuilding* eBoardCity::randomBuilding(const eBuildingValidator& v) const {
    auto blds = mTimedBuildings;
    std::random_shuffle(blds.begin(), blds.end());
    for(const auto b : blds) {
        const bool r = v(b);
        if(r) return b;
    }
    return nullptr;
}

double coverageMultiplier(const int pop) {
    if(pop < 250) return 0.125;
    else if(pop < 500) return 0.25;
    else if(pop < 1000) return 0.375;
    else return 0.5;
}

double eBoardCity::coverageMultiplier() const {
    const int pop = mPopData.population();
    return ::coverageMultiplier(pop);
}

double eBoardCity::winningChance(const eGames game) const {
    int coverage = 0;
    switch(game) {
    case eGames::isthmian:
        coverage = mPhilosophyCoverage;
        break;
    case eGames::nemean:
        coverage = mAthleticsCoverage;
        break;
    case eGames::pythian:
        coverage = mDramaCoverage;
        break;
    case eGames::olympian:
        coverage = mAllDiscCoverage;
        break;
    }

    const double mult = coverageMultiplier();

    if(mult*coverage < 15) {
        return 0.;
    } else {
        const double coveragef = coverage/100.;
        const double chance = mult*coveragef*coveragef;
        return chance;
    }
}

void eBoardCity::updateResources() {
    for(auto& r : mResources) {
        int& count = r.second;
        count = 0;
        const auto type = r.first;
        for(const auto s : mStorBuildings) {
            count += s->count(type);
        }
    }
}

int eBoardCity::resourceCount(const eResourceType type) const {
    if(type == eResourceType::drachmas) {
        const auto p = owningPlayer();
        if(p) return p->drachmas();
        return 0;
    }
    int result = 0;
    for(auto& r : mResources) {
        if(!static_cast<bool>(r.first & type)) continue;
        result += r.second;
    }
    return result;
}

int eBoardCity::takeResource(const eResourceType type, const int count) {
    if(type == eResourceType::drachmas) {
        const auto p = owningPlayer();
        if(p) p->incDrachmas(-count);
        return count;
    }
    int r = 0;
    using eValidator = std::function<bool(eStorageBuilding*)>;
    const auto takeFunc = [&](const eValidator& v) {
        if(r >= count) return;
        for(const auto s : mStorBuildings) {
            if(!v(s)) continue;
            if(r >= count) return;
            r += s->take(type, count - r);
        }
    };
    takeFunc([&](eStorageBuilding* const s) {
        return s->empties(type);
    });
    takeFunc([&](eStorageBuilding* const s) {
        return !s->accepts(type);
    });
    takeFunc([&](eStorageBuilding* const s) {
        return !s->get(type);
    });
    takeFunc([&](eStorageBuilding* const s) {
        (void)s;
        return true;
    });
    return count - r;
}

int eBoardCity::addResource(const eResourceType type, const int count) {
    if(type == eResourceType::drachmas) {
        const auto p = owningPlayer();
        if(p) p->incDrachmas(count);
        return count;
    }
    int rem = count;
    using eValidator = std::function<bool(eStorageBuilding*)>;
    const auto addFunc = [&](const eValidator& v) {
        if(rem <= 0) return;
        for(const auto s : mStorBuildings) {
            if(!v(s)) continue;
            const int c = s->add(type, rem);
            rem -= c;
            if(rem <= 0) break;
        }
    };
    addFunc([&](eStorageBuilding* const s) {
        return s->get(type);
    });
    addFunc([&](eStorageBuilding* const s) {
        (void)s;
        return true;
    });
    return count - rem;
}

int eBoardCity::spaceForResource(const eResourceType type) const {
    if(type == eResourceType::drachmas) {
        return __INT_MAX__/2;
    }
    int r = 0;
    for(const auto s : mStorBuildings) {
        r += s->spaceLeft(type);
    }
    return r;
}

int eBoardCity::maxSingleSpaceForResource(
        const eResourceType type,
        eStorageBuilding** b) const {
    *b = nullptr;
    int r = 0;
    for(const auto s : mStorBuildings) {
        const int ss = s->spaceLeft(type);
        if(ss > r) {
            *b = s;
            r = ss;
        }
    }
    return r;
}

int eBoardCity::maxSanctuarySpaceForResource(
        eSanctuary** b) const {
    *b = nullptr;
    int r = 0;
    for(const auto s : mSanctuaries) {
        int ss = s->spaceLeft(eResourceType::wood);
        ss += s->spaceLeft(eResourceType::marble);
        ss += s->spaceLeft(eResourceType::sculpture);
        if(ss > r) {
            *b = s;
            r = ss;
        }
    }
    return r;
}

void eBoardCity::killCommonFolks(int toKill) {
    auto bs = mTimedBuildings;
    std::random_shuffle(bs.begin(), bs.end());
    for(const auto b : bs) {
        const auto bt = b->type();
        if(bt == eBuildingType::commonHouse) {
            const auto sh = static_cast<eSmallHouse*>(b);
            const int pop = sh->people();
            const int shk = std::min(toKill, pop);
            toKill -= shk;
            sh->kill(shk);
            if(toKill <= 0) break;
        }
    }
}

void eBoardCity::walkerKilled() {
    killCommonFolks(5);
}

void eBoardCity::rockThrowerKilled() {
    killCommonFolks(4);
}

void eBoardCity::hopliteKilled() {
    auto bs = mTimedBuildings;
    std::random_shuffle(bs.begin(), bs.end());
    for(const auto b : bs) {
        const auto bt = b->type();
        if(bt == eBuildingType::eliteHousing) {
            const auto eh = static_cast<eEliteHousing*>(b);
            const int pop = eh->people();
            const int shk = std::min(4, pop);
            eh->kill(shk);
            eh->removeArmor();
            return;
        }
    }
}

void eBoardCity::horsemanKilled() {
    auto bs = mTimedBuildings;
    std::random_shuffle(bs.begin(), bs.end());
    for(const auto b : bs) {
        const auto bt = b->type();
        if(bt == eBuildingType::eliteHousing) {
            const auto eh = static_cast<eEliteHousing*>(b);
            const int pop = eh->people();
            const int shk = std::min(4, pop);
            eh->kill(shk);
            eh->removeArmor();
            eh->removeHorse();
            return;
        }
    }
}

void eBoardCity::updateMaxSoldiers() {
    mMaxRabble = 0;
    mMaxHoplites = 0;
    mMaxHorsemen = 0;
    if(!mPalace) return;
    for(const auto b : mTimedBuildings) {
        const auto bt = b->type();
        if(bt == eBuildingType::commonHouse) {
            const auto ch = static_cast<eSmallHouse*>(b);
            const int l = ch->level();
            if(l < 2) continue;
            int lvlMax = 0;
            if(l == 2) lvlMax = 5;
            else if(l == 3) lvlMax = 6;
            else if(l == 4) lvlMax = 10;
            else if(l == 5) lvlMax = 12;
            else if(l == 6) lvlMax = 15;
            const int pop = ch->people();
            const int popMax = pop/4;
            mMaxRabble += std::min(lvlMax, popMax);
        } else if(bt == eBuildingType::eliteHousing) {
            const auto eh = static_cast<eEliteHousing*>(b);
            const int l = eh->level();
            if(l < 2) continue;
            const int a = eh->arms();
            if(l == 2) {
                mMaxHoplites += std::min(2, a);
            } else if(l == 3) {
                mMaxHoplites += std::min(4, a);
            } else if(l == 4) {
                const int h = eh->horses();
                const int hh = std::min(std::min(a, 4), h);
                mMaxHorsemen += hh;
                mMaxHoplites += std::min(4 - hh, a - hh);
            }
        }
    }
    mMaxRabble /= 6;

    const int nSpaces = 20;
    mMaxHorsemen = std::min(8*nSpaces, mMaxHorsemen);
    mMaxHorsemen = std::max(0, mMaxHorsemen);
    const int nHorsemenB = std::ceil(mMaxHorsemen/8.);
    mMaxHoplites = std::min(8*nSpaces - 8*nHorsemenB, mMaxHoplites);
    mMaxHoplites = std::max(0, mMaxHoplites);
    const int nHoplitesB = std::ceil(mMaxHoplites/8.);
    mMaxRabble = std::min(8*nSpaces - 8*nHorsemenB - 8*nHoplitesB, mMaxRabble);
    mMaxRabble = std::max(0, mMaxRabble);
}

void eBoardCity::distributeSoldiers() {
    int cRabble = 0;
    int cHoplites = 0;
    int cHorsemen = 0;
    for(const auto& b : mPalaceSoldierBanners) {
        const auto bt = b->type();
        const int c = b->count();
        if(bt == eBannerType::rockThrower) {
            cRabble += c;
        } else if(bt == eBannerType::hoplite) {
            cHoplites += c;
        } else if(bt == eBannerType::horseman) {
            cHorsemen += c;
        }
    }
    const int remRabble = mMaxRabble - cRabble;
    const int remHoplites = mMaxHoplites - cHoplites;
    const int remHorsemen = mMaxHorsemen - cHorsemen;

    for(int i = 0; i < -remRabble; i++) {
        removeSoldier(eCharacterType::rockThrower);
    }
    for(int i = 0; i < -remHoplites; i++) {
        removeSoldier(eCharacterType::hoplite);
    }
    for(int i = 0; i < -remHorsemen; i++) {
        removeSoldier(eCharacterType::horseman);
    }
    for(int i = 0; i < remHorsemen; i++) {
        addSoldier(eCharacterType::horseman);
    }
    for(int i = 0; i < remHoplites; i++) {
        addSoldier(eCharacterType::hoplite);
    }
    for(int i = 0; i < remRabble; i++) {
        addSoldier(eCharacterType::rockThrower);
    }
}

void eBoardCity::consolidateSoldiers() {
    using eSoldierBanners = std::vector<stdsptr<eSoldierBanner>>;
    eSoldierBanners rabble;
    eSoldierBanners hoplites;
    eSoldierBanners horsemen;
    for(const auto& s : mPalaceSoldierBanners) {
        if(s->isAbroad()) continue;
        const auto tile = s->tile();
        if(!tile) s->moveToDefault();
        if(!s->isHome()) continue;
        switch(s->type()) {
        case eBannerType::rockThrower:
            rabble.push_back(s);
            break;
        case eBannerType::hoplite:
            hoplites.push_back(s);
            break;
        case eBannerType::horseman:
            horsemen.push_back(s);
            break;
        default:
            break;
        }
    }
    const auto consolidator = [](const eSoldierBanners& banners) {
        for(int i = 0; i < static_cast<int>(banners.size()); i++) {
            const auto s = banners[i];
            const int sc = s->count();
            int sSpace = 8 - sc;
            if(sSpace <= 0) continue;
            for(int j = banners.size() - 1; j > i; j--) {
                const auto ss = banners[j];
                const int ssc = ss->count();
                const int kMax = std::min(sSpace, ssc);
                for(int k = 0; k < kMax; k++) {
                    ss->decCount();
                    s->incCount();
                    sSpace--;
                    if(sSpace <= 0) break;
                }
                if(sSpace <= 0) break;
            }
        }
    };
    consolidator(rabble);
    consolidator(hoplites);
    consolidator(horsemen);
}

void eBoardCity::addSoldier(const eCharacterType st) {
    bool found = false;
    for(const auto& b : mPalaceSoldierBanners) {
        if(b->isAbroad()) continue;
        const auto bt = b->type();
        const int c = b->count();
        if(c >= 8) continue;
        if(bt == eBannerType::rockThrower &&
           st == eCharacterType::rockThrower) {
            found = true;
        } else if(bt == eBannerType::hoplite &&
                  st == eCharacterType::hoplite) {
            found = true;
        } else if(bt == eBannerType::horseman &&
                  st == eCharacterType::horseman) {
            found = true;
        }
        if(found) {
            b->incCount();
            break;
        }
    }
    if(found) return;
    const int nSpaces = 20;
    if(mPalaceSoldierBanners.size() >= nSpaces) return;
    eBannerType bt;
    if(st == eCharacterType::rockThrower) {
        bt = eBannerType::rockThrower;
    } else if(st == eCharacterType::hoplite) {
        bt = eBannerType::hoplite;
    } else if(st == eCharacterType::horseman) {
        bt = eBannerType::horseman;
    } else {
        return;
    }
    const auto b = e::make_shared<eSoldierBanner>(bt, *this);
    b->setCityId(mId);
    registerSoldierBanner(b);
    b->incCount();
    b->moveToDefault();
}
void eBoardCity::removeSoldier(const eCharacterType st,
                               const bool skipNotHome) {
    for(const auto& b : mPalaceSoldierBanners) {
        if(b->isAbroad()) continue;
        if(skipNotHome && !b->isHome()) continue;
        const auto bt = b->type();
        const int c = b->count();
        if(c <= 0) continue;
        bool found = false;
        if(bt == eBannerType::rockThrower &&
           st == eCharacterType::rockThrower) {
            found = true;
        } else if(bt == eBannerType::hoplite &&
                  st == eCharacterType::hoplite) {
            found = true;
        } else if(bt == eBannerType::horseman &&
                  st == eCharacterType::horseman){
            found = true;
        }
        if(found) {
            b->decCount();
            return;
        }
    }
    if(skipNotHome) removeSoldier(st, false);
}

void eBoardCity::registerSoldierBanner(const stdsptr<eSoldierBanner>& b) {
    if(b->militaryAid()) return;
    b->setRegistered(true);
    mSoldierBanners.push_back(b);
    switch(b->type()) {
    case eBannerType::rockThrower:
    case eBannerType::hoplite:
    case eBannerType::horseman:
        mPalaceSoldierBanners.push_back(b);
        break;
    default:
        break;
    }
}

bool eBoardCity::unregisterSoldierBanner(const stdsptr<eSoldierBanner>& b) {
    b->setRegistered(false);
    eVectorHelpers::remove(mPalaceSoldierBanners, b);
    return eVectorHelpers::remove(mSoldierBanners, b);
}

eBoardPlayer* eBoardCity::owningPlayer() const {
    const auto pid = mBoard.cityIdToPlayerId(mId);
    const auto p = mBoard.boardPlayerWithId(pid);
    return p;
}

int eBoardCity::eliteHouses() const {
    int r = 0;
    for(const auto b : mTimedBuildings) {
        const auto bt = b->type();
        if(bt == eBuildingType::eliteHousing) r++;
    }
    return r;
}

eSanctuary* eBoardCity::sanctuary(const eGodType god) const {
    for(const auto s : mSanctuaries) {
        if(s->godType() == god) return s;
    }
    return nullptr;
}

eHerosHall* eBoardCity::heroHall(const eHeroType hero) const {
    for(const auto h : mHeroHalls) {
        if(h->heroType() == hero) return h;
    }
    return nullptr;
}

int eBoardCity::countBanners(const eBannerType bt) const {
    int c = 0;
    for(const auto& bn : mSoldierBanners) {
        if(bn->type() != bt) continue;
        c++;
    }
    return c;
}

int eBoardCity::countSoldiers(const eBannerType bt) const {
    int c = 0;
    for(const auto& bn : mSoldierBanners) {
        if(bn->type() != bt) continue;
        c += bn->count();
    }
    return c;
}
