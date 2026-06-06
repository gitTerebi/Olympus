#include "troops-sent-event.h"

#include "elanguage.h"
#include "estringhelpers.h"
#include "engine/eeventdata.h"
#include "engine/game-board.h"
#include "send-troops-event.h"

TroopsSentEvent::TroopsSentEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    PlayerConquestEventBase(cid, eGameEventType::troopsSent,
                            branch, board) {}

void TroopsSentEvent::initialize(
        const eEnlistedForces& forces,
        const stdsptr<WorldCity>& city,
        const stdsptr<WorldCity>& rivalCity) {
    mForces = forces;
    mCity = city;
    mRivalCity = rivalCity;
}

void TroopsSentEvent::trigger() {
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

    const auto me = mainEvent<SendTroopsEvent>();
    if(defended) {
        me->won();
    } else {
        me->lost();
    }

    planArmyReturn();
}

std::string TroopsSentEvent::longName() const {
    return eLanguage::zeusText(290, 6);
}
