#ifndef ANIMAL_H
#define ANIMAL_H

#include "echaracter.h"

#include "textures/character-textures.h"

class Animal : public eCharacter {
public:
    using eCharTexs = AnimalTextures CharacterTextures::*;
    Animal(GameBoard& board,
            const eCharTexs charTexs,
            const eCharacterType type);

    void setTextures(const eCharTexs& texs);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
private:
    const std::vector<CharacterTextures>& mTextures;
    eCharTexs mCharTexs;
};

#endif // ANIMAL_H
