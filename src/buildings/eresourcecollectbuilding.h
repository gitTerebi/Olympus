#ifndef ERESOURCECOLLECTBUILDING_H
#define ERESOURCECOLLECTBUILDING_H

#include "eresourcecollectbuildingbase.h"

#include "textures/building-textures.h"
#include "characters/actions/ecollectresourceaction.h"

class eResourceCollectorBase;
class SaveArchive;

class eResourceCollectBuilding : public eResourceCollectBuildingBase {
public:
    using eBaseTex = std::shared_ptr<Texture> BuildingTextures::*;
    using eOverlays = TextureCollection BuildingTextures::*;
    using eCharGenerator =  std::function<stdsptr<eResourceCollectorBase>()>;
    eResourceCollectBuilding(GameBoard& board,
                             const eBaseTex baseTex,
                             const double overlayX,
                             const double overlayY,
                             const eOverlays overlays,
                             const int waitingOO,
                             const double waitingOX,
                             const double waitingOY,
                             const eCharGenerator& charGen,
                             const eBuildingType type,
                             const stdsptr<eHasResourceObject>& hr,
                             const int sw, const int sh,
                             const int maxEmployees,
                             const eResourceType resType,
                             const eCityId cid);
    ~eResourceCollectBuilding();

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    void addRaw() override;

    bool spawn();

    void setAddResource(const bool b) { mAddResource = b; }

protected:
    void serializeFields(SaveArchive& ar) override;
    void enableSpawn() { mSpawnEnabled = true; }
    void disableSpawn() { mSpawnEnabled = false; }

    void setCollectedAction(const eTileActionType a);

    int rawCount() const { return mRawCount; }
    void setRawInc(const int i) { mRawInc = i; }

    void setRawCountCollect(const int r) { mRawCountCollect = r; }
private:
    const eCharGenerator mCharGenerator;
    const std::vector<BuildingTextures>& mTextures;

    const eBaseTex mBaseTex;
    const eOverlays mOverlays;

    const double mOverlayX;
    const double mOverlayY;

    const int mWaitingOO = 0;
    const double mWaitingOX = 0;
    const double mWaitingOY = 0;

    const stdsptr<eHasResourceObject> mHasRes;

    eTileActionType mCollectedAction = eTileActionType::none;

    stdptr<eResourceCollectorBase> mCollector;

    bool mSpawnEnabled = true;

    bool mAddResource = true;

    int mRawCount = 0;
    int mRawCountCollect = 0;
    int mRawInc = 1;
    int mProcessDuration = 10000;
    double mProcessTime = 0;

    int mWaitTime = 5000;
    double mSpawnTime = mWaitTime;
};

#endif // ERESOURCECOLLECTBUILDING_H
