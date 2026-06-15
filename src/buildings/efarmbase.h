#ifndef EFARMBASE_H
#define EFARMBASE_H

#include "eresourcebuildingbase.h"
#include "engine/edate.h"

#include "textures/building-textures.h"

class SaveArchive;

class eFarmBase : public eResourceBuildingBase {
public:
    eFarmBase(GameBoard& board,
              const eBuildingType type,
              const int sw, const int sh,
              const eResourceType resType,
              const eCityId cid);

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    int productionPercent() const;
    eMonth nextHarvestMonth() const;
    int producedThisYear() const { return mProducedThisYear; }

    void nextMonth() override;

    void prepareForCollapse() override {
        mNextRipe = 0;
        mGrownFields = 0;
        mFieldStage = 0;
    }

protected:
    void serializeFields(SaveArchive& ar) override;
private:

    const std::vector<BuildingTextures>& mTextures;
    double mNextRipe = 0;
    int mGrownFields = 0;  // 0 - 5
    int mFieldStage = 0;   // 0 - 4

    int mProducedThisYear = 0;
    std::array<int,12> mMonthlyProduced{};
    int mRingIdx = 0;
};

#endif // EFARMBASE_H
