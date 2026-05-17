#include "ehadeshelpaction.h"

#include "etilehelper.h"
#include "buildings/epalace.h"
#include "fileIO/esavearchive.h"

eHadesHelpAction::eHadesHelpAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::hadesHelpAction) {}

bool eHadesHelpAction::decide() {
    switch(mStage) {
    case eHadesHelpStage::none:
        mStage = eHadesHelpStage::appear;
        appear();
        break;
    case eHadesHelpStage::appear:
        mStage = eHadesHelpStage::goTo;
        goToTarget();
        break;
    case eHadesHelpStage::goTo:
        mStage = eHadesHelpStage::give;
        give();
        break;
    case eHadesHelpStage::give:
        mStage = eHadesHelpStage::disappear;
        disappear();
        break;
    case eHadesHelpStage::giving:
        if(!mTarget) {
            finishGiving();
            return decide();
        }
        spawnGiveMissile(mTarget.get());
        break;
    case eHadesHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eHadesHelpAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.buildingField("target", &board(), mTarget);
    ar.field("preGivingStage", mPreGivingStage);
}

void eHadesHelpAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eHadesHelpAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eHadesHelpStage::giving:
        if(!mTarget) {
            finishGiving();
            eGodAction::resumeFromSavedState();
            return;
        }
        decide();
        return;
    case eHadesHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eHadesHelpStage::appear:
        appear();
        return;
    case eHadesHelpStage::goTo:
        goToTarget();
        return;
    case eHadesHelpStage::give:
        decide();
        return;
    case eHadesHelpStage::disappear:
        disappear();
        return;
    }
}

void eHadesHelpAction::finishGiving() {
    mStage = mPreGivingStage;
    mPreGivingStage = eHadesHelpStage::none;
}

bool eHadesHelpAction::sHelpNeeded(const eCityId cid,
                                   const eGameBoard& board) {
    return board.hasPalace(cid);
}

void eHadesHelpAction::goToTarget() {
    auto& board = this->board();
    using eGTTT = eGoToTargetTeleport;
    const auto tele = std::make_shared<eGTTT>(board, this);
    const auto cid = cityId();
    mTarget = board.palace(cid);
    if(mTarget) {
        const auto ct = mTarget->centerTile();
        const int tx = ct->x();
        const int ty = ct->y();
        const auto tile = eTileHelper::closestRoad(tx, ty, board);
        goToTile(tile, tele);
    } else {
        disappear();
    }
}

void eHadesHelpAction::give() {
    if(!mTarget) return;
    mPreGivingStage = mStage;
    mStage = eHadesHelpStage::giving;
    pauseAction();
    spawnGiveMissile(mTarget.get());
}

void eHadesHelpAction::spawnGiveMissile(eBuilding* const target) {
    const auto c = character();
    const auto targetTile = target->centerTile();
    const auto finishCb = std::make_shared<eHdHA_giveFinish>(board(), this);
    using eGPDA = eGodProvideDrachmasAct;
    const auto cid = target->cityId();
    const auto act = std::make_shared<eGPDA>(board(), cid);
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, act,
                    finishCb);
}
