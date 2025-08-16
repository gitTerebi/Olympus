#include "egiftfromevent.h"

#include "engine/egameboard.h"
#include "elanguage.h"
#include "estringhelpers.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"

eGiftFromEvent::eGiftFromEvent(const eCityId cid,
                               const eGameEventBranch branch,
                               eGameBoard& board) :
    eResourceGrantedEventBase(
        cid,
        eEvent::giftCashAccepted,
        eEvent::giftAccepted,
        eEvent::giftPostponed,
        eEvent::giftRefused,
        eEvent::giftForfeited,
        eEvent::giftGranted,
        eEvent::giftLastChance,
        eEvent::giftInsufficientSpace,
        eEvent::giftPartialSpace,
        eGameEventType::giftFrom, branch, board) {}

std::string eGiftFromEvent::longName() const {
    auto tmpl = eLanguage::text("gift_of_from_long_name");
    longNameReplaceResource("%1", tmpl);
    return tmpl;
}
