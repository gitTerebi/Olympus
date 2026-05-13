#include "e-game-board.h"

#include "spawners/ebanner.h"
#include "spawners/eboarspawner.h"
#include "spawners/edeerspawner.h"
#include "spawners/elandinvasionpoint.h"
#include "spawners/eseainvasionpoint.h"
#include "spawners/edisembarkpoint.h"
#include "spawners/emonsterpoint.h"
#include "spawners/eentrypoint.h"
#include "spawners/eexitpoint.h"
#include "spawners/edisasterpoint.h"
#include "spawners/elandslidepoint.h"
#include "spawners/ewolfspawner.h"
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
#include "fileIO/ejsonarchive.h"
#include "fileIO/eblob.h"
#include "engine/e-worldcity.h"

static void boardDbgLog(const char* msg) {
    (void)msg;
}

static void boardDbgLogN(const char* msg, int n) {
    (void)msg;
    (void)n;
}

static void boardDbgLogBuilding(const int i, const eBuildingType type,
                                const size_t pos) {
    (void)i;
    (void)type;
    (void)pos;
}

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
    boardDbgLog("board: size");
    int w;
    ar.field("mWidth", w, 0);
    int h;
    ar.field("mHeight", h, 0);
    initialize(w, h);

    boardDbgLog("board: scalars");
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
        int nc = 0;
        ar.field("mCitiesOnBoard.count", nc, 0);
        boardDbgLogN("board: cities", nc);
        for(int i = 0; i < nc; i++) {
            eCityId cid;
            ar.field("mCitiesOnBoard.id", cid);
            const auto c = addCityToBoard(cid);
            c->read(src);
            boardDbgLogN("board: city done", i);
            scheduleAppealMapUpdate(cid);
        }
    }

    {
        int np = 0;
        ar.field("mPlayersOnBoard.count", np, 0);
        boardDbgLogN("board: players", np);
        for(int i = 0; i < np; i++) {
            ePlayerId pid;
            ar.field("mPlayersOnBoard.id", pid);
            const auto p = std::make_shared<eBoardPlayer>(pid, *this);
            p->read(src);
            mPlayersOnBoard.push_back(p);
            boardDbgLogN("board: player done", i);
        }
    }

    boardDbgLog("board: tiles");
    for(const auto& ts : mTiles) {
        for(const auto& t : ts) {
            t->read(src);
        }
    }
    boardDbgLog("board: tiles done");

    {
        int nbs = 0;
        ar.field("mAllBuildings.count", nbs, 0);
        boardDbgLogN("board: buildings", nbs);
        for(int i = 0; i < nbs; i++) {
            eBuildingType type;
            ar.field("mAllBuildings.type", type);
            boardDbgLogBuilding(i, type, src.position());
            const auto b = eBuildingReader::sRead(*this, type, src);
            if(!b) boardDbgLogN("board: building null", i);
            boardDbgLogN("board: building done", i);
        }
    }


    {
        int ncs = 0;
        ar.field("mCharacters.count", ncs, 0);
        boardDbgLogN("board: chars", ncs);

        for(int i = 0; i < ncs; i++) {
            eCharacterType type;
            ar.field("mCharacters.type", type);
            const auto c = eCharacter::sCreate(type, *this);
            c->read(src);
            boardDbgLogN("board: char done", i);
        }
    }

    {
        int ncs = 0;
        ar.field("mMissiles.count", ncs, 0);
        boardDbgLogN("board: missiles", ncs);

        for(int i = 0; i < ncs; i++) {
            eMissileType type;
            ar.field("mMissiles.type", type);
            const auto c = eMissile::sCreate(*this, type);
            c->read(src);
            boardDbgLogN("board: missile done", i);
        }
    }

    int ng = 0;
    ar.field("mGoals.count", ng, 0);
    boardDbgLogN("board: goals", ng);
    for(int i = 0; i < ng; i++) {
        const auto g = std::make_shared<eEpisodeGoal>();
        g->read(src);
        mGoals.push_back(g);
    }
    ar.field("mGoalsFulfilled", mGoalsFulfilled);

    ar.field("mProgressEarthquakes", mProgressEarthquakes);
    int ne = 0;
    ar.field("mEarthquakes.count", ne, 0);
    boardDbgLogN("board: earthquakes", ne);
    for(int i = 0; i < ne; i++) {
        const auto e = std::make_shared<eEarthquake>();
        e->read(src, *this);
        mEarthquakes.push_back(e);
    }

    ar.field("mProgressWaves", mProgressWaves);
    int nw = 0;
    ar.field("mTidalWaves.count", nw, 0);
    boardDbgLogN("board: waves", nw);
    for(int i = 0; i < nw; i++) {
        const auto w = std::make_shared<eTidalWave>();
        w->read(src, *this);
        mTidalWaves.push_back(w);
    }

    ar.field("mProgressLavaFlows", mProgressLavaFlows);
    int nl = 0;
    ar.field("mLavaFlows.count", nl, 0);
    boardDbgLogN("board: lava", nl);
    for(int i = 0; i < nl; i++) {
        const auto w = std::make_shared<eLavaFlow>();
        w->read(src, *this);
        mLavaFlows.push_back(w);
    }

    ar.field("mProgressLandSlides", mProgressLandSlides);
    int ns = 0;
    ar.field("mLandSlides.count", ns, 0);
    boardDbgLogN("board: slides", ns);
    for(int i = 0; i < ns; i++) {
        const auto w = std::make_shared<eLandSlide>();
        w->read(src, *this);
        mLandSlides.push_back(w);
    }

    int nd = 0;
    ar.field("mConqueredBy.count", nd, 0);
    boardDbgLogN("board: conqueredBy", nd);
    for(int i = 0; i < nd; i++) {
        eCityId cid;
        ar.field("mConqueredBy.id", cid);
        int nc = 0;
        ar.field("mConqueredBy.cities.count", nc, 0);
        for(int j = 0; j < nc; j++) {
            src.readCity(this, [this, cid](const stdsptr<eWorldCity>& c) {
                mConqueredBy[cid].push_back(c);
            });
        }
    }

    int npa = 0;
    ar.field("mPlannedActions.count", npa, 0);
    boardDbgLogN("board: plannedActions", npa);
    for(int i = 0; i < npa; i++) {
        ePlannedActionType type;
        ar.field("mPlannedActions.type", type);
        const auto a = ePlannedAction::sCreate(type);
        a->read(src, *this);
        mPlannedActions.push_back(a);
    }

    serializeYearlyProduction(ar);
    boardDbgLog("board: yearly done");
    serializeMessageLog(ar);
    boardDbgLog("board: messages done");

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
    boardDbgLog("board: DONE");
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

