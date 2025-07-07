#include "eraidresourceevent.h"

#include "engine/egameboard.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

eRaidResourceEvent::eRaidResourceEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
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

std::string eRaidResourceEvent::longName() const {
    return eLanguage::text("raid_resource_long_name");
}
