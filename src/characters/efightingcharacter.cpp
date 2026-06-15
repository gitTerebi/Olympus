#include "efightingcharacter.h"

#include "actions/fighting-action.h"
#include "fileIO/save-archive.h"
#include "echaracter.h"

eFightingCharacter::eFightingCharacter(eCharacter * const c) :
    mChar(c) {}

FightingAction* eFightingCharacter::fightingAction() const {
    const auto a = mChar->action();
    return dynamic_cast<FightingAction*>(a);
}

void eFightingCharacter::serializeFields(SaveArchive& ar) {
    (void)ar;
}
