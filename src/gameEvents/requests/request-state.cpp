#include "request-state.h"

#include "engine/eeventdata.h"
#include "engine/game-board.h"
#include "language.h"
#include "string-helpers.h"

#include <algorithm>

const RequestState& requestState(
    const std::vector<RequestState>& states,
    const int step)
{
    if(step >= 0 && step < static_cast<int>(states.size())) {
        return states[step];
    }
    return states.front();
}

bool requestTerminalState(
    const std::vector<RequestState>& states,
    const int step)
{
    return requestState(states, step).fComplyMonths == 0;
}

int requestRemainingMonths(const eDate& deadline, const eDate& current)
{
    const int daysDiff = deadline - current;
    return (daysDiff + 30) / 31;
}

std::string requestCountdownText(
    const int complyMonths,
    const eDate& complyStartDate,
    const eDate& currentDate)
{
    const int elapsedMonths =
        complyStartDate == eDate(1, eMonth::january, 1) ?
            0 :
            requestRemainingMonths(currentDate, complyStartDate);
    const int remainingMonths = std::max(0, complyMonths - elapsedMonths);
    return std::to_string(remainingMonths);
}

std::string resourceRequestInfo(
    const eResourceType resource,
    const int requested,
    const int stock,
    const bool overdue,
    const int remainingMonths)
{
    const auto resName = eResourceTypeHelpers::typeLongName(resource);
    std::string status;
    if(!overdue) {
        status = Language::zeusText(212, 63); // [months_remaining] months remain
        StringHelpers::replaceAll(status, "[months_remaining]",
                                   std::to_string(remainingMonths));
    } else {
        status = std::to_string(remainingMonths) + " " +
                 Language::zeusText(5, 150) + " " +
                 Language::zeusText(5, 205);
    }

    auto stockText = Language::zeusText(44, 278); // in stock
    auto result = std::to_string(requested) + " " + resName +
                  " (" + std::to_string(stock) + " " + stockText + ")";
    if(!status.empty()) result += ", " + status;
    return result;
}

std::string resourceDispatchText(const std::string& requestInfo)
{
    return Language::zeusText(5, 12) + " " + requestInfo + "?";
}

void addRequestDispatchResponses(
    GameBoard& board,
    eEventData& ed,
    const ePlayerId pid,
    const eResourceType resource,
    const int count,
    const int dispatchResponse)
{
    const auto cids = board.playerCitiesOnBoard(pid);
    if(resource == eResourceType::drachmas) {
        if(!cids.empty() && board.drachmas(pid) >= count) {
            ed.fPrimaryResponse = dispatchResponse;
        }
        return;
    }

    for(const auto cid : cids) {
        const int avCount = board.resourceCount(cid, resource);
        ed.fCityNames[cid] = board.cityName(cid);
        ed.fCSpaceCount[cid] = avCount;
        if(avCount >= count) {
            ed.fCityConditionalResponses[cid] = dispatchResponse;
        }
    }
}
