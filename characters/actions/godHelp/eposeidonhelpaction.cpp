#include "eposeidonhelpaction.h"

ePoseidonHelpAction::ePoseidonHelpAction(eCharacter* const c) :
    eProvideResourceHelpAction(c, eCharActionType::poseidonHelpAction,
                               eResourceType::fish, 32) {}

bool ePoseidonHelpAction::sHelpNeeded(const eCityId cid,
                                      const eGameBoard& board) {
    return eProvideResourceHelpAction::sHelpNeeded(
                cid, board, eResourceType::fish, 18);
}

