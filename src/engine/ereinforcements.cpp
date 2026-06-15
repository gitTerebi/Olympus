#include "ereinforcements.h"
#include "ecityid.h"

#include "vector-helpers.h"
#include "fileIO/save-archive.h"
#include "characters/soldier-banner.h"

eReinforcements::eReinforcements() {}

eReinforcements::eReinforcements(const eEnlistedForces& forces,
                                 const eCityId fromCid) :
    mForces(forces),
    mFromCid(fromCid) {}

bool eReinforcements::remove(const stdsptr<SoldierBanner>& b) {
    return VectorHelpers::remove(mForces.fSoldiers, b);
}

bool eReinforcements::checkEmpty() const {
    return mForces.fSoldiers.empty();
}

void eReinforcements::serialize(SaveArchive& ar, GameBoard* board) {
    ar.field("fromCityId", mFromCid, eCityId::neutralFriendly);
    ar.archiveField("forces", [this, board](SaveArchive& itemAr) {
        mForces.serialize(itemAr, itemAr.reading() ? board : nullptr);
    });
}
