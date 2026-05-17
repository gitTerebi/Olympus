#ifndef EBUILDINGWITHRESOURCE_H
#define EBUILDINGWITHRESOURCE_H

#include "ebuilding.h"

#include "engine/eresourcetype.h"
#include "characters/ecarttransporter.h"

class eSaveArchive;

enum class eCartActionType {
    get, deliver
};

struct eCartTask {
    eCartActionType fType;
    eResourceType fResource;
    int fMaxCount = 0;
    bool fStoragePush = false;
};

struct eStash {
    eResourceType fType;
    int fCount;
};

class eBuildingWithResource : public eBuilding {
public:
    using eBuilding::eBuilding;

    virtual int add(const eResourceType type, const int count) {
        (void)type;
        (void)count;
        return 0;
    }
    virtual int take(const eResourceType type, const int count) {
        (void)type;
        (void)count;
        return 0;
    }

    virtual int count(const eResourceType type) const {
        (void)type;
        return 0;
    }

    virtual int spaceLeft(const eResourceType type) const {
        (void)type;
        return 0;
    }

    virtual std::vector<eCartTask> cartTasks() const {
        return {};
    }

    // cart capacity per resource for carts owned by this building.
    // default uses producer/deliver-cart rule (transportSize).
    // overridden by storage buildings for their 4-per-slot rule.
    virtual int cartCapacity(const eResourceType res, bool doubled) const {
        return eResourceTypeHelpers::transportSize(res, doubled);
    }

    int addProduced(const eResourceType type, const int count);

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;

    const std::vector<eStash>& stash() const { return mStash; }
    int stash(const eResourceType type, const int count);
    int stashCount(const eResourceType type) const;
protected:
    void setStashable(const eResourceType s);
    void addFromStash();
    int takeFromStash(const eResourceType t, const int count);

    stdptr<eCartTransporter> spawnCart(const eCartActionTypeSupport s =
                                    eCartActionTypeSupport::both);
    void serialize(eSaveArchive& ar);
private:
    eResourceType mStashable = eResourceType::none;
    std::vector<eStash> mStash;
};

#endif // EBUILDINGWITHRESOURCE_H
