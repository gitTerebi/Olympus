#include "egameboard.h"

#include "spawners/ebanner.h"
#include "fileIO/ebuildingreader.h"
#include "fileIO/ebuildingwriter.h"
#include "einvasionhandler.h"
#include "missiles/emissile.h"
#include "gameEvents/egameevent.h"
#include "gameEvents/ereceiverequestevent.h"
#include "gameEvents/etroopsrequestevent.h"
#include "eplague.h"
#include "fileIO/esavearchive.h"

void eGameBoard::serializeYearlyProduction(eSaveArchive& ar) {
    if(ar.reading()) {
        int np;
        ar.field("np", np);
        for(int i = 0; i < np; i++) {
            eResourceType type;
            ar.field("type", type);
            auto& y = mYearlyProduction[type];
            ar.field("y.fBest", y.fBest);
            ar.field("y.fLastYear", y.fLastYear);
            ar.field("y.fThisYear", y.fThisYear);
        }
        ar.field("mSavedYear", mSavedYear);
    } else {
        int np = static_cast<int>(mYearlyProduction.size());
        ar.field("np", np);
        for(auto& p : mYearlyProduction) {
            eResourceType type = p.first;
            ar.field("type", type);
            ar.field("p.second.fBest", p.second.fBest);
            ar.field("p.second.fLastYear", p.second.fLastYear);
            ar.field("p.second.fThisYear", p.second.fThisYear);
        }
        ar.field("mSavedYear", mSavedYear);
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
    src >> w;
    int h;
    src >> h;
    initialize(w, h);

    src >> mFogOfWar;

    src >> mEpisodeLost;

    src >> mWageMultiplier;

    for(auto& p : mPrices) {
        src >> p.second;
    }

    mDate.read(src);
    src >> mFrame;
    src >> mTime;
    src >> mTotalTime;

    src >> mSoldiersUpdate;

    {
        int nc;
        src >> nc;
        for(int i = 0; i < nc; i++) {
            eCityId cid;
            src >> cid;
            const auto c = addCityToBoard(cid);
            c->read(src);
            scheduleAppealMapUpdate(cid);
        }
    }

    {
        int np;
        src >> np;
        for(int i = 0; i < np; i++) {
            ePlayerId pid;
            src >> pid;
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
        src >> nbs;
        for(int i = 0; i < nbs; i++) {
            eBuildingType type;
            src >> type;
            eBuildingReader::sRead(*this, type, src);
        }
    }


    {
        int ncs;
        src >> ncs;

        for(int i = 0; i < ncs; i++) {
            eCharacterType type;
            src >> type;
            const auto c = eCharacter::sCreate(type, *this);
            c->read(src);
        }
    }

    {
        int ncs;
        src >> ncs;

        for(int i = 0; i < ncs; i++) {
            eMissileType type;
            src >> type;
            const auto c = eMissile::sCreate(*this, type);
            c->read(src);
        }
    }

    int ng;
    src >> ng;
    for(int i = 0; i < ng; i++) {
        const auto g = std::make_shared<eEpisodeGoal>();
        g->read(src);
        mGoals.push_back(g);
    }
    src >> mGoalsFulfilled;

    src >> mProgressEarthquakes;
    int ne;
    src >> ne;
    for(int i = 0; i < ne; i++) {
        const auto e = std::make_shared<eEarthquake>();
        e->read(src, *this);
        mEarthquakes.push_back(e);
    }

    src >> mProgressWaves;
    int nw;
    src >> nw;
    for(int i = 0; i < nw; i++) {
        const auto w = std::make_shared<eTidalWave>();
        w->read(src, *this);
        mTidalWaves.push_back(w);
    }

    src >> mProgressLavaFlows;
    int nl;
    src >> nl;
    for(int i = 0; i < nl; i++) {
        const auto w = std::make_shared<eLavaFlow>();
        w->read(src, *this);
        mLavaFlows.push_back(w);
    }

    src >> mProgressLandSlides;
    int ns;
    src >> ns;
    for(int i = 0; i < ns; i++) {
        const auto w = std::make_shared<eLandSlide>();
        w->read(src, *this);
        mLandSlides.push_back(w);
    }

    int nd;
    src >> nd;
    for(int i = 0; i < nd; i++) {
        eCityId cid;
        src >> cid;
        int nc;
        src >> nc;
        for(int j = 0; j < nc; j++) {
            src.readCity(this, [this, cid](const stdsptr<eWorldCity>& c) {
                mDefeatedBy[cid].push_back(c);
            });
        }
    }

    int npa;
    src >> npa;
    for(int i = 0; i < npa; i++) {
        ePlannedActionType type;
        src >> type;
        const auto a = ePlannedAction::sCreate(type);
        a->read(src, *this);
        mPlannedActions.push_back(a);
    }

    eSaveArchive ar(src);
    serializeYearlyProduction(ar);
    serializeMessageLog(ar);

    updateMarbleTiles();
    updateTerritoryBorders();
    for(const auto& c : mCitiesOnBoard) {
        c->updateResources();
    }
    src.addPostFunc([this]() {
        for(const auto e : mAllGameEvents) {
            const auto request = dynamic_cast<eReceiveRequestEvent*>(e);
            if(request && request->isActiveCityRequest()) {
                addCityRequest(request);
            }
        }
    }, "cityRequests");
    } else {
        auto& dst = ar.writeStream();
    dst << mWidth;
    dst << mHeight;

    dst << mFogOfWar;

    dst << mEpisodeLost;

    dst << mWageMultiplier;

    for(const auto& p : mPrices) {
        dst << p.second;
    }

    mDate.write(dst);
    dst << mFrame;
    dst << mTime;
    dst << mTotalTime;

    dst << mSoldiersUpdate;

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
        dst << mCitiesOnBoard.size();
        for(const auto& c : mCitiesOnBoard) {
            dst << c->id();
            c->write(dst);
        }
    }

    {
        dst << mPlayersOnBoard.size();
        for(const auto& p : mPlayersOnBoard) {
            dst << p->id();
            p->write(dst);
        }
    }

    for(const auto& ts : mTiles) {
        for(const auto& t : ts) {
            t->write(dst);
        }
    }

    {
        const int nbs = mAllBuildings.size();
        dst << nbs;
        for(const auto b : mAllBuildings) {
            dst << b->type();
            eBuildingWriter::sWrite(b, dst);
        }
    }

    {
        const int ncs = mCharacters.size();
        dst << ncs;
        for(const auto c : mCharacters) {
            dst << c->type();
            c->write(dst);
        }
    }

    {
        const int ncs = mMissiles.size();
        dst << ncs;
        for(const auto c : mMissiles) {
            dst << c->type();
            c->write(dst);
        }
    }

    dst << mGoals.size();
    for(const auto& g : mGoals) {
        g->write(dst);
    }
    dst << mGoalsFulfilled;

    dst << mProgressEarthquakes;
    dst << mEarthquakes.size();
    for(const auto& e : mEarthquakes) {
        e->write(dst);
    }

    dst << mProgressWaves;
    dst << mTidalWaves.size();
    for(const auto& w : mTidalWaves) {
        w->write(dst);
    }

    dst << mProgressLavaFlows;
    dst << mLavaFlows.size();
    for(const auto& w : mLavaFlows) {
        w->write(dst);
    }

    dst << mProgressLandSlides;
    dst << mLandSlides.size();
    for(const auto& w : mLandSlides) {
        w->write(dst);
    }

    dst << mDefeatedBy.size();
    for(const auto& c : mDefeatedBy) {
        dst << c.first;
        dst << c.second.size();
        for(const auto& cc : c.second) {
            dst.writeCity(cc.get());
        }
    }

    dst << mPlannedActions.size();
    for(const auto a : mPlannedActions) {
        dst << a->type();
        a->write(dst);
    }

    eSaveArchive ar(dst);
    const_cast<eGameBoard*>(this)->serializeYearlyProduction(ar);
    const_cast<eGameBoard*>(this)->serializeMessageLog(ar);
    }
}
