#include "echoosecitydialog.h"

#include "engine/eworldboard.h"

void eChooseCityDialog::initialize(eWorldBoard* const board,
                                   const eCityAction& cact,
                                   const bool showId) {
    const auto& cities = board->cities();
    std::vector<stdsptr<eWorldCity>> validCities;
    std::vector<std::string> cityNames;
    for(const auto& c : cities) {
        if(mValidator) {
            const bool v = mValidator(c);
            if(!v) continue;
        }
        const auto cname = showId ? c->nameWithId() : c->name();
        cityNames.push_back(cname);
        validCities.push_back(c);
    }
    if(cityNames.empty()) return;

    const auto act = [validCities, cact](const int val) {
        const auto c = validCities[val];
        if(cact) cact(c);
    };

    eChooseButton::initialize(8, cityNames, act);
}

void eChooseCityDialog::setValidator(const eCityValidator& v) {
    mValidator = v;
}
