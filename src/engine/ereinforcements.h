#ifndef EREINFORCEMENTS_H
#define EREINFORCEMENTS_H

#include "characters/eenlistedforces.h"

class eSaveArchive;
class SoldierBanner;

class eReinforcements {
public:
    eReinforcements();
    eReinforcements(const eEnlistedForces& forces,
                    const eCityId fromCid);

    const eEnlistedForces& forces() const { return mForces; }
    eCityId fromCid() const { return mFromCid; }

    bool remove(const stdsptr<SoldierBanner>& b);
    bool checkEmpty() const;

    void serialize(eSaveArchive& ar, GameBoard* board);
private:

    eEnlistedForces mForces;
    eCityId mFromCid;
};

#endif // EREINFORCEMENTS_H
