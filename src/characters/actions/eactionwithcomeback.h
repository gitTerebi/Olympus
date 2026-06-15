#ifndef EACTIONWITHCOMEBACK_H
#define EACTIONWITHCOMEBACK_H

#include "ecomplexaction.h"
#include "emovepathaction.h"
#include "engine/emovedirection.h"
#include "walkable/walkable-object.h"

#include <SDL2/SDL_rect.h>

class eBuilding;
class SaveArchive;

using eWalkable = std::function<bool(eTileBase*)>;

class eActionWithComeback : public eComplexAction {
    friend class eAWC_goBackFail;
public:
    eActionWithComeback(eCharacter* const c,
                        const eCharActionType type);
    eActionWithComeback(eCharacter* const c,
                        eTile* const startTile,
                        const eCharActionType type);

    bool decide() override;

    void setFinishOnComeback(const bool b)
    { mFinishOnComeback = b; }

    void setDiagonalOnly(const bool d)
    { mDiagonalOnly = d; }

    void goBack(stdsptr<WalkableObject> walkable);
    void goBack(eBuilding* const b,
                const stdsptr<WalkableObject>& walkable);
    void goBack(const SDL_Rect& rect,
                const stdsptr<WalkableObject>& walkable);

    eTile* startTile() const { return mStartTile; }
    void setStartTile(eTile* const t) { mStartTile = t; }
protected:
    void serializeFields(SaveArchive& ar) override;

private:
    void goBackInternal(stdsptr<WalkableObject> walkable);
    void teleportDecision();

    eTile* mStartTile = nullptr;

    SDL_Rect mGoBackRect{0, 0, 0, 0};

    bool mDiagonalOnly = false;
    bool mFinishOnComeback = false;
    bool mDefaultTry = false;
    bool mGoBackFail = false;
};

class eAWC_goBackFail : public eCharActFunc {
public:
    eAWC_goBackFail(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::AWC_goBackFail) {}
    eAWC_goBackFail(GameBoard& board,
                    eActionWithComeback* const t,
                    const stdsptr<WalkableObject>& walkable) :
        eCharActFunc(board, eCharActFuncType::AWC_goBackFail),
        mTptr(t), mWalkable(walkable) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->goBackInternal(mWalkable);
    }
protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
        ar.walkableField("walkable", mWalkable);
    }
private:
    stdptr<eActionWithComeback> mTptr;
    stdsptr<WalkableObject> mWalkable;
};

class eAWC_goBackFinish : public eCharActFunc {
public:
    eAWC_goBackFinish(GameBoard& board) :
        eCharActFunc(board, eCharActFuncType::AWC_goBackFinish) {}
    eAWC_goBackFinish(GameBoard& board, eActionWithComeback* const t) :
        eCharActFunc(board, eCharActFuncType::AWC_goBackFinish),
        mTptr(t) {}

    void call() override {
        if(!mTptr) return;
        const auto t = mTptr.get();
        t->setState(eCharacterActionState::finished);
    }
protected:
    void serializeFields(SaveArchive& ar) override {
        ar.characterActionAsField("target", &board(), mTptr);
    }
private:
    stdptr<eActionWithComeback> mTptr;
};

#endif // EACTIONWITHCOMEBACK_H
