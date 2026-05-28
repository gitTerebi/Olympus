#ifndef EARTEMISSANCTUARY_H
#define EARTEMISSANCTUARY_H

#include "sanctuary.h"

class eSanctuaryWithWarriors : public eSanctuary {
public:
    using eSanctuary::eSanctuary;

    void timeChanged(const int by) override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    int mSoldierSpawn = 0;
    std::vector<stdsptr<SoldierBanner>> mSoldierBanners;
};

class eArtemisSanctuary: public eSanctuaryWithWarriors  {
public:
    eArtemisSanctuary(const int sw, const int sh,
                      GameBoard& board, const eCityId cid);
};

class eAresSanctuary: public eSanctuaryWithWarriors  {
public:
    eAresSanctuary(const int sw, const int sh,
                   GameBoard& board, const eCityId cid);
};

#endif // EARTEMISSANCTUARY_H
