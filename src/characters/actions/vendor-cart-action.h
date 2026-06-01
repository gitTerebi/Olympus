#ifndef VENDOR_CART_ACTION_H
#define VENDOR_CART_ACTION_H

#include "cart-transporter-action.h"

enum class eVendorCartState {
    waitAtHome,
    findTarget,
    moving,
    atTarget,
    returning
};

class eVendorCartAction : public CartTransporterAction {
public:
    eVendorCartAction(eCharacter* const c, eBuildingWithResource* const b);

    bool decide() override;

    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    bool savesCartState() const override;

    void onFindTargetFail() override;
    void onFoundTarget() override;
    void onAtTarget() override;

private:
    void toFindTarget();
    void toDeposit();

    void enterWaitAtHome();
    void enterReturning();

    eVendorCartState mState = eVendorCartState::waitAtHome;
    static const int kFindRetryWait = 1000;
};

#endif // VENDOR_CART_ACTION_H
