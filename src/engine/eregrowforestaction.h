#ifndef EREGROWFORESTACTION_H
#define EREGROWFORESTACTION_H

#include "eplannedaction.h"

class eTile;
class SaveArchive;

class eRegrowForestAction : public ePlannedAction {
public:
    eRegrowForestAction(eTile* const tile);
    eRegrowForestAction();

    void trigger(GameBoard& board) override;
protected:
    void serializeFields(SaveArchive& ar, GameBoard* board) override;
private:
    eTile* mTile = nullptr;
};

#endif // EREGROWFORESTACTION_H
