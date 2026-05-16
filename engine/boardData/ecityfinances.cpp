#include "ecityfinances.h"

eCityFinances::eCityFinances() {}

void eCityFinances::add(const int d, const eFinanceTarget t) {
    mThisYear.add(d, t);
}

void eCityFinances::nextYear() {
    mLastYear = mThisYear;
    mThisYear = eFinanceYear();
}

void eCityFinances::serialize(eSaveArchive& ar) {
    ar.archiveField("lastYear", [this](eSaveArchive& itemAr) { mLastYear.serialize(itemAr); });
    ar.archiveField("thisYear", [this](eSaveArchive& itemAr) { mThisYear.serialize(itemAr); });
}

void eCityFinances::read(eReadStream &src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eCityFinances::write(eWriteStream &dst) const {
    eSaveArchive ar(dst);
    const_cast<eCityFinances*>(this)->serialize(ar);
}
