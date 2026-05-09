#ifndef E_RECEIVE_REQUEST_POSTPONE_H
#define E_RECEIVE_REQUEST_POSTPONE_H

#include "e-fulfill-request-event.h"
#include "e-receive-request-type.h"

class eGameBoard;

bool canPostponeRequestStep(int requestStep);
bool canPostponeRequestStep(const eFulfillRequestEvent* request);
bool isPostponedRequestStep(int requestStep);

int displayMonthsForRequestStep(
    const eFulfillRequestEvent* request,
    int requestStep);

void advanceRequestStep(eFulfillRequestEvent* request, eGameBoard& board);

#endif // E_RECEIVE_REQUEST_POSTPONE_H
