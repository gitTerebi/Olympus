#include "eeconomicmilitarychangeeventbase.h"

#include "fileIO/save-archive.h"
#include "engine/game-board.h"

eEconomicMilitaryChangeEventBase::eEconomicMilitaryChangeEventBase(
    const eCityId cid, const eGameEventType type,
    const eGameEventBranch branch, GameBoard &board) :
    eGameEvent(cid, type, branch, board),
    eCityEventValue(board) {}

void eEconomicMilitaryChangeEventBase::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eCountEventValue::serialize(ar);
    eCityEventValue::serialize(ar, *gameBoard());
}
