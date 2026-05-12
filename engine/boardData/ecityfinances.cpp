#include "ecityfinances.h"
#include "fileIO/ejsonarchive.h"

eCityFinances::eCityFinances() {}

void eCityFinances::add(const int d, const eFinanceTarget t) {
    mThisYear.add(d, t);
}

void eCityFinances::nextYear() {
    mLastYear = mThisYear;
    mThisYear = eFinanceYear();
}

void eCityFinances::read(eReadStream &src) {
    mLastYear.read(src);
    mThisYear.read(src);
}

void eCityFinances::write(eWriteStream &dst) const {
    mLastYear.write(dst);
    mThisYear.write(dst);
}

void eCityFinances::serializeJson(eJsonArchive& ar) {
    auto ly = ar.child("lastYear");
    ly.field("taxesIn",           mLastYear.fTaxesIn);
    ly.field("exports",           mLastYear.fExports);
    ly.field("giftsReceived",     mLastYear.fGiftsReceived);
    ly.field("minedSilver",       mLastYear.fMinedSilver);
    ly.field("tributeReceived",   mLastYear.fTributeReceived);
    ly.field("hippodrome",        mLastYear.fHippodrome);
    ly.field("importCosts",       mLastYear.fImportCosts);
    ly.field("wages",             mLastYear.fWages);
    ly.field("construction",      mLastYear.fConstruction);
    ly.field("bribesTributePaid", mLastYear.fBribesTributePaid);
    ly.field("giftsAndAidGiven",  mLastYear.fGiftsAndAidGiven);
    auto ty = ar.child("thisYear");
    ty.field("taxesIn",           mThisYear.fTaxesIn);
    ty.field("exports",           mThisYear.fExports);
    ty.field("giftsReceived",     mThisYear.fGiftsReceived);
    ty.field("minedSilver",       mThisYear.fMinedSilver);
    ty.field("tributeReceived",   mThisYear.fTributeReceived);
    ty.field("hippodrome",        mThisYear.fHippodrome);
    ty.field("importCosts",       mThisYear.fImportCosts);
    ty.field("wages",             mThisYear.fWages);
    ty.field("construction",      mThisYear.fConstruction);
    ty.field("bribesTributePaid", mThisYear.fBribesTributePaid);
    ty.field("giftsAndAidGiven",  mThisYear.fGiftsAndAidGiven);
}
