#ifndef EMILITARYAID_H
#define EMILITARYAID_H

#include <vector>

#include "pointers/estdselfref.h"

class eWriteStream;
class eReadStream;
class eGameBoard;
class eWorldCity;
class eSoldierBanner;

struct eMilitaryAid {
    bool count() const;

    bool allDead() const {
        return count() == 0;
    }

    void goBack();

    void write(eWriteStream& dst);
    void read(eReadStream& src, eGameBoard* const board);

    stdsptr<eWorldCity> fCity;
    std::vector<stdsptr<eSoldierBanner>> fSoldiers;
};

#endif // EMILITARYAID_H
