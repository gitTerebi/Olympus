#include "ewarning.h"

#include "fileIO/esavearchive.h"

eWarning::eWarning(const int warningMonths,
                   const bool initialWarning,
                   eGameEvent& parent,
                   const eCityId cid,
                   GameBoard& board) :
    mParent(parent),
    mCid(cid),
    mBoard(board),
    mInitialWarning(initialWarning),
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

void eWarning::serialize(eSaveArchive& ar) {
    ar.dateField("nextDate", mNextDate);
    ar.field("finished", mFinished, true);
    ar.field("warningMonths", mWarningMonths, 2);
}
