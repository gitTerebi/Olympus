#ifndef EGODVISITACTION_H
#define EGODVISITACTION_H

#include "characters/gods/actions/god-action.h"

class eSaveArchive;

enum class eGodVisitStage {
    none, appear, patrol, disappear
};

class eGodVisitAction : public eGodAction {
public:
    eGodVisitAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();

    eGodVisitStage mStage{eGodVisitStage::none};

    int mLookForBless = 0;
    int mLookForSoldierAttack = 0;
};

#endif // EGODVISITACTION_H
