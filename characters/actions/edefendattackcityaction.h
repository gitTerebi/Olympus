#ifndef EDEFENDATTACKCITYACTION_H
#define EDEFENDATTACKCITYACTION_H

#include "egodmonsteraction.h"

enum class eDefendAttackCityStage {
    none, appear, goTo, wait, fight, comeback, disappear
};

class eDefendAttackCityAction : public eGodMonsterAction {
public:
    using eGodMonsterAction::eGodMonsterAction;

    void increment(const int by) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
protected:
    int maxKilled() const { return mMaxKilled; }
    void setMaxKilled(const int max) { mMaxKilled = max; }

    int killed() const { return mKilled; }

    void goBack();
    bool goTo(const int fx, const int fy, const int dist);
    int range() const;

    eDefendAttackCityStage mStage{eDefendAttackCityStage::none};
    eTile* mStartTile = nullptr;
private:

    stdptr<eCharacter> mAttackTarget;
    bool mAttack = false;
    int mLookForEnemy = 0;
    int mAttackTime = 0;
    int mRangeAttack = 0;
    double mAngle{0.};
    int mMissile = 0;
    int mMaxKilled = 6;
    int mKilled = 0;
};

#endif // EDEFENDATTACKCITYACTION_H
