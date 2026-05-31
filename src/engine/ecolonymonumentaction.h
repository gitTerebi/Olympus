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

    void trigger(GameBoard& board) override;
protected:
    void serializeFields(eSaveArchive& ar, GameBoard* board) override;
private:
    stdsptr<eWorldCity> mCity;
};

#endif // ECOLONYMONUMENTACTION_H
