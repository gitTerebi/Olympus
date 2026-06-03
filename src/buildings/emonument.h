#ifndef EMONUMENT_H
#define EMONUMENT_H

#include "eemployingbuilding.h"

#include "sanctuaries/esanctbuilding.h"

class eSaveArchive;

class eMonument : public eEmployingBuilding {
public:
    eMonument(GameBoard& board,
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

    void setRotated(const bool r) { mRotateId = r ? 1 : 0; }
    void setRotateId(const int r) { mRotateId = r; }
    bool rotated() const { return mRotateId == 1 || mRotateId == 3; }
    int rotateId() const { return mRotateId; }

    void setWomanTile(eTile* t) { mWomanTile = t; }
    eTile* womanTile() const { return mWomanTile; }

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
    eTile* mWomanTile = nullptr;

    bool mHaltConstruction = false;
    eSanctCost mStored{0, 0, 0};
    eSanctCost mUsed{0, 0, 0};
    int mAltitude = 0;
    int mRotateId = 0;

    std::vector<stdsptr<eSanctBuilding>> mElements;
};

#endif // EMONUMENT_H
