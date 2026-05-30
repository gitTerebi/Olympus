#include "board-city.h"

#include "gameEvents/invasions/invasion-handler.h"
#include "engine/eplague.h"
#include "characters/monsters/emonster.h"
#include "gameEvents/invasions/monster-invasion-event-base.h"
#include "engine/emilitaryaid.h"
#include "gameEvents/egameevent.h"
#include "engine/game-board.h"
#include "buildings/ehippodrome.h"
#include "fileIO/esavearchive.h"

void BoardCity::serialize(eSaveArchive& ar) {
    ar.field("id", mId);
    ar.field("atlantean", mAtlantean);

    ar.archiveField("availableBuildings",
        [this](eSaveArchive& itemAr) { mAvailableBuildings.serialize(itemAr); });

    // mCityEvents/mCityPlan/etc dump fields into parent scope; wrap each in own archive
    ar.archiveField("cityEvents", [this](eSaveArchive& itemAr) { mCityEvents.serialize(itemAr); });
    ar.archiveField("cityPlan", [this](eSaveArchive& itemAr) { mCityPlan.serialize(itemAr); });
    if(ar.reading()) {
        for(int i = 0; i < mCityPlan.districtCount(); i++) {
            const auto& d = mCityPlan.district(i);
            mEditorDistrictConditions[i] = d.fReadyConditions;
        }
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

    ar.archiveField("noFoodSince", [this](eSaveArchive& itemAr) { mNoFoodSince.serialize(itemAr); });
    ar.archiveField("emplDistributor", [this](eSaveArchive& itemAr) { mEmplDistributor.serialize(itemAr); });

    ar.arrayField("shutdownResources", mShutDown,
        [](eSaveArchive& itemAr, eResourceType& r) { itemAr.field("resource", r); });

    ar.arrayField("stockpiledResources", mStockpiled,
        [](eSaveArchive& itemAr, eResourceType& r) { itemAr.field("resource", r); });

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

    // invasionHandlers
    if(ar.reading()) {
        ar.countedArrayField("invasionHandlers", 0, [this](eSaveArchive& itemAr, const int) {
            const auto handler = new eInvasionHandler(mBoard, mId, nullptr, nullptr);
            handler->serialize(itemAr);
        });
    } else {
        ar.countedArrayField("invasionHandlers",
                             static_cast<int>(mInvasionHandlers.size()),
                             [this](eSaveArchive& itemAr, const int i) {
            mInvasionHandlers[i]->serialize(itemAr);
        });
    }

    ar.arrayField("attackingGods", mAttackingGods,
        [this](eSaveArchive& itemAr, eCharacter*& c) {
            itemAr.characterField("character", &mBoard, c);
        });

    ar.arrayField("monsters", mMonsters,
        [this](eSaveArchive& itemAr, eMonster*& m) {
            itemAr.characterField("character", &mBoard, m);
        });

    // plagues
    {
        int plagueCount = ar.writing() ? static_cast<int>(mPlagues.size()) : 0;
        ar.field("plagues.count", plagueCount);
        if(ar.reading()) {
            for(int i = 0; i < plagueCount; i++) {
                const auto p = std::make_shared<ePlague>(mId, mBoard);
                ar.archiveField(("plague." + std::to_string(i)).c_str(),
                    [p](eSaveArchive& itemAr) { p->serialize(itemAr); });
                mPlagues.push_back(p);
            }
        } else {
            for(int i = 0; i < plagueCount; i++) {
                ar.archiveField(("plague." + std::to_string(i)).c_str(),
                    [this, i](eSaveArchive& itemAr) { mPlagues[i]->serialize(itemAr); });
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

    // militaryAid
    {
        int militaryAidCount = ar.writing() ? static_cast<int>(mMilitaryAid.size()) : 0;
        ar.field("militaryAids.count", militaryAidCount);
        if(ar.reading()) {
            for(int i = 0; i < militaryAidCount; i++) {
                const auto ma = std::make_shared<eMilitaryAid>();
                ar.archiveField(("militaryAid." + std::to_string(i)).c_str(),
                    [this, ma](eSaveArchive& itemAr) { ma->serialize(itemAr, &mBoard); });
                addMilitaryAid(ma);
            }
        } else {
            int i = 0;
            for(const auto& a : mMilitaryAid) {
                ar.archiveField(("militaryAid." + std::to_string(i++)).c_str(),
                    [&a](eSaveArchive& itemAr) { a->serialize(itemAr, nullptr); });
            }
        }
    }

    ar.arrayField("summonedHeroes", mSummonedHeroes,
        [](eSaveArchive& itemAr, eHeroType& h) { itemAr.field("heroType", h); });

    ar.field("nextAttackPlanned", mNextAttackPlanned);
    ar.archiveField("nextAttackDate", [this](eSaveArchive& itemAr) { mNextAttackDate.serialize(itemAr); });

    {
        int monsterEventCount = ar.writing() ? static_cast<int>(mMonsterEvents.size()) : 0;
        ar.field("monsterEvents.count", monsterEventCount);
        if(ar.reading()) {
            for(int i = 0; i < monsterEventCount; i++) {
                ar.archiveField(("monsterEvent." + std::to_string(i)).c_str(),
                    [this](eSaveArchive& itemAr) {
                        eMonsterType type;
                        itemAr.field("monsterType", type);
                        auto& slot = mMonsterEvents[type];
                        slot = nullptr;
                        itemAr.gameEventField("event", &mBoard, slot);
                    });
            }
        } else {
            int i = 0;
            for(auto& kv : mMonsterEvents) {
                ar.archiveField(("monsterEvent." + std::to_string(i++)).c_str(),
                    [&kv, this](eSaveArchive& itemAr) {
                        eMonsterType type = kv.first;
                        itemAr.field("monsterType", type);
                        itemAr.gameEventField("event", &mBoard, kv.second);
                    });
            }
        }
    }

    // soldierBanners
    {
        int bannerCount = ar.writing() ? static_cast<int>(mSoldierBanners.size()) : 0;
        ar.field("soldierBanners.count", bannerCount);
        if(ar.reading()) {
            for(int i = 0; i < bannerCount; i++) {
                ar.archiveField(("soldierBanner." + std::to_string(i)).c_str(),
                    [this](eSaveArchive& itemAr) {
                        eBannerType type;
                        itemAr.field("bannerType", type);
                        const auto b = e::make_shared<SoldierBanner>(type, mBoard);
                        b->serialize(itemAr);
                        registerSoldierBanner(b);
                    });
            }
        } else {
            int i = 0;
            for(const auto& s : mSoldierBanners) {
                ar.archiveField(("soldierBanner." + std::to_string(i++)).c_str(),
                    [&s](eSaveArchive& itemAr) {
                        eBannerType type = s->type();
                        itemAr.field("bannerType", type);
                        s->serialize(itemAr);
                    });
            }
        }
    }

    // hippodromes
    {
        int hippodromeCount = ar.writing() ? static_cast<int>(mHippodromes.size()) : 0;
        ar.field("hippodromes.count", hippodromeCount);
        if(ar.reading()) {
            for(int i = 0; i < hippodromeCount; i++) {
                const auto h = std::make_shared<eHippodrome>(mId, mBoard);
                ar.archiveField(("hippodrome." + std::to_string(i)).c_str(),
                    [h](eSaveArchive& itemAr) { h->serialize(itemAr); });
                mHippodromes.push_back(h);
            }
        } else {
            int i = 0;
            for(const auto& h : mHippodromes) {
                ar.archiveField(("hippodrome." + std::to_string(i++)).c_str(),
                    [&h](eSaveArchive& itemAr) { h->serialize(itemAr); });
            }
        }
    }

    // reinforcements
    {
        int reinforcementCount = ar.writing() ? static_cast<int>(mReinforcements.size()) : 0;
        ar.field("reinforcements.count", reinforcementCount);
        if(ar.reading()) {
            for(int i = 0; i < reinforcementCount; i++) {
                auto& r = mReinforcements.emplace_back();
                ar.archiveField(("reinforcement." + std::to_string(i)).c_str(),
                    [this, &r](eSaveArchive& itemAr) { r.serialize(itemAr, &mBoard); });
            }
        } else {
            int i = 0;
            for(auto& r : mReinforcements) {
                ar.archiveField(("reinforcement." + std::to_string(i++)).c_str(),
                    [&r](eSaveArchive& itemAr) { r.serialize(itemAr, nullptr); });
            }
        }
    }

    ar.field("defending", mDefending);

    // exported map<eCityId, map<eResourceType, int>>
    {
        int exportedCityCount = ar.writing() ? static_cast<int>(mExported.size()) : 0;
        ar.field("exportedCities.count", exportedCityCount);
        if(ar.reading()) {
            for(int i = 0; i < exportedCityCount; i++) {
                eCityId cid;
                ar.archiveField(("exportedCity." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        auto& map = mExported[cid];
                        int resourceCount = 0;
                        cityAr.field("resources.count", resourceCount);
                        for(int j = 0; j < resourceCount; j++) {
                            eResourceType r; int n;
                            cityAr.archiveField(("resource." + std::to_string(j)).c_str(),
                                [&](eSaveArchive& it) {
                                    it.field("resource", r);
                                    it.field("amount", n);
                                });
                            map[r] = n;
                        }
                    });
            }
        } else {
            int i = 0;
            for(auto& outer : mExported) {
                eCityId cid = outer.first;
                ar.archiveField(("exportedCity." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int resourceCount = static_cast<int>(outer.second.size());
                        cityAr.field("resources.count", resourceCount);
                        int j = 0;
                        for(auto& inner : outer.second) {
                            eResourceType r = inner.first;
                            int n = inner.second;
                            cityAr.archiveField(("resource." + std::to_string(j++)).c_str(),
                                [&](eSaveArchive& it) {
                                    it.field("resource", r);
                                    it.field("amount", n);
                                });
                        }
                    });
            }
        }
    }

    // imported map<eCityId, map<eResourceType, int>>
    {
        int importedCityCount = ar.writing() ? static_cast<int>(mImported.size()) : 0;
        ar.field("importedCities.count", importedCityCount);
        if(ar.reading()) {
            for(int i = 0; i < importedCityCount; i++) {
                eCityId cid;
                ar.archiveField(("importedCity." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        auto& map = mImported[cid];
                        int resourceCount = 0;
                        cityAr.field("resources.count", resourceCount);
                        for(int j = 0; j < resourceCount; j++) {
                            eResourceType r; int n;
                            cityAr.archiveField(("resource." + std::to_string(j)).c_str(),
                                [&](eSaveArchive& it) {
                                    it.field("resource", r);
                                    it.field("amount", n);
                                });
                            map[r] = n;
                        }
                    });
            }
        } else {
            int i = 0;
            for(auto& outer : mImported) {
                eCityId cid = outer.first;
                ar.archiveField(("importedCity." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& cityAr) {
                        cityAr.field("cityId", cid);
                        int resourceCount = static_cast<int>(outer.second.size());
                        cityAr.field("resources.count", resourceCount);
                        int j = 0;
                        for(auto& inner : outer.second) {
                            eResourceType r = inner.first;
                            int n = inner.second;
                            cityAr.archiveField(("resource." + std::to_string(j++)).c_str(),
                                [&](eSaveArchive& it) {
                                    it.field("resource", r);
                                    it.field("amount", n);
                                });
                        }
                    });
            }
        }
    }
}
