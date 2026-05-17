#include "ebuildaction.h"
#include "fileIO/esavearchive.h"

#include "engine/e-game-board.h"
#include "audio/sounds.h"
#include "buildings/sanctuaries/esanctbuilding.h"
#include "enumbers.h"

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
    if(mTime > eNumbers::sArtisanBuildTime) {
        setState(eCharacterActionState::finished);
    }
}

void eBuildAction::serializeFields(eSaveArchive& ar) {
    eCharacterAction::serializeFields(ar);
    ar.field("soundTime", mSoundTime);
    ar.field("time", mTime);
}
