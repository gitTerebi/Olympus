#ifndef EPIER_H
#define EPIER_H

#include "ebuilding.h"
#include "engine/eorientation.h"

class ePier : public eBuilding {
public:
    ePier(GameBoard& board, const eDiagonalOrientation o, const eCityId cid);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void collapse() override;
    void erase() override;

    void setTradePost(eBuilding* const b);
    eBuilding* tradePost() const { return mTradePost; }

    eDiagonalOrientation orientation() const { return mO; }

    void setLoading(const bool b) { mLoading = b; }
    bool loading() const { return mLoading; }

    static double sOvX[4];
    static double sOvY[4];
    static double sLoadOvX[4];
    static double sLoadOvY[4];
    static int sOrientIndex(const eDiagonalOrientation o);
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    const eDiagonalOrientation mO;
    eBuilding* mTradePost = nullptr;
    bool mLoading = false; // transient, never serialized
};

#endif // EPIER_H
