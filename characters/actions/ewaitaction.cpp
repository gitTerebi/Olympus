#include "ewaitaction.h"
#include "fileIO/esavearchive.h"

eWaitAction::eWaitAction(eCharacter* const c) :
    eCharacterAction(c, eCharActionType::waitAction) {}

void eWaitAction::setTime(const int t) {
    mRemTime = t;
}

void eWaitAction::increment(const int by) {
    mRemTime -= by;
    if(mRemTime <= 0) {
        setState(eCharacterActionState::finished);
    }
}

void eWaitAction::read(eReadStream& src) {
    eCharacterAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eWaitAction::write(eWriteStream& dst) const {
    eCharacterAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eWaitAction*>(this)->serialize(ar);
}

void eWaitAction::serialize(eSaveArchive& ar) {
    ar.field("mRemTime", mRemTime);
}
