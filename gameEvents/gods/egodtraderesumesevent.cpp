#include "egodtraderesumesevent.h"
#include "fileIO/esavearchive.h"

#include "engine/e-game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "elanguage.h"

eGodTradeResumesEvent::eGodTradeResumesEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::godTradeResumes, branch, board) {}

void eGodTradeResumesEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    const auto cid = cityId();
    if(mGod == eGodType::zeus) {
        board->setLandTradeShutdown(cid, false);
        board->setSeaTradeShutdown(cid, false);
    } else if(mGod == eGodType::poseidon) {
        board->setSeaTradeShutdown(cid, false);
    } else if(mGod == eGodType::hermes) {
        board->setLandTradeShutdown(cid, false);
    }
    eEventData ed(cid);
    ed.fGod = mGod;
    board->event(eEvent::godTradeResumes, ed);
}

std::string eGodTradeResumesEvent::longName() const {
    return eLanguage::text("god_trade_resumes_long_name");
}

void eGodTradeResumesEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.field("god", mGod, eGodType::zeus);
}
