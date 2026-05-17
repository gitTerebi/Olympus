#include "eprovideresourcehelpaction.h"

#include "etilehelper.h"
#include "fileIO/esavearchive.h"

eProvideResourceHelpAction::eProvideResourceHelpAction(
        eCharacter* const c,
        const eCharActionType type,
        const eResourceType res,
        const int count) :
    eGodAction(c, type),
    mResource(res),
    mCount(count) {}

bool eProvideResourceHelpAction::decide() {
    switch(mStage) {
    case eProvideResourceHelpStage::none:
        mStage = eProvideResourceHelpStage::appear;
        appear();
        break;
    case eProvideResourceHelpStage::appear:
        mStage = eProvideResourceHelpStage::goTo;
        goToTarget();
        break;
    case eProvideResourceHelpStage::goTo:
        mStage = eProvideResourceHelpStage::give;
        give();
        break;
    case eProvideResourceHelpStage::give: {
        if(mCount > 0) {
            mStage = eProvideResourceHelpStage::goTo;
            goToTarget();
        } else {
            mStage = eProvideResourceHelpStage::disappear;
            disappear();
        }
    } break;
    case eProvideResourceHelpStage::giving:
        if(!mTarget) {
            finishGiving();
            return decide();
        }
        spawnGiveMissile();
        break;
    case eProvideResourceHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eProvideResourceHelpAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.buildingAsField("target", &board(), mTarget);
    ar.field("resource", mResource);
    ar.field("count", mCount);
    ar.field("preGivingStage", mPreGivingStage);
}

void eProvideResourceHelpAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eProvideResourceHelpAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eProvideResourceHelpStage::giving:
        if(!mTarget) {
            finishGiving();
            eGodAction::resumeFromSavedState();
            return;
        }
        decide();
        return;
    case eProvideResourceHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eProvideResourceHelpStage::appear:
        appear();
        return;
    case eProvideResourceHelpStage::goTo:
        goToTarget();
        return;
    case eProvideResourceHelpStage::give:
        decide();
        return;
    case eProvideResourceHelpStage::disappear:
        disappear();
        return;
    }
}

void eProvideResourceHelpAction::finishGiving() {
    mStage = mPreGivingStage;
    mPreGivingStage = eProvideResourceHelpStage::none;
}

void eProvideResourceHelpAction::decCount(const int by) {
    mCount -= by;
}

bool eProvideResourceHelpAction::sHelpNeeded(const eCityId cid,
                                             const eGameBoard& board,
                                             const eResourceType res,
                                             const int minSpace) {
    const int r = board.spaceForResource(cid, res);
    return r > minSpace;
}

void eProvideResourceHelpAction::goToTarget() {
    auto& board = this->board();
    using eGTTT = eGoToTargetTeleport;
    const auto tele = std::make_shared<eGTTT>(board, this);
    eStorageBuilding* target = nullptr;
    const auto cid = cityId();
    board.maxSingleSpaceForResource(cid, mResource, &target);
    mTarget = target;
    if(mTarget) {
        const auto ct = mTarget->centerTile();
        const int tx = ct->x();
        const int ty = ct->y();
        const auto tile = eTileHelper::closestRoad(tx, ty, board);
        goToTile(tile, tele);
    } else {
        mStage = eProvideResourceHelpStage::disappear;
        disappear();
    }
}

void eProvideResourceHelpAction::give() {
    if(!mTarget) return;
    mPreGivingStage = mStage;
    mStage = eProvideResourceHelpStage::giving;
    pauseAction();
    spawnGiveMissile();
}

void eProvideResourceHelpAction::spawnGiveMissile() {
    if(!mTarget) return;
    const auto c = character();
    const auto targetTile = mTarget->centerTile();
    const auto finishCb = std::make_shared<ePRHA_giveFinish>(board(), this);
    using eGPRA = eGodProvideResourceAct;
    const auto act = std::make_shared<eGPRA>(
                board(), this, mTarget,
                mResource, mCount);
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, act,
                    finishCb);
}

