#ifndef EDEFENDATTACKCITYACTION_H
#define EDEFENDATTACKCITYACTION_H

#include "characters/gods/actions/god-monster-action.h"

class eSaveArchive;

enum class eDefendAttackCityStage {
    none, appear, goTo, wait, fight, comeback, disappear
};

class eDefendAttackCityAction : public GodMonsterAction {
public:
    using GodMonsterAction::GodMonsterAction;

    void increment(const int by) override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
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
    int mMeleeTime = 0;
    int mMaxKilled = 6;
    int mKilled = 0;
};

#endif // EDEFENDATTACKCITYACTION_H
