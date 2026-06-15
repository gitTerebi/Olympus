#ifndef EHERMES_H
#define EHERMES_H

#include "god.h"

class Hermes : public God {
public:
    Hermes(GameBoard& board);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
};

#endif // EHERMES_H
