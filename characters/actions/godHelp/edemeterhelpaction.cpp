#include "edemeterhelpaction.h"

eDemeterHelpAction::eDemeterHelpAction(eCharacter* const c) :
    eProvideResourceHelpAction(c, eCharActionType::demeterHelpAction,
                               eResourceType::wheat, 32) {}

bool eDemeterHelpAction::sHelpNeeded(const eCityId cid,
                                     const eGameBoard& board) {
    return eProvideResourceHelpAction::sHelpNeeded(
                cid, board, eResourceType::wheat, 18);
}

