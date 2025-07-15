#ifndef EDEFENDCITYACTION_H
#define EDEFENDCITYACTION_H

#include "edefendattackcityaction.h"

class eInvasionEvent;

class eDefendCityAction : public eDefendAttackCityAction {
public:
    eDefendCityAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    void goToTarget();
    bool goToNearestSoldier();

    stdptr<eInvasionEvent> mEvent;
};

#endif // EDEFENDCITYACTION_H
