#include "e-game-board.h"

#include "spawners/ebanner.h"
#include "fileIO/ebuildingreader.h"
#include "fileIO/ebuildingwriter.h"
#include "gameEvents/invasions/invasion-handler.h"
#include "missiles/emissile.h"
#include "gameEvents/egameevent.h"
#include "gameEvents/requests/e-pay-tribute-event.h"
#include "gameEvents/requests/e-fulfill-request-event.h"
#include "gameEvents/etroopsrequestevent.h"
#include "eplague.h"
#include "fileIO/esavearchive.h"

void eGameBoard::serializeYearlyProduction(eSaveArchive& ar) {
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

void eGameBoard::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eGameBoard::serializeMessageLog(eSaveArchive& ar) {
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

void eGameBoard::serialize(eSaveArchive& ar) {
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
                        it.payloadField("city",
                            [](eWriteStream&) {},
                            [c](eReadStream& src) { c->read(src); });
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
                        it.payloadField("city",
                            [&c](eWriteStream& dst) { c->write(dst); },
                            [](eReadStream&) {});
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
                        it.payloadField("player",
                            [](eWriteStream&) {},
                            [p](eReadStream& src) { p->read(src); });
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
                        it.payloadField("player",
                            [&p](eWriteStream& dst) { p->write(dst); },
                            [](eReadStream&) {});
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
                ar.payloadField(
                    ("tile." + std::to_string(i) + "." + std::to_string(j)).c_str(),
                    [&t](eWriteStream& dst) { t->write(dst); },
                    [&t](eReadStream& src) { t->read(src); });
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
                        eBuildingArchive::load(*this, type, it);
                    });
            }
        } else {
            int i = 0;
            for(const auto b : mAllBuildings) {
                ar.archiveField(("building." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        eBuildingType type = b->type();
                        it.field("buildingType", type);
                        eBuildingArchive::save(b, it);
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
                        it.payloadField("missileData",
                            [](eWriteStream&) {},
                            [c](eReadStream& src) { c->read(src); });
                    });
            }
        } else {
            int i = 0;
            for(const auto c : mMissiles) {
                ar.archiveField(("missile." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        eMissileType type = c->type();
                        it.field("missileType", type);
                        it.payloadField("missileData",
                            [c](eWriteStream& dst) { c->write(dst); },
                            [](eReadStream&) {});
                    });
            }
        }
    }

    // goals
    {
        int goalCount = ar.writing() ? static_cast<int>(mGoals.size()) : 0;
        ar.field("goals.count", goalCount);
        if(ar.reading()) {
            for(int i = 0; i < goalCount; i++) {
                const auto g = std::make_shared<eEpisodeGoal>();
                ar.payloadField(("goal." + std::to_string(i)).c_str(),
                    [](eWriteStream&) {},
                    [g](eReadStream& src) { g->read(src); });
                mGoals.push_back(g);
            }
        } else {
            int i = 0;
            for(const auto& g : mGoals) {
                ar.payloadField(("goal." + std::to_string(i++)).c_str(),
                    [&g](eWriteStream& dst) { g->write(dst); },
                    [](eReadStream&) {});
            }
        }
    }
    ar.field("goalsFulfilled", mGoalsFulfilled);

    ar.field("progressEarthquakes", mProgressEarthquakes);
    {
        int earthquakeCount = ar.writing() ? static_cast<int>(mEarthquakes.size()) : 0;
        ar.field("earthquakes.count", earthquakeCount);
        if(ar.reading()) {
            for(int i = 0; i < earthquakeCount; i++) {
                const auto e = std::make_shared<eEarthquake>();
                ar.payloadField(("earthquake." + std::to_string(i)).c_str(),
                    [](eWriteStream&) {},
                    [this, e](eReadStream& src) { e->read(src, *this); });
                mEarthquakes.push_back(e);
            }
        } else {
            int i = 0;
            for(const auto& e : mEarthquakes) {
                ar.payloadField(("earthquake." + std::to_string(i++)).c_str(),
                    [&e](eWriteStream& dst) { e->write(dst); },
                    [](eReadStream&) {});
            }
        }
    }

    ar.field("progressWaves", mProgressWaves);
    {
        int tidalWaveCount = ar.writing() ? static_cast<int>(mTidalWaves.size()) : 0;
        ar.field("tidalWaves.count", tidalWaveCount);
        if(ar.reading()) {
            for(int i = 0; i < tidalWaveCount; i++) {
                const auto w = std::make_shared<eTidalWave>();
                ar.payloadField(("tidalWave." + std::to_string(i)).c_str(),
                    [](eWriteStream&) {},
                    [this, w](eReadStream& src) { w->read(src, *this); });
                mTidalWaves.push_back(w);
            }
        } else {
            int i = 0;
            for(const auto& w : mTidalWaves) {
                ar.payloadField(("tidalWave." + std::to_string(i++)).c_str(),
                    [&w](eWriteStream& dst) { w->write(dst); },
                    [](eReadStream&) {});
            }
        }
    }

    ar.field("progressLavaFlows", mProgressLavaFlows);
    {
        int lavaFlowCount = ar.writing() ? static_cast<int>(mLavaFlows.size()) : 0;
        ar.field("lavaFlows.count", lavaFlowCount);
        if(ar.reading()) {
            for(int i = 0; i < lavaFlowCount; i++) {
                const auto w = std::make_shared<eLavaFlow>();
                ar.payloadField(("lavaFlow." + std::to_string(i)).c_str(),
                    [](eWriteStream&) {},
                    [this, w](eReadStream& src) { w->read(src, *this); });
                mLavaFlows.push_back(w);
            }
        } else {
            int i = 0;
            for(const auto& w : mLavaFlows) {
                ar.payloadField(("lavaFlow." + std::to_string(i++)).c_str(),
                    [&w](eWriteStream& dst) { w->write(dst); },
                    [](eReadStream&) {});
            }
        }
    }

    ar.field("progressLandSlides", mProgressLandSlides);
    {
        int landSlideCount = ar.writing() ? static_cast<int>(mLandSlides.size()) : 0;
        ar.field("landSlides.count", landSlideCount);
        if(ar.reading()) {
            for(int i = 0; i < landSlideCount; i++) {
                const auto w = std::make_shared<eLandSlide>();
                ar.payloadField(("landSlide." + std::to_string(i)).c_str(),
                    [](eWriteStream&) {},
                    [this, w](eReadStream& src) { w->read(src, *this); });
                mLandSlides.push_back(w);
            }
        } else {
            int i = 0;
            for(const auto& w : mLandSlides) {
                ar.payloadField(("landSlide." + std::to_string(i++)).c_str(),
                    [&w](eWriteStream& dst) { w->write(dst); },
                    [](eReadStream&) {});
            }
        }
    }

    // conqueredBy map<eCityId, vector<stdsptr<eWorldCity>>>
    {
        int conqueredByCount = ar.writing() ? static_cast<int>(mConqueredBy.size()) : 0;
        ar.field("conqueredBy.count", conqueredByCount);
        if(ar.reading()) {
            for(int i = 0; i < conqueredByCount; i++) {
                eCityId cid;
                ar.archiveField(("conqueredBy." + std::to_string(i)).c_str(),
                    [&](eSaveArchive& it) {
                        it.field("cityId", cid);
                        int cityRefCount = 0;
                        it.field("cities.count", cityRefCount);
                        for(int j = 0; j < cityRefCount; j++) {
                            it.payloadField(("city." + std::to_string(j)).c_str(),
                                [](eWriteStream&) {},
                                [this, cid](eReadStream& src) {
                                    src.readCity(this, [this, cid](const stdsptr<eWorldCity>& c) {
                                        mConqueredBy[cid].push_back(c);
                                    });
                                });
                        }
                    });
            }
        } else {
            int i = 0;
            for(auto& p : mConqueredBy) {
                eCityId cid = p.first;
                auto& cities = p.second;
                ar.archiveField(("conqueredBy." + std::to_string(i++)).c_str(),
                    [&](eSaveArchive& it) {
                        it.field("cityId", cid);
                        int cityRefCount = static_cast<int>(cities.size());
                        it.field("cities.count", cityRefCount);
                        for(int j = 0; j < cityRefCount; j++) {
                            auto& c = cities[j];
                            it.payloadField(("city." + std::to_string(j)).c_str(),
                                [&c](eWriteStream& dst) { dst.writeCity(c.get()); },
                                [](eReadStream&) {});
                        }
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
        ar.readStream().addPostFunc([this]() {
            for(const auto e : mAllGameEvents) {
                const auto request = dynamic_cast<eFulfillRequestEvent*>(e);
                if(request && request->isMainEvent() &&
                   request->isActiveCityRequest()) {
                    addCityRequest(request);
                }
                const auto tribute = dynamic_cast<ePayTributeEvent*>(e);
                if(tribute && tribute->isMainEvent() &&
                   !tribute->finished()) {
                    addTributeRequest(tribute);
                }
            }
        }, "requests");
    }
}
