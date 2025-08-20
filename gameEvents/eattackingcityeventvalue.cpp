#include "eattackingcityeventvalue.h"

#include "engine/eworldcity.h"

void eAttackingCityEventValue::write(eWriteStream& dst) const {
    dst.writeCity(mAttackingCity.get());
}

void eAttackingCityEventValue::read(eReadStream& src, eGameBoard& board) {
    src.readCity(&board, [this](const stdsptr<eWorldCity>& c) {
        mAttackingCity = c;
    });
}
