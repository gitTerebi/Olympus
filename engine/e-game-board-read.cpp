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
    if(ar.reading()) {
        int np;
        ar.field("mYearlyProduction.count", np);
        for(int i = 0; i < np; i++) {
            eResourceType type;
            ar.field("mYearlyProduction.type", type);
            auto& y = mYearlyProduction[type];
            ar.field("mYearlyProduction.fBest", y.fBest);
            ar.field("mYearlyProduction.fLastYear", y.fLastYear);
            ar.field("mYearlyProduction.fThisYear", y.fThisYear);
        }
        ar.field("mLastAutosaveYear", mLastAutosaveYear);
    } else {
        int np = static_cast<int>(mYearlyProduction.size());
        ar.field("mYearlyProduction.count", np);
        for(auto& p : mYearlyProduction) {
            eResourceType type = p.first;
            ar.field("mYearlyProduction.type", type);
            ar.field("mYearlyProduction.fBest", p.second.fBest);
            ar.field("mYearlyProduction.fLastYear", p.second.fLastYear);
            ar.field("mYearlyProduction.fThisYear", p.second.fThisYear);
        }
        ar.field("mLastAutosaveYear", mLastAutosaveYear);
    }
}

void eGameBoard::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eGameBoard::serializeMessageLog(eSaveArchive& ar) {
    int n = ar.writing() ? static_cast<int>(mMessageLog.size()) : 0;
    ar.field("messageLog.count", n);
    if(ar.reading()) {
        mMessageLog.clear();
        for(int i = 0; i < n; i++) {
            eLoggedMessage lm;
            ar.field("messageLog.title", lm.fMsg.fTitle);
            ar.field("messageLog.text", lm.fMsg.fText);
            lm.fDate.serialize(ar);
            ar.field("messageLog.player", lm.fEd.fPlayerName);
            ar.field("messageLog.read", lm.fRead);
            lm.fEd.fDate = lm.fDate;
            lm.fEd.fType = eMessageEventType::common;
            mMessageLog.push_back(lm);
        }
    } else {
        for(auto& lm : mMessageLog) {
            ar.field("messageLog.title", lm.fMsg.fTitle);
            ar.field("messageLog.text", lm.fMsg.fText);
            lm.fDate.serialize(ar);
            ar.field("messageLog.player", lm.fEd.fPlayerName);
            ar.field("messageLog.read", lm.fRead);
        }
    }
}

