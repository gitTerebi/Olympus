#include "etroopsrequestfulfilledevent.h"

#include "elanguage.h"
#include "estringhelpers.h"
#include "engine/eeventdata.h"
#include "engine/egameboard.h"
#include "etroopsrequestevent.h"

eTroopsRequestFulfilledEvent::eTroopsRequestFulfilledEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    ePlayerConquestEventBase(cid, eGameEventType::troopsRequestFulfilled,
                             branch, board) {}

void eTroopsRequestFulfilledEvent::initialize(
        const eEnlistedForces& forces,
        const stdsptr<eWorldCity>& city,
        const stdsptr<eWorldCity>& rivalCity) {
    mForces = forces;
    mCity = city;
    mRivalCity = rivalCity;
}

void eTroopsRequestFulfilledEvent::trigger() {
    removeArmyEvent();
    removeConquestEvent();
    if(!mCity) return;

    const int enemyStr = mRivalCity->troops();
    const int str = mCity->troops() + mForces.strength();

    const double killFrac = std::clamp(0.5*enemyStr/str, 0., 1.);
    mForces.kill(killFrac);
    const int t = mCity->troops();
    mCity->setTroops((1 - 0.5*killFrac)*t);

    const bool defended = str > enemyStr;

    const auto me = mainEvent<eTroopsRequestEvent>();
    if(defended) {
        me->won();
    } else {
        me->lost();
    }

    planArmyReturn();
}

std::string eTroopsRequestFulfilledEvent::longName() const {
    return eLanguage::zeusText(290, 6);
}
