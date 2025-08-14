#include "eplayerconquestevent.h"

#include "engine/egameboard.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

#include "einvasionevent.h"

ePlayerConquestEvent::ePlayerConquestEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    ePlayerConquestEventBase(cid, eGameEventType::playerConquestEvent,
                             branch, board) {}

void ePlayerConquestEvent::initialize(
        const eDate& date,
        const eEnlistedForces& forces,
        const stdsptr<eWorldCity>& city) {
    mForces = forces;
    mCity = city;

    const auto board = gameBoard();
    if(!board) return;
    if(!mCity) return;

    const auto cid = mCity->cityId();
    const auto c = board->boardCityWithId(cid);
    if(c) {
        const auto e = e::make_shared<eInvasionEvent>(
                           cid, eGameEventBranch::root, *board);
        const auto& wBoard = board->world();
        const auto pcid = cityId();
        const auto playerCity = wBoard.cityWithId(pcid);
        e->initializeDate(date, 0, 1);
        e->initialize(playerCity, mForces, this);
        e->updateWarnings();
        c->addRootGameEvent(e);
        mInvasionEvent = e.get();
    }
}

void ePlayerConquestEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    removeArmyEvent();
    removeConquestEvent();
    if(!mCity) return;

    const auto cid = mCity->cityId();
    const auto c = board->boardCityWithId(cid);
    if(c) {
    } else {
        const int enemyStr = mCity->troops();
        const int str = mForces.strength();

        {
            const double killFrac = std::clamp(0.5*enemyStr/str, 0., 1.);
            mForces.kill(killFrac);
        }

        {
            const double killFrac = std::clamp(0.5*str/enemyStr, 0., 1.);
            mCity->setTroops((1 - killFrac)*enemyStr);
        }

        const bool conquered = str > enemyStr;

        const auto pid = playerId();
        eEventData ed(pid);
        ed.fCity = mCity;
        const auto rel = mCity->relationship();
        if(rel == eForeignCityRelationship::ally) {
            const auto w = worldBoard();
            if(w) w->attackedAlly(pid);
            board->event(eEvent::allyAttackedByPlayer, ed);
        }
        if(conquered) {
            mCity->setConqueredBy(nullptr);
        }
        if(mCity->isColony()) {
            if(conquered) {
                board->event(eEvent::colonyRestored, ed);
                mCity->incAttitude(50, pid);
            } else {
                board->event(eEvent::cityConquerFailed, ed);
            }
        } else {
            if(conquered) {
                board->event(eEvent::cityConquered, ed);
                const auto cid = cityId();
                board->allow(cid, eBuildingType::commemorative, 4);
                mCity->setRelationship(eForeignCityRelationship::vassal);
            } else {
                board->event(eEvent::cityConquerFailed, ed);
            }
            mCity->incAttitude(-50, pid);
        }

        planArmyReturn();
    }
}

std::string ePlayerConquestEvent::longName() const {
    return eLanguage::text("player_conquest_event_long_name");
}

bool ePlayerConquestEvent::finished() const {
    return ePlayerConquestEventBase::finished() &&
            (!mInvasionEvent || mInvasionEvent->finished());
}

void ePlayerConquestEvent::write(eWriteStream& dst) const {
    ePlayerConquestEventBase::write(dst);

    dst.writeGameEvent(mInvasionEvent);
}

void ePlayerConquestEvent::read(eReadStream& src) {
    ePlayerConquestEventBase::read(src);

    src.readGameEvent(gameBoard(), [this](eGameEvent* const e) {
        mInvasionEvent = static_cast<eInvasionEvent*>(e);
    });
}
