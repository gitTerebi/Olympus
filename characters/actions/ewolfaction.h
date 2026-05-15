#ifndef EWOLFACTION_H
#define EWOLFACTION_H

#include "eanimalaction.h"

class eWolfAction : public eAnimalAction
{
public:
    eWolfAction(eCharacter *const c, const int spawnerX, const int spawnerY);
    eWolfAction(eCharacter *const c);

    void increment(const int by) override;
    bool decide() override;

    void write(eWriteStream &dst) const override;
    void read(eReadStream &src) override;

public:
    void findPrey();

private:
    void goBack();
    void attackWall(eBuilding *wall);

    bool mHunting = false;
    stdptr<eBuilding> mWallTarget;
};

#endif // EWOLFACTION_H
