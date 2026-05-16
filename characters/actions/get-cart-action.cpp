#include "get-cart-action.h"

#include "fileIO/esavearchive.h"

eGetCartAction::eGetCartAction(eCharacter* const c,
                               eBuildingWithResource* const b)
    : eCartTransporterAction(c, b, eCharActionType::getCartAction) {}

// ── decide: pure state transition ────────────────────────────────────────────

bool eGetCartAction::decide() {
    if(!building()) return true;
    switch(mGetState) {
    case eGetState::idle:        toFindTarget(); break;
    case eGetState::findTarget:  toFindTarget(); break; // retry after sleep
    case eGetState::moving:      toAtOrReturn(); break;
    case eGetState::atTarget:    toAtOrReturn(); break;
    case eGetState::returning:   toIdle();       break;
    }
    return true;
}

// ── transition helpers ────────────────────────────────────────────────────────

void eGetCartAction::toIdle() {
    enterIdle();
}

void eGetCartAction::toFindTarget() {
    mGetState = eGetState::findTarget;
    findTarget(); // BFS all GET tasks
}

void eGetCartAction::toAtOrReturn() {
    const auto c = cart();
    const int count = c->resCount();
    const auto res = c->resType();
    const int max = res == eResourceType::sculpture ? 1 : 4;
    const bool canTakeMore = mTask.fMaxCount > 0 &&
                             mTask.fResource == res &&
                             mTask.fType == eCartActionType::get &&
                             (max - count) > 0;
    if(canTakeMore) {
        mGetState = eGetState::findTarget;
        findTarget(mTask);
    } else {
        enterReturning();
    }
}

// ── enter: actions (side effects) ────────────────────────────────────────────

void eGetCartAction::enterIdle() {
    mGetState = eGetState::idle;
    clearTask();
}

void eGetCartAction::enterReturning() {
    mGetState = eGetState::returning;
    goBack();
}

// ── findTarget fail → sleep ───────────────────────────────────────────────────

void eGetCartAction::onFoundTarget() {
    mGetState = eGetState::moving;
}

void eGetCartAction::onFindTargetFail() {
    wait(kFindRetryWait); // sleep → decide() → toFindTarget() retry forever
}

// ── serialize ────────────────────────────────────────────────────────────────

void eGetCartAction::serializeFields(eSaveArchive& ar) {
    eCartTransporterAction::serializeFields(ar);
    ar.field("getState", mGetState);
}

void eGetCartAction::resumeFromSavedState() {
    setCurrentAction(nullptr);
    switch(mGetState) {
    case eGetState::idle:
        break;
    case eGetState::findTarget:
    case eGetState::moving:
        toFindTarget();
        break;
    case eGetState::atTarget:
        toAtOrReturn();
        break;
    case eGetState::returning:
        enterReturning();
        break;
    }
}

bool eGetCartAction::savesCartState() const {
    return false;
}
