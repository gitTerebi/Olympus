#ifndef EPATROLMOVEACTION_H
#define EPATROLMOVEACTION_H

#include "emoveaction.h"
#include "engine/emovedirection.h"
#include "engine/edirectionlastusetime.h"
#include "ewalkablehelpers.h"
#include "fileIO/esavearchive.h"

#include <SDL2/SDL_rect.h>

class eMovePathAction;

class eBuilding;

class ePatrolMoveAction : public eMoveAction {
public:
    ePatrolMoveAction(eCharacter* const c,
                      const bool diagonalOnly = true,
                      const stdsptr<eWalkableObject>& walkable =
                        eWalkableObject::sCreateRoad(),
                      const stdsptr<eDirectionTimes>& os =
                        std::make_shared<eDirectionTimes>());

    void setMaxWalkDistance(const int dist)
    { mMaxWalkDistance = dist; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eCharacterActionState nextTurn(eOrientation& t) override;

    bool mDiagonalOnly;
    stdsptr<eWalkableObject> mWalkable;
    stdsptr<eDirectionTimes> mOs;

    eOrientation mO{eOrientation::topRight};
    int mMaxWalkDistance = 10000;
    int mWalkedDistance = 0;
};

#endif // EPATROLMOVEACTION_H
