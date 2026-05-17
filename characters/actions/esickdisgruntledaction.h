#ifndef ESICKDISGRUNTLEDACTION_H
#define ESICKDISGRUNTLEDACTION_H

#include "eactionwithcomeback.h"

class eSmallHouse;
class eSaveArchive;

enum class eSickDisgruntledActionStage {
    idle, patrolling, goingBack
};

class eSickDisgruntledAction : public eActionWithComeback {
public:
    eSickDisgruntledAction(eCharacter* const c,
                           eSmallHouse* const ch);

    bool decide() override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    void patrol();
    void goBackDecision(const stdsptr<eWalkableObject>& w =
                            eWalkableObject::sCreateRoadAvenue());

    eSmallHouse* mBuilding = nullptr;

    bool mGoBackNext = false;
    eSickDisgruntledActionStage mStage = eSickDisgruntledActionStage::idle;
};

#endif // ESICKDISGRUNTLEDACTION_H
