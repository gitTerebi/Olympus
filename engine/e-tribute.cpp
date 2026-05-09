#include "e-tribute.h"

#include "e-game-board.h"
#include "eevent.h"
#include "eeventdata.h"
#include "e-worldcity.h"
#include "gameEvents/e-pay-tribute-event.h"
#include "gameEvents/ereceivetributeevent.h"

#include <cmath>
#include <cstdio>

eTributePayment eTributeHelpers::payTribute(const eWorldCity &city, const eDifficulty diff)
{
    const double mult = eDifficultyHelpers::costMultiplier(diff);
    const int count = std::round(city.payTributeCount() * mult);
    return {city.payTributeType(), count};
}

eTributePayment eTributeHelpers::receiveTribute(const eWorldCity &city)
{
    return {city.receiveTributeType(), city.receiveTributeCount()};
}

void eTributeHelpers::receiveTributeFromCity(eGameBoard &board, const ePlayerId pid, const stdsptr<eWorldCity> &city, const bool postpone)
{
    if (!city)
        return;

    const auto tribute = receiveTribute(*city);
    const auto type = tribute.fType;
    const int count = tribute.fCount;

    eEventData ed(pid);
    ed.fType = eMessageEventType::requestTributeGranted;
    ed.fCity = city;
    if (type == eResourceType::drachmas)
    {
        ed.fPrimaryAction = [&board, city, count, pid]() { // accept
            const auto p = board.boardPlayerWithId(pid);
            if (p)
                p->incDrachmas(count, eFinanceTarget::tributeReceived);
            return count;
        };
    }
    else
    {
        const auto cids = board.playerCitiesOnBoard(pid);
        for (const auto cid : cids)
        {
            ed.fCSpaceCount[cid] = board.spaceForResource(cid, type);
            ed.fCityNames[cid] = board.cityName(cid);
            ed.fCityConditionalActions[cid] = [&board, cid, city, type, count, pid]() { // accept
                const int a = board.addResource(cid, type, count);
                if (a == count)
                    return;
                eEventData ed(pid);
                ed.fType = eMessageEventType::resourceGranted;
                ed.fCity = city;
                ed.fResourceType = type;
                ed.fResourceCount = a;
                board.event(eEvent::tributeAccepted, ed);
            };
        }
    }
    ed.fResourceType = type;
    ed.fResourceCount = count;
    if (postpone)
    {
        ed.fSecondaryAction = [&board, city, type, count, pid]() { // postpone
            eEventData ed(pid);
            ed.fType = eMessageEventType::resourceGranted;
            ed.fCity = city;
            ed.fResourceType = type;
            ed.fResourceCount = count;
            board.event(eEvent::tributePostponed, ed);

            const auto e = e::make_shared<eReceiveTributeEvent>(
                board.currentCityId(),
                eGameEventBranch::root, board);
            e->initialize(city);
            auto date = board.date();
            date.nextYears(1);
            e->initializeDate(date);
            board.addRootGameEvent(e);
        };
    }
    ed.fTertiaryAction = [&board, city, type, count, pid]() { // decline
        eEventData ed(pid);
        ed.fType = eMessageEventType::resourceGranted;
        ed.fCity = city;
        ed.fResourceType = type;
        ed.fResourceCount = count;
        board.event(eEvent::tributeDeclined, ed);
    };
    board.event(eEvent::tributePaid, ed);
}

void eTributeHelpers::payTributeToCity(eGameBoard &board, const eCityId playerCityId, const stdsptr<eWorldCity> &parentCity)
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
