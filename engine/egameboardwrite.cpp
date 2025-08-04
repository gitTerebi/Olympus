#include "egameboard.h"

#include "spawners/ebanner.h"
#include "fileIO/ebuildingwriter.h"
#include "einvasionhandler.h"
#include "missiles/emissile.h"
#include "gameEvents/egameevent.h"
#include "gameEvents/ereceiverequestevent.h"
#include "gameEvents/etroopsrequestevent.h"
#include "eplague.h"

void eGameBoard::write(eWriteStream& dst) const {
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
            const auto b = t->banner();
            const bool has = b != nullptr;
            dst << has;
            if(!has) continue;
            dst << b->type();
            dst << b->id();
            b->write(dst);
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
        const int nb = mAllSoldierBanners.size();
        dst << nb;
        for(const auto& b : mAllSoldierBanners) {
            dst << b->type();
            b->write(dst);
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

    dst << mProgressEarthquakes;
    dst << mEarthquakes.size();
    for(const auto& e : mEarthquakes) {
        e->write(dst);
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

    dst << mYearlyProduction.size();
    for(const auto& p : mYearlyProduction) {
        dst << p.first;
        dst << p.second.fBest;
        dst << p.second.fLastYear;
        dst << p.second.fThisYear;
    }
}
