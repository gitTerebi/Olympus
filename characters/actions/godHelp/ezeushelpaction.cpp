#include "ezeushelpaction.h"
#include "fileIO/esavearchive.h"

#include "../ewaitaction.h"
#include "gameEvents/invasions/invasion-handler.h"

eZeusHelpAction::eZeusHelpAction(
        eCharacter* const c) :
    eGodAction(c, eCharActionType::zeusHelpAction) {}

bool eZeusHelpAction::decide() {
    switch(mStage) {
    case eZeusHelpStage::none:
        mStage = eZeusHelpStage::appear;
        appear();
        break;
    case eZeusHelpStage::appear:
        mStage = eZeusHelpStage::kill;
        kill();
        break;
    case eZeusHelpStage::kill:
        mStage = eZeusHelpStage::killing;
        spawnKillWait();
        break;
    case eZeusHelpStage::killing:
        mStage = eZeusHelpStage::disappear;
        disappear();
        break;
    case eZeusHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eZeusHelpAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
}

void eZeusHelpAction::resumeFromSavedState() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eZeusHelpStage::killing:
        spawnKillWait();
        return;
    case eZeusHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eZeusHelpStage::appear:
        appear();
        return;
    case eZeusHelpStage::kill:
        decide();
        return;
    case eZeusHelpStage::disappear:
        disappear();
        return;
    }
}

bool eZeusHelpAction::sHelpNeeded(const eCityId cid,
                                  const eGameBoard& board) {
    const auto& ivs = board.invasionHandlers(cid);
    return !ivs.empty();
}

void eZeusHelpAction::kill() {
    spawnKillWait();
}

void eZeusHelpAction::spawnKillWait() {
    const auto c = character();
    c->setActionType(eCharacterActionType::fight2);
    const auto a = e::make_shared<eWaitAction>(c);
    using eF = eZHA_killFinish;
    const auto cid = cityId();
    const auto finish = std::make_shared<eF>(board(), cid);
    a->setFailAction(finish);
    a->setFinishAction(finish);
    a->setTime(500);
    setCurrentAction(a);
}

void eZHA_killFinish::call() {
    const auto& b = board();
    const auto& ivs = b.invasionHandlers(mCid);
    for(const auto iv : ivs) {
        iv->killAllWithCorpse();
    }
}
