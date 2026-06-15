#ifndef EARTISAN_H
#define EARTISAN_H

#include "echaracter.h"

class eArtisan : public eCharacter {
public:
    eArtisan(GameBoard& board);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
};

#endif // EARTISAN_H
