#ifndef EFIGHTINGPATROLER_H
#define EFIGHTINGPATROLER_H

#include "echaracter.h"

#include "textures/character-textures.h"

class eFightingPatroler : public eCharacter {
public:
    using eCharTexs = FightingCharacterTextures CharacterTextures::*;
    eFightingPatroler(GameBoard& board, const eCharTexs charTexs,
                      const eCharacterType type);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;

    void setCharTexs(const eCharTexs& texs);
private:
    eCharTexs mCharTexs;
};

#endif // EFIGHTINGPATROLER_H
