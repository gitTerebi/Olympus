#ifndef EMILITARYAID_H
#define EMILITARYAID_H

#include <vector>

#include "pointers/estdselfref.h"

class eGameBoard;
class eWorldCity;
class eSoldierBanner;
class eSaveArchive;

struct eMilitaryAid {
    bool count() const;

    bool allDead() const {
        return count() == 0;
    }

    void goBack();

    void serialize(eSaveArchive& ar, eGameBoard* board);

    stdsptr<eWorldCity> fCity;
    std::vector<stdsptr<eSoldierBanner>> fSoldiers;
};

#endif // EMILITARYAID_H
