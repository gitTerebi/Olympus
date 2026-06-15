#ifndef ESOLDIER_H
#define ESOLDIER_H

#include "efightingpatroler.h"
#include "efightingcharacter.h"

class SoldierAction;
class SoldierBanner;
class SaveArchive;

class eSoldier : public eFightingPatroler,
                 public eFightingCharacter {
public:
    using eCharTexs = FightingCharacterTextures CharacterTextures::*;
    eSoldier(GameBoard& board,
             const eCharTexs charTexs,
             const eCharacterType type);
    ~eSoldier();

    void beingKilled() override;

    SoldierAction* soldierAction() const;

    SoldierBanner* banner() const;
    void setBanner(SoldierBanner* const b);
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdptr<SoldierBanner> mBanner;
};

#endif // ESOLDIER_H
