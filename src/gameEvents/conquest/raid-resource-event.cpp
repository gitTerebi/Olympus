#include "raid-resource-event.h"

#include "engine/game-board.h"
#include "language.h"
#include "string-helpers.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

RaidResourceEvent::RaidResourceEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eResourceGrantedEventBase(
        cid,
        eEvent::raidCashAccepted,
        eEvent::raidAccepted,
        eEvent::raidPostponed,
        eEvent::raidRefused,
        eEvent::raidForfeited,
        eEvent::raidGranted,
        eEvent::raidLastChance,
        eEvent::raidInsufficientSpace,
        eEvent::raidPartialSpace,
        eGameEventType::raidResourceReceive, branch,
        board) {}

std::string RaidResourceEvent::longName() const {
    return Language::text("raid_resource_long_name");
}
