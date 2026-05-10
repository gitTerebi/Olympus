#include "e-city-attitude.h"

namespace {
eCityAttitudeMessage attitudeMessageForStatus(
        const eWorldCity& city,
        const ePlayerId pid,
        const eCityAttitude attitude) {
    const auto rel = city.relationshipToPlayer(pid);
    const bool bad = city.attitude(pid) < 50;

    if(rel == eForeignCityRelationship::rival) {
        if(bad && attitude == eCityAttitude::hostile) {
            return eCityAttitudeMessage::rivalHostile;
        }
        if(!bad && attitude == eCityAttitude::admiring) {
            return eCityAttitudeMessage::rivalPleased;
        }
        return eCityAttitudeMessage::none;
    }

    if(city.isParentCity()) {
        if(bad && attitude == eCityAttitude::angry) {
            return eCityAttitudeMessage::parentResentful;
        }
        if(!bad && attitude == eCityAttitude::devoted) {
            return eCityAttitudeMessage::parentLovesYou;
        }
        return eCityAttitudeMessage::none;
    }

    if(city.isColony()) {
        if(bad && attitude == eCityAttitude::angry) {
            return eCityAttitudeMessage::colonyRebellious;
        }
        if(!bad && attitude == eCityAttitude::devoted) {
            return eCityAttitudeMessage::colonyLovesYou;
        }
        return eCityAttitudeMessage::none;
    }

    if(rel == eForeignCityRelationship::vassal) {
        if(bad && attitude == eCityAttitude::angry) {
            return eCityAttitudeMessage::vassalRebellious;
        }
        if(!bad && attitude == eCityAttitude::devoted) {
            return eCityAttitudeMessage::vassalLovesYou;
        }
        return eCityAttitudeMessage::none;
    }

    if(rel == eForeignCityRelationship::ally) {
        if(bad && attitude == eCityAttitude::annoyed) {
            return eCityAttitudeMessage::allyResentful;
        }
        if(!bad && attitude == eCityAttitude::helpful) {
            return eCityAttitudeMessage::allyLovesYou;
        }
    }

    return eCityAttitudeMessage::none;
}
}

eCityAttitudeMessage eCityAttitudeMessageForChange(
        const eWorldCity& city,
        const ePlayerId pid,
        const eCityAttitude oldAttitude,
        const eCityAttitude newAttitude) {
    if(oldAttitude == newAttitude) return eCityAttitudeMessage::none;

    return attitudeMessageForStatus(city, pid, newAttitude);
}

eCityAttitudeMessage eCityAttitudeMessageForInitialStatus(
        const eWorldCity& city,
        const ePlayerId pid) {
    return attitudeMessageForStatus(city, pid, city.attitudeClass(pid));
}
