#ifndef EHUNTACTION_H
#define EHUNTACTION_H

#include "eactionwithcomeback.h"

#include "characters/ehunter.h"

class eHuntingLodge;
class eSaveArchive;

enum class eHuntActionStage {
    idle, findingResource, goingBack, waiting
};

class eHuntAction : public eActionWithComeback {
public:
    eHuntAction(eHuntingLodge* const b,
                eCharacter* const c);
    eHuntAction(eCharacter* const c);

    void increment(const int by) override;
    bool decide() override;
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void rebuildCurrentStage();
    void findResourceDecision();
    void goBackDecision();
    void waitDecision();

    eHuntingLodge* mLodge = nullptr;
    eHunter* mHunter = nullptr;

    bool mNoResource = false;
    eHuntActionStage mStage = eHuntActionStage::idle;
    int mWaitRemaining = 0;
};

#endif // EHUNTACTION_H
