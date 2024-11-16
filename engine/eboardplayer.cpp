#include "eboardplayer.h"

#include "egameboard.h"
#include "eevent.h"
#include "eeventdata.h"

eBoardPlayer::eBoardPlayer(eGameBoard& board) :
    mBoard(board) {}

void eBoardPlayer::nextMonth() {
    if(mDrachmas < 0) {
        const auto date = mBoard.date();
        const bool sameMonth = date.month() == mInDebtSince.month();
        const bool oneYear = date.year() - mInDebtSince.year() == 1;
        if(sameMonth && oneYear) {
            if(isPerson()) {
                eEventData ed;
                mBoard.event(eEvent::debtAnniversary, ed);
            }
        }
    }
}

void eBoardPlayer::incDrachmas(const int by) {
    const bool wasInDebt = mDrachmas < 0;
    mDrachmas += by;
    const bool isInDebt = mDrachmas < 0;
    if(!wasInDebt && isInDebt) {
        mInDebtSince = mBoard.date();
    }
}

bool eBoardPlayer::isPerson() const {
    return mBoard.personPlayer() == mId;
}
