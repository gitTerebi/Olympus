#ifndef EPIER_H
#define EPIER_H

#include "ebuilding.h"

class ePier : public eBuilding {
public:
    ePier(eGameBoard& board, const eDiagonalOrientation o, const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;

    void collapse() override;

    void setTradePost(eBuilding* const b);
    eBuilding* tradePost() const { return mTradePost; }

    eDiagonalOrientation orientation() const { return mO; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    const eDiagonalOrientation mO;
    eBuilding* mTradePost = nullptr;
};

#endif // EPIER_H
