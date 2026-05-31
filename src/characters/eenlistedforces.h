#ifndef EENLISTEDFORCES_H
#define EENLISTEDFORCES_H

#include "characters/heroes/ehero.h"
#include "engine/e-worldcity.h"

class eSaveArchive;
class SoldierBanner;

struct eEnlistedForces {
    std::vector<stdsptr<SoldierBanner>> fSoldiers;
    std::vector<std::pair<eCityId, eHeroType>> fHeroes;
    std::vector<stdsptr<eWorldCity>> fAllies;
    bool fAres = false;
    eCityId fAresCity;

    std::map<eCityId, eEnlistedForces> splitIntoCities() const;

    void serialize(eSaveArchive& ar, GameBoard* board);

    void clear();

    void add(const eEnlistedForces& o);

    int strength() const;
    void kill(const double killFrac) const;

    int count() const;
};

#endif // EENLISTEDFORCES_H
