#ifndef REQUESTED_RESOURCES_TYPE_H
#define REQUESTED_RESOURCES_TYPE_H

#include "send-resources-to-city-event.h"
#include "engine/eevent.h"
#include "messages.h"
#include "engine/world-city.h"

enum class RequestedResourcesFinish {
    tooLate,
    comply,
    refuse
};

eEvent requestedResourcesFinishEvent(
    const RequestedResourcesType type,
    const WorldCity& city,
    const RequestedResourcesFinish finish);

eEvent requestedResourcesStepEvent(
    const RequestedResourcesType type,
    const WorldCity& city,
    int requestStep);

const RequestedResourcesMessages* requestedResourcesMessages(
    const RequestedResourcesType type,
    const WorldCity& city);

#endif // REQUESTED_RESOURCES_TYPE_H
