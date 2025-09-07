#include "ereinforcements.h"

#include "evectorhelpers.h"

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
    dst << mFromCid;
    mForces.write(dst);
}

void eReinforcements::read(eGameBoard& board, eReadStream& src) {
    src >> mFromCid;
    mForces.read(board, src);
}
