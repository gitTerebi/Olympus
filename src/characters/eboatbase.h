#ifndef EBOATBASE_H
#define EBOATBASE_H

#include "echaracter.h"

#include "textures/character-textures.h"

class eBoatBase : public eCharacter {
public:
    using eCharTexs = TradeBoatTextures CharacterTextures::*;
    eBoatBase(GameBoard& board, const eCharTexs charTexs,
              const eCharacterType type);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;

    void setCharTexs(const eCharTexs& texs);
private:
    eCharTexs mCharTexs;
};

#endif // EBOATBASE_H
