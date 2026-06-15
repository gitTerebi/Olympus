#ifndef ECHARIOTFACTORY_H
#define ECHARIOTFACTORY_H

#include "eemployingbuilding.h"

#include <algorithm>

class SaveArchive;

class eChariotFactory : public eEmployingBuilding {
public:
    eChariotFactory(GameBoard& board, const eCityId cid);
    ~eChariotFactory();

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    void timeChanged(const int by) override;

    int add(const eResourceType type, const int count) override;
    int take(const eResourceType type, const int count) override;

    int count(const eResourceType type) const override;
    int spaceLeft(const eResourceType type) const override;

    std::vector<eCartTask> cartTasks() const override;

    int wood() const { return mWood; }
    int horses() const { return mHorses; }
    int chariotCount() const { return std::min(mHorses, mChariots); }
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    int mWood = 0;
    int mChariotBuildingTime = 0;
    int mHorses = 0;
    int mChariots = 0;

    stdptr<eCartTransporter> mWoodCart;
    stdptr<eCartTransporter> mHorseCart;
};

#endif // ECHARIOTFACTORY_H
