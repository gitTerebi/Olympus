#include "edieaction.h"
#include "fileIO/esavearchive.h"

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

void eDieAction::read(eReadStream& src) {
    eCharacterAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eDieAction::write(eWriteStream& dst) const {
    eCharacterAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eDieAction*>(this)->serialize(ar);
}

void eDieAction::serialize(eSaveArchive& ar) {
    ar.value(mTime);
}
