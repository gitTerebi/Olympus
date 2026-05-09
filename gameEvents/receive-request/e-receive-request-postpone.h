#ifndef E_RECEIVE_REQUEST_POSTPONE_H
#define E_RECEIVE_REQUEST_POSTPONE_H

#include "e-receive-request-event.h"
#include "e-receive-request-type.h"

class eGameBoard;

bool canPostponeRequestStep(int requestStep);
bool canPostponeRequestStep(const eReceiveRequestEvent* request);
bool isPostponedRequestStep(int requestStep);

int displayMonthsForRequestStep(
    const eReceiveRequestEvent* request,
    int requestStep);

void advanceRequestStep(eReceiveRequestEvent* request, eGameBoard& board);

#endif // E_RECEIVE_REQUEST_POSTPONE_H
