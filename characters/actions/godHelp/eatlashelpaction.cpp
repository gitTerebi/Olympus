#include "eatlashelpaction.h"

#include "etilehelper.h"
#include "fileIO/esavearchive.h"

eAtlasHelpAction::eAtlasHelpAction(
        eCharacter* const c) :
    eGodAction(c, eCharActionType::atlasHelpAction) {}

bool eAtlasHelpAction::decide() {
    switch(mStage) {
    case eAtlasHelpStage::none:
        mStage = eAtlasHelpStage::appear;
        appear();
        break;
    case eAtlasHelpStage::appear:
        mStage = eAtlasHelpStage::goTo;
        goToTarget();
        break;
    case eAtlasHelpStage::goTo:
        mStage = eAtlasHelpStage::give;
        give();
        break;
    case eAtlasHelpStage::give:
        mStage = eAtlasHelpStage::disappear;
        disappear();
        break;
    case eAtlasHelpStage::giving:
        if(!mTarget) {
            finishGiving();
            return decide();
        }
        spawnGiveMissile(mTarget.get());
        break;
    case eAtlasHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eAtlasHelpAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.buildingAsField("target", &board(), mTarget);
    ar.field("preGivingStage", mPreGivingStage);
}

void eAtlasHelpAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eAtlasHelpAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eAtlasHelpStage::giving:
        if(!mTarget) {
            finishGiving();
            eGodAction::resumeFromSavedState();
            return;
        }
        decide();
        return;
    case eAtlasHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eAtlasHelpStage::appear:
        appear();
        return;
    case eAtlasHelpStage::goTo:
        goToTarget();
        return;
    case eAtlasHelpStage::give:
        decide();
        return;
    case eAtlasHelpStage::disappear:
        disappear();
        return;
    }
}

void eAtlasHelpAction::finishGiving() {
    mStage = mPreGivingStage;
    mPreGivingStage = eAtlasHelpStage::none;
}

bool eAtlasHelpAction::sHelpNeeded(const eCityId cid,
                                   const eGameBoard& board) {
    eMonument* b = nullptr;
    const int r = board.maxMonumentSpaceForResource(cid, &b);
    return r > 0;
}

void eAtlasHelpAction::goToTarget() {
    auto& board = this->board();
    using eGTTT = eGoToTargetTeleport;
    const auto tele = std::make_shared<eGTTT>(board, this);
    eMonument* target = nullptr;
    const auto cid = cityId();
    board.maxMonumentSpaceForResource(cid, &target);
    mTarget = target;
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

void eAtlasHelpAction::give() {
    if(!mTarget) return;
    mPreGivingStage = mStage;
    mStage = eAtlasHelpStage::giving;
    pauseAction();
    spawnGiveMissile(mTarget.get());
}

void eAtlasHelpAction::spawnGiveMissile(eMonument* const target) {
    const auto c = character();
    const auto targetTile = target->centerTile();
    const auto finishCb = std::make_shared<eAtHA_giveFinish>(board(), this);
    const auto act = std::make_shared<eAtlasHelpAct>(board(), target);
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, act,
                    finishCb);
}
