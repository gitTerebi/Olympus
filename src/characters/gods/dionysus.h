#ifndef EDIONYSUS_H
#define EDIONYSUS_H

#include "god.h"

class Dionysus : public God {
public:
    Dionysus(GameBoard& board);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
};

#endif // EDIONYSUS_H
