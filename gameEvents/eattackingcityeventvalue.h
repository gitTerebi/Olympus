#ifndef EATTACKINGCITYEVENTVALUE_H
#define EATTACKINGCITYEVENTVALUE_H

#include "engine/eworldcity.h"

class eAttackingCityEventValue {
public:
    void write(eWriteStream& dst) const;
    void read(eReadStream& src, eGameBoard& board);

    void setAttackingCity(const stdsptr<eWorldCity>& c) { mAttackingCity = c; }
    const stdsptr<eWorldCity>& attackingCity() const { return mAttackingCity; }
protected:
    stdsptr<eWorldCity> mAttackingCity;
};

#endif // EATTACKINGCITYEVENTVALUE_H
