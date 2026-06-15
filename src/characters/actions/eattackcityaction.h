#ifndef EATTACKCITYACTION_H
#define EATTACKCITYACTION_H

#include "edefendattackcityaction.h"
#include "rand.h"

class SaveArchive;

class eAttackCityAction : public eDefendAttackCityAction {
public:
    eAttackCityAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;

    void invasionFinished();
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    bool goToNearestSoldier();

    int mLookForGod = Rand::rand() % 2000;
};

#endif // EATTACKCITYACTION_H
