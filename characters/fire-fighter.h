#ifndef FIRE_FIGHTER_H
#define FIRE_FIGHTER_H

#include "echaracter.h"

#include "textures/echaractertextures.h"

class FireFighter : public eCharacter {
public:
    using eCharTexs = eFireFighterTextures eCharacterTextures::*;
    FireFighter(GameBoard& board);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    void beingKilled() override;
private:
    const std::vector<eCharacterTextures>& mTextures;
    const eCharTexs mCharTexs;
};

#endif // FIRE_FIGHTER_H
