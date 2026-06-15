#include "animal-action.h"

#include "move-around-action.h"
#include "characters/echaracter.h"
#include "ewaitaction.h"
#include "numbers.h"
#include "fileIO/save-archive.h"
#include "engine/etile.h"

AnimalAction::AnimalAction(eCharacter* const c,
                           const int spawnerX, const int spawnerY,
                           const stdsptr<WalkableObject>& tileWalkable,
                           const eCharActionType type) :
    eComplexAction(c, type),
    mSpawnerX(spawnerX), mSpawnerY(spawnerY),
    mTileWalkable(tileWalkable) {}

AnimalAction::AnimalAction(eCharacter* const c) :
    AnimalAction(c, 0, 0) {}

bool AnimalAction::decide() {
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
    if(shared || Rand::rand() % 2 == 0) {
        walkAround();
    } else {
        lay();
    }
    return true;
}

void AnimalAction::serializeFields(SaveArchive& ar) {
    eComplexAction::serializeFields(ar);
    ar.field("spawnerX", mSpawnerX);
    ar.field("spawnerY", mSpawnerY);
    ar.walkableField("tileWalkable", mTileWalkable);
    ar.field("layTime", mLayTime);
    ar.field("walkTime", mWalkTime);
    ar.field("animalStage", mStage);
}

void AnimalAction::resumeFromSavedState() {
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

void AnimalAction::walkAround() {
    mStage = eAnimalActionStage::walking;
    const auto c = character();
    c->setActionType(eCharacterActionType::walk);
    if(c->type() == eCharacterType::horse) {
        const SDL_Rect rect{mSpawnerX - 1, mSpawnerY - 2, 4, 4};
        mTileWalkable = WalkableObject::sCreateRanch(rect);
    }
    const auto m = e::make_shared<MoveAroundAction>(
                       c, mSpawnerX, mSpawnerY,
                       mTileWalkable);
    m->setMaxDistance(Numbers::sAnimalMoveRange);
    m->setTime(mWalkTime);
    setCurrentAction(m);
}

void AnimalAction::lay() {
    mStage = eAnimalActionStage::laying;
    const auto c = character();
    c->setActionType(eCharacterActionType::lay);
    const auto w = e::make_shared<eWaitAction>(c);
    w->setTime(mLayTime);
    setCurrentAction(w);
}
