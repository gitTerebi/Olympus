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

void eFightAction::read(eReadStream& src) {
    eCharacterAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eFightAction::write(eWriteStream& dst) const {
    eCharacterAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eFightAction*>(this)->serialize(ar);
}

void eFightAction::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        ar.readStream().readCharacter(&board(), [this](eCharacter* const c) {
            mOpponent = c;
        });
    } else {
        ar.writeStream().writeCharacter(mOpponent);
    }
    ar.field("mTime", mTime);
}
