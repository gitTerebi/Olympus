#ifndef EBASICHERO_H
#define EBASICHERO_H

#include "ehero.h"
#include "textures/character-textures.h"

class eBasicHero : public eHero {
public:
    using eCharTexs = HeroTextures CharacterTextures::*;
    eBasicHero(GameBoard& board,
               const eCharTexs charTexs,
               const eHeroType type);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
private:
    eCharTexs mCharTexs;
};

#endif // EBASICHERO_H
