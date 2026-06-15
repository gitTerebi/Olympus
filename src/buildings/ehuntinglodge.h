#ifndef EHUNTINGLODGE_H
#define EHUNTINGLODGE_H

#include "eresourcecollectbuildingbase.h"

class eHunter;
class SaveArchive;

class eHuntingLodge : public eResourceCollectBuildingBase {
public:
    eHuntingLodge(GameBoard& board, const eCityId cid);
    ~eHuntingLodge();

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    void addRaw() override {}
    void hunterDelivered(const eResourceType type, const int count);
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    bool spawn();

    stdptr<eHunter> mHunter;
    double mSpawnTime = 1000000;
};

#endif // EHUNTINGLODGE_H
