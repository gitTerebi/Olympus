#ifndef EPIER_H
#define EPIER_H

#include "ebuilding.h"

class ePier : public eBuilding {
public:
    ePier(eGameBoard& board, const eDiagonalOrientation o, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;

    void erase();

    void setTradePost(eBuilding* const b);
    eBuilding* tradePost() const { return mTradePost; }

    eDiagonalOrientation orientation() const { return mO; }
private:
    const eDiagonalOrientation mO;
    eBuilding* mTradePost = nullptr;
};

#endif // EPIER_H
