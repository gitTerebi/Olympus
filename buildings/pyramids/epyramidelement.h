#ifndef EPYRAMIDELEMENT_H
#define EPYRAMIDELEMENT_H

#include "../sanctuaries/esanctbuilding.h"

class ePyramid;

class ePyramidElement : public eSanctBuilding {
public:
    ePyramidElement(const std::vector<eSanctCost>& cost,
                    eGameBoard& board,
                    const eBuildingType type,
                    const int elevation,
                    const int dim,
                    const eCityId cid);
    ePyramidElement(ePyramid* const pyramid,
                    const std::vector<eSanctCost>& cost,
                    eGameBoard& board,
                    const eBuildingType type,
                    const int elevation,
                    const int dim,
                    const eCityId cid);

    eTextureSpace
    getTextureSpace(const int tx, const int ty,
                    const eTileSize size) const override;

    void progressed() override;

    int currentElevation() const { return mCurrentElevation; }
    int elevation() const { return mElevation; }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    const int mElevation = 0;
    const int mDim = 1;
    int mCurrentElevation = 0;
};

#endif // EPYRAMIDELEMENT_H
