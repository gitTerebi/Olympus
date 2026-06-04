#ifndef EPATROLMOVEACTION_H
#define EPATROLMOVEACTION_H

#include "emoveaction.h"
#include "engine/emovedirection.h"
#include "engine/edirectionlastusetime.h"
#include "ewalkablehelpers.h"
#include "fileIO/esavearchive.h"

#include <SDL2/SDL_rect.h>
#include <functional>
#include <vector>

class eMovePathAction;

class eBuilding;
class eTile;

class ePatrolMoveAction : public eMoveAction {
public:
    using eOrientationPicker = std::function<eOrientation(
        const std::vector<eOrientation>& options,
        eOrientation current)>;

    ePatrolMoveAction(eCharacter* const c,
                      const bool diagonalOnly = true,
                      const stdsptr<WalkableObject>& walkable =
                        WalkableObject::sCreateRoad(),
                      const stdsptr<eDirectionTimes>& os =
                        std::make_shared<eDirectionTimes>());

    void setMaxWalkDistance(const int dist)
    { mMaxWalkDistance = dist; }

    static eCharacterActionState sNextTurn(
        eTile* tile,
        bool diagonalOnly,
        const WalkableObject& walkable,
        eDirectionTimes& times,
        eOrientation& orientation,
        int time,
        const eOrientationPicker& pickOrientation);
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    eCharacterActionState nextTurn(eOrientation& t) override;

    bool mDiagonalOnly;
    stdsptr<WalkableObject> mWalkable;
    stdsptr<eDirectionTimes> mOs;

    eOrientation mO{eOrientation::topRight};
    int mMaxWalkDistance = 10000;
    int mWalkedDistance = 0;
};

#endif // EPATROLMOVEACTION_H
