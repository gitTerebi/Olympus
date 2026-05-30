#include "esupplydemandchangeevent.h"

#include "fileIO/esavearchive.h"
#include "engine/game-board.h"

eSupplyDemandChangeEvent::eSupplyDemandChangeEvent(
    const eCityId cid, const eGameEventType type,
    const eGameEventBranch branch, GameBoard &board) :
    eGameEvent(cid, type, branch, board),
    eCityEventValue(board) {}

void eSupplyDemandChangeEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    eResourceEventValue::serialize(ar);
    eCountEventValue::serialize(ar);
    eCityEventValue::serialize(ar, *gameBoard());
}
