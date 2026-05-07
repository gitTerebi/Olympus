#include "eattackingcityeventvalue.h"

#include "engine/eworldcity.h"
#include "fileIO/esavearchive.h"

void eAttackingCityEventValue::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    auto& self = const_cast<eAttackingCityEventValue&>(*this);
    self.serialize(ar, nullptr);
}

void eAttackingCityEventValue::read(eReadStream& src, eGameBoard& board) {
    eSaveArchive ar(src);
    serialize(ar, &board);
}

void eAttackingCityEventValue::serialize(eSaveArchive& ar, eGameBoard* board) {
    if(ar.reading()) {
        ar.readStream().readCity(board, [this](const stdsptr<eWorldCity>& c) {
            mAttackingCity = c;
        });
    } else {
        ar.writeStream().writeCity(mAttackingCity.get());
    }
}
