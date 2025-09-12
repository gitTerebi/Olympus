#include "eboardcity.h"

#include "einvasionhandler.h"
#include "engine/eplague.h"
#include "characters/monsters/emonster.h"
#include "gameEvents/emonsterinvasioneventbase.h"
#include "engine/emilitaryaid.h"
#include "gameEvents/egameevent.h"
#include "engine/egameboard.h"
#include "buildings/ehippodrome.h"

void eBoardCity::read(eReadStream& src) {
    src >> mId;

    src >> mAtlantean;

    mAvailableBuildings.read(src);

    mCityEvents.read(src);

    mCityPlan.read(src);
    for(int i = 0; i < mCityPlan.districtCount(); i++) {
        const auto& d = mCityPlan.district(i);
        mEditorDistrictConditions[i] = d.fReadyConditions;
    }

    src >> mWageRate;
    src >> mTaxRate;
    src >> mTaxesPaidLastYear;
    src >> mTaxesPaidThisYear;
    src >> mPeoplePaidTaxesLastYear;
    src >> mPeoplePaidTaxesThisYear;

    src >> mMaxSanctuaries;

    src >> mImmigrationLimit;
    src >> mNoFood;
    mNoFoodSince.read(src);

    int ne;
    src >> ne;
    for(int i = 0; i < ne; i++) {
        eCityId cid;
        src >> cid;
        auto& e = mExported[cid];
        int nr;
        src >> nr;
        for(int j = 0; j < nr; j++) {
            eResourceType r;
            src >> r;
            int n;
            src >> n;
            e[r] = n;
        }
    }

    mEmplDistributor.read(src);

    int ns;
    src >> ns;
    for(int i = 0; i < ns; i++) {
        eResourceType type;
        src >> type;
        mShutDown.push_back(type);
    }

    src >> mManTowers;

    src >> mShutdownLandTrade;
    src >> mShutdownSeaTrade;

    src >> mMaxRabble;
    src >> mMaxHoplites;
    src >> mMaxHorsemen;

    src >> mAthleticsCoverage;
    src >> mPhilosophyCoverage;
    src >> mDramaCoverage;
    src >> mAllDiscCoverage;
    src >> mTaxesCoverage;
    src >> mUnrest;
    src >> mPopularity;
    src >> mHealth;
    src >> mExcessiveMilitaryServiceCount;
    src >> mMonthsOfMilitaryService;

    src >> mWonGames;

    {
        int ni;
        src >> ni;

        for(int i = 0; i < ni; i++) {
            const auto ii = new eInvasionHandler(mBoard, mId, nullptr, nullptr);
            ii->read(src);
        }
    }

    {
        int ngs;
        src >> ngs;
        for(int i = 0; i < ngs; i++) {
            src.readCharacter(&mBoard, [this](eCharacter* const c) {
                mAttackingGods.push_back(c);
            });
        }
    }

    {
        int nms;
        src >> nms;
        for(int i = 0; i < nms; i++) {
            src.readCharacter(&mBoard, [this](eCharacter* const c) {
                mMonsters.push_back(static_cast<eMonster*>(c));
            });
        }
    }

    {
        int n;
        src >> n;

        for(int i = 0; i < n; i++) {
            const auto p = std::make_shared<ePlague>(mId, mBoard);
            p->read(src);
            mPlagues.push_back(p);
        }
    }

    src >> mPop100;
    src >> mPop500;
    src >> mPop1000;
    src >> mPop2000;
    src >> mPop3000;
    src >> mPop5000;
    src >> mPop10000;
    src >> mPop15000;
    src >> mPop20000;
    src >> mPop25000;

    {
        int na;
        src >> na;
        for(int i = 0; i < na; i++) {
            const auto ma = std::make_shared<eMilitaryAid>();
            ma->read(src, &mBoard);
            addMilitaryAid(ma);
        }
    }

    {
        int nh;
        src >> nh;
        for(int i = 0; i < nh; i++) {
            eHeroType h;
            src >> h;
            mSummonedHeroes.push_back(h);
        }
    }

    src >> mNextAttackPlanned;
    mNextAttackDate.read(src);

    {
        int nq;
        src >> nq;
        for(int i = 0; i < nq; i++) {
            eMonsterType type;
            src >> type;
            src.readGameEvent(&mBoard, [this, type](eGameEvent* const e) {
                const auto me = static_cast<eMonsterInvasionEventBase*>(e);
                mMonsterEvents[type] = me;
            });
        }
    }

    {
        int nb;
        src >> nb;
        for(int i = 0; i < nb; i++) {
            eBannerType type;
            src >> type;
            const auto b = e::make_shared<eSoldierBanner>(type, mBoard);
            b->read(src);
            registerSoldierBanner(b);
        }
    }

    {
        int nh;
        src >> nh;
        for(int i = 0; i < nh; i++) {
            const auto h = std::make_shared<eHippodrome>(mId, mBoard);
            h->read(src);
            mHippodromes.push_back(h);
        }
    }

    {
        int nr;
        src >> nr;
        for(int i = 0; i < nr; i++) {
            auto& r = mReinforcements.emplace_back();
            r.read(mBoard, src);
        }
    }

    src >> mDefending;
}
