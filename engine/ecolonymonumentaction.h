#ifndef ECOLONYMONUMENTACTION_H
#define ECOLONYMONUMENTACTION_H

#include "eplannedaction.h"

#include "pointers/estdselfref.h"

class eWorldCity;
class eSaveArchive;

class eColonyMonumentAction : public ePlannedAction {
public:
    eColonyMonumentAction(const stdsptr<eWorldCity>& city);
    eColonyMonumentAction();

    void trigger(eGameBoard& board) override;

    void read(eReadStream& src, eGameBoard& board) override;
    void write(eWriteStream& dst) const override;
private:
    void serialize(eSaveArchive& ar, eGameBoard* board);

    stdsptr<eWorldCity> mCity;
};

#endif // ECOLONYMONUMENTACTION_H
