#ifndef EBASICMONSTER_H
#define EBASICMONSTER_H

#include "emonster.h"
#include "textures/character-textures.h"

class eBasicMonster : public eMonster {
public:
    using eCharTexs = MonsterTextures CharacterTextures::*;
    eBasicMonster(GameBoard& board,
                  const eCharTexs charTexs,
                  const eMonsterType type);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
private:
    eCharTexs mCharTexs;
};

#endif // EBASICMONSTER_H
