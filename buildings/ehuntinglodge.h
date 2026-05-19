#ifndef EHUNTINGLODGE_H
#define EHUNTINGLODGE_H

#include "eresourcecollectbuildingbase.h"

class eHunter;
class eSaveArchive;

class eHuntingLodge : public eResourceCollectBuildingBase {
public:
    eHuntingLodge(eGameBoard& board, const eCityId cid);
    ~eHuntingLodge();

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    void addRaw() override {}
    void hunterDelivered(const eResourceType type, const int count);
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool spawn();

    stdptr<eHunter> mHunter;
    double mSpawnTime = 1000000;
};

#endif // EHUNTINGLODGE_H
