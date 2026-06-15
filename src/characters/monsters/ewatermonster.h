#ifndef EWATERMONSTER_H
#define EWATERMONSTER_H

#include "emonster.h"
#include "textures/character-textures.h"

class eWaterMonster : public eMonster {
public:
    using eCharTexs = WaterMonsterTextures CharacterTextures::*;
    eWaterMonster(GameBoard& board,
                  const eCharTexs charTexs,
                  const eMonsterType type);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
private:
    eCharTexs mCharTexs;
};

#endif // EWATERMONSTER_H
