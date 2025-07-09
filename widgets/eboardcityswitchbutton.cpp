#include "eboardcityswitchbutton.h"

#include "engine/egameboard.h"
#include "evectorhelpers.h"

void eBoardCitySwitchButton::initialize(
        const eGameBoard& board, const eValidator& v,
        const eSwitchAction& a) {
    setUnderline(false);
    const auto cids = board.citiesOnBoard();
    for(const auto cid : cids) {
        if(v && !v(cid)) continue;
        mCities.push_back(cid);
        addValue(board.cityName(cid));
    }
    fitValidContent();

    eSwitchButton::setSwitchAction([this, a](const int v) {
        if(a) a(mCities[v]);
    });
}

eCityId eBoardCitySwitchButton::currentCity() const {
    const int v = currentValue();
    return mCities[v];
}

bool eBoardCitySwitchButton::setCurrentCity(const eCityId cid) {
    const int v = eVectorHelpers::index(mCities, cid);
    if(v == -1) return false;
    setValue(v);
    return true;
}

eBoardCitySwitchButton::eValidator
eBoardCitySwitchButton::sPlayerCityValidator(
        const eGameBoard& board, const ePlayerId pid) {
    const auto boardPtr = &board;
    return [boardPtr, pid](const eCityId cid) {
        const auto cpid = boardPtr->cityIdToPlayerId(cid);
        return cpid == pid;
    };
}

eBoardCitySwitchButton::eValidator
eBoardCitySwitchButton::sPlayerEnemyCityValidator(
        const eGameBoard& board, const ePlayerId pid) {
    const auto boardPtr = &board;
    const auto ptid = board.playerIdToTeamId(pid);
    return [boardPtr, ptid](const eCityId cid) {
        const auto ctid = boardPtr->cityIdToTeamId(cid);
        return ptid != ctid;
    };
}
