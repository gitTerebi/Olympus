#ifndef EHEPHAESTUSHELPACTION_H
#define EHEPHAESTUSHELPACTION_H

#include "characters/gods/actions/god-action.h"

class SaveArchive;

enum class eHephaestusHelpStage {
    none, appear, provide, providing, disappear
};

class eHephaestusHelpAction : public eGodAction {
public:
    eHephaestusHelpAction(eCharacter* const c);

    bool decide() override;

    static bool sHelpNeeded(const eCityId cid,
                            const GameBoard& board);

    void rebuildCurrentStage();
    void finishProviding();
protected:
    void serializeFields(SaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void provide();
    void spawnProvideMissile();

    eHephaestusHelpStage mStage{eHephaestusHelpStage::none};
    eHephaestusHelpStage mPreProvidingStage{eHephaestusHelpStage::none};
    bool mTalosSpawned = false;
};

class eHfHA_provideFinish : public eCharActFunc {
public:
    eHfHA_provideFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::HfHA_provideFinish) {}
    eHfHA_provideFinish(GameBoard& board, eHephaestusHelpAction* const ca) :
        eCharActFunc(board, eCharActFuncType::HfHA_provideFinish),
        mTptr(ca) {}

    void call() override {
        const stdptr<eHephaestusHelpAction> t = mTptr;
        if(!t) return;
        t->finishProviding();
        t->resumeAction();
        if(t && !t->currentAction()) t->rebuildCurrentStage();
    }

protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eHephaestusHelpAction> mTptr;
};

#endif // EHEPHAESTUSHELPACTION_H
