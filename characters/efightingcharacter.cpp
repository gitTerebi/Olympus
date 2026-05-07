#include "efightingcharacter.h"

#include "actions/efightingaction.h"
#include "fileIO/esavearchive.h"

eFightingCharacter::eFightingCharacter(eCharacter * const c) :
    mChar(c) {}

eFightingAction* eFightingCharacter::fightingAction() const {
    const auto a = mChar->action();
    return dynamic_cast<eFightingAction*>(a);
}

void eFightingCharacter::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eFightingCharacter::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eFightingCharacter*>(this)->serialize(ar);
}

void eFightingCharacter::serialize(eSaveArchive& ar) {
    ar.field("mRange", mRange);
}
