#include "einvasionevent.h"

#include "engine/egameboard.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "einvasionhandler.h"
#include "elanguage.h"
#include "einvasionwarning.h"
#include "audio/emusic.h"
#include "evectorhelpers.h"
#include "eplayerconquestevent.h"
#include "engine/epathfinder.h"
#include "eiteratesquare.h"

eInvasionEvent::eInvasionEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::invasion,
               branch, board),
    ePointEventValue(eBannerTypeS::landInvasion,
                    cid, board),
    eCityEventValue(board, [this, cid](eWorldCity& c) {
        auto& board = *gameBoard();
        const auto ccid = c.cityId();
        const auto tid = board.cityIdToTeamId(cid);
        const auto ctid = board.cityIdToTeamId(ccid);
        const bool e = eTeamIdHelpers::isEnemy(tid, ctid);
        return e;
    }) {}

eInvasionEvent::~eInvasionEvent() {
    const auto board = gameBoard();
    if(board) board->removeInvasion(this);
}

void eInvasionEvent::pointerCreated() {
    const auto warnTypes = {
        eInvasionWarningType::warningInitial,
        eInvasionWarningType::warning24,
        eInvasionWarningType::warning12,
        eInvasionWarningType::warning6,
        eInvasionWarningType::warning1
    };
    const auto cid = cityId();
    auto& board = *gameBoard();
    for(const auto w : warnTypes) {
        int months;
        switch(w) {
        case eInvasionWarningType::warningInitial:
            months = 36;
            break;
        case eInvasionWarningType::warning24:
            months = 24;
            break;
        case eInvasionWarningType::warning12:
            months = 12;
            break;
        case eInvasionWarningType::warning6:
            months = 6;
            break;
        case eInvasionWarningType::warning1:
            months = 1;
            break;
        }
        const auto e = std::make_shared<eInvasionWarning>(
            months, *this, cid, board, w);
        if(w == eInvasionWarningType::warningInitial) {
            mInitialWarning = e.get();
        }
        addWarning(e);
    }
}

void eInvasionEvent::initialize(const stdsptr<eWorldCity>& city,
                                const int count, const ePlayerId sentBy) {
    setSingleCity(city);
    setMinCount(count);
    setMaxCount(count);
    mSentByPlayer = sentBy;
}

void eInvasionEvent::initialize(const stdsptr<eWorldCity>& city,
                                const eEnlistedForces& forces,
                                ePlayerConquestEvent* const conquestEvent) {
    mCity = city;
    if(city) {
        const auto cid = city->cityId();
        const int i = static_cast<int>(cid);
        setMinCityId(i);
        setMaxCityId(i);
    }
    mForces = forces;
    mConquestEvent = conquestEvent;
}

eTile* nearestDisembarkTile(eTile* const tile, eGameBoard& board,
                            const eCityId cid) {
    eTile* final = nullptr;
    ePathFinder p([](eTileBase* const t) {
        return t->hasWater();
    }, [&](eTileBase* const t) {
        const auto tt = static_cast<eTile*>(t);
        const auto& bs = tt->banners();
        for(const auto& b : bs) {
            const auto type = b->type();
            const bool r = type == eBannerTypeS::disembarkPoint;
            if(r) {
                final = tt;
                return true;
            }
        }
        return false;
    });
    const int w = board.width();
    const int h = board.height();
    const auto rect = board.boardCityTileBRect(cid);
    const bool r = p.findPath(rect, tile, 1000, true, w, h);
    if(!r) return nullptr;
    return final;
}


eTile* nearestShoreTile(eTile* const tile) {
    if(!tile) return nullptr;
    eTile* result = nullptr;
    const auto prcs = [&](const int dx, const int dy) {
        const auto t = tile->tileRel<eTile>(dx, dy);
        if(!t) return false;
         if(!t->hasBridge() && t->walkable()) {
            result = t;
            return true;
        }
        return false;
    };
    for(int i = 0; i < 9; i++) {
        eIterateSquare::iterateSquare(i, prcs);
        if(result) return result;
    }
    return result;
}

void eInvasionEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    if(!mWarned) {
        choosePointId();
        chooseCity();
        updateDisembarkAndShoreTile();
    }
    chooseCount();
    const int c = count();
    const auto cid = cityId();
    board->removeInvasion(this);
    mWarned = false;
    const auto tile = invasionTile();

    int infantry = 0;
    int cavalry = 0;
    int archers = 0;

    const auto city = mCity;

    if(mHardcoded) {
        city->troopsByType(c, infantry, cavalry, archers);
    } else {
        city->troopsByType(infantry, cavalry, archers);
    }
    const auto startInvasion = [this, board, tile, cid, city,
                                infantry, cavalry, archers]() {
        if(!tile) return;
        const auto invadingCid = mCity->cityId();
        const auto invadingC = board->boardCityWithId(invadingCid);

        eInvasionHandler* eh = nullptr;
        if(tile->hasWater()) {
            if(!mDisembarkTile || !mShoreTile) return;
            eh = new eInvasionHandler(*board, cid, mCity, this);
            if(invadingC) {
                eh->initializeSeaInvasion(tile, mDisembarkTile, mShoreTile,
                                          mForces, mConquestEvent);
            } else {
                eh->initializeSeaInvasion(tile, mDisembarkTile, mShoreTile,
                                          infantry, cavalry, archers);
            }
            } else {
            eh = new eInvasionHandler(*board, cid, mCity, this);
            if(invadingC) {
                eh->initializeLandInvasion(tile, mForces, mConquestEvent);
            } else {
                eh->initializeLandInvasion(tile, infantry, cavalry, archers);
            }
        }

        const auto invadingPid = board->cityIdToPlayerId(invadingCid);
        const auto ppid = board->personPlayer();
        if(invadingPid == ppid && eh) {
            eEventData ed(invadingCid);
            ed.fTile = eh->currentTile();
            board->event(eEvent::playerInvasion, ed);
        }
    };

    const auto pid = board->cityIdToPlayerId(cid);
    const int drachmas = board->drachmas(pid);
    const int bribe = bribeCost();
    const auto bribeFunc = [this, board, pid, bribe, city, cid]() {
        const auto invadingPid = mCity->playerId();
        board->incDrachmas(invadingPid, bribe, eFinanceTarget::tributeReceived);
        board->incDrachmas(pid, -bribe, eFinanceTarget::bribesTributePaid);
        eEventData ed(cid);
        ed.fCity = city;
        board->event(eEvent::invasionBribed, ed);
        board->updateMusic();
        if(mConquestEvent) {
            mConquestEvent->planArmyReturn();
        }
    };

    if(!isPersonPlayer()) {
        startInvasion();
    } else {
        eEventData ed(cid);
        ed.fCity = mCity;
        ed.fType = eMessageEventType::invasion;
        ed.fBribe = bribe;
        ed.fReason = reason();

        ed.fA0 = [this, board, city, cid]() { // surrender
            eEventData ed(cid);
            ed.fCity = city;
            board->event(eEvent::invasionDefeat, ed);
            board->updateMusic();
            invadersWon();
        };
        if(drachmas >= bribe) { // bribe
            ed.fA1 = bribeFunc;
        }

        ed.fTile = tile;
        ed.fA2 = startInvasion; // fight
        board->event(eEvent::invasion, ed);
        eMusic::playRandomBattleMusic();
    }
}

std::string eInvasionEvent::longName() const {
    return eLanguage::zeusText(156, 2);
}

void eInvasionEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    ePointEventValue::write(dst);
    eCityEventValue::write(dst);
    eCountEventValue::write(dst);

    dst << mHardcoded;
    dst << mSentByPlayer;

    dst.writeGameEvent(mConquestEvent);
    mForces.write(dst);

    dst << mWarned;
    mFirstWarning.write(dst);
}

void eInvasionEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    ePointEventValue::read(src);
    const auto board = gameBoard();
    eCityEventValue::read(src, *board);
    eCountEventValue::read(src);

    src >> mHardcoded;
    src >> mSentByPlayer;

    src.readGameEvent(board, [this](eGameEvent* const e) {
        mConquestEvent = static_cast<ePlayerConquestEvent*>(e);
    });
    mForces.read(*board, src);

    src >> mWarned;
    mFirstWarning.read(src);
    if(mWarned) {
        board->addInvasion(this);
        updateDisembarkAndShoreTile();
    }
}

bool eInvasionEvent::finished() const {
    return mHandlers.empty() && eGameEvent::finished();
}

void eInvasionEvent::setWarningMonths(const int ms) {
    eGameEvent::setWarningMonths(ms);
    mInitialWarning->setWarningMonths(ms);
}

eTile* eInvasionEvent::invasionTile() const {
    const auto cid = cityId();
    const auto board = gameBoard();
    const int ptId = pointId();
    const auto tile = board->invasionTile(cid, ptId);
    return tile;
}

eTile* eInvasionEvent::landInvasionTile() const {
    if(mShoreTile) return mShoreTile;
    return invasionTile();
}

void eInvasionEvent::setFirstWarning(const eDate& w) {
    const auto board = gameBoard();
    if(!board) return;
    choosePointId();
    chooseCity();
    if(!mCity) return;
    updateDisembarkAndShoreTile();
    board->addInvasion(this);
    mFirstWarning = w;
    mWarned = true;
}

bool eInvasionEvent::activeInvasions() const {
    return !mHandlers.empty();
}

void eInvasionEvent::addInvasionHandler(eInvasionHandler* const i) {
    mHandlers.push_back(i);
}

void eInvasionEvent::removeInvasionHandler(eInvasionHandler* const i) {
    eVectorHelpers::remove(mHandlers, i);
}

bool eInvasionEvent::nearestSoldier(const int fromX, const int fromY,
                                    int& toX, int& toY) const {
    bool found = false;
    int minDist = 99999;
    for(const auto i : mHandlers) {
        int toXX;
        int toYY;
        const bool r = i->nearestSoldier(fromX, fromY, toXX, toYY);
        if(!r) continue;
        const int dx = fromX - toXX;
        const int dy = fromY - toYY;
        const int dist = sqrt(dx*dx + dy*dy);
        if(dist > minDist) continue;
        found = true;
        toX = toXX;
        toY = toYY;
        minDist = dist;
    }
    return found;
}

ePlayerConquestEvent* eInvasionEvent::conquestEvent() const
{ return mConquestEvent; }

int eInvasionEvent::bribeCost() const {
    const auto board = gameBoard();
    if(!board) return 0;
    const auto cid = cityId();
    const auto pid = board->cityIdToPlayerId(cid);
    const auto diff = board->difficulty(pid);
    const int rt = eDifficultyHelpers::soliderBribe(
                       diff, eCharacterType::rockThrower);
    const int ht = eDifficultyHelpers::soliderBribe(
                       diff, eCharacterType::hoplite);
    const int hm = eDifficultyHelpers::soliderBribe(
                       diff, eCharacterType::horseman);

    int infantry = 0;
    int cavalry = 0;
    int archers = 0;

    if(mConquestEvent) {
        for(const auto& b : mForces.fSoldiers) {
            const auto type = b->type();
            switch(type) {
            case eBannerType::hoplite:
            case eBannerType::aresWarrior:
            case eBannerType::amazon:
                infantry += b->count();
                break;
            case eBannerType::horseman:
                cavalry += b->count();
                break;
            case eBannerType::rockThrower:
                archers += b->count();
                break;
            case eBannerType::enemy:
            case eBannerType::trireme:
                break;
            }
        }
        for(const auto& a : mForces.fAllies) {
            int aInfantry;
            int aCavalry;
            int aArchers;
            a->troopsByType(aInfantry, aCavalry, aArchers);
            infantry += aInfantry;
            cavalry += aCavalry;
            archers += aArchers;
        }
    } else {
        soldiersByType(infantry, cavalry, archers);
    }

    const int bribe = rt*archers + ht*infantry + hm*cavalry;
    return bribe;
}

