#ifndef E_RECEIVE_REQUEST_TYPE_H
#define E_RECEIVE_REQUEST_TYPE_H

#include "e-fulfill-request-event.h"
#include "engine/eevent.h"
#include "emessages.h"
#include "engine/e-worldcity.h"

enum class eReceiveRequestFinish {
    tooLate,
    comply,
    refuse
};

eEvent receiveRequestFinishEvent(
    const eReceiveRequestType type,
    const eWorldCity& city,
    const eReceiveRequestFinish finish);

eEvent receiveRequestStepEvent(
    const eReceiveRequestType type,
    const eWorldCity& city,
    int requestStep);

const eReceiveRequestMessages* receiveRequestMessages(
    const eReceiveRequestType type,
    const eWorldCity& city);

#endif // E_RECEIVE_REQUEST_TYPE_H
