#include "player-raid-event.h"
#include "fileIO/save-archive.h"

#include "engine/game-board.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "language.h"
#include "engine/egifthelpers.h"
#include "raid-resource-event.h"
#include "rand.h"

#include <algorithm>

PlayerRaidEvent::PlayerRaidEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    PlayerConquestEventBase(cid,
                            eGameEventType::playerRaidEvent,
                            branch, board) {}

void PlayerRaidEvent::initialize(
        const eEnlistedForces& forces,
        const stdsptr<WorldCity>& city,
        const eResourceType resource) {
    mForces = forces;
    mCity = city;
    mResource = resource;
}

int PlayerRaidEvent::raidTargetStrength(const int enemyStr) {
    return std::max(1, (3*enemyStr + 3)/4);
}

void PlayerRaidEvent::trigger() {
    removeArmyEvent();
    removeConquestEvent();
    if(!mCity) return;
    const auto board = gameBoard();
    if(!board) return;

    const int enemyStr = mCity->troops();
    const int str = mForces.strength();

    const double killFrac = std::clamp(0.5*enemyStr/str, 0., 1.);
    mForces.kill(killFrac);

    const bool raided = Rand::combatRoll(str, raidTargetStrength(enemyStr));
    const auto pid = playerId();
    eEventData ed(pid);
    ed.fCity = mCity;
    const auto rel = mCity->relationship();
    if(rel == eForeignCityRelationship::ally) {
        board->attackedAllyAttitude(pid);
        board->event(eEvent::allyAttackedByPlayer, ed);
    }
    if(raided) {
        eResourceType res = mResource;
        if(res == eResourceType::none) {
            if(Rand::rand() % 2) {
                res = eResourceType::drachmas;
            } else {
                const auto& sells = mCity->sells();
                if(sells.empty()) {
                    res = eResourceType::drachmas;
                } else {
                    const int ss = sells.size();
                    res = sells[Rand::rand() % ss].fType;
                }
            }
        }
        const int count = 2*eGiftHelpers::giftCount(res);
        const auto e = e::make_shared<RaidResourceEvent>(
                           cityId(), eGameEventBranch::child, *board);
        const auto boardDate = board->date();
        const int period = 75;
        const auto date = boardDate + period;
        e->initializeDate(date, period, 1);
        e->initialize(true, res, count, mCity);
        addConsequence(e);
    } else {
        board->event(eEvent::cityRaidFailed, ed);
    }

    planArmyReturn();
}

std::string PlayerRaidEvent::longName() const {
    return Language::text("player_raid_event_long_name");
}

void PlayerRaidEvent::serializeFields(SaveArchive& ar) {
    ArmyEventBase::serializeFields(ar);
    ar.field("resource", mResource, eResourceType::none);
}
