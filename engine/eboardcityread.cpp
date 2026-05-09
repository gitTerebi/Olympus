#include "eboardcity.h"

#include "e-invasion-handler.h"
#include "engine/eplague.h"
#include "characters/monsters/emonster.h"
#include "gameEvents/emonsterinvasioneventbase.h"
#include "engine/emilitaryaid.h"
#include "gameEvents/egameevent.h"
#include "engine/e-game-board.h"
#include "buildings/ehippodrome.h"
#include "fileIO/esavearchive.h"

void eBoardCity::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}
void eBoardCity::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        auto& src = ar.readStream();
    ar.field("id", mId);

    ar.field("atlantean", mAtlantean);

    mAvailableBuildings.read(src);

    mCityEvents.read(src);

    mCityPlan.read(src);
    for(int i = 0; i < mCityPlan.districtCount(); i++) {
        const auto& d = mCityPlan.district(i);
        mEditorDistrictConditions[i] = d.fReadyConditions;
    }

    ar.field("wageRate", mWageRate);
    ar.field("taxRate", mTaxRate);
    ar.field("taxesPaidLastYear", mTaxesPaidLastYear);
    ar.field("taxesPaidThisYear", mTaxesPaidThisYear);
    ar.field("peoplePaidTaxesLastYear", mPeoplePaidTaxesLastYear);
    ar.field("peoplePaidTaxesThisYear", mPeoplePaidTaxesThisYear);

    ar.field("maxSanctuaries", mMaxSanctuaries);

    ar.field("immigrationLimit", mImmigrationLimit);
    ar.field("noFood", mNoFood);
    mNoFoodSince.read(src);

    int ne;
    ar.field("exportedCityCount", ne);
    for(int i = 0; i < ne; i++) {
        eCityId cid;
        ar.field("exportedCityId", cid);
        auto& e = mExported[cid];
        int nr;
        ar.field("exportedResourceCount", nr);
        for(int j = 0; j < nr; j++) {
            eResourceType r;
            ar.field("exportedResource", r);
            int n;
            ar.field("exportedAmount", n);
            e[r] = n;
        }
    }

    mEmplDistributor.read(src);

    int ns;
    ar.field("shutdownResourceCount", ns);
    for(int i = 0; i < ns; i++) {
        eResourceType type;
        ar.field("shutdownResource", type);
        mShutDown.push_back(type);
    }

    ar.field("manTowers", mManTowers);

    ar.field("shutdownLandTrade", mShutdownLandTrade);
    ar.field("shutdownSeaTrade", mShutdownSeaTrade);

    ar.field("maxRabble", mMaxRabble);
    ar.field("maxHoplites", mMaxHoplites);
    ar.field("maxHorsemen", mMaxHorsemen);

    ar.field("athleticsCoverage", mAthleticsCoverage);
    ar.field("philosophyCoverage", mPhilosophyCoverage);
    ar.field("dramaCoverage", mDramaCoverage);
    ar.field("allDiscCoverage", mAllDiscCoverage);
    ar.field("taxesCoverage", mTaxesCoverage);
    ar.field("unrest", mUnrest);
    ar.field("popularity", mPopularity);
    ar.field("health", mHealth);
    ar.field("excessiveMilitaryServiceCount", mExcessiveMilitaryServiceCount);
    ar.field("monthsOfMilitaryService", mMonthsOfMilitaryService);

    ar.field("wonGames", mWonGames);

    {
        int ni;
        ar.field("invasionHandlerCount", ni);

        for(int i = 0; i < ni; i++) {
            const auto ii = new eInvasionHandler(mBoard, mId, nullptr, nullptr);
            ii->read(src);
        }
    }

    {
        int ngs;
        ar.field("attackingGodCount", ngs);
        for(int i = 0; i < ngs; i++) {
            src.readCharacter(&mBoard, [this](eCharacter* const c) {
                mAttackingGods.push_back(c);
            });
        }
    }

    {
        int nms;
        ar.field("monsterCount", nms);
        for(int i = 0; i < nms; i++) {
            src.readCharacter(&mBoard, [this](eCharacter* const c) {
                if(!c) return;
                mMonsters.push_back(static_cast<eMonster*>(c));
            });
        }
    }

    {
        int n;
        ar.field("plagueCount", n);

        for(int i = 0; i < n; i++) {
            const auto p = std::make_shared<ePlague>(mId, mBoard);
            p->read(src);
            mPlagues.push_back(p);
        }
    }

    ar.field("pop100", mPop100);
    ar.field("pop500", mPop500);
    ar.field("pop1000", mPop1000);
    ar.field("pop2000", mPop2000);
    ar.field("pop3000", mPop3000);
    ar.field("pop5000", mPop5000);
    ar.field("pop10000", mPop10000);
    ar.field("pop15000", mPop15000);
    ar.field("pop20000", mPop20000);
    ar.field("pop25000", mPop25000);

    {
        int na;
        ar.field("militaryAidCount", na);
        for(int i = 0; i < na; i++) {
            const auto ma = std::make_shared<eMilitaryAid>();
            ma->read(src, &mBoard);
            addMilitaryAid(ma);
        }
    }

    {
        int nh;
        ar.field("summonedHeroCount", nh);
        for(int i = 0; i < nh; i++) {
            eHeroType h;
            ar.field("summonedHero", h);
            mSummonedHeroes.push_back(h);
        }
    }

    ar.field("nextAttackPlanned", mNextAttackPlanned);
    mNextAttackDate.read(src);

    {
        int nq;
        ar.field("monsterEventCount", nq);
        for(int i = 0; i < nq; i++) {
            eMonsterType type;
            ar.field("monsterEventType", type);
            src.readGameEvent(&mBoard, [this, type](eGameEvent* const e) {
                if(!e) return;
                const auto me = static_cast<eMonsterInvasionEventBase*>(e);
                mMonsterEvents[type] = me;
            });
        }
    }

    {
        int nb;
        ar.field("soldierBannerCount", nb);
        for(int i = 0; i < nb; i++) {
            eBannerType type;
            ar.field("soldierBannerType", type);
            const auto b = e::make_shared<eSoldierBanner>(type, mBoard);
            b->read(src);
            registerSoldierBanner(b);
        }
    }

    {
        int nh;
        ar.field("hippodromeCount", nh);
        for(int i = 0; i < nh; i++) {
            const auto h = std::make_shared<eHippodrome>(mId, mBoard);
            h->read(src);
            mHippodromes.push_back(h);
        }
    }

    {
        int nr;
        ar.field("reinforcementCount", nr);
        for(int i = 0; i < nr; i++) {
            auto& r = mReinforcements.emplace_back();
            r.read(mBoard, src);
        }
    }

    ar.field("defending", mDefending);
    } else {
        auto& dst = ar.writeStream();
    ar.field("id", mId);

    ar.field("atlantean", mAtlantean);

    mAvailableBuildings.write(dst);

    mCityEvents.write(dst);

    mCityPlan.write(dst);

    ar.field("wageRate", mWageRate);
    ar.field("taxRate", mTaxRate);
    ar.field("taxesPaidLastYear", mTaxesPaidLastYear);
    ar.field("taxesPaidThisYear", mTaxesPaidThisYear);
    ar.field("peoplePaidTaxesLastYear", mPeoplePaidTaxesLastYear);
    ar.field("peoplePaidTaxesThisYear", mPeoplePaidTaxesThisYear);

    ar.field("maxSanctuaries", mMaxSanctuaries);

    ar.field("immigrationLimit", mImmigrationLimit);
    ar.field("noFood", mNoFood);
    mNoFoodSince.write(dst);

    int exportedCityCount = mExported.size();
    ar.field("exportedCityCount", exportedCityCount);
    for(const auto& e : mExported) {
        auto cityId = e.first;
        ar.field("exportedCityId", cityId);
        const auto& map = e.second;
        int exportedResourceCount = map.size();
        ar.field("exportedResourceCount", exportedResourceCount);
        for(const auto& r : map) {
            auto resource = r.first;
            auto amount = r.second;
            ar.field("exportedResource", resource);
            ar.field("exportedAmount", amount);
        }
    }

    mEmplDistributor.write(dst);

    int shutdownResourceCount = mShutDown.size();
    ar.field("shutdownResourceCount", shutdownResourceCount);
    for(const auto i : mShutDown) {
        auto resource = i;
        ar.field("shutdownResource", resource);
    }

    ar.field("manTowers", mManTowers);

    ar.field("shutdownLandTrade", mShutdownLandTrade);
    ar.field("shutdownSeaTrade", mShutdownSeaTrade);

    ar.field("maxRabble", mMaxRabble);
    ar.field("maxHoplites", mMaxHoplites);
    ar.field("maxHorsemen", mMaxHorsemen);

    ar.field("athleticsCoverage", mAthleticsCoverage);
    ar.field("philosophyCoverage", mPhilosophyCoverage);
    ar.field("dramaCoverage", mDramaCoverage);
    ar.field("allDiscCoverage", mAllDiscCoverage);
    ar.field("taxesCoverage", mTaxesCoverage);
    ar.field("unrest", mUnrest);
    ar.field("popularity", mPopularity);
    ar.field("health", mHealth);
    ar.field("excessiveMilitaryServiceCount", mExcessiveMilitaryServiceCount);
    ar.field("monthsOfMilitaryService", mMonthsOfMilitaryService);

    ar.field("wonGames", mWonGames);

    {
        const int ni = mInvasionHandlers.size();
        auto invasionHandlerCount = ni;
        ar.field("invasionHandlerCount", invasionHandlerCount);
        for(const auto i : mInvasionHandlers) {
            i->write(dst);
        }
    }

    {
        const int ngs = mAttackingGods.size();
        auto attackingGodCount = ngs;
        ar.field("attackingGodCount", attackingGodCount);
        for(const auto g : mAttackingGods) {
            dst.writeCharacter(g);
        }
    }

    {
        const int nms = mMonsters.size();
        auto monsterCount = nms;
        ar.field("monsterCount", monsterCount);
        for(const auto g : mMonsters) {
            dst.writeCharacter(g);
        }
    }

    {
        int plagueCount = mPlagues.size();
        ar.field("plagueCount", plagueCount);

        for(const auto& p : mPlagues) {
            p->write(dst);
        }
    }

    ar.field("pop100", mPop100);
    ar.field("pop500", mPop500);
    ar.field("pop1000", mPop1000);
    ar.field("pop2000", mPop2000);
    ar.field("pop3000", mPop3000);
    ar.field("pop5000", mPop5000);
    ar.field("pop10000", mPop10000);
    ar.field("pop15000", mPop15000);
    ar.field("pop20000", mPop20000);
    ar.field("pop25000", mPop25000);

    int militaryAidCount = mMilitaryAid.size();
    ar.field("militaryAidCount", militaryAidCount);
    for(const auto& a : mMilitaryAid) {
        a->write(dst);
    }

    int summonedHeroCount = mSummonedHeroes.size();
    ar.field("summonedHeroCount", summonedHeroCount);
    for(const auto h : mSummonedHeroes) {
        auto hero = h;
        ar.field("summonedHero", hero);
    }

    ar.field("nextAttackPlanned", mNextAttackPlanned);
    mNextAttackDate.write(dst);

    int monsterEventCount = mMonsterEvents.size();
    ar.field("monsterEventCount", monsterEventCount);
    for(const auto& m : mMonsterEvents) {
        auto monsterType = m.first;
        ar.field("monsterEventType", monsterType);
        dst.writeGameEvent(m.second);
    }

    int soldierBannerCount = mSoldierBanners.size();
    ar.field("soldierBannerCount", soldierBannerCount);
    for(const auto& s : mSoldierBanners) {
        auto bannerType = s->type();
        ar.field("soldierBannerType", bannerType);
        s->write(dst);
    }

    int hippodromeCount = mHippodromes.size();
    ar.field("hippodromeCount", hippodromeCount);
    for(const auto& h : mHippodromes) {
        h->write(dst);
    }

    int reinforcementCount = mReinforcements.size();
    ar.field("reinforcementCount", reinforcementCount);
    for(const auto& r : mReinforcements) {
        r.write(dst);
    }

    ar.field("defending", mDefending);
    }
}
