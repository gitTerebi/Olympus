#ifndef ANIMAL_ACTION_H
#define ANIMAL_ACTION_H

#include "ecomplexaction.h"

#include "walkable/walkable-object.h"

class eTileBase;
class eSaveArchive;

enum class eAnimalActionStage {
    idle, walking, laying
};

class AnimalAction : public eComplexAction {
public:
    AnimalAction(eCharacter* const c,
                 const int spawnerX, const int spawnerY,
                 const stdsptr<WalkableObject>& tileWalkable =
                       WalkableObject::sCreateDefault(),
                 const eCharActionType type =
                       eCharActionType::animalAction);
    AnimalAction(eCharacter* const c);

    bool decide() override;

    void setLayTime(const int l) { mLayTime = l; }
    void setWalkTime(const int w) { mWalkTime = w; }

    int spawnerX() const { return mSpawnerX; }
    int spawnerY() const { return mSpawnerY; }
protected:
    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    void walkAround();
    void lay();
    int mSpawnerX;
    int mSpawnerY;

    stdsptr<WalkableObject> mTileWalkable;

    int mLayTime = 2000;
    int mWalkTime = 3000;
    eAnimalActionStage mStage = eAnimalActionStage::idle;
};

#endif // ANIMAL_ACTION_H
