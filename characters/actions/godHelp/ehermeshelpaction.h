#ifndef EHERMESHELPACTION_H
#define EHERMESHELPACTION_H

#include "characters/gods/actions/god-action.h"

class eSaveArchive;

enum class eHermesHelpStage {
    none, appear, provide, providing, disappear
};

class eHermesHelpAction : public eGodAction {
public:
    eHermesHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const ePlayerId pid,
                            const eGameBoard& board);

    void rebuildCurrentStage();
    void finishProviding();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void provide();
    void spawnProvideMissile();

    eHermesHelpStage mStage{eHermesHelpStage::none};
    eHermesHelpStage mPreProvidingStage{eHermesHelpStage::none};
    bool mRequestFulfilled = false;
};

class eHmHA_provideFinish : public eCharActFunc {
public:
    eHmHA_provideFinish(eGameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HmHA_provideFinish) {}
    eHmHA_provideFinish(eGameBoard& board, eHermesHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HmHA_provideFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eHermesHelpAction> t = mTptr;
        if(!t) return;
        t->finishProviding();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eHermesHelpAction> mTptr;
};

#endif // EHERMESHELPACTION_H
