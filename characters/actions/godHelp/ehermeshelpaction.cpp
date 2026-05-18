#include "ehermeshelpaction.h"
#include "fileIO/esavearchive.h"

#include "etilehelper.h"
#include "buildings/epalace.h"
#include "gameEvents/requests/e-fulfill-request-event.h"

eHermesHelpAction::eHermesHelpAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::hermesHelpAction) {}

bool eHermesHelpAction::decide() {
    switch(mStage) {
    case eHermesHelpStage::none:
        mStage = eHermesHelpStage::appear;
        appear();
        break;
    case eHermesHelpStage::appear:
        mStage = eHermesHelpStage::provide;
        provide();
        break;
    case eHermesHelpStage::provide:
        mStage = eHermesHelpStage::disappear;
        disappear();
        break;
    case eHermesHelpStage::providing:
        spawnProvideMissile();
        break;
    case eHermesHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eHermesHelpAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("preProvidingStage", mPreProvidingStage);
    ar.field("requestFulfilled", mRequestFulfilled);
}

void eHermesHelpAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eHermesHelpAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eHermesHelpStage::providing:
        spawnProvideMissile();
        return;
    case eHermesHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eHermesHelpStage::appear:
        appear();
        return;
    case eHermesHelpStage::provide:
        decide();
        return;
    case eHermesHelpStage::disappear:
        disappear();
        return;
    }
}

void eHermesHelpAction::finishProviding() {
    mStage = mPreProvidingStage;
    mPreProvidingStage = eHermesHelpStage::none;
}

bool eHermesHelpAction::sHelpNeeded(const ePlayerId pid,
                                    const eGameBoard& board) {
    const auto& crs = board.cityRequests(pid);
    return !crs.empty();
}

void eHermesHelpAction::provide() {
    mPreProvidingStage = mStage;
    mStage = eHermesHelpStage::providing;
    pauseAction();
    if(!mRequestFulfilled) {
        auto& board = eHermesHelpAction::board();
        const auto cid = cityId();
        const auto pid = board.cityIdToPlayerId(cid);
        const auto& crs = board.cityRequests(pid);
        if(!crs.empty()) {
            crs[0]->finish(eReceiveRequestResult::comply);
        }
        mRequestFulfilled = true;
    }
    spawnProvideMissile();
}

void eHermesHelpAction::spawnProvideMissile() {
    auto& board = eHermesHelpAction::board();
    const auto c = character();
    const auto cid = cityId();
    const auto p = board.palace(cid);
    const int bw = board.width();
    const int bh = board.height();
    const auto centerTile = board.dtile(bw/2, bh/2);
    const auto targetTile = p ? p->centerTile() : centerTile;
    const auto finishCb = std::make_shared<eHmHA_provideFinish>(board, this);
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, nullptr,
                    finishCb);
}
