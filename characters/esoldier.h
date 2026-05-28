#ifndef ESOLDIER_H
#define ESOLDIER_H

#include "efightingpatroler.h"
#include "efightingcharacter.h"

class eSoldierAction;
class SoldierBanner;
class eSaveArchive;

class eSoldier : public eFightingPatroler,
                 public eFightingCharacter {
public:
    using eCharTexs = eFightingCharacterTextures eCharacterTextures::*;
    eSoldier(GameBoard& board,
             const eCharTexs charTexs,
             const eCharacterType type);
    ~eSoldier();

    void beingKilled() override;

    eSoldierAction* soldierAction() const;

    SoldierBanner* banner() const;
    void setBanner(SoldierBanner* const b);
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    stdptr<SoldierBanner> mBanner;
};

#endif // ESOLDIER_H
