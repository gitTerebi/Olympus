#ifndef EWOLFACTION_H
#define EWOLFACTION_H

#include "eanimalaction.h"

enum class eWolfActionStage {
    idle, hunting, goingBack, attackingWall
};

class eWolfAction : public eAnimalAction
{
public:
    eWolfAction(eCharacter *const c, const int spawnerX, const int spawnerY);
    eWolfAction(eCharacter *const c);

    void increment(const int by) override;
    bool decide() override;

    void findPrey();
    void retaliate(eCharacter* attacker);
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;

private:
    void goBack();
    void attackWall(eBuilding *wall);
    void moveToRetaliationTarget();
    bool canAttackRetaliationTarget() const;

    bool mHunting = false;
    eWolfActionStage mStage = eWolfActionStage::idle;
    stdptr<eBuilding> mWallTarget;
    stdptr<eCharacter> mRetaliationTarget;
};

#endif // EWOLFACTION_H
