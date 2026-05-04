#ifndef EFARMBASE_H
#define EFARMBASE_H

#include "eresourcebuildingbase.h"
#include "engine/edate.h"

#include "textures/ebuildingtextures.h"

class eFarmBase : public eResourceBuildingBase {
public:
    eFarmBase(eGameBoard& board,
              const eBuildingType type,
              const int sw, const int sh,
              const eResourceType resType,
              const eCityId cid);

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    int productionPercent() const;
    eMonth nextHarvestMonth() const;
    int producedThisYear() const { return mProducedThisYear; }

    void nextMonth() override;

    void prepareForCollapse() override {
        mNextRipe = 0;
        mCurrentTile = 0;
        mCurrentStage = 0;
    }

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    const std::vector<eBuildingTextures>& mTextures;
    double mNextRipe = 0;
    int mCurrentTile = 0;  // 0 - 4
    int mCurrentStage = 0; // 0 - 4

    int mProducedThisYear = 0;
    std::array<int,12> mMonthlyProduced{};
    int mRingIdx = 0;
};

#endif // EFARMBASE_H
