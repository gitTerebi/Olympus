#include "ewarning.h"

#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

eWarning::eWarning(const int warningMonths,
                   const bool initialWarning,
                   eGameEvent& parent,
                   const eCityId cid,
                   eGameBoard& board) :
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

void eWarning::write(eWriteStream &dst) const {
    eSaveArchive ar(dst);
    if(ar.writing()) const_cast<eDate&>(mNextDate).write(ar.writeStream());
    ar.field("mFinished", const_cast<bool&>(mFinished));
    ar.field("mWarningMonths", const_cast<int&>(mWarningMonths));
}

void eWarning::read(eReadStream &src) {
    eSaveArchive ar(src);
    if(ar.reading()) mNextDate.read(ar.readStream());
    ar.field("mFinished", mFinished);
    ar.field("mWarningMonths", mWarningMonths);
}

void eWarning::serializeJson(eJsonArchive& ar) {
    { auto a = ar.child("nextDate"); mNextDate.serializeJson(a); }
    ar.field("mFinished", mFinished);
    ar.field("mWarningMonths", mWarningMonths);
}