#define JLOG(msg) do { (void)sizeof(msg); } while(0)
#define JLOGF(fmt, ...) do { (void)sizeof(fmt); } while(0)

void eGameBoard::serializeJson(eJsonArchive& ar) {
    if(ar.reading()) {
        JLOG("serializeJson: start");
        int w = 0, h = 0;
        ar.field("mWidth", w);
        ar.field("mHeight", h);
        initialize(w, h);
        JLOGF("serializeJson: init %dx%d", w, h);

        ar.field("mFogOfWar", mFogOfWar);
        ar.field("mEpisodeLost", mEpisodeLost);
        ar.field("mWageMultiplier", mWageMultiplier);

        for(auto& p : mPrices) {
            const auto key = "price." + std::to_string(static_cast<int>(p.first));
            ar.field(key.c_str(), p.second);
        }

        { auto da = ar.child("mDate"); mDate.serializeJson(da); }
        ar.field("mFrame", mFrame);
        ar.field("mTime", mTime);
        ar.field("mTotalTime", mTotalTime);
        ar.field("mSoldiersUpdate", mSoldiersUpdate);

        // cities
        {
            int nc = 0;
            ar.field("mCitiesOnBoard.count", nc);
            for(int i = 0; i < nc; i++) {
                eCityId cid{};
                ar.field(("mCitiesOnBoard." + std::to_string(i) + ".id").c_str(), cid);
                const auto c = addCityToBoard(cid);
                const auto key = "mCitiesOnBoard." + std::to_string(i);
                auto ca = ar.child(key.c_str());
                c->serializeJson(ca);
                scheduleAppealMapUpdate(cid);
            }
        }

        // players
        {
            int np = 0;
            ar.field("mPlayersOnBoard.count", np);
            for(int i = 0; i < np; i++) {
                ePlayerId pid{};
                ar.field(("mPlayersOnBoard." + std::to_string(i) + ".id").c_str(), pid);
                const auto p = std::make_shared<eBoardPlayer>(pid, *this);
                const auto key = "mPlayersOnBoard." + std::to_string(i);
                auto pa = ar.child(key.c_str());
                p->serializeJson(pa);
                mPlayersOnBoard.push_back(p);
            }
        }

        // tiles
        {
            int tileIdx = 0;
            for(const auto& ts : mTiles)
                for(const auto& t : ts) {
                    auto tAr = ar.childAt("tiles", tileIdx++);
                    t->serializeJson(tAr);
                }
        }

        // buildings
        {
            int nb = 0;
            ar.field("mAllBuildingsCount", nb);
            JLOGF("buildings: loading %d", nb);
            for(int i = 0; i < nb; i++) {
                auto bAr = ar.childAt("mAllBuildings", i);
                eBuildingType type{};
                bAr.field("type", type);
                JLOGF("building %d: type %d", i, (int)type);
                (void)eBuildingReader::sReadJson(*this, type, bAr);
                JLOGF("building %d: done", i);
            }
            JLOGF("buildings: runPostFuncs count=%zu", ar.postFuncCount());
            try {
                ar.runPostFuncs([&](int idx){ JLOGF("postFunc %d start", idx); },
                                [&](int idx){ JLOGF("postFunc %d done",  idx); });
            } catch(const std::exception& e) { JLOGF("runPostFuncs exception: %s", e.what()); throw; } catch(...) { JLOG("runPostFuncs unknown exception"); throw; }
            JLOG("buildings: done");
        }

        // characters
        {
            int nc = 0;
            ar.field("mCharacters.count", nc);
            for(int i = 0; i < nc; i++) {
                const auto key = "mCharacters." + std::to_string(i);
                auto cAr = ar.child(key.c_str());
                eCharacterType type{};
                cAr.field("type", type);
                const auto c = eCharacter::sCreate(type, *this);
                c->serializeJson(cAr);
            }
            ar.runPostFuncs();
        }

        // banners
        {
            int nb = 0;
            ar.field("mBanners.count", nb);
            std::vector<stdsptr<eBanner>> loadedBanners;
            for(int i = 0; i < nb; i++) {
                const auto key = "mBanners." + std::to_string(i);
                auto bAr = ar.child(key.c_str());
                eBannerTypeS type{};
                bAr.field("type", type);
                const auto b = eBanner::sCreate(i, nullptr, *this, type);
                if(b) {
                    b->serializeJson(bAr);
                    loadedBanners.push_back(b);
                }
            }
            ar.runPostFuncs();
            for(const auto& b : loadedBanners) {
                if(b->tile()) b->tile()->addBanner(b);
            }
        }

        // missiles
        {
            int nm = 0;
            ar.field("mMissiles.count", nm);
            for(int i = 0; i < nm; i++) {
                eMissileType type{};
                ar.field(("mMissiles." + std::to_string(i) + ".type").c_str(), type);
                const auto m = eMissile::sCreate(*this, type);
                auto ma = ar.child(("mMissiles." + std::to_string(i)).c_str());
                m->serializeJson(ma, *this);
            }
            ar.runPostFuncs();
        }

        // goals
        {
            int ng = 0;
            ar.field("mGoals.count", ng);
            for(int i = 0; i < ng; i++) {
                const auto g = std::make_shared<eEpisodeGoal>();
                const auto key = "mGoals." + std::to_string(i);
                auto ga = ar.child(key.c_str());
                g->serializeJson(ga);
                mGoals.push_back(g);
            }
        }
        ar.field("mGoalsFulfilled", mGoalsFulfilled);

        ar.field("earthquakeProgress", mProgressEarthquakes);
        {
            int ne = 0;
            ar.field("earthquakeCount", ne);
            for(int i = 0; i < ne; i++) {
                const auto e = std::make_shared<eEarthquake>();
                auto ea = ar.childAt("earthquakes", i);
                e->serializeJson(ea, *this);
                mEarthquakes.push_back(e);
            }
        }

        ar.field("tidalWaveProgress", mProgressWaves);
        {
            int nw = 0;
            ar.field("tidalWaveCount", nw);
            for(int i = 0; i < nw; i++) {
                const auto w = std::make_shared<eTidalWave>();
                auto wa = ar.childAt("tidalWaves", i);
                w->serializeJson(wa, *this);
                mTidalWaves.push_back(w);
            }
        }

        ar.field("lavaFlowProgress", mProgressLavaFlows);
        {
            int nl = 0;
            ar.field("lavaFlowCount", nl);
            for(int i = 0; i < nl; i++) {
                const auto lf = std::make_shared<eLavaFlow>();
                auto la = ar.childAt("lavaFlows", i);
                lf->serializeJson(la, *this);
                mLavaFlows.push_back(lf);
            }
        }

        ar.field("landSlideProgress", mProgressLandSlides);
        {
            int ns = 0;
            ar.field("landSlideCount", ns);
            for(int i = 0; i < ns; i++) {
                const auto ls = std::make_shared<eLandSlide>();
                auto sa = ar.childAt("landSlides", i);
                ls->serializeJson(sa, *this);
                mLandSlides.push_back(ls);
            }
        }

        // conqueredBy
        {
            int nd = 0;
            ar.field("mConqueredBy.count", nd);
            for(int i = 0; i < nd; i++) {
                eCityId cid{};
                ar.field(("mConqueredBy." + std::to_string(i) + ".id").c_str(), cid);
                int nc = 0;
                ar.field(("mConqueredBy." + std::to_string(i) + ".count").c_str(), nc);
                for(int j = 0; j < nc; j++) {
                    const auto c = std::make_shared<eWorldCity>();
                    auto ca = ar.child(("mConqueredBy." + std::to_string(i) + ".city." + std::to_string(j)).c_str());
                    c->serializeJson(ca, &world());
                    mConqueredBy[cid].push_back(c);
                }
            }
            ar.runPostFuncs();
        }

        // planned actions
        {
            int npa = 0;
            ar.field("mPlannedActions.count", npa);
            for(int i = 0; i < npa; i++) {
                ePlannedActionType type{};
                ar.field(("mPlannedActions." + std::to_string(i) + ".type").c_str(), type);
                const auto a = ePlannedAction::sCreate(type);
                auto aa = ar.child(("mPlannedActions." + std::to_string(i)).c_str());
                a->serializeJson(aa, *this);
                mPlannedActions.push_back(a);
            }
            ar.runPostFuncs();
        }

        // yearly production
        {
            int np = 0;
            ar.field("mYearlyProduction.count", np);
            for(int i = 0; i < np; i++) {
                eResourceType type{};
                ar.field(("mYearlyProduction." + std::to_string(i) + ".type").c_str(), type);
                auto& y = mYearlyProduction[type];
                ar.field(("mYearlyProduction." + std::to_string(i) + ".best").c_str(), y.fBest);
                ar.field(("mYearlyProduction." + std::to_string(i) + ".last").c_str(), y.fLastYear);
                ar.field(("mYearlyProduction." + std::to_string(i) + ".this").c_str(), y.fThisYear);
            }
            ar.field("mLastAutosaveYear", mLastAutosaveYear);
        }

        // message log
        {
            int nm = 0;
            ar.field("messageLog.count", nm);
            for(int i = 0; i < nm; i++) {
                eLoggedMessage lm;
                ar.field(("messageLog." + std::to_string(i) + ".title").c_str(), lm.fMsg.fTitle);
                ar.field(("messageLog." + std::to_string(i) + ".text").c_str(), lm.fMsg.fText);
                { auto da = ar.child(("messageLog." + std::to_string(i) + ".date").c_str()); lm.fDate.serializeJson(da); }
                ar.field(("messageLog." + std::to_string(i) + ".player").c_str(), lm.fEd.fPlayerName);
                ar.field(("messageLog." + std::to_string(i) + ".read").c_str(), lm.fRead);
                lm.fEd.fDate = lm.fDate;
                lm.fEd.fType = eMessageEventType::common;
                mMessageLog.push_back(lm);
            }
        }

        updateMarbleTiles();
        updateTerritoryBorders();
        for(const auto& c : mCitiesOnBoard) c->updateResources();

        // post-load: wire up active requests
        // (no eReadStream here so use direct loop)
        for(const auto e : mAllGameEvents) {
            const auto request = dynamic_cast<eFulfillRequestEvent*>(e);
            if(request && request->isMainEvent() && request->isActiveCityRequest())
                addCityRequest(request);
            const auto tribute = dynamic_cast<ePayTributeEvent*>(e);
            if(tribute && tribute->isMainEvent() && !tribute->finished())
                addTributeRequest(tribute);
        }

    } else {
        ar.field("mWidth", mWidth);
        ar.field("mHeight", mHeight);
        ar.field("mFogOfWar", mFogOfWar);
        ar.field("mEpisodeLost", mEpisodeLost);
        ar.field("mWageMultiplier", mWageMultiplier);

        for(const auto& p : mPrices) {
            const auto key = "price." + std::to_string(static_cast<int>(p.first));
            int val = p.second;
            ar.field(key.c_str(), val);
        }

        { auto da = ar.child("mDate"); mDate.serializeJson(da); }
        ar.field("mFrame", mFrame);
        ar.field("mTime", mTime);
        ar.field("mTotalTime", mTotalTime);
        ar.field("mSoldiersUpdate", mSoldiersUpdate);

        // assign IOIDs before writing
        { int id = 0; for(const auto b : mAllBuildings)      b->setIOID(id++); }
        { int id = 0; for(const auto c : mCharacters)        c->setIOID(id++); }
        { int id = 0; for(const auto ca : mCharacterActions) ca->setIOID(id++); }
        { int id = 0; for(const auto b : mBanners)           b->setIOID(id++); }
        { int id = 0; for(const auto b : mAllSoldierBanners) b->setIOID(id++); }
        { int id = 0; for(const auto e : mAllGameEvents)     e->setIOID(id++); }
        { int id = 0; for(const auto& c : mCitiesOnBoard)    c->setInvasionHandlersIOIDs(id); }

        // cities
        {
            int nc = static_cast<int>(mCitiesOnBoard.size());
            ar.field("mCitiesOnBoard.count", nc);
            for(int i = 0; i < nc; i++) {
                eCityId cid = mCitiesOnBoard[i]->id();
                ar.field(("mCitiesOnBoard." + std::to_string(i) + ".id").c_str(), cid);
                const auto key = "mCitiesOnBoard." + std::to_string(i);
                auto ca = ar.child(key.c_str());
                mCitiesOnBoard[i]->serializeJson(ca);
            }
        }

        // players
        {
            int np = static_cast<int>(mPlayersOnBoard.size());
            ar.field("mPlayersOnBoard.count", np);
            for(int i = 0; i < np; i++) {
                ePlayerId pid = mPlayersOnBoard[i]->id();
                ar.field(("mPlayersOnBoard." + std::to_string(i) + ".id").c_str(), pid);
                const auto key = "mPlayersOnBoard." + std::to_string(i);
                auto pa = ar.child(key.c_str());
                mPlayersOnBoard[i]->serializeJson(pa);
            }
        }

        // tiles
        {
            int tileIdx = 0;
            for(const auto& ts : mTiles)
                for(const auto& t : ts) {
                    auto tAr = ar.childAt("tiles", tileIdx++);
                    t->serializeJson(tAr);
                }
        }

        // buildings
        {
            int nb = static_cast<int>(mAllBuildings.size());
            ar.field("mAllBuildingsCount", nb);
            int i = 0;
            for(const auto b : mAllBuildings) {
                auto bAr = ar.childAt("mAllBuildings", i);
                eBuildingType type = b->type();
                bAr.field("type", type);
                eBuildingWriter::sWriteJson(b, bAr);
                i++;
            }
        }

        // characters
        {
            int nc = static_cast<int>(mCharacters.size());
            ar.field("mCharacters.count", nc);
            int i = 0;
            for(const auto c : mCharacters) {
                const auto key = "mCharacters." + std::to_string(i);
                auto cAr = ar.child(key.c_str());
                eCharacterType type = c->type();
                cAr.field("type", type);
                c->serializeJson(cAr);
                i++;
            }
        }

        // banners
        {
            int nb = static_cast<int>(mBanners.size());
            ar.field("mBanners.count", nb);
            int i = 0;
            for(const auto b : mBanners) {
                const auto key = "mBanners." + std::to_string(i);
                auto bAr = ar.child(key.c_str());
                eBannerTypeS type = b->type();
                bAr.field("type", type);
                b->serializeJson(bAr);
                i++;
            }
        }

        // missiles
        {
            int nm = static_cast<int>(mMissiles.size());
            ar.field("mMissiles.count", nm);
            for(int i = 0; i < nm; i++) {
                eMissileType type = mMissiles[i]->type();
                ar.field(("mMissiles." + std::to_string(i) + ".type").c_str(), type);
                auto ma = ar.child(("mMissiles." + std::to_string(i)).c_str());
                mMissiles[i]->serializeJson(ma, *this);
            }
        }

        // goals
        {
            int ng = static_cast<int>(mGoals.size());
            ar.field("mGoals.count", ng);
            for(int i = 0; i < ng; i++) {
                const auto key = "mGoals." + std::to_string(i);
                auto ga = ar.child(key.c_str());
                mGoals[i]->serializeJson(ga);
            }
        }
        ar.field("mGoalsFulfilled", mGoalsFulfilled);

        ar.field("mProgressEarthquakes", mProgressEarthquakes);
        {
            int ne = static_cast<int>(mEarthquakes.size());
            ar.field("mEarthquakes.count", ne);
            for(int i = 0; i < ne; i++) {
                auto ea = ar.child(("mEarthquakes." + std::to_string(i)).c_str());
                mEarthquakes[i]->serializeJson(ea, *this);
            }
        }

        ar.field("mProgressWaves", mProgressWaves);
        {
            int nw = static_cast<int>(mTidalWaves.size());
            ar.field("mTidalWaves.count", nw);
            for(int i = 0; i < nw; i++) {
                auto wa = ar.child(("mTidalWaves." + std::to_string(i)).c_str());
                mTidalWaves[i]->serializeJson(wa, *this);
            }
        }

        ar.field("mProgressLavaFlows", mProgressLavaFlows);
        {
            int nl = static_cast<int>(mLavaFlows.size());
            ar.field("mLavaFlows.count", nl);
            for(int i = 0; i < nl; i++) {
                auto la = ar.child(("mLavaFlows." + std::to_string(i)).c_str());
                mLavaFlows[i]->serializeJson(la, *this);
            }
        }

        ar.field("mProgressLandSlides", mProgressLandSlides);
        {
            int ns = static_cast<int>(mLandSlides.size());
            ar.field("mLandSlides.count", ns);
            for(int i = 0; i < ns; i++) {
                auto sa = ar.child(("mLandSlides." + std::to_string(i)).c_str());
                mLandSlides[i]->serializeJson(sa, *this);
            }
        }

        // conqueredBy
        {
            int nd = static_cast<int>(mConqueredBy.size());
            ar.field("mConqueredBy.count", nd);
            int i = 0;
            for(const auto& p : mConqueredBy) {
                eCityId cid = p.first;
                ar.field(("mConqueredBy." + std::to_string(i) + ".id").c_str(), cid);
                int nc = static_cast<int>(p.second.size());
                ar.field(("mConqueredBy." + std::to_string(i) + ".count").c_str(), nc);
                for(int j = 0; j < nc; j++) {
                    auto ca = ar.child(("mConqueredBy." + std::to_string(i) + ".city." + std::to_string(j)).c_str());
                    p.second[j]->serializeJson(ca, &world());
                }
                i++;
            }
        }

        // planned actions
        {
            int npa = static_cast<int>(mPlannedActions.size());
            ar.field("mPlannedActions.count", npa);
            for(int i = 0; i < npa; i++) {
                ePlannedActionType type = mPlannedActions[i]->type();
                ar.field(("mPlannedActions." + std::to_string(i) + ".type").c_str(), type);
                auto aa = ar.child(("mPlannedActions." + std::to_string(i)).c_str());
                mPlannedActions[i]->serializeJson(aa, *this);
            }
        }

        // yearly production
        {
            int np = static_cast<int>(mYearlyProduction.size());
            ar.field("mYearlyProduction.count", np);
            int i = 0;
            for(const auto& p : mYearlyProduction) {
                eResourceType type = p.first;
                ar.field(("mYearlyProduction." + std::to_string(i) + ".type").c_str(), type);
                int best = p.second.fBest, last = p.second.fLastYear, thisY = p.second.fThisYear;
                ar.field(("mYearlyProduction." + std::to_string(i) + ".best").c_str(), best);
                ar.field(("mYearlyProduction." + std::to_string(i) + ".last").c_str(), last);
                ar.field(("mYearlyProduction." + std::to_string(i) + ".this").c_str(), thisY);
                i++;
            }
            ar.field("mLastAutosaveYear", mLastAutosaveYear);
        }

        // message log
        {
            int nm = static_cast<int>(mMessageLog.size());
            ar.field("messageLog.count", nm);
            for(int i = 0; i < nm; i++) {
                auto& lm = mMessageLog[i];
                ar.field(("messageLog." + std::to_string(i) + ".title").c_str(), lm.fMsg.fTitle);
                ar.field(("messageLog." + std::to_string(i) + ".text").c_str(), lm.fMsg.fText);
                { auto da = ar.child(("messageLog." + std::to_string(i) + ".date").c_str()); lm.fDate.serializeJson(da); }
                ar.field(("messageLog." + std::to_string(i) + ".player").c_str(), lm.fEd.fPlayerName);
                ar.field(("messageLog." + std::to_string(i) + ".read").c_str(), lm.fRead);
            }
        }
    }
}
