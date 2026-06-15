#ifndef STORAGE_DELIVERY_CART_H
#define STORAGE_DELIVERY_CART_H

#include "cart-transporter-action.h"

class eStorageDeliveryCartAction : public CartTransporterAction {
public:
    eStorageDeliveryCartAction(eCharacter* const c,
                               eBuildingWithResource* const b);

    bool decide() override;
    int cartCapacity(const eResourceType res) const override;

private:
    void serializeFields(SaveArchive& ar) override;
    bool acceptsTargetForTask(const eCartTask& task,
                              const eThreadBuilding& target) const override;
    bool hasDeliveryTarget(const eCartTask& task) const;
    void startResourceAction(const eCartTask& task) override;

    void enterStorageIdle();
    void enterStorageLoadingDeliver();
    void enterStorageLoadingGet();
    void enterStorageWaitOutside();
    void enterStorageMovingToTarget(const eCartTask& task);
    void enterStorageReturning();
    void dumpStockAtHome();

    bool mWaitAfterNoDeliveryTarget = false;
    static const int kNoDeliveryTargetWait = 1000;
};

#endif // STORAGE_DELIVERY_CART_H
