#include "ecityeventvalue.h"

#include "estringhelpers.h"
#include "elanguage.h"
#include "engine/e-worldcity.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

eCityEventValue::eCityEventValue(eGameBoard &board,
                                 const eValidator& v) :
    mBoard(board), mValidator(v) {}

void eCityEventValue::serialize(eSaveArchive& ar, eGameBoard& board) {
    ar.worldCityField("city", &board, mCity);
    ar.field("minCityId", mMinCityId, 0);
    ar.field("maxCityId", mMaxCityId, 0);
}

void eCityEventValue::setSingleCity(const stdsptr<eWorldCity> &c) {
    if(c) {
        mCity = c;
        const auto cid = c->cityId();
        const int iCid = static_cast<int>(cid);
        mMinCityId = iCid;
        mMaxCityId = iCid;
    } else {
        mCity = nullptr;
        mMinCityId = -1;
        mMaxCityId = -1;
    }
}

void eCityEventValue::chooseCity() {
    const int id = chooseCityId();
    const auto& world = mBoard.world();
    const auto cid = static_cast<eCityId>(id);
    mCity = world.cityWithId(cid);
}

int eCityEventValue::chooseCityId() const {
    std::vector<int> options;
    for(int i = mMinCityId; i <= mMaxCityId; i++) {
        options.push_back(i);
    }
    std::random_shuffle(options.begin(), options.end());
    const auto& world = mBoard.world();
    for(const int i : options) {
        const auto cid = static_cast<eCityId>(i);
        const auto city = world.cityWithId(cid);
        if(!city) continue;
        if(mValidator) {
            const bool r = mValidator(*city);
            if(!r) continue;
        }
        return i;
    }
    return -1;
}

void eCityEventValue::longNameReplaceCity(
        const std::string& id, std::string& tmpl) const {
    const auto none = eLanguage::text("none");
    const auto city = this->city();
    const auto ctstr = city ? city->name() : none;
    eStringHelpers::replace(tmpl, id, ctstr);
}
