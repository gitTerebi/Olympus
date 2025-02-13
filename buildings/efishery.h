#ifndef EFISHERY_H
#define EFISHERY_H

#include "eresourcecollectbuildingbase.h"

class eFishingBoat;

enum class eFisheryState {
    none,
    buildingBoat,
    waiting,
    unpacking
};

class eFishery : public eResourceCollectBuildingBase {
public:
    eFishery(eGameBoard& board,
             const eDiagonalOrientation o,
             const eCityId cid);
    ~eFishery();

    void timeChanged(const int by) override;

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void addRaw() override;

    int take(const eResourceType type, const int count) override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    eDiagonalOrientation orientation() const { return mO; }

    eFisheryState state() const { return mState; }
private:
    void spawnBoat();
    void updateDisabled();

    const eDiagonalOrientation mO;

    bool mDisabled = false;
    int mStateCount = 0;
    eFisheryState mState = eFisheryState::none;
    stdptr<eFishingBoat> mBoat;
};

#endif // EFISHERY_H
