#include "dionysus-follow-action.h"

#include "characters/echaracter.h"
#include "fileIO/esavearchive.h"

DionysusFollowAction::DionysusFollowAction(
        eCharacter* const f, eCharacter* const c) :
    eFollowAction(f, c, eCharActionType::dionysusFollowAction) {}

DionysusFollowAction::DionysusFollowAction(
        eCharacter* const c) :
    DionysusFollowAction(nullptr, c) {}

void DionysusFollowAction::setFollower(eCharacter* const f) {
    mFollower = f;
}

void DionysusFollowAction::serializeFields(eSaveArchive& ar) {
    eFollowAction::serializeFields(ar);
    ar.characterField("follower", &board(), mFollower);
    ar.field("killOnFinish", mKillOnFinish);
}

void DionysusFollowAction::resumeFromSavedState() {
    eFollowAction::resumeFromSavedState();
    if(mKillOnFinish) attachKillCallback();
}

void DionysusFollowAction::setKillFollowerOnFinish() {
    mKillOnFinish = true;
    attachKillCallback();
}

void DionysusFollowAction::attachKillCallback() {
    const auto c = character();
    if(!c) return;
    const auto killA = std::make_shared<eChar_killWithCorpseFinish>(
        board(), c, true);
    setFinishAction(killA);
    setFailAction(killA);
}

bool DionysusFollowAction::sShouldFollow(const eCharacterType c) {
    switch(c) {
    case eCharacterType::settler:
    case eCharacterType::homeless:
    case eCharacterType::shepherd:
    case eCharacterType::goatherd:
    case eCharacterType::grower:
    case eCharacterType::peddler:
    case eCharacterType::lumberjack:
    case eCharacterType::bronzeMiner:
    case eCharacterType::silverMiner:
    case eCharacterType::artisan:
    case eCharacterType::hunter:
    case eCharacterType::gymnast:
    case eCharacterType::philosopher:
    case eCharacterType::actor:
    case eCharacterType::competitor:
    case eCharacterType::scholar:
    case eCharacterType::astronomer:
    case eCharacterType::inventor:
    case eCharacterType::curator:
    case eCharacterType::taxCollector:
    case eCharacterType::watchman:
    case eCharacterType::waterDistributor:
    case eCharacterType::fireFighter:
    case eCharacterType::healer:
    case eCharacterType::cartTransporter:
        return true;
    default:
        return false;
    }
    return false;
}

void DionysusFollowAction::increment(const int by) {
    if(!mFollower) {
        auto& board = this->board();
        const auto c = character();
        const auto tile = c->tile();
        const auto& chars = tile->characters();
        for(const auto& cc : chars) {
            if(cc.get() == c) continue;
            const auto cType = cc->type();
            const bool r = sShouldFollow(cType);
            if(!r) continue;
            const auto ccaa = cc->actionType();
            if(ccaa == eCharacterActionType::die) continue;
            const auto cca = cc->action();
            const auto eDFA = eCharActionType::dionysusFollowAction;
            if(cca && cca->type() == eDFA) continue;
            const auto fa = e::make_shared<DionysusFollowAction>(c, cc.get());
            cc->setAction(fa);
            fa->setKillFollowerOnFinish();
            mFollower = cc.get();
            break;
        }
    }
    eFollowAction::increment(by);
}
