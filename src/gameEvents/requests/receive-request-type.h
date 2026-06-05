#ifndef RECEIVE_REQUEST_TYPE_H
#define RECEIVE_REQUEST_TYPE_H

#include "fulfill-request-event.h"
#include "engine/eevent.h"
#include "emessages.h"
#include "engine/world-city.h"

enum class ReceiveRequestFinish {
    tooLate,
    comply,
    refuse
};

eEvent receiveRequestFinishEvent(
    const ReceiveRequestType type,
    const WorldCity& city,
    const ReceiveRequestFinish finish);

eEvent receiveRequestStepEvent(
    const ReceiveRequestType type,
    const WorldCity& city,
    int requestStep);

const eReceiveRequestMessages* receiveRequestMessages(
    const ReceiveRequestType type,
    const WorldCity& city);

#endif // RECEIVE_REQUEST_TYPE_H
