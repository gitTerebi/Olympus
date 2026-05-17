#include "edionysusfollowaction.h"

#include "characters/echaracter.h"
#include "fileIO/esavearchive.h"

eDionysusFollowAction::eDionysusFollowAction(
        eCharacter* const f, eCharacter* const c) :
    eFollowAction(f, c, eCharActionType::dionysusFollowAction) {}

eDionysusFollowAction::eDionysusFollowAction(
        eCharacter* const c) :
    eDionysusFollowAction(nullptr, c) {}

void eDionysusFollowAction::setFollower(eCharacter* const f) {
    mFollower = f;
}

void eDionysusFollowAction::serializeFields(eSaveArchive& ar) {
    eFollowAction::serializeFields(ar);
    ar.characterField("follower", &board(), mFollower);
    ar.field("killOnFinish", mKillOnFinish);
}

void eDionysusFollowAction::resumeFromSavedState() {
    eFollowAction::resumeFromSavedState();
    if(mKillOnFinish) attachKillCallback();
}

void eDionysusFollowAction::setKillFollowerOnFinish() {
    mKillOnFinish = true;
    attachKillCallback();
}

void eDionysusFollowAction::attachKillCallback() {
    const auto c = character();
    if(!c) return;
    const auto killA = std::make_shared<eChar_killWithCorpseFinish>(
        board(), c, true);
    setFinishAction(killA);
    setFailAction(killA);
}

bool eDionysusFollowAction::sShouldFollow(const eCharacterType c) {
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

void eDionysusFollowAction::increment(const int by) {
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
            const auto fa = e::make_shared<eDionysusFollowAction>(c, cc.get());
            cc->setAction(fa);
            fa->setKillFollowerOnFinish();
            mFollower = cc.get();
            break;
        }
    }
    eFollowAction::increment(by);
}
