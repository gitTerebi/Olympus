#ifndef EBASICPATROLER_H
#define EBASICPATROLER_H

#include "echaracter.h"

#include "textures/character-textures.h"

class eBasicPatroler : public eCharacter {
public:
    using eCharTexs = BasicCharacterTextures CharacterTextures::*;
    eBasicPatroler(GameBoard& board,
                   const eCharTexs charTexs,
                   const eCharacterType type);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    void beingKilled() override;
protected:
    void setCharTextures(const eCharTexs& texs);
private:
    eCharTexs mCharTexs;
};

#endif // EBASICPATROLER_H
