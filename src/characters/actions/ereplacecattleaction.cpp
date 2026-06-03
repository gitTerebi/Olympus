#include "ereplacecattleaction.h"

#include "emovetoaction.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

eReplaceCattleAction::eReplaceCattleAction(
        eCharacter* const c, eCharacter* const cc) :
    eActionWithComeback(c, eCharActionType::replaceCattleAction),
    mCattle(cc) {
    setFinishOnComeback(true);
}

bool eReplaceCattleAction::decide() {
    goCattle();
    return true;
}

void eReplaceCattleAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.characterField("cattle", &board(), mCattle);
    ar.field("stage", mStage);
    ar.field("cattleHomeX", mCattleHomeX, 0);
    ar.field("cattleHomeY", mCattleHomeY, 0);
}

void eReplaceCattleAction::resumeFromSavedState() {
    switch(mStage) {
    case eReplaceCattleActionStage::idle:
        eActionWithComeback::resumeFromSavedState();
        break;
    case eReplaceCattleActionStage::goingToCattle:
        goCattle();
        break;
    case eReplaceCattleActionStage::goingBack:
        sendCattleHome();
        goBack(WalkableObject::sCreateDefault());
        break;
    }
}

void eReplaceCattleAction::goCattle() {
    const auto c = character();
    mStage = eReplaceCattleActionStage::goingToCattle;

    const auto ca = dynamic_cast<AnimalAction*>(mCattle ? mCattle->action() : nullptr);
    if(!ca) return;
    mCattleHomeX = ca->spawnerX();
    mCattleHomeY = ca->spawnerY();
    const auto homeTile = board().tile(mCattleHomeX, mCattleHomeY);

    const auto a = e::make_shared<eMoveToAction>(c);
    const stdptr<eCharacter> cptr(c);
    a->setFoundAction([cptr, c]() {
        if(cptr) c->setActionType(eCharacterActionType::walk);
    });
    const stdptr<eReplaceCattleAction> tptr(this);
    const auto findFailFunc = [tptr, this]() {
        if(tptr) goBack(WalkableObject::sCreateDefault());
    };
    a->setFindFailAction(findFailFunc);
    const auto finish = std::make_shared<eRC_finishAction>(
                            board(), this, c, mCattle);
    a->setFinishAction(finish);
    if(homeTile) a->start(homeTile);
    else a->setState(eCharacterActionState::failed);
    setCurrentAction(a);
}

void eReplaceCattleAction::finishReplacing() {
    mStage = eReplaceCattleActionStage::goingBack;
    goBack(WalkableObject::sCreateDefault());
    sendCattleHome();
}

void eReplaceCattleAction::sendCattleHome() {
    if(!mCattle) return;
    const auto c = mCattle.get();
    const auto homeTile = board().tile(mCattleHomeX, mCattleHomeY);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);

    const auto finish = std::make_shared<eRC_finishWalkingAction>(
                            board(), c);
    a->setFinishAction(finish);
    if(homeTile) a->start(homeTile);
    else a->setState(eCharacterActionState::failed);
    c->setAction(a);
}

void eRC_finishAction::call() {
    if(mButcherA) {
        mButcherA->finishReplacing();
        return;
    }
    if(!mCattle) return;
    const auto c = mCattle.get();

    const auto ca = dynamic_cast<AnimalAction*>(c->action());
    if(!ca) return;
    const auto homeTile = board().tile(ca->spawnerX(), ca->spawnerY());

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::domesticatedAnimals |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);

    const auto finish = std::make_shared<eRC_finishWalkingAction>(
                            board(), c);
    a->setFinishAction(finish);
    if(homeTile) a->start(homeTile);
    else a->setState(eCharacterActionState::failed);
    c->setAction(a);
}

void eRC_finishWalkingAction::call() {
    const auto t = mCattle->tile();
    if(!t) return;
    const int tx = t->x();
    const int ty = t->y();
    const auto walkable = WalkableObject::sCreateFertile();
    const auto a = e::make_shared<AnimalAction>(
                       mCattle, tx, ty, walkable);
    mCattle->setAction(a);
}
