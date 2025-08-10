#ifndef EPYRAMIDBUILDINGPART_H
#define EPYRAMIDBUILDINGPART_H

#include "epyramidelement.h"

class ePyramidBuildingPart : public ePyramidElement {
public:
    ePyramidBuildingPart(const std::vector<eSanctCost>& cost,
                         eGameBoard& board,
                         const int elevation,
                         const eCityId cid);
    ePyramidBuildingPart(ePyramid* const pyramid,
                         eGameBoard& board,
                         const int elevation,
                         const eCityId cid);

    stdsptr<eTexture> getTexture(const eTileSize) const override {
        return nullptr;
    }

    eTextureSpace
    getTextureSpace(const int tx, const int ty,
                    const eTileSize size) const override;

    void setPaint(ePyramidElement* const paint,
                  const eWorldDirection dir);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    ePyramidElement* mPaint = nullptr;
    eWorldDirection mPaintDir;
};

#endif // EPYRAMIDBUILDINGPART_H
