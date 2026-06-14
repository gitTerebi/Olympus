#ifndef EHERMES_H
#define EHERMES_H

#include "god.h"

class Hermes : public God {
public:
    Hermes(GameBoard& board);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
};

#endif // EHERMES_H