void eGameBoard::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        auto& src = ar.readStream();
    int w;
    ar.field("mWidth", w);
    int h;
    ar.field("mHeight", h);
    initialize(w, h);

    ar.field("mFogOfWar", mFogOfWar);

    ar.field("mEpisodeLost", mEpisodeLost);

    ar.field("mWageMultiplier", mWageMultiplier);

    for(auto& p : mPrices) {
        ar.field("mPrices.value", p.second);
    }

    mDate.serialize(ar);
    ar.field("mFrame", mFrame);
    ar.field("mTime", mTime);
    ar.field("mTotalTime", mTotalTime);

    ar.field("mSoldiersUpdate", mSoldiersUpdate);

    {
        int nc;
        ar.field("mCitiesOnBoard.count", nc);
        for(int i = 0; i < nc; i++) {
            eCityId cid;
            ar.field("mCitiesOnBoard.id", cid);
            const auto c = addCityToBoard(cid);
            c->read(src);
            scheduleAppealMapUpdate(cid);
        }
    }

    {
        int np;
        ar.field("mPlayersOnBoard.count", np);
        for(int i = 0; i < np; i++) {
            ePlayerId pid;
            ar.field("mPlayersOnBoard.id", pid);
            const auto p = std::make_shared<eBoardPlayer>(pid, *this);
            p->read(src);
            mPlayersOnBoard.push_back(p);
        }
    }

    for(const auto& ts : mTiles) {
        for(const auto& t : ts) {
            t->read(src);
        }
    }

    {
        int nbs;
        ar.field("mAllBuildings.count", nbs);
        for(int i = 0; i < nbs; i++) {
            eBuildingType type;
            ar.field("mAllBuildings.type", type);
            eBuildingReader::sRead(*this, type, src);
        }
    }


    {
        int ncs;
        ar.field("mCharacters.count", ncs);

        for(int i = 0; i < ncs; i++) {
            eCharacterType type;
            ar.field("mCharacters.type", type);
            const auto c = eCharacter::sCreate(type, *this);
            c->read(src);
        }
    }

    {
        int ncs;
        ar.field("mMissiles.count", ncs);

        for(int i = 0; i < ncs; i++) {
            eMissileType type;
            ar.field("mMissiles.type", type);
            const auto c = eMissile::sCreate(*this, type);
            c->read(src);
        }
    }

    int ng;
    ar.field("mGoals.count", ng);
    for(int i = 0; i < ng; i++) {
        const auto g = std::make_shared<eEpisodeGoal>();
        g->read(src);
        mGoals.push_back(g);
    }
    ar.field("mGoalsFulfilled", mGoalsFulfilled);

    ar.field("mProgressEarthquakes", mProgressEarthquakes);
    int ne;
    ar.field("mEarthquakes.count", ne);
    for(int i = 0; i < ne; i++) {
        const auto e = std::make_shared<eEarthquake>();
        e->read(src, *this);
        mEarthquakes.push_back(e);
    }

    ar.field("mProgressWaves", mProgressWaves);
    int nw;
    ar.field("mTidalWaves.count", nw);
    for(int i = 0; i < nw; i++) {
        const auto w = std::make_shared<eTidalWave>();
        w->read(src, *this);
        mTidalWaves.push_back(w);
    }

    ar.field("mProgressLavaFlows", mProgressLavaFlows);
    int nl;
    ar.field("mLavaFlows.count", nl);
    for(int i = 0; i < nl; i++) {
        const auto w = std::make_shared<eLavaFlow>();
        w->read(src, *this);
        mLavaFlows.push_back(w);
    }

    ar.field("mProgressLandSlides", mProgressLandSlides);
    int ns;
    ar.field("mLandSlides.count", ns);
    for(int i = 0; i < ns; i++) {
        const auto w = std::make_shared<eLandSlide>();
        w->read(src, *this);
        mLandSlides.push_back(w);
    }

    int nd;
    ar.field("mConqueredBy.count", nd);
    for(int i = 0; i < nd; i++) {
        eCityId cid;
        ar.field("mConqueredBy.id", cid);
        int nc;
        ar.field("mConqueredBy.cities.count", nc);
        for(int j = 0; j < nc; j++) {
            src.readCity(this, [this, cid](const stdsptr<eWorldCity>& c) {
                mConqueredBy[cid].push_back(c);
            });
        }
    }

    int npa;
    ar.field("mPlannedActions.count", npa);
    for(int i = 0; i < npa; i++) {
        ePlannedActionType type;
        ar.field("mPlannedActions.type", type);
        const auto a = ePlannedAction::sCreate(type);
        a->read(src, *this);
        mPlannedActions.push_back(a);
    }

    serializeYearlyProduction(ar);
    serializeMessageLog(ar);

    updateMarbleTiles();
    updateTerritoryBorders();
    for(const auto& c : mCitiesOnBoard) {
        c->updateResources();
    }
    src.addPostFunc([this]() {
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
    } else {
        auto& dst = ar.writeStream();
    ar.field("mWidth", mWidth);
    ar.field("mHeight", mHeight);

    ar.field("mFogOfWar", mFogOfWar);

    ar.field("mEpisodeLost", mEpisodeLost);

    ar.field("mWageMultiplier", mWageMultiplier);

    for(const auto& p : mPrices) {
        int price = p.second;
        ar.field("mPrices.value", price);
    }

    mDate.serialize(ar);
    ar.field("mFrame", mFrame);
    ar.field("mTime", mTime);
    ar.field("mTotalTime", mTotalTime);

    ar.field("mSoldiersUpdate", mSoldiersUpdate);

    {
        int id = 0;
        for(const auto b : mAllBuildings) {
            b->setIOID(id++);
        }
    }
    {
        int id = 0;
        for(const auto c : mCharacters) {
            c->setIOID(id++);
        }
    }
    {
        int id = 0;
        for(const auto ca : mCharacterActions) {
            ca->setIOID(id++);
        }
    }
    {
        int id = 0;
        for(const auto b : mBanners) {
            b->setIOID(id++);
        }
    }
    {
        int id = 0;
        for(const auto b : mAllSoldierBanners) {
            b->setIOID(id++);
        }
    }
    {
        int id = 0;
        for(const auto e : mAllGameEvents) {
            e->setIOID(id++);
        }
    }
    {
        int id = 0;
        for(const auto& c : mCitiesOnBoard) {
            c->setInvasionHandlersIOIDs(id);
        }
    }


    {
        int count = static_cast<int>(mCitiesOnBoard.size());
        ar.field("mCitiesOnBoard.count", count);
        for(const auto& c : mCitiesOnBoard) {
            eCityId id = c->id();
            ar.field("mCitiesOnBoard.id", id);
            c->write(dst);
        }
    }

    {
        int count = static_cast<int>(mPlayersOnBoard.size());
        ar.field("mPlayersOnBoard.count", count);
        for(const auto& p : mPlayersOnBoard) {
            ePlayerId id = p->id();
            ar.field("mPlayersOnBoard.id", id);
            p->write(dst);
        }
    }

    for(const auto& ts : mTiles) {
        for(const auto& t : ts) {
            t->write(dst);
        }
    }

    {
        int nbs = static_cast<int>(mAllBuildings.size());
        ar.field("mAllBuildings.count", nbs);
        for(const auto b : mAllBuildings) {
            eBuildingType type = b->type();
            ar.field("mAllBuildings.type", type);
            eBuildingWriter::sWrite(b, dst);
        }
    }

    {
        int ncs = static_cast<int>(mCharacters.size());
        ar.field("mCharacters.count", ncs);
        for(const auto c : mCharacters) {
            eCharacterType type = c->type();
            ar.field("mCharacters.type", type);
            c->write(dst);
        }
    }

    {
        int ncs = static_cast<int>(mMissiles.size());
        ar.field("mMissiles.count", ncs);
        for(const auto c : mMissiles) {
            eMissileType type = c->type();
            ar.field("mMissiles.type", type);
            c->write(dst);
        }
    }

    int goalsCount = static_cast<int>(mGoals.size());
    ar.field("mGoals.count", goalsCount);
    for(const auto& g : mGoals) {
        g->write(dst);
    }
    ar.field("mGoalsFulfilled", mGoalsFulfilled);

    ar.field("mProgressEarthquakes", mProgressEarthquakes);
    int earthquakesCount = static_cast<int>(mEarthquakes.size());
    ar.field("mEarthquakes.count", earthquakesCount);
    for(const auto& e : mEarthquakes) {
        e->write(dst);
    }

    ar.field("mProgressWaves", mProgressWaves);
    int tidalWavesCount = static_cast<int>(mTidalWaves.size());
    ar.field("mTidalWaves.count", tidalWavesCount);
    for(const auto& w : mTidalWaves) {
        w->write(dst);
    }

    ar.field("mProgressLavaFlows", mProgressLavaFlows);
    int lavaFlowsCount = static_cast<int>(mLavaFlows.size());
    ar.field("mLavaFlows.count", lavaFlowsCount);
    for(const auto& w : mLavaFlows) {
        w->write(dst);
    }

    ar.field("mProgressLandSlides", mProgressLandSlides);
    int landSlidesCount = static_cast<int>(mLandSlides.size());
    ar.field("mLandSlides.count", landSlidesCount);
    for(const auto& w : mLandSlides) {
        w->write(dst);
    }

    int conqueredByCount = static_cast<int>(mConqueredBy.size());
    ar.field("mConqueredBy.count", conqueredByCount);
    for(const auto& p : mConqueredBy) {
        eCityId cid = p.first;
        ar.field("mConqueredBy.id", cid);
        int nc = static_cast<int>(p.second.size());
        ar.field("mConqueredBy.cities.count", nc);
        for(const auto& c : p.second) {
            dst.writeCity(c.get());
        }
    }

    int plannedActionsCount = static_cast<int>(mPlannedActions.size());
    ar.field("mPlannedActions.count", plannedActionsCount);
    for(const auto a : mPlannedActions) {
        ePlannedActionType type = a->type();
        ar.field("mPlannedActions.type", type);
        a->write(dst);
    }

    serializeYearlyProduction(ar);
    serializeMessageLog(ar);
    }
}
