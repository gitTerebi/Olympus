#ifndef EATTACKCITYACTION_H
#define EATTACKCITYACTION_H

#include "edefendattackcityaction.h"

class eAttackCityAction : public eDefendAttackCityAction {
public:
    eAttackCityAction(eCharacter* const c);

    bool decide() override;

    void invasionFinished();
private:
    bool goToNearestSoldier();
};

#endif // EATTACKCITYACTION_H
