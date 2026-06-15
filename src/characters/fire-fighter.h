#ifndef FIRE_FIGHTER_H
#define FIRE_FIGHTER_H

#include "echaracter.h"

#include "textures/character-textures.h"

class FireFighter : public eCharacter {
public:
    using eCharTexs = FireFighterTextures CharacterTextures::*;
    FireFighter(GameBoard& board);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    void beingKilled() override;
private:
    const std::vector<CharacterTextures>& mTextures;
    const eCharTexs mCharTexs;
};

#endif // FIRE_FIGHTER_H
