#include "ereinforcements.h"
#include "ecityid.h"

#include "evectorhelpers.h"
#include "fileIO/esavearchive.h"

eReinforcements::eReinforcements() {}

eReinforcements::eReinforcements(const eEnlistedForces& forces,
                                 const eCityId fromCid) :
    mForces(forces),
    mFromCid(fromCid) {}

bool eReinforcements::remove(const stdsptr<SoldierBanner>& b) {
    return eVectorHelpers::remove(mForces.fSoldiers, b);
}

bool eReinforcements::checkEmpty() const {
    return mForces.fSoldiers.empty();
}

void eReinforcements::serialize(eSaveArchive& ar, eGameBoard* board) {
    ar.field("fromCityId", mFromCid, eCityId::neutralFriendly);
    ar.archiveField("forces", [this, board](eSaveArchive& itemAr) {
        mForces.serialize(itemAr, itemAr.reading() ? board : nullptr);
    });
}
