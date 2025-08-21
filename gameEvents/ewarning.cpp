#include "ewarning.h"

eWarning::eWarning(const int warningDays,
                   eGameEvent& parent,
                   const eCityId cid,
                   eGameBoard& board) :
    mWarningDays(warningDays),
    mParent(parent),
    mCid(cid),
    mBoard(board) {}

void eWarning::setNextDate(const eDate &date) {
    mNextDate = date;
    mFinished = false;
}

void eWarning::handleNewDate(const eDate &date) {
    if(mFinished) return;
    if(date > mNextDate) {
        trigger();
        mFinished = true;
    }
}

void eWarning::write(eWriteStream &dst) const {
    mNextDate.write(dst);
    dst << mFinished;
}

void eWarning::read(eReadStream &src) {
    mNextDate.read(src);
    src >> mFinished;
}
