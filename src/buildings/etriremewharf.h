#ifndef ETRIREMEWHARF_H
#define ETRIREMEWHARF_H

#include "eresourcebuildingbase.h"

class eTrireme;
class SaveArchive;

class eTriremeWharf : public eEmployingBuilding {
public:
    eTriremeWharf(GameBoard& board,
                  const eDiagonalOrientation o,
                  const eCityId cid);
    ~eTriremeWharf();

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    int add(const eResourceType type, const int count) override;

    int count(const eResourceType type) const override;
    int spaceLeft(const eResourceType type) const override;

    std::vector<eCartTask> cartTasks() const override;

    eDiagonalOrientation orientation() const { return mO; }

    void triremeCameBack();
    void triremeLeaving();
    bool abroad() const { return mAbroad; }
    eTrireme* trireme() const;
    bool hasTrireme() const;
    bool isAtWharf() const;
    bool isBuildingTrireme() const;
    int triremeBuildProgress() const;
    void dockTrireme(eTrireme* const trireme) const;

    eTile* triremeTile() const;
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    void spawnTrireme();

    const eDiagonalOrientation mO;

    bool mAbroad = false;

    stdptr<eCartTransporter> mTakeCart;
    stdptr<eTrireme> mTrireme;

    int mTriremeBuildingStage = 0;
    double mTriremeBuildingTime = 0;

    const int mMaxWood = 8;
    int mWoodCount{0};
    const int mMaxArmor = 4;
    int mArmorCount{0};
};

#endif // ETRIREMEWHARF_H
