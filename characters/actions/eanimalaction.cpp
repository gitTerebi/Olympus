#include "eanimalaction.h"

#include "move-around-action.h"
#include "characters/echaracter.h"
#include "ewaitaction.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"
#include "engine/etile.h"

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
    const auto t = c->tile();
    bool shared = false;
    if(t) {
        const auto cType = c->type();
        for(const auto& oc : t->characters()) {
            if(oc.get() != c && oc->type() == cType) {
                shared = true;
                break;
            }
        }
    }
    if(shared || eRand::rand() % 2 == 0) {
        walkAround();
    } else {
        lay();
    }
    return true;
}

void eAnimalAction::serializeFields(eSaveArchive& ar) {
    eComplexAction::serializeFields(ar);
    ar.field("spawnerX", mSpawnerX);
    ar.field("spawnerY", mSpawnerY);
    ar.walkableField("tileWalkable", mTileWalkable);
    ar.field("layTime", mLayTime);
    ar.field("walkTime", mWalkTime);
    ar.field("animalStage", mStage);
}

void eAnimalAction::resumeFromSavedState() {
    switch(mStage) {
    case eAnimalActionStage::idle:
        eComplexAction::resumeFromSavedState();
        break;
    case eAnimalActionStage::walking:
        walkAround();
        break;
    case eAnimalActionStage::laying:
        lay();
        break;
    }
}

void eAnimalAction::walkAround() {
    mStage = eAnimalActionStage::walking;
    const auto c = character();
    c->setActionType(eCharacterActionType::walk);
    const auto m = e::make_shared<MoveAroundAction>(
                       c, mSpawnerX, mSpawnerY,
                       mTileWalkable);
    m->setMaxDistance(eNumbers::sAnimalMoveRange);
    m->setTime(mWalkTime);
    setCurrentAction(m);
}

void eAnimalAction::lay() {
    mStage = eAnimalActionStage::laying;
    const auto c = character();
    c->setActionType(eCharacterActionType::lay);
    const auto w = e::make_shared<eWaitAction>(c);
    w->setTime(mLayTime);
    setCurrentAction(w);
}
