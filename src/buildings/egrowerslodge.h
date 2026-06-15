#ifndef EGROWERSLODGE_H
#define EGROWERSLODGE_H

#include "eemployingbuilding.h"

#include "textures/building-textures.h"
#include "characters/egrower.h"

class eCartTransporter;
class SaveArchive;

class eGrowersLodge : public eEmployingBuilding {
public:
    eGrowersLodge(GameBoard& board, const eGrowerType type,
                  const eCityId cid);
    ~eGrowersLodge();
    void erase() override;

    std::shared_ptr<Texture> getTexture(const eTileSize size) const override;
    std::vector<Overlay> getOverlays(const eTileSize size) const override;

    bool spawnsCartWalkers() const override { return true; }

    void timeChanged(const int by) override;

    int add(const eResourceType type, const int count) override;
    int take(const eResourceType type, const int count) override;

    int count(const eResourceType type) const override;
    int spaceLeft(const eResourceType type) const override;

    std::vector<eCartTask> cartTasks() const override;

    void nextMonth() override;

    int producedThisYear() const { return mProducedThisYear; }
    void growerDelivered(const eResourceType type, const int count);

    bool spawnGrower(stdptr<eGrower>& grower,
                     const bool oliveHarvester = false,
                     const bool genericHarvester = false);

    void setNoTarget(const bool t);
    bool noTarget() const { return mNoTarget; }

    bool hasReadyOlives() const;

    eGrowerType type() const { return mType; }
    int grapeUnits() const { return mGrapes; }
    int oliveUnits() const { return mOlives; }
    int orangeUnits() const { return mOranges; }

    static constexpr int sUnitsPerLoad = 100;
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    void killWalkers();
    int readyOliveCount() const;
    int readyGrapeCount() const;
    int readyOrangeCount() const;
    bool isGrapeHarvestMonth() const;
    bool isOrangeHarvestMonth() const;

    static constexpr int sMaxUnits = 500;

    const eGrowerType mType;

    bool mNoTarget = false;

    bool mSpawnEnabled = true;

    int mGrapes{0};
    int mOlives{0};
    int mOranges{0};

    stdptr<eCartTransporter> mCart;

    double mSpawnTime = 1000000;
    double mGrowerSpawnTime = 1000000;

    stdptr<eGrower> mGrower;
    std::array<stdptr<eGrower>, 4> mOliveHarvesters;
    std::array<double, 4> mOliveHarvesterSpawnTimes{
        1000000, 1000000, 1000000, 1000000};

    int mProducedThisYear = 0;
    std::array<int,12> mMonthlyProduced{};
    int mRingIdx = 0;
};

#endif // EGROWERSLODGE_H
