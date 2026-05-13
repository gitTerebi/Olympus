#ifndef EMOVEPATHACTION_H
#define EMOVEPATHACTION_H

#include "emoveaction.h"

class eSaveArchive;

class eMovePathAction : public eMoveAction {
public:
    eMovePathAction(eCharacter* const c,
                    const std::vector<eOrientation>& path,
                    const stdsptr<eWalkableObject>& tileWalkable);
    eMovePathAction(eCharacter* const c);

    void setMaxDistance(const int dist) { mMaxDistance = dist; }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar) override;
private:
    eCharacterActionState nextTurn(eOrientation& turn) override;
    void serialize(eSaveArchive& ar);

    std::vector<eOrientation> mTurns;

    int mMaxDistance = 1000;
    int mWalkedDistance = 0;
};

#endif // EMOVEPATHACTION_H
