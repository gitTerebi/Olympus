#ifndef EMILITARYAID_H
#define EMILITARYAID_H

#include <vector>

#include "pointers/estdselfref.h"

class GameBoard;
class WorldCity;
class SoldierBanner;
class SaveArchive;

struct eMilitaryAid {
    bool count() const;

    bool allDead() const {
        return count() == 0;
    }

    void goBack();

    void serialize(SaveArchive& ar, GameBoard* board);

    stdsptr<WorldCity> fCity;
    std::vector<stdsptr<SoldierBanner>> fSoldiers;
};

#endif // EMILITARYAID_H
