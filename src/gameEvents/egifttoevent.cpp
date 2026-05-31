#include "egifttoevent.h"

#include "engine/game-board.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "fileIO/esavearchive.h"

eGiftToEvent::eGiftToEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::giftTo, branch, board) {}

void eGiftToEvent::initialize(const stdsptr<eWorldCity>& c,
                              const eResourceType type,
                              const int count) {
    mCity = c;
    mResource = type;
    mCount = count;
}

void eGiftToEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    const auto pid = playerId();
    board->giftToReceived(mCity, mResource, mCount, pid);
}

std::string eGiftToEvent::longName() const {
    auto tmpl = eLanguage::text("gift_of_to");
    const auto resName = eResourceTypeHelpers::typeName(mResource);
    const auto cStr = std::to_string(mCount);
    eStringHelpers::replace(tmpl, "%1", cStr + " " + resName);
    const auto none = eLanguage::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    eStringHelpers::replace(tmpl, "%2", ctstr);
    return tmpl;
}

void eGiftToEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.field("count", mCount, 0);
    ar.field("resource", mResource, eResourceType{});
    ar.worldCityField("city", worldBoard(), mCity);
}
