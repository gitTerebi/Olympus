#include "game-board.h"

#include "spawners/ebanner.h"
#include "fileIO/building-reader.h"
#include "fileIO/building-writer.h"
#include "gameEvents/invasions/invasion-handler.h"
#include "missiles/emissile.h"
#include "gameEvents/egameevent.h"
#include "gameEvents/requests/e-pay-tribute-event.h"
#include "gameEvents/requests/e-fulfill-request-event.h"
#include "gameEvents/etroopsrequestevent.h"
#include "eplague.h"
#include "fileIO/esavearchive.h"

void GameBoard::serializeYearlyProduction(eSaveArchive& ar) {
    int productionCount = ar.writing() ? static_cast<int>(mYearlyProduction.size()) : 0;
    ar.field("yearlyProduction.count", productionCount);
    if(ar.reading()) {
        for(int i = 0; i < productionCount; i++) {
            eResourceType type;
            ar.archiveField(("yearlyProduction." + std::to_string(i)).c_str(),
                [&](eSaveArchive& it) {
                    it.field("resource", type);
                    auto& y = mYearlyProduction[type];
                    it.field("best", y.fBest);
                    it.field("lastYear", y.fLastYear);
                    it.field("thisYear", y.fThisYear);
                });
        }
    } else {
        int i = 0;
        for(auto& p : mYearlyProduction) {
            eResourceType type = p.first;
            ar.archiveField(("yearlyProduction." + std::to_string(i++)).c_str(),
                [&](eSaveArchive& it) {
                    it.field("resource", type);
                    it.field("best", p.second.fBest);
                    it.field("lastYear", p.second.fLastYear);
                    it.field("thisYear", p.second.fThisYear);
                });
        }
    }
    ar.field("lastAutosaveYear", mLastAutosaveYear);
}

void GameBoard::serializeMessageLog(eSaveArchive& ar) {
    int messageCount = ar.writing() ? static_cast<int>(mMessageLog.size()) : 0;
    ar.field("messageLog.count", messageCount);
    if(ar.reading()) {
        mMessageLog.clear();
        for(int i = 0; i < messageCount; i++) {
            eLoggedMessage lm;
            ar.archiveField(("message." + std::to_string(i)).c_str(),
                [&](eSaveArchive& it) {
                    it.field("title", lm.fMsg.fTitle);
                    it.field("text", lm.fMsg.fText);
                    it.archiveField("date", [&](eSaveArchive& dAr) { lm.fDate.serialize(dAr); });
                    it.field("playerName", lm.fEd.fPlayerName);
                    it.field("read", lm.fRead);
                });
            lm.fEd.fDate = lm.fDate;
            lm.fEd.fType = eMessageEventType::common;
            mMessageLog.push_back(lm);
        }
    } else {
        int i = 0;
        for(auto& lm : mMessageLog) {
            ar.archiveField(("message." + std::to_string(i++)).c_str(),
                [&](eSaveArchive& it) {
                    it.field("title", lm.fMsg.fTitle);
                    it.field("text", lm.fMsg.fText);
                    it.archiveField("date", [&](eSaveArchive& dAr) { lm.fDate.serialize(dAr); });
                    it.field("playerName", lm.fEd.fPlayerName);
                    it.field("read", lm.fRead);
                });
        }
    }
}

