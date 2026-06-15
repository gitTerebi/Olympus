#ifndef EMOVEPATHACTION_H
#define EMOVEPATHACTION_H

#include "emoveaction.h"

class SaveArchive;

class eMovePathAction : public eMoveAction {
public:
    eMovePathAction(eCharacter* const c,
                    const std::vector<eOrientation>& path,
                    const stdsptr<WalkableObject>& tileWalkable);
    eMovePathAction(eCharacter* const c);

    void setMaxDistance(const int dist) { mMaxDistance = dist; }
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    eCharacterActionState nextTurn(eOrientation& turn) override;

    std::vector<eOrientation> mTurns;

    int mMaxDistance = 1000;
    int mWalkedDistance = 0;
};

#endif // EMOVEPATHACTION_H
