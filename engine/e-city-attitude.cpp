#include "e-city-attitude.h"

#include "e-game-board.h"
#include "eevent.h"
#include "eeventdata.h"

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

eCityAttitudeMessage attitudeMessageForInitialStatus(
        const eWorldCity& city,
        const ePlayerId pid) {
    return attitudeMessageForStatus(city, pid, city.attitudeClass(pid));
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

void GameBoard::changeCityAttitude(const stdsptr<eWorldCity>& c,
                                 const double amount,
                                 const ePlayerId pid) {
    if(!c) return;
    const auto oldAttitude = c->attitudeClass(pid);
    c->incAttitude(amount, pid);
    const auto newAttitude = c->attitudeClass(pid);
    const auto msg = eCityAttitudeMessageForChange(
        *c, pid, oldAttitude, newAttitude);
    if(msg == eCityAttitudeMessage::none) return;

    eEventData ed(pid);
    ed.fCity = c;
    ed.fCityAttitudeMessage = msg;
    event(eEvent::cityAttitudeChanged, ed);
}

void GameBoard::sendInitialCityAttitudeMessages() {
    const auto pid = personPlayer();
    for(const auto& c : mWorld.cities()) {
        const auto msg = attitudeMessageForInitialStatus(*c, pid);
        if(msg == eCityAttitudeMessage::none) continue;

        eEventData ed(pid);
        ed.fCity = c;
        ed.fCityAttitudeMessage = msg;
        event(eEvent::cityAttitudeChanged, ed);
    }
}
