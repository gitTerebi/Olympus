#include "eboardcity.h"

#include "einvasionhandler.h"
#include "engine/eplague.h"
#include "engine/emilitaryaid.h"
#include "characters/monsters/emonster.h"
#include "gameEvents/egameevent.h"

void eBoardCity::write(eWriteStream& dst) const {
    dst << mId;

    dst << mAtlantean;

    mAvailableBuildings.write(dst);

    mCityEvents.write(dst);

    mCityPlan.write(dst);

    dst << mWageRate;
    dst << mTaxRate;
    dst << mTaxesPaidLastYear;
    dst << mTaxesPaidThisYear;
    dst << mPeoplePaidTaxesLastYear;
    dst << mPeoplePaidTaxesThisYear;

    dst << mImmigrationLimit;
    dst << mNoFood;
    mNoFoodSince.write(dst);

    dst << mExported.size();
    for(const auto& e : mExported) {
        dst << e.first;
        const auto& map = e.second;
        dst << map.size();
        for(const auto& r : map) {
            dst << r.first;
            dst << r.second;
        }
    }

    mEmplDistributor.write(dst);

    dst << mShutDown.size();
    for(const auto i : mShutDown) {
        dst << i;
    }

    dst << mManTowers;

    dst << mShutdownLandTrade;
    dst << mShutdownSeaTrade;

    dst << mMaxRabble;
    dst << mMaxHoplites;
    dst << mMaxHorsemen;

    dst << mAthleticsCoverage;
    dst << mPhilosophyCoverage;
    dst << mDramaCoverage;
    dst << mAllDiscCoverage;
    dst << mTaxesCoverage;
    dst << mUnrest;
    dst << mPopularity;
    dst << mHealth;
    dst << mExcessiveMilitaryServiceCount;
    dst << mMonthsOfMilitaryService;

    dst << mWonGames;

    {
        const int ni = mInvasionHandlers.size();
        dst << ni;
        for(const auto i : mInvasionHandlers) {
            i->write(dst);
        }
    }

    {
        const int ngs = mAttackingGods.size();
        dst << ngs;
        for(const auto g : mAttackingGods) {
            dst.writeCharacter(g);
        }
    }

    {
        const int nms = mMonsters.size();
        dst << nms;
        for(const auto g : mMonsters) {
            dst.writeCharacter(g);
        }
    }

    {
        dst << mPlagues.size();

        for(const auto& p : mPlagues) {
            p->write(dst);
        }
    }

    dst << mPop100;
    dst << mPop500;
    dst << mPop1000;
    dst << mPop2000;
    dst << mPop3000;
    dst << mPop5000;
    dst << mPop10000;
    dst << mPop15000;
    dst << mPop20000;
    dst << mPop25000;

    dst << mMilitaryAid.size();
    for(const auto& a : mMilitaryAid) {
        a->write(dst);
    }

    dst << mSummonedHeroes.size();
    for(const auto h : mSummonedHeroes) {
        dst << h;
    }

    dst << mNextAttackPlanned;
    mNextAttackDate.write(dst);
}
