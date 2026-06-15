#ifndef EPROCESSINGBUILDING_H
#define EPROCESSINGBUILDING_H

#include "eresourcebuildingbase.h"
#include "textures/building-textures.h"

class SaveArchive;

class eProcessingBuilding : public eResourceBuildingBase {
public:
    using eBaseTex = std::shared_ptr<Texture> BuildingTextures::*;
    using eOverlays = TextureCollection BuildingTextures::*;
    eProcessingBuilding(GameBoard& board,
                        const eBaseTex baseTex,
                        const double overlayX,
                        const double overlayY,
                        const eOverlays overlays,
                        const eBuildingType type,
                        const int sw, const int sh,
                        const int maxEmployees,
                        const eResourceType rawMaterial,
                        const eResourceType product,
                        const int rawUse,
                        const int time,
                        const eCityId cid);
    ~eProcessingBuilding();

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;
    void nextMonth() override;

    int add(const eResourceType type, const int count) override;

    int count(const eResourceType type) const override;
    int spaceLeft(const eResourceType type) const override;

    std::vector<eCartTask> cartTasks() const override;

    int rawCount() const { return mRawCount; }
    eResourceType rawMaterial() const { return mRawMaterial; }
    int processWaitTime() const { return mProcessWaitTime; }
    int producedThisYear() const { return mProducedThisYear; }

    int productionPercent() const;
protected:
    void serializeFields(SaveArchive& ar) override;
private:

    const std::vector<BuildingTextures>& mTextures;

    const eBaseTex mBaseTex;
    const eOverlays mOverlays;
    const double mOverlayX;
    const double mOverlayY;

    const eResourceType mRawMaterial;

    const int mMaxRaw = 4;
    const int mRawUse;
    int mRawCount{0};

    const int mProcessWaitTime;
    double mProcessTime = 0;

    int mProducedThisYear = 0;
    int mLastMonth = -1;
    std::array<int,12> mMonthlyProduced{};
    int mRingIdx = 0;
};

#endif // EPROCESSINGBUILDING_H
