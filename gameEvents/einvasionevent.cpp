#include "einvasionevent.h"

#include "engine/egameboard.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "einvasionhandler.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "einvasionwarningevent.h"
#include "audio/emusic.h"
#include "evectorhelpers.h"
#include "eplayerconquestevent.h"
#include "engine/epathfinder.h"
#include "eiteratesquare.h"

#include <algorithm>

eInvasionEvent::eInvasionEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::invasion, branch, board) {}

eInvasionEvent::~eInvasionEvent() {
    const auto board = gameBoard();
    if(board) board->removeInvasion(this);
}

void eInvasionEvent::pointerCreated() {
    const auto warnTypes = {
        eInvasionWarningType::warning36,
        eInvasionWarningType::warning24,
        eInvasionWarningType::warning12,
        eInvasionWarningType::warning6,
        eInvasionWarningType::warning1
    };
    for(const auto w : warnTypes) {
        int months;
        switch(w) {
        case eInvasionWarningType::warning36:
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
        const int daysBefore = 31*months;
        const auto e = e::make_shared<eInvasionWarningEvent>(
                           cityId(), eGameEventBranch::child, *gameBoard());
        e->initialize(w, mCity);
        addWarning(daysBefore, e);
    }
}

void eInvasionEvent::initialize(const stdsptr<eWorldCity>& city,
                                const int infantry,
                                const int cavalry,
                                const int archers) {
    setCity(city);

    mInfantry = infantry;
    mCavalry = cavalry;
    mArchers = archers;
}

void eInvasionEvent::initialize(const stdsptr<eWorldCity>& city,
                                const eEnlistedForces& forces,
                                ePlayerConquestEvent* const conquestEvent) {
    setCity(city);

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
        if(const auto b = tt->banner()) {
            const auto type = b->type();
            const bool r = type == eBannerTypeS::disembarkPoint;
            if(r) final = tt;
            return r;
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
    eTile* result = nullptr;
    const auto prcs = [&](const int dx, const int dy) {
        const auto t = tile->tileRel<eTile>(dx, dy);
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
    const auto cid = cityId();
    board->removeInvasion(this);
    const auto tile = invasionTile();

    int infantry = 0;
    int cavalry = 0;
    int archers = 0;

    const auto city = mCity;

    if(mHardcoded) {
        infantry = mInfantry;
        cavalry = mCavalry;
        archers = mArchers;
    } else {
        city->troopsByType(infantry, cavalry, archers);
    }

    const auto startInvasion = [this, board, tile, cid, city,
                                infantry, cavalry, archers]() {
        if(!tile) return;
        const auto invadingCid = mCity->cityId();
        const auto invadingC = board->boardCityWithId(invadingCid);
        if(tile->hasWater()) {
            if(!mDisembarkTile || !mShoreTile) return;
            const auto eh = new eInvasionHandler(*board, cid, mCity, this);
            if(invadingC) {
                eh->initializeSeaInvasion(tile, mDisembarkTile, mShoreTile,
                                          mForces, mConquestEvent);
            } else {
                eh->initializeSeaInvasion(tile, mDisembarkTile, mShoreTile,
                                          infantry, cavalry, archers);
            }
        } else {
            const auto eh = new eInvasionHandler(*board, cid, mCity, this);
            if(invadingC) {
                eh->initializeLandInvasion(tile, mForces, mConquestEvent);
            } else {
                eh->initializeLandInvasion(tile, infantry, cavalry, archers);
            }
        }
    };

    const auto pid = board->cityIdToPlayerId(cid);
    const int drachmas = board->drachmas(pid);
    const int bribe = bribeCost();
    const auto bribeFunc = [this, board, pid, bribe, city, cid]() {
        const auto invadingPid = mCity->playerId();
        board->incDrachmas(invadingPid, bribe);
        board->incDrachmas(pid, -bribe);
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
            defeated();
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
    auto tmpl = eLanguage::text("invasion_by");
    const auto none = eLanguage::text("none");
    const auto cstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%1", cstr);
    return tmpl;
}

void eInvasionEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    dst.writeCity(mCity.get());

    dst << mHardcoded;

    dst << mInfantry;
    dst << mCavalry;
    dst << mArchers;

    dst.writeGameEvent(mConquestEvent);
    mForces.write(dst);

    dst << mInvasionPoint;

    dst << mWarned;
    mFirstWarning.write(dst);
}

void eInvasionEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    const auto board = gameBoard();
    src.readCity(board, [this](const stdsptr<eWorldCity>& c) {
        mCity = c;
    });

    src >> mHardcoded;

    src >> mInfantry;
    src >> mCavalry;
    src >> mArchers;

    src.readGameEvent(board, [this](eGameEvent* const e) {
        mConquestEvent = static_cast<ePlayerConquestEvent*>(e);
    });
    const auto wboard = board->getWorldBoard();
    mForces.read(*board, *wboard, src);

    src >> mInvasionPoint;
    updateDisembarkAndShoreTile();

    src >> mWarned;
    mFirstWarning.read(src);
    if(mWarned) {
        board->addInvasion(this);
    }
}

bool eInvasionEvent::finished() const {
    return mHandlers.empty() && eGameEvent::finished();
}

void eInvasionEvent::setCity(const stdsptr<eWorldCity>& c) {
    mCity = c;
    const auto& ws = warnings();
    for(const auto& w : ws) {
        const auto& ws = w.second;
        const auto iw = static_cast<eInvasionWarningEvent*>(ws.get());
        iw->setCity(c);
    }
}

void eInvasionEvent::setInvasionPoint(const int p) {
    mInvasionPoint = p;
    updateDisembarkAndShoreTile();
}

eTile* eInvasionEvent::invasionTile() const {
    const auto cid = cityId();
    const auto board = gameBoard();
    const auto tile = board->invasionTile(cid, mInvasionPoint);
    return tile;
}

eTile* eInvasionEvent::landInvasionTile() const {
    if(mShoreTile) return mShoreTile;
    return invasionTile();
}

void eInvasionEvent::setFirstWarning(const eDate& w) {
    mFirstWarning = w;
    mWarned = true;
    const auto board = gameBoard();
    if(!board) return;
    board->addInvasion(this);
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
    const int bribe = rt*mArchers + ht*mInfantry + hm*mCavalry;
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

void eInvasionEvent::updateWarnings() {
    auto& board = *gameBoard();
    const auto date = board.date();
    const auto& ws = warnings();
    for(const auto& w : ws) {
        const auto we = w.second;
        const auto wnd = we->nextDate();
        if(date > wnd) we->setRepeat(0);
    }
}

void eInvasionEvent::defeated() {
    auto& board = *gameBoard();
    const auto targetCity = cityId();
    board.defeatedBy(targetCity, mCity);
    eEventData ed(targetCity);
    ed.fCity = mCity;
    board.event(eEvent::invasionDefeat, ed);
    const auto invadingCid = mCity->cityId();
    const auto invadingPid = board.cityIdToPlayerId(invadingCid);
    const auto invadingC = board.boardCityWithId(invadingCid);
    const auto ppid = board.personPlayer();
    const auto wboard = board.getWorldBoard();
    const auto targetWCity = wboard->cityWithId(targetCity);
    if(invadingC) {
        eEventData ied(invadingPid);
        board.event(eEvent::cityConquered, ied);
        board.allow(invadingCid, eBuildingType::commemorative, 4);
        if(invadingPid == ppid) {
            targetWCity->setRelationship(eForeignCityRelationship::vassal);
        }
        const auto ppc = board.personPlayerCapital();
        if(targetCity != ppc) {
            board.moveCityToPlayer(targetCity, invadingPid);
        }
        if(mConquestEvent) mConquestEvent->planArmyReturn();
    }
}
