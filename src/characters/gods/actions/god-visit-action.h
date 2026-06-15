#ifndef EGODVISITACTION_H
#define EGODVISITACTION_H

#include "characters/gods/actions/god-action.h"

class SaveArchive;

enum class GodVisitStage {
    none, appear, patrol, disappear
};

class GodVisitAction : public eGodAction {
public:
    GodVisitAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();

    GodVisitStage mStage{GodVisitStage::none};

    int mLookForBless = 0;
    int mLookForSoldierAttack = 0;
};

#endif // EGODVISITACTION_H
