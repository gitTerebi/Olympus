#ifndef ETRIREMEWHARF_H
#define ETRIREMEWHARF_H

#include "eresourcebuildingbase.h"

class eTrireme;

class eTriremeWharf : public eEmployingBuilding {
public:
    eTriremeWharf(eGameBoard& board,
                  const eDiagonalOrientation o,
                  const eCityId cid);
    ~eTriremeWharf();

    std::shared_ptr<eTexture> getTexture(const eTileSize size) const override;
    std::vector<eOverlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    int add(const eResourceType type, const int count) override;

    int count(const eResourceType type) const override;
    int spaceLeft(const eResourceType type) const override;

    std::vector<eCartTask> cartTasks() const override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    eDiagonalOrientation orientation() const { return mO; }

    bool hasTrireme() const;
private:
    void spawnTrireme();

    const eDiagonalOrientation mO;

    stdptr<eCartTransporter> mTakeCart;
    stdptr<eTrireme> mTrireme;

    int mTriremeBuildingStage = 0;
    int mTriremeBuildingTime = 0;

    const int mMaxWood = 8;
    int mWoodCount{0};
    const int mMaxArmor = 4;
    int mArmorCount{0};
};

#endif // ETRIREMEWHARF_H
