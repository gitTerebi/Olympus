#ifndef EATTACKINGCITYEVENTVALUE_H
#define EATTACKINGCITYEVENTVALUE_H

#include "engine/world-city.h"

class eSaveArchive;

class eAttackingCityEventValue {
public:
    void setAttackingCity(const stdsptr<WorldCity>& c) { mAttackingCity = c; }
    const stdsptr<WorldCity>& attackingCity() const { return mAttackingCity; }
protected:
    void serialize(eSaveArchive& ar, GameBoard* board);

    stdsptr<WorldCity> mAttackingCity;
};

#endif // EATTACKINGCITYEVENTVALUE_H
