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
    std::string resName;
    if(mResources[0] != eResourceType::none) {
        resName += eResourceTypeHelpers::typeName(mResources[0]);
    }
    if(mResources[1] != eResourceType::none) {
        if(!resName.empty()) resName += "/";
        resName += eResourceTypeHelpers::typeName(mResources[1]);
    }
    if(mResources[2] != eResourceType::none) {
        if(!resName.empty()) resName += "/";
        resName += eResourceTypeHelpers::typeName(mResources[2]);
    }
    const auto cStr = std::to_string(mMinCount) + "-" +
                      std::to_string(mMaxCount);
    eStringHelpers::replace(tmpl, "%1", cStr + " " + resName);
    return tmpl;
}