void GameBoard::serialize(eSaveArchive& ar) {
    // assign ids before write
    if(ar.writing()) {
        int id = 0;
        for(const auto b : mAllBuildings) b->setIOID(id++);
        id = 0;
        for(const auto c : mCharacters) c->setIOID(id++);
        id = 0;
        for(const auto ca : mCharacterActions) ca->setIOID(id++);
        id = 0;
        for(const auto b : mBanners) b->setIOID(id++);
        id = 0;
        for(const auto b : mAllSoldierBanners) b->setIOID(id++);
        id = 0;
        for(const auto e : mAllGameEvents) e->setIOID(id++);
        id = 0;
        for(const auto& c : mCitiesOnBoard) c->setInvasionHandlersIOIDs(id);
    }

    int width = ar.writing() ? mWidth : 0;
    int height = ar.writing() ? mHeight : 0;
    ar.field("width", width);
    ar.field("height", height);
    if(ar.reading()) initialize(width, height);

    ar.field("fogOfWar", mFogOfWar);
    ar.field("episodeLost", mEpisodeLost);
    ar.field("wageMultiplier", mWageMultiplier);

    // prices map<eResourceType, int>
    {
        int i = 0;
        for(auto& p : mPrices) {
            ar.field(("price." + std::to_string(static_cast<int>(p.first))).c_str(),
                     p.second);
            i++;
        }
    }

    ar.archiveField("date", [this](eSaveArchive& it) { mDate.serialize(it); });
    ar.field("frame", mFrame);
    ar.field("time", mTime);
    ar.field("totalTime", mTotalTime);
    ar.field("soldiersUpdate", mSoldiersUpdate);

    // citiesOnBoard
    {
        int cityCount = ar.writing() ? static_cast<int>(mCitiesOnBoard.size()) : 0;
        ar.field("citiesOnBoard.count", cityCount);
        if(ar.reading()) {
            for(int i = 0; i < cityCount; i++) {
                eCityId cid;
                ar.archiveField(("cityOnBoard." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& it) {
                        it.field("cityId", cid);
                        const auto c = addCityToBoard(cid);
                        it.archiveField("city",
                            [&c](eSaveArchive& cAr) { c->serialize(cAr); });
                    });
                scheduleAppealMapUpdate(cid);
            }
        } else {
            int i = 0;
            for(const auto& c : mCitiesOnBoard) {
                ar.archiveField(("cityOnBoard." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        eCityId cid = c->id();
                        it.field("cityId", cid);
                        it.archiveField("city",
                            [&c](eSaveArchive& cAr) { c->serialize(cAr); });
                    });
            }
        }
    }

    // playersOnBoard
    {
        int playerCount = ar.writing() ? static_cast<int>(mPlayersOnBoard.size()) : 0;
        ar.field("playersOnBoard.count", playerCount);
        if(ar.reading()) {
            for(int i = 0; i < playerCount; i++) {
                ePlayerId pid;
                ar.archiveField(("playerOnBoard." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& it) {
                        it.field("playerId", pid);
                        const auto p = std::make_shared<eBoardPlayer>(pid, *this);
                        it.archiveField("player",
                            [&p](eSaveArchive& playerAr) { p->serialize(playerAr); });
                        mPlayersOnBoard.push_back(p);
                    });
            }
        } else {
            int i = 0;
            for(const auto& p : mPlayersOnBoard) {
                ar.archiveField(("playerOnBoard." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        ePlayerId pid = p->id();
                        it.field("playerId", pid);
                        it.archiveField("player",
                            [&p](eSaveArchive& playerAr) { p->serialize(playerAr); });
                    });
            }
        }
    }

    // tiles (fixed dim, no count needed)
    {
        int i = 0;
        for(const auto& ts : mTiles) {
            int j = 0;
            for(const auto& t : ts) {
                ar.archiveField(
                    ("tile." + std::to_string(i) + "." + std::to_string(j)).c_str(),
                    [&t](eSaveArchive& tAr) {
                        t->eTileBase::serialize(tAr);
                        t->serialize(tAr);
                    });
                j++;
            }
            i++;
        }
    }

    // buildings
    {
        int buildingCount = ar.writing() ? static_cast<int>(mAllBuildings.size()) : 0;
        ar.field("buildings.count", buildingCount);
        if(ar.reading()) {
            for(int i = 0; i < buildingCount; i++) {
                eBuildingType type;
                ar.archiveField(("building." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& it) {
                        it.field("buildingType", type);
                        BuildingArchive::load(*this, type, it);
                    });
            }
        } else {
            int i = 0;
            for(const auto b : mAllBuildings) {
                ar.archiveField(("building." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        eBuildingType type = b->type();
                        it.field("buildingType", type);
                        BuildingArchive::save(b, it);
                    });
            }
        }
    }

    // characters
    {
        int characterCount = ar.writing() ? static_cast<int>(mCharacters.size()) : 0;
        ar.field("characters.count", characterCount);
        if(ar.reading()) {
            for(int i = 0; i < characterCount; i++) {
                eCharacterType type;
                ar.archiveField(("character." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& it) {
                        it.field("characterType", type);
                        const auto c = eCharacter::sCreate(type, *this);
                        it.archiveField("state",
                            [c](eSaveArchive& charAr) { c->serialize(charAr); });
                    });
            }
        } else {
            int i = 0;
            for(const auto c : mCharacters) {
                ar.archiveField(("character." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        eCharacterType type = c->type();
                        it.field("characterType", type);
                        it.archiveField("state",
                            [c](eSaveArchive& charAr) { c->serialize(charAr); });
                    });
            }
        }
    }

    // missiles
    {
        int missileCount = ar.writing() ? static_cast<int>(mMissiles.size()) : 0;
        ar.field("missiles.count", missileCount);
        if(ar.reading()) {
            for(int i = 0; i < missileCount; i++) {
                eMissileType type;
                ar.archiveField(("missile." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& it) {
                        it.field("missileType", type);
                        const auto c = eMissile::sCreate(*this, type);
                        it.archiveField("missileData",
                            [c](eSaveArchive& childAr) { c->serialize(childAr); });
                    });
            }
        } else {
            int i = 0;
            for(const auto c : mMissiles) {
                ar.archiveField(("missile." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        eMissileType type = c->type();
                        it.field("missileType", type);
                        it.archiveField("missileData",
                            [c](eSaveArchive& childAr) { c->serialize(childAr); });
                    });
            }
        }
    }

    // goals
    ar.arrayField("goals", mGoals,
        [](eSaveArchive& itemAr, stdsptr<eEpisodeGoal>& g) {
            if(itemAr.reading() && !g) g = std::make_shared<eEpisodeGoal>();
            g->serialize(itemAr);
        });
    ar.field("goalsFulfilled", mGoalsFulfilled);

    ar.field("progressEarthquakes", mProgressEarthquakes);
    ar.arrayField("earthquakes", mEarthquakes,
        [this](eSaveArchive& itemAr, stdsptr<eEarthquake>& e) {
            if(itemAr.reading() && !e) e = std::make_shared<eEarthquake>();
            e->serialize(itemAr, *this);
        });

    ar.field("progressWaves", mProgressWaves);
    ar.arrayField("tidalWaves", mTidalWaves,
        [this](eSaveArchive& itemAr, stdsptr<eTidalWave>& w) {
            if(itemAr.reading() && !w) w = std::make_shared<eTidalWave>();
            w->serialize(itemAr, *this);
        });

    ar.field("progressLavaFlows", mProgressLavaFlows);
    ar.arrayField("lavaFlows", mLavaFlows,
        [this](eSaveArchive& itemAr, stdsptr<eLavaFlow>& w) {
            if(itemAr.reading() && !w) w = std::make_shared<eLavaFlow>();
            w->serialize(itemAr, *this);
        });

    ar.field("progressLandSlides", mProgressLandSlides);
    ar.arrayField("landSlides", mLandSlides,
        [this](eSaveArchive& itemAr, stdsptr<eLandSlide>& w) {
            if(itemAr.reading() && !w) w = std::make_shared<eLandSlide>();
            w->serialize(itemAr, *this);
        });

    // conqueredBy map<eCityId, vector<stdsptr<eWorldCity>>>
    {
        int conqueredByCount = ar.writing() ? static_cast<int>(mConqueredBy.size()) : 0;
        ar.field("conqueredBy.count", conqueredByCount);
        if(ar.reading()) {
            for(int i = 0; i < conqueredByCount; i++) {
                ar.archiveField(("conqueredBy." + std::to_string(i)).c_str(),
                    [this](eSaveArchive& it) {
                        eCityId cid;
                        it.field("cityId", cid);
                        auto& cities = mConqueredBy[cid];
                        it.arrayField("cities", cities,
                            [this](eSaveArchive& itemAr, stdsptr<eWorldCity>& c) {
                                itemAr.worldCityField("city", this, c);
                            });
                    });
            }
        } else {
            int i = 0;
            for(auto& p : mConqueredBy) {
                eCityId cid = p.first;
                auto& cities = p.second;
                ar.archiveField(("conqueredBy." + std::to_string(i++)).c_str(),
                    [this, &cid, &cities](eSaveArchive& it) {
                        it.field("cityId", cid);
                        it.arrayField("cities", cities,
                            [this](eSaveArchive& itemAr, stdsptr<eWorldCity>& c) {
                                itemAr.worldCityField("city", this, c);
                            });
                    });
            }
        }
    }

    // plannedActions
    {
        int plannedActionCount = ar.writing() ? static_cast<int>(mPlannedActions.size()) : 0;
        ar.field("plannedActions.count", plannedActionCount);
        if(ar.reading()) {
            for(int i = 0; i < plannedActionCount; i++) {
                ePlannedActionType type;
                ar.archiveField(("plannedAction." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& it) {
                        it.field("actionType", type);
                        const auto a = ePlannedAction::sCreate(type);
                        a->serialize(it, this);
                        mPlannedActions.push_back(a);
                    });
            }
        } else {
            int i = 0;
            for(const auto a : mPlannedActions) {
                ar.archiveField(("plannedAction." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        ePlannedActionType type = a->type();
                        it.field("actionType", type);
                        a->serialize(it, this);
                    });
            }
        }
    }

    serializeYearlyProduction(ar);
    serializeMessageLog(ar);

    if(ar.reading()) {
        updateMarbleTiles();
        updateTerritoryBorders();
        for(const auto& c : mCitiesOnBoard) {
            c->updateResources();
        }
        ar.addPostFunc([this]() {
            for(const auto e : mAllGameEvents) {
                const auto request = dynamic_cast<eFulfillRequestEvent*>(e);
                if(request && request->isMainEvent() &&
                   request->isActiveCityRequest()) {
                    request->advanceIfNeeded(date());
                    if(request->isActiveCityRequest()) {
                        addCityRequest(request);
                    }
                }
                const auto tribute = dynamic_cast<ePayTributeEvent*>(e);
                if(tribute && tribute->isMainEvent() &&
                   !tribute->finished()) {
                    tribute->advanceIfNeeded(date());
                    if(!tribute->finished()) {
                        addTributeRequest(tribute);
                    }
                }
            }
        }, "requests");
    }
}
