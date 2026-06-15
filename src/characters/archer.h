#ifndef ARCHER_H
#define ARCHER_H

#include "echaracter.h"

class Archer : public eCharacter {
public:
    Archer(GameBoard& board);

    std::shared_ptr<Texture>
    getTexture(const eTileSize size) const;
};

#endif // ARCHER_H
