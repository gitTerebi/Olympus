#include "eboardcity.h"
#include <cstdio>
static void dbgLog(const char* msg) {
    (void)msg;
}
static void dbgLogN(const char* msg, int n) {
    (void)msg;
    (void)n;
}

#include "gameEvents/invasions/invasion-handler.h"
#include "engine/eplague.h"
#include "characters/monsters/emonster.h"
#include "gameEvents/invasions/monster-invasion-event-base.h"
#include "engine/emilitaryaid.h"
#include "engine/ereinforcements.h"
#include "characters/esoldierbanner.h"
#include "gameEvents/egameevent.h"
#include "engine/e-game-board.h"
#include "buildings/ehippodrome.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"
#include "buildings/eavailablebuildings.h"
#include "engine/ai/eaicityplan.h"
#include "engine/eemploymentdistributor.h"
#include "engine/egameevents.h"

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
    ar.field("excessSheepCullUpdate", mExcessSheepCullUpdate, 0);

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
            printf("Deprecated binary invasion handler read skipped; JSON serializeJson should be used\n");
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
            printf("Deprecated binary plague read skipped; JSON serializeJson should be used\n");
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
            printf("Deprecated binary military aid read skipped; JSON serializeJson should be used\n");
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
            printf("Deprecated binary board-city building read skipped; JSON serializeJson should be used\n");
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
            printf("Deprecated binary board-city hall read skipped; JSON serializeJson should be used\n");
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

