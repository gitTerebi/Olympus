#include "deliver-cart-action.h"

#include "fileIO/esavearchive.h"

eDeliverCartAction::eDeliverCartAction(eCharacter* const c,
                                       eBuildingWithResource* const b)
    : eCartTransporterAction(c, b, eCharActionType::deliverCartAction) {}

// ── decide: pure state transition ────────────────────────────────────────────

bool eDeliverCartAction::decide() {
    if(!building()) return true;
    switch(mDeliverState) {
    case eDeliverState::idle:        toLoading();     break;
    case eDeliverState::loading:     toWaitOrIdle();  break;
    case eDeliverState::waitOutside: toFindTarget();  break;
    case eDeliverState::findTarget:  toFindTarget();  break; // retry after sleep
    case eDeliverState::moving:      toAtOrReturn();  break;
    case eDeliverState::atTarget:    toAtOrReturn();  break;
    case eDeliverState::idleOutside: toFindTarget();  break; // retry after sleep
    case eDeliverState::returning:   toLoadOrIdle();  break;
    }
    return true;
}

// ── transition helpers ────────────────────────────────────────────────────────

void eDeliverCartAction::toLoading() {
    enterLoading();
}

void eDeliverCartAction::toWaitOrIdle() {
    const auto c = cart();
    if(c->hasResource()) {
        enterWaitOutside();
    } else {
        enterIdle();
    }
}

void eDeliverCartAction::toFindTarget() {
    const auto c = cart();
    if(c->hasResource()) {
        eCartTask task;
        task.fMaxCount = c->resCount();
        task.fResource = c->resType();
        task.fType = eCartActionType::deliver;
        mDeliverState = eDeliverState::findTarget;
        findTarget(task);
    } else {
        enterReturning();
    }
}

void eDeliverCartAction::toAtOrReturn() {
    const auto c = cart();
    const int count = c->resCount();
    const auto res = c->resType();
    if(count > 0 && mTask.fResource == res) {
        // partial — still has stock, find next target
        enterIdleOutside();
    } else {
        enterReturning();
    }
}

void eDeliverCartAction::toLoadOrIdle() {
    const auto c = cart();
    if(c->hasResource()) {
        enterLoading();
    } else {
        enterIdle();
    }
}

// ── enter: actions (side effects) ────────────────────────────────────────────

void eDeliverCartAction::enterIdle() {
    mDeliverState = eDeliverState::idle;
    mFindRetry = 0;
    clearTask();
}

void eDeliverCartAction::enterLoading() {
    mDeliverState = eDeliverState::loading;
    const auto c = cart();
    // top-up: take as much deliver stock as possible
    const auto tasks = building()->cartTasks();
    for(const auto& task : tasks) {
        if(task.fType != eCartActionType::deliver) continue;
        startResourceAction(task);
        if(c->resCount() > 0) return; // loaded — decide() will call toWaitOrIdle
    }
    // nothing taken — decide() will call toWaitOrIdle → enterIdle
}

void eDeliverCartAction::enterWaitOutside() {
    mDeliverState = eDeliverState::waitOutside;
    waitOutside(); // walk to road/adjacent tile; on arrive → spread() → decide()
}

void eDeliverCartAction::enterIdleOutside() {
    mDeliverState = eDeliverState::idleOutside;
    wait(kIdleOutsideWait); // sleep → decide() fires → toFindTarget()
}

void eDeliverCartAction::enterReturning() {
    mDeliverState = eDeliverState::returning;
    mFindRetry = 0;
    goBack();
}

// ── findTarget fail callback ──────────────────────────────────────────────────
// called by base findTarget internals via throttleDropoffRetry / enterIdle paths
// Override: use our own retry logic instead

void eDeliverCartAction::onFindTargetFail() {
    mFindRetry++;
    if(mFindRetry >= kMaxFindRetries) {
        enterReturning();
    } else {
        wait(kFindRetryWait);
    }
}

void eDeliverCartAction::onFoundTarget() {
    mDeliverState = eDeliverState::moving;
}

void eDeliverCartAction::onAtTarget() {
    mDeliverState = eDeliverState::atTarget;
}

// ── serialize ────────────────────────────────────────────────────────────────

void eDeliverCartAction::serializeFields(eSaveArchive& ar) {
    eCartTransporterAction::serializeFields(ar);
    ar.field("deliverState", mDeliverState);
    ar.field("findRetry", mFindRetry);
}

void eDeliverCartAction::resumeFromSavedState() {
    setCurrentAction(nullptr);
    switch(mDeliverState) {
    case eDeliverState::idle:
        break;
    case eDeliverState::loading:
        toWaitOrIdle();
        break;
    case eDeliverState::waitOutside:
        enterWaitOutside();
        break;
    case eDeliverState::findTarget:
    case eDeliverState::moving:
        toFindTarget();
        break;
    case eDeliverState::atTarget:
        toAtOrReturn();
        break;
    case eDeliverState::idleOutside:
        enterIdleOutside();
        break;
    case eDeliverState::returning:
        enterReturning();
        break;
    }
}

bool eDeliverCartAction::savesCartState() const {
    return false;
}
