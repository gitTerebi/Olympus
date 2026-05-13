#ifndef EPATROLMOVEACTION_H
#define EPATROLMOVEACTION_H

#include "emoveaction.h"
#include "engine/emovedirection.h"
#include "ewalkablehelpers.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

#include <SDL2/SDL_rect.h>

class eMovePathAction;

class eBuilding;
class eSaveArchive;

class eDirectionLastUseTime {
public:
    eDirectionLastUseTime() {
        for(int i = 0; i < 8; i++) {
            mTime.push_back(0);
        }
    }

    int& time(const eOrientation o) {
        const int id = static_cast<int>(o);
        return mTime[id];
    }

    void read(eReadStream& src) {
        eSaveArchive ar(src);
        ar.field("time", mTime);
    }

    void write(eWriteStream& dst) const {
        eSaveArchive ar(dst);
        ar.field("time", const_cast<std::vector<int>&>(mTime));
    }
    void serializeJson(eJsonArchive& ar) {
        for(int i = 0; i < 8; i++) { const auto key = "time" + std::to_string(i); ar.field(key.c_str(), mTime[i]); }
}

private:
    std::vector<int> mTime;
};

using eDirectionTimes = std::map<eTile*, eDirectionLastUseTime>;

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

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
private:
    void serialize(eSaveArchive& ar);

    eCharacterActionState nextTurn(eOrientation& t) override;

    bool mDiagonalOnly;
    stdsptr<eWalkableObject> mWalkable;
    stdsptr<eDirectionTimes> mOs;

    eOrientation mO{eOrientation::topRight};
    int mMaxWalkDistance = 10000;
    int mWalkedDistance = 0;
};

#endif // EPATROLMOVEACTION_H
