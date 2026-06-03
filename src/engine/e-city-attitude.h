#ifndef ECITYATTITUDE_H
#define ECITYATTITUDE_H

#include "world-city.h"

enum class eCityAttitudeMessage {
    none,
    allyResentful,
    rivalHostile,
    vassalRebellious,
    colonyRebellious,
    parentResentful,
    allyLovesYou,
    rivalPleased,
    vassalLovesYou,
    colonyLovesYou,
    parentLovesYou
};

eCityAttitudeMessage eCityAttitudeMessageForChange(
        const WorldCity& city,
        const ePlayerId pid,
        const eCityAttitude oldAttitude,
        const eCityAttitude newAttitude);

#endif // ECITYATTITUDE_H
