#ifndef GET_CART_ACTION_H
#define GET_CART_ACTION_H

#include "ecarttransporteraction.h"

enum class eGetState {
    idle,
    findTarget,
    moving,
    atTarget,
    returning
};

class eGetCartAction : public eCartTransporterAction {
public:
    eGetCartAction(eCharacter* const c, eBuildingWithResource* const b);

    bool decide() override;

    void serializeFields(eSaveArchive& ar) override;
    void resumeFromSavedState() override;
    bool savesCartState() const override;

    void onFindTargetFail() override;
    void onFoundTarget() override;
    void onAtTarget() override;

private:
    // transitions
    void toFindTarget();
    void toAtOrReturn();
    void toIdle();

    // actions
    void enterIdle();
    void enterReturning();

    eGetState mGetState = eGetState::idle;
    static const int kFindRetryWait = 1000;
};

#endif // GET_CART_ACTION_H
