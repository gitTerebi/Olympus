#include "eeconomicmilitarychangeeventbase.h"

#include "fileIO/esavearchive.h"
#include "engine/e-game-board.h"

eEconomicMilitaryChangeEventBase::eEconomicMilitaryChangeEventBase(
    const eCityId cid, const eGameEventType type,
    const eGameEventBranch branch, eGameBoard &board) :
    eGameEvent(cid, type, branch, board),
    eCityEventValue(board) {}

void eEconomicMilitaryChangeEventBase::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eCountEventValue::serialize(ar);
    eCityEventValue::serialize(ar, *gameBoard());
}
