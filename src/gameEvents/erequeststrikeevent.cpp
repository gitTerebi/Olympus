#include "erequeststrikeevent.h"


#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "fileIO/esavearchive.h"
#include "elanguage.h"
#include "gameEvents/invasions/invasion-event.h"

eRequestStrikeEvent::eRequestStrikeEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::requestStrike, branch, board) {}

const double gStrikeFrac = 0.67;

void eRequestStrikeEvent::trigger() {
    if(!mCity || !mRivalCity) return;
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    ed.fRivalCity = mRivalCity;

   const auto date = board->date();

    const int str = gStrikeFrac*mCity->troops();
    if(mEnd) {
        const int rstr = mRivalCity->troops();

        if(str > 1.5*rstr) {
            mRivalCity->setRelationship(eForeignCityRelationship::vassal);
            board->event(eEvent::strikeSuccessful, ed);
        } else {
            board->event(eEvent::strikeUnsuccessful, ed);
        }
        {
            const int troops = mRivalCity->troops();
            const double killFrac = std::clamp(0.5*str/rstr, 0., 1.);
            mRivalCity->setTroops((1 - killFrac)*troops);
        }

        {
            const int troops = mCity->troops();
            const double killFrac = std::clamp(0.5*rstr/str, 0., 1.);
            mCity->setTroops((1 - gStrikeFrac*killFrac)*troops);
        }
    } else {
        if(mRivalCity->isOnBoard()) {
            const auto rCid = mRivalCity->cityId();
            const auto e = e::make_shared<eInvasionEvent>(
                rCid, eGameEventBranch::root, *board);
            e->initialize(mCity, str, pid);
            e->initializeDate(date + 100);
            board->addRootGameEvent(e);
        } else {
            const auto e = e::make_shared<eRequestStrikeEvent>(
                               cityId(), eGameEventBranch::child, *board);
            e->setCity(mCity);
            e->setRivalCity(mRivalCity);
            e->setEnd(true);
            e->initializeDate(date + 100);
            addConsequence(e);
        }
        board->event(eEvent::strikeDeparture, ed);
    }
}

std::string eRequestStrikeEvent::longName() const {
    return eLanguage::text("request_strike_long_name");
}

void eRequestStrikeEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.worldCityField("city", worldBoard(), mCity);
    ar.worldCityField("rivalCity", worldBoard(), mRivalCity);
    ar.field("end", mEnd, false);
}
