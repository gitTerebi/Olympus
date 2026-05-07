#include "eanimalaction.h"

#include "emovearoundaction.h"
#include "characters/echaracter.h"
#include "ewaitaction.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

eAnimalAction::eAnimalAction(eCharacter* const c,
                             const int spawnerX, const int spawnerY,
                             const stdsptr<eWalkableObject>& tileWalkable,
                             const eCharActionType type) :
    eComplexAction(c, type),
    mSpawnerX(spawnerX), mSpawnerY(spawnerY),
    mTileWalkable(tileWalkable) {}

eAnimalAction::eAnimalAction(eCharacter* const c) :
    eAnimalAction(c, 0, 0) {}

bool eAnimalAction::decide() {
    const auto c = character();
    if(eRand::rand() % 2 == 0) {
        c->setActionType(eCharacterActionType::walk);
        const auto m = e::make_shared<eMoveAroundAction>(
                           c, mSpawnerX, mSpawnerY,
                           mTileWalkable);
        m->setMaxDistance(eNumbers::sAnimalMoveRange);
        m->setTime(mWalkTime);
        setCurrentAction(m);
    } else {
        c->setActionType(eCharacterActionType::lay);
        const auto w = e::make_shared<eWaitAction>(c);
        w->setTime(mLayTime);
        setCurrentAction(w);
    }
    return true;
}

void eAnimalAction::read(eReadStream& src) {
    eComplexAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eAnimalAction::write(eWriteStream& dst) const {
    eComplexAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eAnimalAction*>(this)->serialize(ar);
}

void eAnimalAction::serialize(eSaveArchive& ar) {
    ar.field("mSpawnerX", mSpawnerX);
    ar.field("mSpawnerY", mSpawnerY);
    if(ar.reading()) {
        mTileWalkable = ar.readStream().readWalkable();
    } else {
        ar.writeStream().writeWalkable(mTileWalkable.get());
    }
    ar.field("mLayTime", mLayTime);
    ar.field("mWalkTime", mWalkTime);
}
