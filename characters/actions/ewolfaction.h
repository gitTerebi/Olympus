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
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;

private:
    void goBack();
    void attackWall(eBuilding *wall);

    bool mHunting = false;
    eWolfActionStage mStage = eWolfActionStage::idle;
    stdptr<eBuilding> mWallTarget;
};

#endif // EWOLFACTION_H
