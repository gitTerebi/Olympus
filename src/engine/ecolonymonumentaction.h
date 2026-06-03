#ifndef ECOLONYMONUMENTACTION_H
#define ECOLONYMONUMENTACTION_H

#include "eplannedaction.h"

#include "pointers/estdselfref.h"

class WorldCity;
class eSaveArchive;

class eColonyMonumentAction : public ePlannedAction {
public:
    eColonyMonumentAction(const stdsptr<WorldCity>& city);
    eColonyMonumentAction();

    void trigger(GameBoard& board) override;
protected:
    void serializeFields(eSaveArchive& ar, GameBoard* board) override;
private:
    stdsptr<WorldCity> mCity;
};

#endif // ECOLONYMONUMENTACTION_H
