#include "eathenahelpaction.h"

eAthenaHelpAction::eAthenaHelpAction(eCharacter* const c) :
    eProvideResourceHelpAction(c, eCharActionType::athenaHelpAction,
                               eResourceType::oliveOil, 16) {}

bool eAthenaHelpAction::sHelpNeeded(const eCityId cid,
                                    const eGameBoard& board) {
    return eProvideResourceHelpAction::sHelpNeeded(
                cid, board, eResourceType::oliveOil, 8);
}

