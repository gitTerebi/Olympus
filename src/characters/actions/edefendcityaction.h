#ifndef EDEFENDCITYACTION_H
#define EDEFENDCITYACTION_H

#include "edefendattackcityaction.h"

class eInvasionEvent;
class eSaveArchive;

class eDefendCityAction : public eDefendAttackCityAction {
public:
    eDefendCityAction(eCharacter* const c);

    bool decide() override;
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    void goToTarget();
    bool goToNearestSoldier();

    stdptr<eInvasionEvent> mEvent;
};

#endif // EDEFENDCITYACTION_H
