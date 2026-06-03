#ifndef E_RECEIVE_REQUEST_TYPE_H
#define E_RECEIVE_REQUEST_TYPE_H

#include "e-fulfill-request-event.h"
#include "engine/eevent.h"
#include "emessages.h"
#include "engine/world-city.h"

enum class eReceiveRequestFinish {
    tooLate,
    comply,
    refuse
};

eEvent receiveRequestFinishEvent(
    const eReceiveRequestType type,
    const WorldCity& city,
    const eReceiveRequestFinish finish);

eEvent receiveRequestStepEvent(
    const eReceiveRequestType type,
    const WorldCity& city,
    int requestStep);

const eReceiveRequestMessages* receiveRequestMessages(
    const eReceiveRequestType type,
    const WorldCity& city);

#endif // E_RECEIVE_REQUEST_TYPE_H
