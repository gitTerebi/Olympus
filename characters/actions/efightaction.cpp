#include "efightaction.h"

#include "characters/echaracter.h"

#include "characters/ebasicpatroler.h"
#include "characters/efightingpatroler.h"
#include "characters/eresourcecollector.h"
#include "characters/eanimal.h"
#include "fileIO/esavearchive.h"

eFightAction::eFightAction(eCharacter* const c, eCharacter* const o) :
    eCharacterAction(c, eCharActionType::fightAction),
    mOpponent(o) {
    c->setActionType(eCharacterActionType::fight);
}

void eFightAction::increment(const int by) {
    const auto c = character();
    const double a = c->attack();
    const bool dead = mOpponent ? mOpponent->takeDamage(by*a) : true;
    if(dead || c->dead()) {
        setState(eCharacterActionState::finished);
    }
}

void eFightAction::serializeFields(eSaveArchive& ar) {
    eCharacterAction::serializeFields(ar);
    ar.characterField("opponent", &board(), mOpponent);
    ar.field("time", mTime);
}

void eFightAction::resumeFromSavedState() {
    const auto c = character();
    if(!mOpponent || mOpponent->dead() || c->dead()) {
        setState(eCharacterActionState::finished);
        return;
    }
    c->setActionType(eCharacterActionType::fight);
}
