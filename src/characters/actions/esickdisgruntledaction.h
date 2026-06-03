#ifndef ESICKDISGRUNTLEDACTION_H
#define ESICKDISGRUNTLEDACTION_H

#include "eactionwithcomeback.h"

class SmallHouse;
class eSaveArchive;

enum class eSickDisgruntledActionStage {
    idle, patrolling, goingBack
};

class eSickDisgruntledAction : public eActionWithComeback {
public:
    eSickDisgruntledAction(eCharacter* const c,
                           SmallHouse* const ch);

    bool decide() override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    void patrol();
    void goBackDecision(const stdsptr<WalkableObject>& w =
                            WalkableObject::sCreateRoadAvenue());

    SmallHouse* mBuilding = nullptr;

    bool mGoBackNext = false;
    eSickDisgruntledActionStage mStage = eSickDisgruntledActionStage::idle;
};

#endif // ESICKDISGRUNTLEDACTION_H
