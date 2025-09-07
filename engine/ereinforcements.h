#ifndef EREINFORCEMENTS_H
#define EREINFORCEMENTS_H

#include "characters/eenlistedforces.h"

class eReinforcements {
public:
    eReinforcements();
    eReinforcements(const eEnlistedForces& forces,
                    const eCityId fromCid);

    const eEnlistedForces& forces() const { return mForces; }
    eCityId fromCid() const { return mFromCid; }

    bool remove(const stdsptr<eSoldierBanner>& b);
    bool checkEmpty() const;

    void write(eWriteStream& dst) const;
    void read(eGameBoard& board, eReadStream& src);
private:
    eEnlistedForces mForces;
    eCityId mFromCid;
};

#endif // EREINFORCEMENTS_H
