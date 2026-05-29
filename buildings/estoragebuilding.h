#ifndef ESTORAGEBUILDING_H
#define ESTORAGEBUILDING_H

#include "eemployingbuilding.h"

class eSaveArchive;
class eCartTransporter;
class eBuildingWithResource;

class eStorageBuilding : public eEmployingBuilding {
public:
    eStorageBuilding(GameBoard& board,
                     const eBuildingType type,
                     const int sw, const int sh,
                     const int maxEmployees,
                     const eResourceType canAccept,
                     const eCityId cid,
                     const int spaceCount = 8);
    ~eStorageBuilding();

    bool spawnsCartWalkers() const override { return true; }

    void timeChanged(const int by) override;

    int addNotAccept(const eResourceType type, const int count);

    int add(const eResourceType type, const int count) override;
    int take(const eResourceType type, const int count) override;

    int count(const eResourceType type) const override;
    int spaceLeft(const eResourceType type) const override;

    int spaceLeftDontAccept(const eResourceType type) const;

    std::vector<eCartTask> cartTasks() const override;
    bool deliveryTargetExists(const eResourceType res,
                              const bool allowStorageTargets) const;
    static bool acceptsInputDelivery(eBuildingWithResource* target,
                                     eResourceType res);

    static int sCount(const eResourceType type,
                      const int resourceCount[15],
                      const eResourceType resourceType[15],
                      const int spaceCount);
    static int sSpaceLeftDontAccept(const eResourceType type,
                                    const int resourceCount[15],
                                    const eResourceType resourceType[15],
                                    const std::map<eResourceType, int>& maxCounts,
                                    const int spaceCount);
    static int sSpaceLeft(const eResourceType type,
                          const int resourceCount[15],
                          const eResourceType resourceType[15],
                          const eResourceType accepts,
                          const std::map<eResourceType, int>& maxCounts,
                          const int spaceCount);

    int resourceCount(const int id) const { return mResourceCount[id]; }
    eResourceType resourceType(const int id) const { return mResource[id]; }

    const int* resourceCounts() const { return &mResourceCount[0]; }
    const eResourceType* resourceTypes() const { return &mResource[0]; }
    const std::map<eResourceType, int>& maxCount() const { return mMaxCount; };
    void setMaxCount(const std::map<eResourceType, int>& m);

    eResourceType accepts() const { return mAccept; }
    eResourceType get() const { return mGet; }
    eResourceType empties() const { return mEmpty; }
    eResourceType canAccept() const { return mCanAccept; }

    bool accepts(const eResourceType res) const
    { return static_cast<bool>(mAccept & res); }
    bool get(const eResourceType res) const
    { return static_cast<bool>(mGet & res); }
    bool empties(const eResourceType res) const
    { return static_cast<bool>(mEmpty & res); }
    bool canAccept(const eResourceType res) const
    { return static_cast<bool>(mCanAccept & res); }

    void setOrders(const eResourceType get,
                   const eResourceType empty,
                   const eResourceType accept);

    void getOrders(eResourceType& get,
                   eResourceType& empty,
                   eResourceType& accept) const;

    int spaceCount() const { return mSpaceCount; }

    void clearStorage() {
        for(int i = 0; i < 15; i++) {
            mResourceCount[i] = 0;
            mResource[i] = eResourceType::none;
        }
    }

    void prepareForCollapse() override { clearStorage(); }

    static const int sMaxCarts = 3;
    eCartTransporter* cart(const int i) const { return mCarts[i].get(); }
    eCartTransporter* cart1() const { return mCarts[0].get(); }
    eCartTransporter* cart2() const { return mCarts[1].get(); }

    // sum of res of type `res` already en-route to `target` from any
    // yard in `cityId` (live; no save state).
    static int incomingReservedFor(const eBuilding* target,
                                   eResourceType res,
                                   const GameBoard& board,
                                   eCityId cid);

protected:
    void serializeFields(eSaveArchive& ar) override;
    // which held resources this building is allowed to push out to consumers.
    // trade post overrides this to push imported goods only (not exports).
    virtual bool pushAllows(const eResourceType) const { return true; }
    // cart budget: storehouse/granary = 2 get + 1 deliver; trade post = 1 + 1.
    virtual int maxGetCarts() const { return 2; }
    virtual int maxDeliverCarts() const { return 1; }
private:
    stdptr<eCartTransporter> spawnStorageDeliveryCart();
    std::vector<eCartTask> orderCartTasks() const;
    std::vector<eCartTask> pushCartTasks() const;
    bool getTargetExists(const eResourceType res) const;

    const eResourceType mCanAccept;

    const int mSpaceCount;

    eResourceType mGet = eResourceType::none;
    eResourceType mEmpty = eResourceType::none;
    eResourceType mAccept = mCanAccept; // includes get
    int mNextGetCartTask = 0;

    int mResourceCount[15] = {0};
    eResourceType mResource[15] = {eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none,
                                   eResourceType::none};

    std::map<eResourceType, int> mMaxCount;

    stdptr<eCartTransporter> mCarts[sMaxCarts];
};

#endif // ESTORAGEBUILDING_H
