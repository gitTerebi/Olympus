#ifndef ECOMPLEXACTION_H
#define ECOMPLEXACTION_H

#include "echaracteraction.h"

class eSaveArchive;

class eComplexAction : public eCharacterAction {
public:
    using eCharacterAction::eCharacterAction;

    virtual bool decide() = 0;

    void increment(const int by) override;

    eCharacterAction* currentAction() const { return mCurrentAction.get(); }
    const stdsptr<eCharacterAction>& currentActionPtr() const { return mCurrentAction; }
    void setCurrentAction(const stdsptr<eCharacterAction>& a);
protected:
    void wait(const int t);
    void wait();
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;

private:
    stdsptr<eCharacterAction> mCurrentAction;
};

#endif // ECOMPLEXACTION_H
