#ifndef ESOLDIER_H
#define ESOLDIER_H

#include "efightingpatroler.h"
#include "efightingcharacter.h"

class eSoldierAction;
class eSoldierBanner;

class eSoldier : public eFightingPatroler,
                 public eFightingCharacter {
public:
    using eCharTexs = eFightingCharacterTextures eCharacterTextures::*;
    eSoldier(eGameBoard& board,
             const eCharTexs charTexs,
             const eCharacterType type);
    ~eSoldier();

    void beingKilled() override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    eSoldierAction* soldierAction() const;

    eSoldierBanner* banner() const;
    void setBanner(eSoldierBanner* const b);
private:
    stdptr<eSoldierBanner> mBanner;
};

#endif // ESOLDIER_H
