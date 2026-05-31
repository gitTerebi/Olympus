#ifndef DELIVER_CART_ACTION_H
#define DELIVER_CART_ACTION_H

#include "ecarttransporteraction.h"

enum class eDeliverState {
    idle,
    loading,
    waitOutside,
    findTarget,
    moving,
    atTarget,
    idleOutside,
    returning
};

class eDeliverCartAction : public eCartTransporterAction {
public:
    eDeliverCartAction(eCharacter* const c, eBuildingWithResource* const b);

    bool decide() override;

    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    bool savesCartState() const override;

    void onFindTargetFail() override;
    void onFoundTarget() override;
    void onAtTarget() override;

private:
    // transitions
    void toLoading();
    void toWaitOrIdle();
    void toFindTarget();
    void toAtOrReturn();
    void toLoadOrIdle();

    // actions
    void enterIdle();
    void enterLoading();
    void enterWaitOutside();
    void enterIdleOutside();
    void enterReturning();

    eDeliverState mDeliverState = eDeliverState::idle;
    int mFindRetry = 0;
    static const int kIdleWait = 1000;
    static const int kMaxFindRetries = 250;
    static const int kFindRetryWait = 1000;
    static const int kIdleOutsideWait = 1000;
};

#endif // DELIVER_CART_ACTION_H
