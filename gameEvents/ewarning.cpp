#include "ewarning.h"

eWarning::eWarning(const int warningMonths,
                   eGameEvent& parent,
                   const eCityId cid,
                   eGameBoard& board) :
    mParent(parent),
    mCid(cid),
    mBoard(board),
    mWarningMonths(warningMonths) {}

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
    dst << mWarningMonths;
}

void eWarning::read(eReadStream &src) {
    mNextDate.read(src);
    src >> mFinished;
    src >> mWarningMonths;
}
