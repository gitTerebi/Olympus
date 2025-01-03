#ifndef EDEMETERHELPACTION_H
#define EDEMETERHELPACTION_H

#include "eprovideresourcehelpaction.h"

class eDemeterHelpAction : public eProvideResourceHelpAction {
public:
    eDemeterHelpAction(eCharacter* const c);

    static bool sHelpNeeded(const eCityId cid,
                            const eGameBoard& board);
};

#endif // EDEMETERHELPACTION_H
