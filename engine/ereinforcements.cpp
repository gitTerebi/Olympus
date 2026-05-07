#include "ereinforcements.h"

#include "evectorhelpers.h"
#include "fileIO/esavearchive.h"

eReinforcements::eReinforcements() {}

eReinforcements::eReinforcements(const eEnlistedForces& forces,
                                 const eCityId fromCid) :
    mForces(forces),
    mFromCid(fromCid) {}

bool eReinforcements::remove(const stdsptr<eSoldierBanner>& b) {
    return eVectorHelpers::remove(mForces.fSoldiers, b);
}

bool eReinforcements::checkEmpty() const {
    return mForces.fSoldiers.empty();
}

void eReinforcements::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eReinforcements*>(this)->serialize(ar, nullptr);
}

void eReinforcements::read(eGameBoard& board, eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar, &board);
}

void eReinforcements::serialize(eSaveArchive& ar, eGameBoard* board) {
    ar.value(mFromCid);
    if(ar.reading()) {
        mForces.serialize(ar, board);
    } else {
        mForces.serialize(ar, nullptr);
    }
}
