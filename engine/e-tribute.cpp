#include "e-tribute.h"

#include "e-game-board.h"
#include "eevent.h"
#include "eeventdata.h"
#include "e-worldcity.h"
#include "gameEvents/requests/e-pay-tribute-event.h"
#include "gameEvents/ereceivetributeevent.h"

#include <cmath>
#include <cstdio>

eTributePayment eTributeHelpers::payTribute(const eWorldCity &city, const Difficulty diff)
{
    const double mult = eDifficultyHelpers::costMultiplier(diff);
    const int count = std::round(city.payTributeCount() * mult);
    return {city.payTributeType(), count};
}

eTributePayment eTributeHelpers::receiveTribute(const eWorldCity &city)
{
    return {city.receiveTributeType(), city.receiveTributeCount()};
}

void eTributeHelpers::receiveTributeFromCity(GameBoard &board, const ePlayerId pid, const stdsptr<eWorldCity> &city, const bool postpone)
{
    if (!city)
        return;
    const auto e = e::make_shared<eReceiveTributeEvent>(
        board.currentCityId(), eGameEventBranch::root, board);
    e->initialize(city);
    e->initializeDate(board.date());
    board.addRootGameEvent(e);
    e->trigger();
}

void eTributeHelpers::payTributeToCity(GameBoard &board, const eCityId playerCityId, const stdsptr<eWorldCity> &parentCity)
{
    if (!parentCity)
        return;
    if (!board.boardCityWithId(playerCityId))
        return;

    const auto e = e::make_shared<ePayTributeEvent>(
        playerCityId, eGameEventBranch::root, board);
    e->initialize(parentCity);
    e->initializeDate(board.date());
    board.addRootGameEvent(e);
}
