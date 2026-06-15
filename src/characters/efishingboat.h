#ifndef EFISHINGBOAT_H
#define EFISHINGBOAT_H

#include "eresourcecollectorbase.h"

class eFishingBoat : public eResourceCollectorBase {
public:
    eFishingBoat(GameBoard& board);
    std::shared_ptr<Texture> getTexture(const eTileSize size) const;
};

#endif // EFISHINGBOAT_H
