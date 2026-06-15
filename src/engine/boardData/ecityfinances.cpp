#include "ecityfinances.h"

eCityFinances::eCityFinances() {}

void eCityFinances::add(const int d, const eFinanceTarget t) {
    mThisYear.add(d, t);
}

void eCityFinances::nextYear() {
    mLastYear = mThisYear;
    mThisYear = eFinanceYear();
}

void eCityFinances::serialize(SaveArchive& ar) {
    ar.archiveField("lastYear", [this](SaveArchive& itemAr) { mLastYear.serialize(itemAr); });
    ar.archiveField("thisYear", [this](SaveArchive& itemAr) { mThisYear.serialize(itemAr); });
}
