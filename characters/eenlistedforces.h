#ifndef EENLISTEDFORCES_H
#define EENLISTEDFORCES_H

#include "characters/esoldierbanner.h"
#include "characters/heroes/ehero.h"
#include "engine/e-worldcity.h"

class eSaveArchive;

struct eEnlistedForces {
    std::vector<stdsptr<eSoldierBanner>> fSoldiers;
    std::vector<std::pair<eCityId, eHeroType>> fHeroes;
    std::vector<stdsptr<eWorldCity>> fAllies;
    bool fAres = false;
    eCityId fAresCity;

    std::map<eCityId, eEnlistedForces> splitIntoCities() const;

    void serialize(eSaveArchive& ar, eGameBoard* board);

    void clear();

    void add(const eEnlistedForces& o);

    int strength() const;
    void kill(const double killFrac) const;

    int count() const;
};

#endif // EENLISTEDFORCES_H
