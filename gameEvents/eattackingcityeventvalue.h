#ifndef EATTACKINGCITYEVENTVALUE_H
#define EATTACKINGCITYEVENTVALUE_H

#include "engine/e-worldcity.h"

class eSaveArchive;

class eAttackingCityEventValue {
public:
    void write(eWriteStream& dst) const;
    void read(eReadStream& src, eGameBoard& board);
    void serializeJson(eJsonArchive& ar, eGameBoard& board);

    void setAttackingCity(const stdsptr<eWorldCity>& c) { mAttackingCity = c; }
    const stdsptr<eWorldCity>& attackingCity() const { return mAttackingCity; }
protected:
    void serialize(eSaveArchive& ar, eGameBoard* board);

    stdsptr<eWorldCity> mAttackingCity;
};

#endif // EATTACKINGCITYEVENTVALUE_H
