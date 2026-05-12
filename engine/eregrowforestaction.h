#ifndef EREGROWFORESTACTION_H
#define EREGROWFORESTACTION_H

#include "eplannedaction.h"

class eTile;
class eSaveArchive;

class eRegrowForestAction : public ePlannedAction {
public:
    eRegrowForestAction(eTile* const tile);
    eRegrowForestAction();

    void trigger(eGameBoard& board) override;

    void read(eReadStream& src, eGameBoard& board) override;
    void write(eWriteStream& dst) const override;
    void serializeJson(eJsonArchive& ar, eGameBoard& board) override;
private:
    void serialize(eSaveArchive& ar, eGameBoard* board);

    eTile* mTile = nullptr;
};

#endif // EREGROWFORESTACTION_H
