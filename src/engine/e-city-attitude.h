#ifndef ECITYATTITUDE_H
#define ECITYATTITUDE_H

#include "e-worldcity.h"

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
        const eWorldCity& city,
        const ePlayerId pid,
        const eCityAttitude oldAttitude,
        const eCityAttitude newAttitude);

#endif // ECITYATTITUDE_H
