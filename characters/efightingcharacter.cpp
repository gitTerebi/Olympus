#include "efightingcharacter.h"

#include "actions/efightingaction.h"
#include "fileIO/esavearchive.h"

eFightingCharacter::eFightingCharacter(eCharacter * const c) :
    mChar(c) {}

eFightingAction* eFightingCharacter::fightingAction() const {
    const auto a = mChar->action();
    return dynamic_cast<eFightingAction*>(a);
}

void eFightingCharacter::serializeFields(eSaveArchive& ar) {
    ar.field("mRange", mRange);
}
