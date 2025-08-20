#include "ecityeventvalue.h"

#include "estringhelpers.h"
#include "elanguage.h"
#include "engine/eworldcity.h"

void eCityEventValue::write(eWriteStream& dst) const {
    dst.writeCity(mCity.get());
}

void eCityEventValue::read(eReadStream& src, eGameBoard& board) {
    src.readCity(&board, [this](const stdsptr<eWorldCity>& c) {
        mCity = c;
    });
}

void eCityEventValue::longNameReplaceCity(
        const std::string& id, std::string& tmpl) const {
    const auto none = eLanguage::text("none");
    const auto city = this->city();
    const auto ctstr = city ? city->name() : none;
    eStringHelpers::replace(tmpl, id, ctstr);
}
