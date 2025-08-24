#include "efightingcharacter.h"

#include "actions/efightingaction.h"

eFightingCharacter::eFightingCharacter(eCharacter * const c) :
    mChar(c) {}

eFightingAction* eFightingCharacter::fightingAction() const {
    const auto a = mChar->action();
    return dynamic_cast<eFightingAction*>(a);
}

void eFightingCharacter::read(eReadStream& src) {
    src >> mRange;
}

void eFightingCharacter::write(eWriteStream& dst) const {
    dst << mRange;
}
