#include "egameboard.h"

#include "spawners/ebanner.h"
#include "fileIO/ebuildingreader.h"
#include "einvasionhandler.h"
#include "missiles/emissile.h"
#include "gameEvents/egameevent.h"
#include "gameEvents/ereceiverequestevent.h"
#include "gameEvents/etroopsrequestevent.h"
#include "eplague.h"
#include "fileIO/esavearchive.h"
#include "fileIO/efileformat.h"

#include <fstream>

static void loadDebugLogBoard(const std::string& msg) {
    std::ofstream log("load-debug.log", std::ios::app);
    log << msg << '\n';
    log.flush();
}

void eGameBoard::serializeYearlyProduction(eSaveArchive& ar) {
    if(!ar.versionAtLeast(eFileFormat::cartTarget)) return;

    if(ar.reading()) {
        int np;
        ar.value(np);
        loadDebugLogBoard("eGameBoard::read yearlyProduction count=" +
                          std::to_string(np));
        for(int i = 0; i < np; i++) {
            eResourceType type;
            ar.value(type);
            auto& y = mYearlyProduction[type];
            ar.value(y.fBest);
            ar.value(y.fLastYear);
            ar.value(y.fThisYear);
        }
        ar.value(mSavedYear);
        loadDebugLogBoard("eGameBoard::read savedYear=" +
                          std::to_string(mSavedYear));
    } else {
        int np = static_cast<int>(mYearlyProduction.size());
        ar.value(np);
        for(auto& p : mYearlyProduction) {
            eResourceType type = p.first;
            ar.value(type);
            ar.value(p.second.fBest);
            ar.value(p.second.fLastYear);
            ar.value(p.second.fThisYear);
        }
        ar.value(mSavedYear);
    }
}

void eGameBoard::read(eReadStream& src) {
    loadDebugLogBoard("eGameBoard::read begin");
    int w;
    src >> w;
    int h;
    src >> h;
    loadDebugLogBoard("eGameBoard::read size " + std::to_string(w) +
                      "x" + std::to_string(h));
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
        loadDebugLogBoard("eGameBoard::read cities count=" +
                          std::to_string(nc));
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
        loadDebugLogBoard("eGameBoard::read players count=" +
                          std::to_string(np));
        for(int i = 0; i < np; i++) {
            ePlayerId pid;
            src >> pid;
            const auto p = std::make_shared<eBoardPlayer>(pid, *this);
            p->read(src);
            mPlayersOnBoard.push_back(p);
        }
    }

    loadDebugLogBoard("eGameBoard::read tiles begin");
    for(const auto& ts : mTiles) {
        for(const auto& t : ts) {
            t->read(src);
        }
    }
    loadDebugLogBoard("eGameBoard::read tiles done");

    {
        int nbs;
        src >> nbs;
        loadDebugLogBoard("eGameBoard::read buildings count=" +
                          std::to_string(nbs));
        for(int i = 0; i < nbs; i++) {
            eBuildingType type;
            src >> type;
            loadDebugLogBoard("eGameBoard::read building " +
                              std::to_string(i) + "/" +
                              std::to_string(nbs) + " type=" +
                              std::to_string(static_cast<int>(type)));
            eBuildingReader::sRead(*this, type, src);
        }
    }


    {
        int ncs;
        src >> ncs;
        loadDebugLogBoard("eGameBoard::read characters count=" +
                          std::to_string(ncs));

        for(int i = 0; i < ncs; i++) {
            eCharacterType type;
            src >> type;
            loadDebugLogBoard("eGameBoard::read character " +
                              std::to_string(i) + "/" +
                              std::to_string(ncs) + " type=" +
                              std::to_string(static_cast<int>(type)));
            const auto c = eCharacter::sCreate(type, *this);
            c->read(src);
        }
    }

    {
        int ncs;
        src >> ncs;
        loadDebugLogBoard("eGameBoard::read missiles count=" +
                          std::to_string(ncs));

        for(int i = 0; i < ncs; i++) {
            eMissileType type;
            src >> type;
            const auto c = eMissile::sCreate(*this, type);
            c->read(src);
        }
    }

    int ng;
    src >> ng;
    loadDebugLogBoard("eGameBoard::read goals count=" + std::to_string(ng));
    for(int i = 0; i < ng; i++) {
        const auto g = std::make_shared<eEpisodeGoal>();
        g->read(src);
        mGoals.push_back(g);
    }
    src >> mGoalsFulfilled;

    src >> mProgressEarthquakes;
    int ne;
    src >> ne;
    loadDebugLogBoard("eGameBoard::read earthquakes count=" +
                      std::to_string(ne));
    for(int i = 0; i < ne; i++) {
        const auto e = std::make_shared<eEarthquake>();
        e->read(src, *this);
        mEarthquakes.push_back(e);
    }

    src >> mProgressWaves;
    int nw;
    src >> nw;
    loadDebugLogBoard("eGameBoard::read tidalWaves count=" +
                      std::to_string(nw));
    for(int i = 0; i < nw; i++) {
        const auto w = std::make_shared<eTidalWave>();
        w->read(src, *this);
        mTidalWaves.push_back(w);
    }

    src >> mProgressLavaFlows;
    int nl;
    src >> nl;
    loadDebugLogBoard("eGameBoard::read lavaFlows count=" +
                      std::to_string(nl));
    for(int i = 0; i < nl; i++) {
        const auto w = std::make_shared<eLavaFlow>();
        w->read(src, *this);
        mLavaFlows.push_back(w);
    }

    src >> mProgressLandSlides;
    int ns;
    src >> ns;
    loadDebugLogBoard("eGameBoard::read landSlides count=" +
                      std::to_string(ns));
    for(int i = 0; i < ns; i++) {
        const auto w = std::make_shared<eLandSlide>();
        w->read(src, *this);
        mLandSlides.push_back(w);
    }

    int nd;
    src >> nd;
    loadDebugLogBoard("eGameBoard::read defeatedBy count=" +
                      std::to_string(nd));
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
    loadDebugLogBoard("eGameBoard::read plannedActions count=" +
                      std::to_string(npa));
    for(int i = 0; i < npa; i++) {
        ePlannedActionType type;
        src >> type;
        const auto a = ePlannedAction::sCreate(type);
        a->read(src, *this);
        mPlannedActions.push_back(a);
    }

    eSaveArchive ar(src);
    serializeYearlyProduction(ar);

    loadDebugLogBoard("eGameBoard::read post updates begin");
    updateMarbleTiles();
    updateTerritoryBorders();
    for(const auto& c : mCitiesOnBoard) {
        c->updateResources();
    }
    loadDebugLogBoard("eGameBoard::read done");
}
