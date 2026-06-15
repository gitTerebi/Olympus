#ifndef EURCHINQUAY_H
#define EURCHINQUAY_H

#include "eresourcecollectbuildingbase.h"

class eUrchinGatherer;
class SaveArchive;

enum class eUrchinQuayState {
    waiting,
    unpacking
};

class eUrchinQuay : public eResourceCollectBuildingBase {
public:
    eUrchinQuay(GameBoard& board,
                const eDiagonalOrientation o,
                const eCityId cid);
    ~eUrchinQuay();

    void timeChanged(const int by) override;

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void addRaw() override;

    int take(const eResourceType type, const int count) override;

    eDiagonalOrientation orientation() const { return mO; }

    eUrchinQuayState state() const { return mState; }
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    void spawnGatherer();
    void updateDisabled();

    const eDiagonalOrientation mO;

    bool mDisabled = false;
    int mStateCount = 0;
    eUrchinQuayState mState = eUrchinQuayState::waiting;
    stdptr<eUrchinGatherer> mGatherer;
};

#endif // EURCHINQUAY_H
