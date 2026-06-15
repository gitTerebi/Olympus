#include "ewaitaction.h"
#include "fileIO/save-archive.h"

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

void eWaitAction::serializeFields(SaveArchive& ar) {
    eCharacterAction::serializeFields(ar);
    ar.field("remTime", mRemTime);
}
