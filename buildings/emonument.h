#ifndef EMONUMENT_H
#define EMONUMENT_H

#include "eemployingbuilding.h"

#include "sanctuaries/esanctbuilding.h"

class eSaveArchive;

class eMonument : public eEmployingBuilding {
public:
    eMonument(eGameBoard& board,
              const eBuildingType type,
              const int sw, const int sh,
              const int maxEmployees,
              const eCityId cid);
    ~eMonument();

    void erase() override;

    std::shared_ptr<eTexture>
    getTexture(const eTileSize) const override
    { return nullptr; }

    bool spawnsCartWalkers() const override { return true; }

    void timeChanged(const int by) override;

    int spaceLeft(const eResourceType type) const override;
    int add(const eResourceType type, const int count) override;

    std::vector<eCartTask> cartTasks() const override;

    virtual void buildingProgressed() {}

    void setRotated(const bool r) { mRotated = r; }
    bool rotated() const { return mRotated; }

    void registerElement(const stdsptr<eSanctBuilding>& e);

    int progress() const; // 0-100
    bool finished() const;

    eSanctCost cost() const;
    const eSanctCost& stored() const { return mStored; }
    const eSanctCost& used() const { return mUsed; }

    void useResources(const eSanctCost& r);

    int altitude() const { return mAltitude; }
    void setAltitude(const int a) { mAltitude = a; }

    void setConstructionHalted(const bool h);
    bool constructionHalted() const { return mHaltConstruction; }
protected:
    void serializeFields(eSaveArchive& ar) override;

    stdptr<eCartTransporter> mCart;

    bool mHaltConstruction = false;
    eSanctCost mStored{0, 0, 0};
    eSanctCost mUsed{0, 0, 0};
    int mAltitude = 0;
    bool mRotated = false;

    std::vector<stdsptr<eSanctBuilding>> mElements;
};

#endif // EMONUMENT_H
