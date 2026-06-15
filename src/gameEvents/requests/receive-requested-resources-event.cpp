#include "receive-requested-resources-event.h"

#include "engine/game-board.h"
#include "language.h"
#include "string-helpers.h"
#include "engine/eeventdata.h"
#include "engine/eevent.h"
#include "engine/egifthelpers.h"

ReceiveRequestedResourcesEvent::ReceiveRequestedResourcesEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eResourceGrantedEventBase(cid,
                              eEvent::requestAccepted,
                              eEvent::requestAccepted,
                              eEvent::requestPostponed,
                              eEvent::requestRefused,
                              eEvent::requestForfeited,
                              eEvent::requestGranted,
                              eEvent::requestLastChance,
                              eEvent::requestInsufficientSpace,
                              eEvent::requestPartialSpace,
                              eGameEventType::receiveRequestedResources,
                              branch, board) {}

void ReceiveRequestedResourcesEvent::initialize(
        const bool postpone,
        const eResourceType res,
        const stdsptr<WorldCity> &c) {
    mPostpone = postpone;
    mResource = res;
    mCity = c;
    mCount = 2*eGiftHelpers::giftCount(mResource);
}

std::string ReceiveRequestedResourcesEvent::longName() const {
    auto tmpl = Language::text("make_request");
    const auto resName = eResourceTypeHelpers::typeName(mResource);
    StringHelpers::replace(tmpl, "%1", resName);
    const auto none = Language::text("none");
    const auto ctstr = mCity ? mCity->name() : none;
    StringHelpers::replace(tmpl, "%2", ctstr);
    return tmpl;
}
