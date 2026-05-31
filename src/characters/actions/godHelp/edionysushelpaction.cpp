#include "edionysushelpaction.h"

eDionysusHelpAction::eDionysusHelpAction(eCharacter* const c) :
    eProvideResourceHelpAction(c, eCharActionType::dionysusHelpAction,
                               eResourceType::wine, 16) {}

bool eDionysusHelpAction::sHelpNeeded(const eCityId cid,
                                      const GameBoard& board) {
    return eProvideResourceHelpAction::sHelpNeeded(
                cid, board, eResourceType::wine, 8);
}

