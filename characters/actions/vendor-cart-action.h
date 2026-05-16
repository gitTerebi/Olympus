#ifndef VENDOR_CART_ACTION_H
#define VENDOR_CART_ACTION_H

#include "ecarttransporteraction.h"

enum class eVendorCartState {
    waitAtHome,
    findTarget,
    moving,
    atTarget,
    returning
};

class eVendorCartAction : public eCartTransporterAction {
public:
    eVendorCartAction(eCharacter* const c, eBuildingWithResource* const b);

    bool decide() override;

    void serializeFields(eSaveArchive& ar) override;

    void onFindTargetFail() override;
    void onFoundTarget() override;

private:
    void toFindTarget();
    void toDeposit();

    void enterWaitAtHome();
    void enterReturning();

    eVendorCartState mState = eVendorCartState::waitAtHome;
    static const int kFindRetryWait = 1000;
};

#endif // VENDOR_CART_ACTION_H
