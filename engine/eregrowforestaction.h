#ifndef EREGROWFORESTACTION_H
#define EREGROWFORESTACTION_H

#include "eplannedaction.h"

class eTile;
class eSaveArchive;

class eRegrowForestAction : public ePlannedAction {
public:
    eRegrowForestAction(eTile* const tile);
    eRegrowForestAction();

    void trigger(GameBoard& board) override;
protected:
    void serializeFields(eSaveArchive& ar, GameBoard* board) override;
private:
    eTile* mTile = nullptr;
};

#endif // EREGROWFORESTACTION_H
