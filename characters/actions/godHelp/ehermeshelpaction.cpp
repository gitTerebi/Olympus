#include "ehermeshelpaction.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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
    case eHermesHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eHermesHelpAction::read(eReadStream& src) {
    eGodAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eHermesHelpAction::write(eWriteStream& dst) const {
    eGodAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eHermesHelpAction*>(this)->serialize(ar);
}

void eHermesHelpAction::serialize(eSaveArchive& ar) {
    ar.field("mStage", mStage);
}

void eHermesHelpAction::serializeJson(eJsonArchive& ar) {
    eGodAction::serializeJson(ar);
    ar.field("mStage", mStage);
}

bool eHermesHelpAction::sHelpNeeded(const ePlayerId pid,
                                    const eGameBoard& board) {
    const auto& crs = board.cityRequests(pid);
    return !crs.empty();
}

void eHermesHelpAction::provide() {
    auto& board = eHermesHelpAction::board();
    const auto c = character();
    const auto cid = cityId();
    const auto p = board.palace(cid);
    const int bw = board.width();
    const int bh = board.height();
    const auto centerTile = board.dtile(bw/2, bh/2);
    const auto targetTile = p ? p->centerTile() : centerTile;
    using eGA_LFRAF = eGA_lookForRangeActionFinish;
    const auto finishAttackA = std::make_shared<eGA_LFRAF>(
                                   board, this);
    pauseAction();
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, nullptr,
                    finishAttackA);
    const auto pid = board.cityIdToPlayerId(cid);
    const auto& crs = board.cityRequests(pid);
    if(crs.empty()) return;
    crs[0]->finish(eReceiveRequestResult::comply);
}
