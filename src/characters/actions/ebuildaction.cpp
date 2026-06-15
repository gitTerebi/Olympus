#include "ebuildaction.h"
#include "fileIO/save-archive.h"

#include "engine/game-board.h"
#include "audio/sounds.h"
#include "buildings/sanctuaries/esanctbuilding.h"
#include "numbers.h"

eBuildAction::eBuildAction(eCharacter* const c) :
    eCharacterAction(c, eCharActionType::buildAction) {}

void eBuildAction::increment(const int by) {
    mTime += by;
    mSoundTime += sqrt(by);
    if(mSoundTime > 100) {
        mSoundTime = 0;
        const auto c = character();
        auto& board = c->getBoard();
        board.ifVisible(c->tile(), []() {
            eSounds::playArtisanSound();
        });
    }
    if(mTime > Numbers::sArtisanBuildTime) {
        setState(eCharacterActionState::finished);
    }
}

void eBuildAction::serializeFields(SaveArchive& ar) {
    eCharacterAction::serializeFields(ar);
    ar.field("soundTime", mSoundTime);
    ar.field("time", mTime);
}
