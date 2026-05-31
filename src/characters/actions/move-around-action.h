#ifndef MOVE_AROUND_ACTION_H
#define MOVE_AROUND_ACTION_H

#include "emoveaction.h"

#include "walkable/ewalkableobject.h"
#include "elimits.h"
#include <vector>

class eSaveArchive;

class MoveAroundAction : public eMoveAction {
public:
    MoveAroundAction(eCharacter* const c,
                      const int startX, const int startY,
                      const stdsptr<eWalkableObject>& walkable =
                            eWalkableObject::sCreateDefault());
    MoveAroundAction(eCharacter* const c);

    void increment(const int by) override;

    void setTime(const int t);

    void setMaxDistance(const int md);
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eCharacterActionState nextTurn(eOrientation& turn) override;

    int mStartTX;
    int mStartTY;

    int mMaxDist = 5;
    int mRemTime{__INT_MAX__};
};

#endif // MOVE_AROUND_ACTION_H
