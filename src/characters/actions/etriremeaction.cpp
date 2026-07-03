#include "etriremeaction.h"

#include "buildings/ebuilding.h"
#include "ekillcharacterfinishfail.h"
#include "characters/actions/emovetoaction.h"
#include "buildings/etriremewharf.h"
#include "characters/etrireme.h"
#include "engine/game-board.h"
#include "fileIO/save-archive.h"

eTriremeAction::eTriremeAction(eTriremeWharf* const home,
                               eCharacter* const trireme) :
    FightingAction(trireme, eCharActionType::triremeAction),
    mHome(home) {}

bool eTriremeAction::decide() {
    return true;
}

void eTriremeAction::increment(const int by) {
    const auto r = lookForEnemy(by);
    if(r == LookForEnemyState::dead) return;
    eComplexAction::increment(by);
    if(!currentAction() &&
       mHome && !mHome->abroad() && mHome->isAtWharf()) {
        const auto c = character();
        c->setActionType(eCharacterActionType::stand);
        mHome->dockTrireme(static_cast<eTrireme*>(c));
        mStage = eTriremeActionStage::idle;
    }
}

void eTriremeAction::serializeFields(SaveArchive& ar) {
    FightingAction::serializeFields(ar);
    ar.buildingAsField("home", &board(), mHome);
    ar.field("stage", mStage);
    ar.field("leavingNotified", mLeavingNotified);
}

void eTriremeAction::resumeFromSavedState() {
    if(isAttacking()) {
        FightingAction::resumeFromSavedState();
        return;
    }
    switch(mStage) {
    case eTriremeActionStage::idle:
        break;
    case eTriremeActionStage::home:
        goHome();
        break;
    case eTriremeActionStage::abroad:
        goAbroad();
        break;
    }
}

void eTriremeAction::goHome() {
    const auto c = character();
    mStage = eTriremeActionStage::home;

    const auto tile = mHome->triremeTile();
    const stdptr<eCharacter> cptr(c);

    const auto a = e::make_shared<eMoveToAction>(cptr.get());
    a->setStateRelevance(eStateRelevance::terrain);
    a->setFoundAction([cptr]() {
        if(!cptr) return;
        cptr->setActionType(eCharacterActionType::walk);
    });
    a->start(tile, WalkableObject::sCreateDeepWater());
    setCurrentAction(a);
}

eTile* eTriremeAction::exitPoint() const {
    auto& board = eTriremeAction::board();
    const auto cid = cityId();
    return board.riverExitPoint(cid);
}

void eTriremeAction::markLeaving() {
    if(mLeavingNotified || !mHome) return;
    const auto c = character();
    auto& board = FightingAction::board();
    const auto trireme = static_cast<eTrireme*>(c);
    board.deselectTrireme(trireme);
    mHome->triremeLeaving();
    mLeavingNotified = true;
}

void eTriremeAction::goAbroad() {
    const auto c = character();
    auto& board = FightingAction::board();
    const auto trireme = static_cast<eTrireme*>(c);
    mStage = eTriremeActionStage::abroad;
    markLeaving();
    const stdptr<eTrireme> cptr(trireme);
    const auto fail = std::make_shared<eKillCharacterFinishFail>(
        board, trireme);
    const auto finish = std::make_shared<eKillCharacterFinishFail>(
        board, trireme);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::terrain);
    a->setFailAction(fail);
    a->setFinishAction(finish);
    a->setFindFailAction([cptr]() {
        if(cptr) cptr->kill();
    });
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);

    const auto exitPoint = eTriremeAction::exitPoint();
    a->start(exitPoint, WalkableObject::sCreateDeepWater());
}

eTriremeWharf *eTriremeAction::home() const {
    return mHome.get();
}