void eBoardCity::serializeJson(eJsonArchive& ar) {
    ar.field("id", mId);
    ar.field("atlantean", mAtlantean);

    { auto a = ar.child("availableBuildings"); mAvailableBuildings.serializeJson(a); }
    { auto a = ar.child("cityEvents"); mCityEvents.serializeJson(a); }
    { auto a = ar.child("cityPlan"); mCityPlan.serializeJson(a); }

    ar.field("wageRate", mWageRate);
    ar.field("taxRate", mTaxRate);
    ar.field("taxesPaidLastYear", mTaxesPaidLastYear);
    ar.field("taxesPaidThisYear", mTaxesPaidThisYear);
    ar.field("peoplePaidTaxesLastYear", mPeoplePaidTaxesLastYear);
    ar.field("peoplePaidTaxesThisYear", mPeoplePaidTaxesThisYear);
    ar.field("excessSheepCullUpdate", mExcessSheepCullUpdate);
    ar.field("maxSanctuaries", mMaxSanctuaries);
    ar.field("immigrationLimit", mImmigrationLimit);
    ar.field("noFood", mNoFood);
    { auto a = ar.child("noFoodSince"); mNoFoodSince.serializeJson(a); }
    { auto a = ar.child("emplDistributor"); mEmplDistributor.serializeJson(a); }

    {
        int ns = ar.reading() ? 0 : static_cast<int>(mShutDown.size());
        ar.field("shutdownCount", ns);
        for(int i = 0; i < ns; i++) {
            eResourceType type = ar.reading() ? eResourceType::none : mShutDown[i];
            ar.field(("shutdown." + std::to_string(i)).c_str(), type);
            if(ar.reading()) mShutDown.push_back(type);
        }
    }
    {
        int ns = ar.reading() ? 0 : static_cast<int>(mStockpiled.size());
        ar.field("stockpiledCount", ns);
        for(int i = 0; i < ns; i++) {
            eResourceType type = ar.reading() ? eResourceType::none : mStockpiled[i];
            ar.field(("stockpiled." + std::to_string(i)).c_str(), type);
            if(ar.reading()) mStockpiled.push_back(type);
        }
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
        int ni = ar.reading() ? 0 : static_cast<int>(mInvasionHandlers.size());
        ar.field("invasionHandlerCount", ni);
        for(int i = 0; i < ni; i++) {
            auto ca = ar.childAt("invasionHandlers", i);
            if(ar.reading()) {
                const auto ii = new eInvasionHandler(mBoard, mId, nullptr, nullptr);
                ii->serializeJson(ca);
            } else {
                mInvasionHandlers[i]->serializeJson(ca);
            }
        }
    }

    // attacking gods — character IOIDs
    {
        int ngs = ar.reading() ? 0 : static_cast<int>(mAttackingGods.size());
        ar.field("attackingGodCount", ngs);
        for(int i = 0; i < ngs; i++) {
            if(ar.reading()) {
                ar.addPostFunc([this, &ar](){
                    // resolved via character load pass
                });
            } else {
                // characters serialized separately — skip
            }
        }
    }

    // monsters — skip (restored via character load pass)

    {
        int np = ar.reading() ? 0 : static_cast<int>(mPlagues.size());
        ar.field("plagueCount", np);
        for(int i = 0; i < np; i++) {
            auto ca = ar.childAt("plagues", i);
            if(ar.reading()) {
                const auto p = std::make_shared<ePlague>(mId, mBoard);
                p->serializeJson(ca);
                mPlagues.push_back(p);
            } else {
                mPlagues[i]->serializeJson(ca);
            }
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
        int na = ar.reading() ? 0 : static_cast<int>(mMilitaryAid.size());
        ar.field("militaryAidCount", na);
        for(int i = 0; i < na; i++) {
            auto ca = ar.childAt("militaryAid", i);
            if(ar.reading()) {
                const auto ma = std::make_shared<eMilitaryAid>();
                ma->serializeJson(ca, mBoard);
                addMilitaryAid(ma);
            } else {
                mMilitaryAid[i]->serializeJson(ca, mBoard);
            }
        }
    }

    {
        int nh = ar.reading() ? 0 : static_cast<int>(mSummonedHeroes.size());
        ar.field("summonedHeroCount", nh);
        for(int i = 0; i < nh; i++) {
            eHeroType h = ar.reading() ? eHeroType::theseus : mSummonedHeroes[i];
            ar.field(("summonedHero." + std::to_string(i)).c_str(), h);
            if(ar.reading()) mSummonedHeroes.push_back(h);
        }
    }

    ar.field("nextAttackPlanned", mNextAttackPlanned);
    { auto a = ar.child("nextAttackDate"); mNextAttackDate.serializeJson(a); }

    // monster events — game event cross-refs (skip; restored via event load pass)

    {
        int nb = ar.reading() ? 0 : static_cast<int>(mSoldierBanners.size());
        ar.field("soldierBannerCount", nb);
        for(int i = 0; i < nb; i++) {
            auto ca = ar.childAt("soldierBanners", i);
            eBannerType type = ar.reading() ? eBannerType::hoplite : mSoldierBanners[i]->type();
            ca.field("type", type);
            if(ar.reading()) {
                const auto b = e::make_shared<eSoldierBanner>(type, mBoard);
                b->serializeJson(ca);
                registerSoldierBanner(b);
            } else {
                mSoldierBanners[i]->serializeJson(ca);
            }
        }
    }

    {
        int nh = ar.reading() ? 0 : static_cast<int>(mHippodromes.size());
        ar.field("hippodromeCount", nh);
        for(int i = 0; i < nh; i++) {
            auto ca = ar.childAt("hippodromes", i);
            if(ar.reading()) {
                const auto h = std::make_shared<eHippodrome>(mId, mBoard);
                h->serializeJson(ca);
                mHippodromes.push_back(h);
            } else {
                mHippodromes[i]->serializeJson(ca);
            }
        }
    }

    {
        int nr = ar.reading() ? 0 : static_cast<int>(mReinforcements.size());
        ar.field("reinforcementCount", nr);
        for(int i = 0; i < nr; i++) {
            auto ca = ar.childAt("reinforcements", i);
            if(ar.reading()) {
                auto& r = mReinforcements.emplace_back();
                r.serializeJson(ca, mBoard);
            } else {
                mReinforcements[i].serializeJson(ca, mBoard);
            }
        }
    }

    ar.field("defending", mDefending);

    // exported
    {
        int ne = ar.reading() ? 0 : static_cast<int>(mExported.size());
        ar.field("exportedCityCount", ne);
        int ei = 0;
        if(ar.reading()) {
            for(int i = 0; i < ne; i++) {
                auto ca = ar.childAt("exported", i);
                eCityId cid{}; ca.field("cid", cid);
                auto& e = mExported[cid];
                int nr2 = 0; ca.field("resourceCount", nr2);
                for(int j = 0; j < nr2; j++) {
                    auto ra = ca.childAt("resources", j);
                    eResourceType r{}; ra.field("type", r);
                    int n = 0; ra.field("amount", n);
                    e[r] = n;
                }
            }
        } else {
            for(const auto& [cid, map] : mExported) {
                auto ca = ar.childAt("exported", ei++);
                auto cidv = cid; ca.field("cid", cidv);
                int nr2 = static_cast<int>(map.size()); ca.field("resourceCount", nr2);
                int ji = 0;
                for(const auto& [r, n] : map) {
                    auto ra = ca.childAt("resources", ji++);
                    auto rv = r; ra.field("type", rv);
                    auto nv = n; ra.field("amount", nv);
                }
            }
        }
    }

    // imported
    {
        int ni2 = ar.reading() ? 0 : static_cast<int>(mImported.size());
        ar.field("importedCityCount", ni2);
        if(ar.reading()) {
            for(int i = 0; i < ni2; i++) {
                auto ca = ar.childAt("imported", i);
                eCityId cid{}; ca.field("cid", cid);
                auto& e = mImported[cid];
                int nr2 = 0; ca.field("resourceCount", nr2);
                for(int j = 0; j < nr2; j++) {
                    auto ra = ca.childAt("resources", j);
                    eResourceType r{}; ra.field("type", r);
                    int n = 0; ra.field("amount", n);
                    e[r] = n;
                }
            }
        } else {
            int ii2 = 0;
            for(const auto& [cid, map] : mImported) {
                auto ca = ar.childAt("imported", ii2++);
                auto cidv = cid; ca.field("cid", cidv);
                int nr2 = static_cast<int>(map.size()); ca.field("resourceCount", nr2);
                int ji = 0;
                for(const auto& [r, n] : map) {
                    auto ra = ca.childAt("resources", ji++);
                    auto rv = r; ra.field("type", rv);
                    auto nv = n; ra.field("amount", nv);
                }
            }
        }
    }
}