void eInvasionEvent::updateDisembarkAndShoreTile() {
    mDisembarkTile = nullptr;
    const auto tile = invasionTile();
    if(!tile) return;
    const auto terr = tile->terrain();
    if(terr != eTerrain::water) return;
    auto& board = *gameBoard();
    const auto cid = cityId();
    mDisembarkTile = nearestDisembarkTile(tile, board, cid);
    mShoreTile = nearestShoreTile(mDisembarkTile);
}

void eInvasionEvent::soldiersByType(int& infantry,
                                    int& cavalry,
                                    int& archers) const {
    if(mCity) {
        const int c = count();
        mCity->troopsByType(c, infantry, cavalry, archers);
    } else {
        infantry = 0;
        cavalry = 0;
        archers = 0;
    }
}

void eInvasionEvent::invadersWon() {
    auto& board = *gameBoard();
    const auto targetCity = cityId();
    board.defeatedBy(targetCity, mCity);
    eEventData ed(targetCity);
    ed.fCity = mCity;
    board.event(eEvent::invasionDefeat, ed);
    if(mSentByPlayer != ePlayerId::neutralFriendly) {
        const auto ppid = board.personPlayer();
        const auto& wboard = board.world();
        const auto targetWCity = wboard.cityWithId(targetCity);
        eEventData ed(mSentByPlayer);
        ed.fCity = mCity;
        ed.fRivalCity = targetWCity;
        board.event(eEvent::strikeSuccessful, ed);
        if(mSentByPlayer == ppid) {
            targetWCity->setRelationship(eForeignCityRelationship::vassal);
        }
        const auto ppc = board.currentCityId();
        if(targetCity != ppc) {
            board.moveCityToPlayer(targetCity, mSentByPlayer);
        }
    } else {
        const auto invadingCid = mCity->cityId();
        const auto invadingPid = board.cityIdToPlayerId(invadingCid);
        const auto invadingC = board.boardCityWithId(invadingCid);
        const auto ppid = board.personPlayer();
        const auto& wboard = board.world();
        const auto targetWCity = wboard.cityWithId(targetCity);
        if(invadingC) {
            eEventData ed(invadingPid);
            ed.fCity = targetWCity;
            board.event(eEvent::cityConquered, ed);
            board.allow(invadingCid, eBuildingType::commemorative, 4);
            if(invadingPid == ppid) {
                targetWCity->setRelationship(eForeignCityRelationship::vassal);
            }
            const auto ppc = board.currentCityId();
            if(targetCity != ppc) {
                board.moveCityToPlayer(targetCity, invadingPid);
            }
            if(mConquestEvent) mConquestEvent->planArmyReturn();
        }
    }
}

void eInvasionEvent::invadersDefeated() {
    auto& board = *gameBoard();
    const auto targetCity = cityId();
    eEventData ed(targetCity);
    ed.fCity = mCity;

    const bool monn = eRand::rand() % 2;
    if(monn) {
        board.allow(targetCity, eBuildingType::commemorative, 1);
        board.event(eEvent::invasionVictoryMonn, ed);
    } else {
        board.event(eEvent::invasionVictory, ed);
    }

    const auto& wboard = board.world();
    const auto targetWCity = wboard.cityWithId(targetCity);
    if(mSentByPlayer != ePlayerId::neutralFriendly) {
        eEventData ed(mSentByPlayer);
        ed.fCity = mCity;
        ed.fRivalCity = targetWCity;
        board.event(eEvent::strikeUnsuccessful, ed);
    } else {
        const auto invadingCid = mCity->cityId();
        const auto invadingC = board.boardCityWithId(invadingCid);
        if(invadingC) {
            eEventData ied(invadingCid);
            ied.fCity = targetWCity;

            board.event(eEvent::cityConquerFailed, ied);
            if(mConquestEvent) {
                const auto& forces = mConquestEvent->forces();
                forces.kill(1.);
                mConquestEvent->planArmyReturn();
            }
        }
    }
}
