#ifndef EDEFENDCITYACTION_H
#define EDEFENDCITYACTION_H

#include "edefendattackcityaction.h"

class eInvasionEvent;
class eSaveArchive;

class eDefendCityAction : public eDefendAttackCityAction {
public:
    eDefendCityAction(eCharacter* const c);

    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    void serialize(eSaveArchive& ar);

    void goToTarget();
    bool goToNearestSoldier();

    stdptr<eInvasionEvent> mEvent;
};

#endif // EDEFENDCITYACTION_H
