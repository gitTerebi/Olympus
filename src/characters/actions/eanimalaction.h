#ifndef EANIMALACTION_H
#define EANIMALACTION_H

#include "ecomplexaction.h"

#include "walkable/ewalkableobject.h"

class eTileBase;
class eSaveArchive;

enum class eAnimalActionStage {
    idle, walking, laying
};

class eAnimalAction : public eComplexAction {
public:
    eAnimalAction(eCharacter* const c,
                  const int spawnerX, const int spawnerY,
                  const stdsptr<eWalkableObject>& tileWalkable =
                        eWalkableObject::sCreateDefault(),
                  const eCharActionType type =
                        eCharActionType::animalAction);
    eAnimalAction(eCharacter* const c);

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

    stdsptr<eWalkableObject> mTileWalkable;

    int mLayTime = 2000;
    int mWalkTime = 3000;
    eAnimalActionStage mStage = eAnimalActionStage::idle;
};

#endif // EANIMALACTION_H
