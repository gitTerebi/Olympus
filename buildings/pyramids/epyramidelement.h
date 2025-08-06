#ifndef EPYRAMIDELEMENT_H
#define EPYRAMIDELEMENT_H

#include "../sanctuaries/esanctbuilding.h"

class ePyramid;

class ePyramidElement : public eSanctBuilding {
public:
    ePyramidElement(const std::vector<eSanctCost>& cost,
                    eGameBoard& board,
                    const eBuildingType type,
                    const int sw, const int sh,
                    const int elevation,
                    const eCityId cid);
    ePyramidElement(ePyramid* const pyramid,
                    const std::vector<eSanctCost>& cost,
                    eGameBoard& board,
                    const eBuildingType type,
                    const int sw, const int sh,
                    const int elevation,
                    const eCityId cid);

    eTextureSpace
    getTextureSpace(const int tx, const int ty,
                    const eTileSize size) const override;

    void progressed() override;

    int currentElevation() const { return mCurrentElevation; }
    int elevation() const { return mElevation; }

    SDL_Rect nextElement() const;
    SDL_Rect prevElement() const;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    const int mElevation = 0;
    int mCurrentElevation = 0;
};

#endif // EPYRAMIDELEMENT_H
