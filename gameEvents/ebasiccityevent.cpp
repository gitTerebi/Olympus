#include "ebasiccityevent.h"

#include "estringhelpers.h"
#include "elanguage.h"
#include "engine/eworldcity.h"

void eBasicCityEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    dst.writeCity(mCity.get());
}

void eBasicCityEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    src.readCity(worldBoard(), [this](const stdsptr<eWorldCity>& c) {
        mCity = c;
    });
}

void eBasicCityEvent::longNameReplaceCity(
        const std::string& id, std::string& tmpl) const {
    const auto none = eLanguage::text("none");
    const auto city = this->city();
    const auto ctstr = city ? city->name() : none;
    eStringHelpers::replace(tmpl, id, ctstr);
}
