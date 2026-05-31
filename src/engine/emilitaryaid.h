#ifndef EMILITARYAID_H
#define EMILITARYAID_H

#include <vector>

#include "pointers/estdselfref.h"

class GameBoard;
class eWorldCity;
class SoldierBanner;
class eSaveArchive;

struct eMilitaryAid {
    bool count() const;

    bool allDead() const {
        return count() == 0;
    }

    void goBack();

    void serialize(eSaveArchive& ar, GameBoard* board);

    stdsptr<eWorldCity> fCity;
    std::vector<stdsptr<SoldierBanner>> fSoldiers;
};

#endif // EMILITARYAID_H
