#include "tribute.h"

#include "game-board.h"
#include "eevent.h"
#include "eeventdata.h"
#include "world-city.h"
#include "gameEvents/requests/get-tribute-event.h"
#include "gameEvents/requests/pay-tribute-event.h"

#include <cmath>
#include <cstdio>

TributePayment TributeHelpers::payTribute(const WorldCity &city, const Difficulty diff)
{
    const double mult = DifficultyHelpers::costMultiplier(diff);
    const int count = std::round(city.payTributeCount() * mult);
    return {city.payTributeType(), count};
}

TributePayment TributeHelpers::receiveTribute(const WorldCity &city)
{
    return {city.receiveTributeType(), city.receiveTributeCount()};
}

void TributeHelpers::receiveTributeFromCity(GameBoard &board, const ePlayerId pid, const stdsptr<WorldCity> &city, const bool postpone)
{
    if (!city)
        return;
    const auto e = e::make_shared<PayTributeEvent>(
        board.currentCityId(), eGameEventBranch::root, board);
    e->initialize(city);
    e->initializeDate(board.date());
    board.addRootGameEvent(e);
}

void TributeHelpers::payTributeToCity(GameBoard &board, const eCityId playerCityId, const stdsptr<WorldCity> &parentCity)
{
    if (!parentCity)
        return;
    if (!board.boardCityWithId(playerCityId))
        return;

    const auto e = e::make_shared<GetTributeEvent>(
        playerCityId, eGameEventBranch::root, board);
    e->initialize(parentCity);
    e->initializeDate(board.date());
    board.addRootGameEvent(e);
}
