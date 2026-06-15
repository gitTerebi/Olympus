#include "egifttoevent.h"

#include "engine/game-board.h"
#include "language.h"
#include "string-helpers.h"
#include "fileIO/save-archive.h"

eGiftToEvent::eGiftToEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::giftTo, branch, board) {}

void eGiftToEvent::initialize(const stdsptr<WorldCity>& c,
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
    auto tmpl = Language::text("gift_of_to");
    const auto resName = eResourceTypeHelpers::typeName(mResource);
    const auto cStr = std::to_string(mCount);
    StringHelpers::replace(tmpl, "%1", cStr + " " + resName);
    const auto none = Language::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    StringHelpers::replace(tmpl, "%2", ctstr);
    return tmpl;
}

void eGiftToEvent::serializeFields(SaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.field("count", mCount, 0);
    ar.field("resource", mResource, eResourceType{});
    ar.worldCityField("city", worldBoard(), mCity);
}
