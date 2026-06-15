#ifndef EPLACEHOLDER_H
#define EPLACEHOLDER_H

#include "ebuilding.h"

class ePlaceholder : public eBuilding {
public:
    ePlaceholder(GameBoard& board, const eCityId cid);

    stdsptr<Texture> getTexture(const eTileSize size) const override;

    void erase() override;

    void sanctuaryErase();
};

#endif // EPLACEHOLDER_H
