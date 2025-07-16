#ifndef EATTACKCITYACTION_H
#define EATTACKCITYACTION_H

#include "edefendattackcityaction.h"

class eAttackCityAction : public eDefendAttackCityAction {
public:
    eAttackCityAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    void invasionFinished();
private:
    bool goToNearestSoldier();

    int mLookForGod = eRand::rand() % 2000;
};

#endif // EATTACKCITYACTION_H
