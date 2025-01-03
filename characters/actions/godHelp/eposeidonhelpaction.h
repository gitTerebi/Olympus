#ifndef EPOSEIDONHELPACTION_H
#define EPOSEIDONHELPACTION_H

#include "eprovideresourcehelpaction.h"

class ePoseidonHelpAction : public eProvideResourceHelpAction {
public:
    ePoseidonHelpAction(eCharacter* const c);

    static bool sHelpNeeded(const eCityId cid, const eGameBoard& board);
};

#endif // EPOSEIDONHELPACTION_H
