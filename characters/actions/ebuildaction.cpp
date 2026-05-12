#include "ebuildaction.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

#include "engine/e-game-board.h"
#include "audio/esounds.h"
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

void eBuildAction::read(eReadStream& src) {
    eCharacterAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eBuildAction::write(eWriteStream& dst) const {
    eCharacterAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eBuildAction*>(this)->serialize(ar);
}

void eBuildAction::serialize(eSaveArchive& ar) {
    ar.field("mSoundTime", mSoundTime);
    ar.field("mTime", mTime);
}

void eBuildAction::serializeJson(eJsonArchive& ar) {
    eCharacterAction::serializeJson(ar);
    ar.field("mSoundTime", mSoundTime);
    ar.field("mTime", mTime);
}
