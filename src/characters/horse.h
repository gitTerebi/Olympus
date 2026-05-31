#ifndef HORSE_H
#define HORSE_H

#include "echaracter.h"

class Horse : public eCharacter {
public:
    Horse(GameBoard& board);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;

    std::shared_ptr<eTexture> getNotRotatedTexture(const eTileSize size) const;
};

#endif // HORSE_H
