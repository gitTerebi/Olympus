#ifndef EREINFORCEMENTS_H
#define EREINFORCEMENTS_H

#include "characters/eenlistedforces.h"

class eSaveArchive;

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
    void serialize(eSaveArchive& ar, eGameBoard* board);

    eEnlistedForces mForces;
    eCityId mFromCid;
};

#endif // EREINFORCEMENTS_H
