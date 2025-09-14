#include "ecityfinances.h"

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
