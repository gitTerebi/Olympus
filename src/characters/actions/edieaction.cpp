#include "edieaction.h"
#include "fileIO/save-archive.h"

#include "characters/echaracter.h"

eDieAction::eDieAction(eCharacter* const c) :
    eCharacterAction(c, eCharActionType::dieAction) {
    const auto aType = c->actionType();
    if(aType != eCharacterActionType::none) {
        c->setActionType(eCharacterActionType::die);
    }
}

void eDieAction::increment(const int by) {
    mTime += by;
    if(mTime > 2048) setState(eCharacterActionState::finished);
}

void eDieAction::serializeFields(SaveArchive& ar) {
    eCharacterAction::serializeFields(ar);
    ar.field("time", mTime);
}

void eDieAction::resumeFromSavedState() {
    const auto c = character();
    const auto aType = c->actionType();
    if(aType != eCharacterActionType::none) {
        c->setActionType(eCharacterActionType::die);
    }
}
