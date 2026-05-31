#ifndef ANIMAL_H
#define ANIMAL_H

#include "echaracter.h"

#include "textures/echaractertextures.h"

class Animal : public eCharacter {
public:
    using eCharTexs = eAnimalTextures eCharacterTextures::*;
    Animal(GameBoard& board,
            const eCharTexs charTexs,
            const eCharacterType type);

    void setTextures(const eCharTexs& texs);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
private:
    const std::vector<eCharacterTextures>& mTextures;
    eCharTexs mCharTexs;
};

#endif // ANIMAL_H
