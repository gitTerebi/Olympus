#ifndef EREPLACECATTLEACTION_H
#define EREPLACECATTLEACTION_H

#include "eactionwithcomeback.h"

#include "characters/echaracter.h"
#include "characters/actions/eanimalaction.h"

class eSaveArchive;

enum class eReplaceCattleActionStage {
    idle, goingToCattle, goingBack
};

class eReplaceCattleAction : public eActionWithComeback {
public:
    eReplaceCattleAction(eCharacter* const c,
                         eCharacter* const cc = nullptr);

    bool decide() override;
    void finishReplacing();
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
private:
    void goCattle();
    void sendCattleHome();

    stdptr<eCharacter> mCattle;
    eReplaceCattleActionStage mStage = eReplaceCattleActionStage::idle;
    int mCattleHomeX = 0;
    int mCattleHomeY = 0;
};

class eRC_finishAction : public eCharActFunc {
public:
    eRC_finishAction(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::RC_finishAction) {}
    eRC_finishAction(GameBoard& board,
                     eReplaceCattleAction* const a,
                     eCharacter* const c,
                     eCharacter* const cc) :
        eCharActFunc(board, eCharActFuncType::RC_finishAction),
        mButcherA(a), mButcher(c), mCattle(cc) {}

    void call() override;

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterActionAsField("butcherAction", &board(), mButcherA);
        ar.characterField("butcher", &board(), mButcher);
        ar.characterField("cattle", &board(), mCattle);
    }
private:
    stdptr<eReplaceCattleAction> mButcherA;
    stdptr<eCharacter> mButcher;
    stdptr<eCharacter> mCattle;
};

class eRC_finishWalkingAction : public eCharActFunc {
public:
    eRC_finishWalkingAction(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::RC_finishWalkingAction) {}
    eRC_finishWalkingAction(GameBoard& board,
                            eCharacter* const c) :
        eCharActFunc(board, eCharActFuncType::RC_finishWalkingAction),
        mCattle(c) {}

    void call() override;

protected:
    void serializeFields(eSaveArchive& ar) override {
        ar.characterField("cattle", &board(), mCattle);
    }
private:
    stdptr<eCharacter> mCattle;
};

#endif // EREPLACECATTLEACTION_H
