#include "eherahelpaction.h"
#include "fileIO/esavearchive.h"

eHeraHelpAction::eHeraHelpAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::heraHelpAction) {}

bool eHeraHelpAction::decide() {
    switch(mStage) {
    case eHeraHelpStage::none:
        mStage = eHeraHelpStage::appear;
        appear();
        break;
    case eHeraHelpStage::appear: {
        auto& board = this->board();
        const auto cid = cityId();
        const auto as = board.agoras(cid);
        for(const auto a : as) {
            mFutureTargets.push_back(a);
        }
        goToTarget();
    } break;
    case eHeraHelpStage::goTo:
        mStage = eHeraHelpStage::give;
        give();
        break;
    case eHeraHelpStage::give:
        goToTarget();
        break;
    case eHeraHelpStage::giving:
        if(!mGiveTarget) {
            finishGiving();
            return decide();
        }
        spawnGiveMissile(mGiveTarget.get());
        break;
    case eHeraHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eHeraHelpAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.buildingAsField("target", &board(), mTarget);
    ar.arrayField("futureTargets", mFutureTargets,
                  [this](eSaveArchive& itemAr, stdptr<eAgoraBase>& target) {
        itemAr.buildingAsField("agora", &board(), target);
    });
    ar.field("preGivingStage", mPreGivingStage);
    ar.buildingAsField("giveTarget", &board(), mGiveTarget);
}

void eHeraHelpAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eHeraHelpAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eHeraHelpStage::giving:
        if(!mGiveTarget) {
            finishGiving();
            eGodAction::resumeFromSavedState();
            return;
        }
        decide();
        return;
    case eHeraHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eHeraHelpStage::appear:
        appear();
        return;
    case eHeraHelpStage::goTo:
        goToTarget();
        return;
    case eHeraHelpStage::give:
        decide();
        return;
    case eHeraHelpStage::disappear:
        disappear();
        return;
    }
}

void eHeraHelpAction::finishGiving() {
    mStage = mPreGivingStage;
    mPreGivingStage = eHeraHelpStage::none;
    mGiveTarget = nullptr;
}

bool eHeraHelpAction::sHelpNeeded(const eCityId cid,
                                  const eGameBoard& board) {
    const auto as = board.agoras(cid);
    for(const auto a : as) {
        int space = 0;
        space += a->spaceLeft(eResourceType::food);
        space += a->spaceLeft(eResourceType::fleece);
        space += a->spaceLeft(eResourceType::oliveOil);
        space += a->spaceLeft(eResourceType::wine);
        if(space > 4) return true;
    }
    return false;
}

void eHeraHelpAction::goToTarget() {
    while(!mTarget && !mFutureTargets.empty()) {
        mTarget = mFutureTargets.back();
        mFutureTargets.pop_back();
    }
    if(mTarget) {
        mStage = eHeraHelpStage::goTo;
        auto& board = this->board();
        using eGTTT = eGoToTargetTeleport;
        const auto tele = std::make_shared<eGTTT>(board, this);
        const auto ct = mTarget->centerTile();
        goToTile(ct, tele);
    } else {
        mStage = eHeraHelpStage::disappear;
        disappear();
    }
}

void eHeraHelpAction::give() {
    if(!mTarget) return;
    mPreGivingStage = mStage;
    mStage = eHeraHelpStage::giving;
    mGiveTarget = mTarget;
    mTarget = nullptr;
    pauseAction();
    spawnGiveMissile(mGiveTarget.get());
}

void eHeraHelpAction::spawnGiveMissile(eAgoraBase* const target) {
    const auto c = character();
    const auto targetTile = target->centerTile();
    const auto finishCb = std::make_shared<eHrHA_giveFinish>(board(), this);
    using eGPAA = eGodProvideAgoraAct;
    const auto act = std::make_shared<eGPAA>(board(), target);
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, act,
                    finishCb);
}
