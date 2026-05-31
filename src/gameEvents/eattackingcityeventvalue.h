#ifndef EATTACKINGCITYEVENTVALUE_H
#define EATTACKINGCITYEVENTVALUE_H

#include "engine/e-worldcity.h"

class eSaveArchive;

class eAttackingCityEventValue {
public:
    void setAttackingCity(const stdsptr<eWorldCity>& c) { mAttackingCity = c; }
    const stdsptr<eWorldCity>& attackingCity() const { return mAttackingCity; }
protected:
    void serialize(eSaveArchive& ar, GameBoard* board);

    stdsptr<eWorldCity> mAttackingCity;
};

#endif // EATTACKINGCITYEVENTVALUE_H
