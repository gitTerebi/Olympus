#ifndef EATTACKCITYACTION_H
#define EATTACKCITYACTION_H

#include "edefendattackcityaction.h"
#include "erand.h"

class eSaveArchive;

class eAttackCityAction : public eDefendAttackCityAction {
public:
    eAttackCityAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    void invasionFinished();
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool goToNearestSoldier();

    int mLookForGod = eRand::rand() % 2000;
};

#endif // EATTACKCITYACTION_H
