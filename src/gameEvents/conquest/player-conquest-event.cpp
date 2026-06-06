#include "player-conquest-event.h"
#include "erand.h"

#include "engine/game-board.h"
#include "engine/board-city.h"
#include "buildings/sanctuaries/sanctuary.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

#include "gameEvents/invasions/invasion-event.h"
#include "fileIO/esavearchive.h"

PlayerConquestEvent::PlayerConquestEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    PlayerConquestEventBase(cid, eGameEventType::playerConquestEvent,
                            branch, board) {}

void PlayerConquestEvent::initialize(
        const eDate& date,
        const eEnlistedForces& forces,
        const stdsptr<WorldCity>& city) {
    mForces = forces;
    mCity = city;

    const auto board = gameBoard();
    if(!board) return;
    if(!mCity) return;

    if(mForces.fAres) {
        const auto bc = board->boardCityWithId(mForces.fAresCity);
        if(bc) {
            const auto aresSanct = bc->sanctuary(eGodType::ares);
            if(aresSanct) {
                aresSanct->consumeAresBuff();
                aresSanct->sendAresAbroad();
            }
        }
    }

    const auto cid = mCity->cityId();
    const auto c = board->boardCityWithId(cid);
    if(c) {
        const auto e = e::make_shared<eInvasionEvent>(
                           cid, eGameEventBranch::root, *board);
        const auto& wBoard = board->world();
        const auto pcid = cityId();
        const auto playerCity = wBoard.cityWithId(pcid);
        e->setWarningMonths(warningMonths());
        e->initializeDate(date, 0, 1);
        e->initialize(playerCity, mForces, this);
        c->addRootGameEvent(e);
        mInvasionEvent = e.get();
    }
}

void PlayerConquestEvent::trigger() {
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
        const int playerStr = mForces.strength();
        const bool unbeatable = mCity->militaryStrength() == 6;

        {
            const double killFrac = std::clamp(0.5*enemyStr/playerStr, 0., 1.);
            mForces.kill(killFrac);
        }

        {
            const double killFrac = std::clamp(0.5*playerStr/enemyStr, 0., 1.);
            mCity->setTroops((1 - killFrac)*enemyStr);
        }

        const bool conquered = !unbeatable &&
                               eRand::combatRoll(playerStr, enemyStr);

        const auto pid = playerId();
        eEventData ed(pid);
        ed.fCity = mCity;
        const auto rel = mCity->relationship();
        if(rel == eForeignCityRelationship::ally) {
            board->attackedAllyAttitude(pid);
            board->event(eEvent::allyAttackedByPlayer, ed);
        }
        if(conquered) {
            mCity->setConqueredBy(nullptr);
        }
        if(mCity->isColony()) {
            if(conquered) {
                board->event(eEvent::colonyRestored, ed);
                board->changeCityAttitude(mCity, 50, pid);
            } else {
                board->event(eEvent::cityConquerFailed, ed);
            }
        } else {
            if(conquered) {
                board->event(eEvent::cityConquered, ed);
                const auto cid = cityId();
                board->allow(cid, eBuildingType::commemorative, 4);
                mCity->setRelationship(eForeignCityRelationship::vassal);
                const auto pid = mCity->playerId();
                board->setPlayerTeam(pid, eTeamId::team0);
            } else {
                board->event(eEvent::cityConquerFailed, ed);
            }
            board->changeCityAttitude(mCity, -50, pid);
        }

        planArmyReturn();
    }
}

std::string PlayerConquestEvent::longName() const {
    return eLanguage::text("player_conquest_event_long_name");
}

bool PlayerConquestEvent::finished() const {
    return PlayerConquestEventBase::finished() &&
            (!mInvasionEvent || mInvasionEvent->finished());
}

void PlayerConquestEvent::serializeFields(eSaveArchive& ar) {
    ArmyEventBase::serializeFields(ar);
    ar.gameEventField("invasionEvent", gameBoard(), mInvasionEvent);
}

bool PlayerConquestEvent::warned() const {
    if(!mInvasionEvent) return false;
    return mInvasionEvent->warned();
}
