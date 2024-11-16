#include "eboardcity.h"

#include "buildings/ebuilding.h"
#include "buildings/eemployingbuilding.h"
#include "buildings/estoragebuilding.h"
#include "buildings/esmallhouse.h"
#include "buildings/eelitehousing.h"
#include "evectorhelpers.h"

#include "egameboard.h"

eBoardCity::eBoardCity(eGameBoard& board) :
    mBoard(board),
    mHusbData(mPopData, board), mEmplData(mPopData, board),
    mEmplDistributor(mEmplData) {}

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
