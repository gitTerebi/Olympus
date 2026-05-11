#include "eboardcity.h"
#include <cstdio>
static void dbgLog(const char* msg) {
    FILE* f = fopen("C:/Users/somtam/Desktop/load_dbg.txt", "a");
    if(f) { fprintf(f, "%s\n", msg); fclose(f); }
}
static void dbgLogN(const char* msg, int n) {
    FILE* f = fopen("C:/Users/somtam/Desktop/load_dbg.txt", "a");
    if(f) { fprintf(f, "%s %d\n", msg, n); fclose(f); }
}

#include "gameEvents/invasions/invasion-handler.h"
#include "engine/eplague.h"
#include "characters/monsters/emonster.h"
#include "gameEvents/invasions/monster-invasion-event-base.h"
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
    dbgLog("boardcity: id");
    ar.field("id", mId);

    ar.field("atlantean", mAtlantean);

    dbgLog("boardcity: availableBuildings");
    mAvailableBuildings.serialize(ar);
    dbgLog("boardcity: cityEvents");
    mCityEvents.serialize(ar);
    dbgLog("boardcity: cityPlan");
    mCityPlan.serialize(ar);
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
    dbgLog("boardcity: noFoodSince");
    mNoFoodSince.serialize(ar);

    dbgLog("boardcity: emplDistributor start");
    mEmplDistributor.serialize(ar);
    dbgLog("boardcity: emplDistributor done");

    int ns;
    dbgLog("boardcity: shutdownResourceCount");
    ar.field("shutdownResourceCount", ns);
    dbgLogN("boardcity: shutdownResourceCount val", ns);
    for(int i = 0; i < ns; i++) {
        eResourceType type;
        ar.field("shutdownResource", type);
        mShutDown.push_back(type);
    }
    dbgLog("boardcity: shutdownResourceCount done");

    int nsp = 0;
    dbgLog("boardcity: stockpiledResourceCount");
    ar.field("stockpiledResourceCount", nsp, 0);
    dbgLogN("boardcity: stockpiledResourceCount val", nsp);
    for(int i = 0; i < nsp; i++) {
        eResourceType type;
        ar.field("stockpiledResource", type);
        mStockpiled.push_back(type);
    }
    dbgLog("boardcity: stockpiledResourceCount done");

    dbgLog("boardcity: manTowers");
    ar.field("manTowers", mManTowers);
    dbgLog("boardcity: shutdownLandTrade");
    ar.field("shutdownLandTrade", mShutdownLandTrade);
    dbgLog("boardcity: shutdownSeaTrade");
    ar.field("shutdownSeaTrade", mShutdownSeaTrade);

    dbgLog("boardcity: maxRabble");
    ar.field("maxRabble", mMaxRabble);
    ar.field("maxHoplites", mMaxHoplites);
    ar.field("maxHorsemen", mMaxHorsemen);
    dbgLog("boardcity: coverage");
    ar.field("athleticsCoverage", mAthleticsCoverage);
    ar.field("philosophyCoverage", mPhilosophyCoverage);
    ar.field("dramaCoverage", mDramaCoverage);
    ar.field("allDiscCoverage", mAllDiscCoverage);
    ar.field("taxesCoverage", mTaxesCoverage);
    dbgLog("boardcity: unrest");
    ar.field("unrest", mUnrest);
    ar.field("popularity", mPopularity);
    ar.field("health", mHealth);
    ar.field("excessiveMilitaryServiceCount", mExcessiveMilitaryServiceCount);
    ar.field("monthsOfMilitaryService", mMonthsOfMilitaryService);
    dbgLog("boardcity: wonGames");
    ar.field("wonGames", mWonGames);
    dbgLog("boardcity: wonGames done");

    {
        int ni = 0;
        ar.field("invasionHandlerCount", ni, 0);
        dbgLogN("boardcity: invasionHandlers", ni);
        for(int i = 0; i < ni; i++) {
            dbgLogN("boardcity: invasionHandler", i);
            const auto ii = new eInvasionHandler(mBoard, mId, nullptr, nullptr);
            ii->read(src);
        }
    }

    {
        int ngs = 0;
        ar.field("attackingGodCount", ngs, 0);
        dbgLogN("boardcity: attackingGods", ngs);
        for(int i = 0; i < ngs; i++) {
            src.readCharacter(&mBoard, [this](eCharacter* const c) {
                mAttackingGods.push_back(c);
            });
        }
    }

    {
        int nms = 0;
        ar.field("monsterCount", nms, 0);
        dbgLogN("boardcity: monsters", nms);
        for(int i = 0; i < nms; i++) {
            src.readCharacter(&mBoard, [this](eCharacter* const c) {
                if(!c) return;
                mMonsters.push_back(static_cast<eMonster*>(c));
            });
        }
    }

    {
        int n = 0;
        ar.field("plagueCount", n, 0);
        dbgLogN("boardcity: plagues", n);
        for(int i = 0; i < n; i++) {
            dbgLogN("boardcity: plague", i);
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
        int na = 0;
        ar.field("militaryAidCount", na, 0);
        dbgLogN("boardcity: militaryAid", na);
        for(int i = 0; i < na; i++) {
            dbgLogN("boardcity: militaryAidItem", i);
            const auto ma = std::make_shared<eMilitaryAid>();
            ma->read(src, &mBoard);
            addMilitaryAid(ma);
        }
    }

    {
        int nh = 0;
        ar.field("summonedHeroCount", nh, 0);
        for(int i = 0; i < nh; i++) {
            eHeroType h;
            ar.field("summonedHero", h);
            mSummonedHeroes.push_back(h);
        }
    }

    ar.field("nextAttackPlanned", mNextAttackPlanned);
    dbgLog("boardcity: nextAttackDate");
    mNextAttackDate.serialize(ar);

    {
        int nq = 0;
        ar.field("monsterEventCount", nq, 0);
        dbgLogN("boardcity: monsterEvents", nq);
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
        int nb = 0;
        ar.field("soldierBannerCount", nb, 0);
        dbgLogN("boardcity: soldierBanners", nb);
        for(int i = 0; i < nb; i++) {
            eBannerType type;
            ar.field("soldierBannerType", type);
            const auto b = e::make_shared<eSoldierBanner>(type, mBoard);
            b->read(src);
            registerSoldierBanner(b);
        }
    }

    {
        int nh = 0;
        ar.field("hippodromeCount", nh, 0);
        dbgLogN("boardcity: hippodromes", nh);
        for(int i = 0; i < nh; i++) {
            dbgLogN("boardcity: hippodrome", i);
            const auto h = std::make_shared<eHippodrome>(mId, mBoard);
            h->read(src);
            mHippodromes.push_back(h);
        }
    }

    {
        int nr = 0;
        ar.field("reinforcementCount", nr, 0);
        dbgLogN("boardcity: reinforcements", nr);
        for(int i = 0; i < nr; i++) {
            dbgLogN("boardcity: reinforcement", i);
            auto& r = mReinforcements.emplace_back();
            r.read(mBoard, src);
        }
    }

    dbgLog("boardcity: defending");
    ar.field("defending", mDefending);

    int ne = 0;
    dbgLog("boardcity: exportedCityCount");
    ar.field("exportedCityCount", ne, 0);
    dbgLogN("boardcity: exportedCityCount val", ne);
    for(int i = 0; i < ne; i++) {
        eCityId cid;
        dbgLogN("boardcity: exportedCityId index", i);
        ar.field("exportedCityId", cid);
        auto& e = mExported[cid];
        int nr = 0;
        dbgLog("boardcity: exportedResourceCount");
        ar.field("exportedResourceCount", nr, 0);
        dbgLogN("boardcity: exportedResourceCount val", nr);
        for(int j = 0; j < nr; j++) {
            eResourceType r;
            dbgLogN("boardcity: exportedResource index", j);
            ar.field("exportedResource", r);
            int n = 0;
            ar.field("exportedAmount", n, 0);
            e[r] = n;
        }
    }
    dbgLog("boardcity: exported done");

    int ni = 0;
    dbgLog("boardcity: importedCityCount");
    ar.field("importedCityCount", ni, 0);
    dbgLogN("boardcity: importedCityCount val", ni);
    for(int i = 0; i < ni; i++) {
        eCityId cid;
        dbgLogN("boardcity: importedCityId index", i);
        ar.field("importedCityId", cid);
        auto& e = mImported[cid];
        int nr = 0;
        dbgLog("boardcity: importedResourceCount");
        ar.field("importedResourceCount", nr, 0);
        dbgLogN("boardcity: importedResourceCount val", nr);
        for(int j = 0; j < nr; j++) {
            eResourceType r;
            dbgLogN("boardcity: importedResource index", j);
            ar.field("importedResource", r);
            int n = 0;
            ar.field("importedAmount", n, 0);
            e[r] = n;
        }
    }
    dbgLog("boardcity: imported done");
    dbgLog("boardcity: DONE");
    } else {
        auto& dst = ar.writeStream();
    ar.field("id", mId);

    ar.field("atlantean", mAtlantean);

    mAvailableBuildings.serialize(ar);

    mCityEvents.serialize(ar);

    mCityPlan.serialize(ar);

    ar.field("wageRate", mWageRate);
    ar.field("taxRate", mTaxRate);
    ar.field("taxesPaidLastYear", mTaxesPaidLastYear);
    ar.field("taxesPaidThisYear", mTaxesPaidThisYear);
    ar.field("peoplePaidTaxesLastYear", mPeoplePaidTaxesLastYear);
    ar.field("peoplePaidTaxesThisYear", mPeoplePaidTaxesThisYear);

    ar.field("maxSanctuaries", mMaxSanctuaries);

    ar.field("immigrationLimit", mImmigrationLimit);
    ar.field("noFood", mNoFood);
    mNoFoodSince.serialize(ar);

    mEmplDistributor.serialize(ar);

    int shutdownResourceCount = mShutDown.size();
    ar.field("shutdownResourceCount", shutdownResourceCount);
    for(const auto i : mShutDown) {
        auto resource = i;
        ar.field("shutdownResource", resource);
    }

    int stockpiledResourceCount = mStockpiled.size();
    ar.field("stockpiledResourceCount", stockpiledResourceCount);
    for(const auto i : mStockpiled) {
        auto resource = i;
        ar.field("stockpiledResource", resource);
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
    mNextAttackDate.serialize(ar);

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

    int importedCityCount = mImported.size();
    ar.field("importedCityCount", importedCityCount);
    for(const auto& e : mImported) {
        auto cityId = e.first;
        ar.field("importedCityId", cityId);
        const auto& map = e.second;
        int importedResourceCount = map.size();
        ar.field("importedResourceCount", importedResourceCount);
        for(const auto& r : map) {
            auto resource = r.first;
            auto amount = r.second;
            ar.field("importedResource", resource);
            ar.field("importedAmount", amount);
        }
    }
    }
}
