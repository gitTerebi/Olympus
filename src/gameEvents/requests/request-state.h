#ifndef REQUEST_STATE_H
#define REQUEST_STATE_H

#include "engine/ecityid.h"
#include "engine/edate.h"
#include "engine/eresourcetype.h"

#include <string>
#include <vector>

class GameBoard;
struct eEventData;

struct RequestState {
    int fPopupMonth;
    int fComplyMonths;
};

const RequestState& requestState(
    const std::vector<RequestState>& states,
    int step);
bool requestTerminalState(
    const std::vector<RequestState>& states,
    int step);
int requestRemainingMonths(const eDate& deadline, const eDate& current);
std::string requestCountdownText(int complyMonths,
                                 const eDate& complyStartDate,
                                 const eDate& currentDate);
std::string resourceRequestInfo(eResourceType resource,
                                int requested,
                                int stock,
                                bool overdue,
                                int remainingMonths);
std::string resourceDispatchText(const std::string& requestInfo);
void addRequestDispatchResponses(
    GameBoard& board,
    eEventData& ed,
    ePlayerId pid,
    eResourceType resource,
    int count,
    int dispatchResponse);

#endif // REQUEST_STATE_H
