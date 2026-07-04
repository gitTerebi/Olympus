#include "deliver-cart-action.h"

#include "fileIO/save-archive.h"

eDeliverCartAction::eDeliverCartAction(eCharacter* const c,
                                       eBuildingWithResource* const b)
    : CartTransporterAction(c, b, eCharActionType::deliverCartAction) {}

// ── decide: pure state transition ────────────────────────────────────────────

bool eDeliverCartAction::decide() {
    if(!building()) return true;
    switch(mDeliverState) {
    case eDeliverState::idle:        toFindTarget(); break;
    case eDeliverState::loading:     toFindTarget(); break; // legacy save
    case eDeliverState::waitOutside: toFindTarget(); break; // legacy save
    case eDeliverState::findTarget:  toFindTarget(); break; // retry after sleep
    case eDeliverState::moving:      toAtOrReturn(); break;
    case eDeliverState::atTarget:    toAtOrReturn(); break;
    case eDeliverState::idleOutside: toFindTarget(); break; // retry after sleep
    case eDeliverState::returning:   toLoadOrIdle(); break;
    }
    return true;
}

// ── transition helpers ────────────────────────────────────────────────────────

void eDeliverCartAction::toFindTarget() {
    const auto c = cart();
    mDeliverState = eDeliverState::findTarget;
    if(c->hasResource()) {
        // mid-journey: re-run the same BFS with the cargo
        eCartTask task;
        task.fMaxCount = c->resCount();
        task.fResource = c->resType();
        task.fType = eCartActionType::deliver;
        findTarget(task);
        return;
    }
    // at home: probe with the real road BFS before taking any stock —
    // the BFS is the single judge of reachable targets, stock is only
    // loaded in its found-callback (startResourceAction)
    std::vector<eCartTask> dtasks;
    const auto tasks = building()->cartTasks();
    for(const auto& task : tasks) {
        if(task.fType != eCartActionType::deliver) continue;
        if(task.fMaxCount <= 0) continue;
        dtasks.push_back(task);
    }
    if(dtasks.empty()) {
        enterIdle();
        return;
    }
    findTarget(dtasks);
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
        const auto res = c->resType();
        const int count = c->resCount();
        const int added = building()->add(res, count);
        c->setResource(res, count - added);
        // home full — stash leftover so cart frees up for other items
        const int leftover = c->resCount();
        if(leftover > 0) {
            building()->stash(res, leftover);
            c->take(res, leftover);
        }
    }
    enterIdle();
}

// ── enter: actions (side effects) ────────────────────────────────────────────

void eDeliverCartAction::enterIdle() {
    mDeliverState = eDeliverState::idle;
    mFindRetry = 0;
    clearTask();
    if(!cart()->hasResource()) {
        cart()->setType(eCartTransporterType::basic);
    }
    cart()->setVisible(false);
    wait(kIdleWait);
}

void eDeliverCartAction::enterIdleOutside() {
    mDeliverState = eDeliverState::idleOutside;
    cart()->setVisible(true);
    wait(kIdleOutsideWait); // sleep → decide() fires → toFindTarget()
}

void eDeliverCartAction::enterReturning() {
    mDeliverState = eDeliverState::returning;
    mFindRetry = 0;
    cart()->setVisible(true);
    goBack();
}

// ── findTarget callbacks ──────────────────────────────────────────────────────

void eDeliverCartAction::onFindTargetFail() {
    if(cart()->hasResource()) {
        // mid-journey fail — brief retries for transient races, then home
        mFindRetry++;
        if(mFindRetry >= kMaxFindRetries) {
            enterReturning();
        } else {
            wait(kFindRetryWait);
        }
    } else {
        // home probe fail — nothing was loaded, stay in, retry later
        enterIdle();
    }
}

void eDeliverCartAction::onFoundTarget() {
    // stock gets loaded by the BFS found-callback; step out now
    mFindRetry = 0;
    cart()->setVisible(true);
    mDeliverState = eDeliverState::moving;
}

void eDeliverCartAction::onAtTarget() {
    mDeliverState = eDeliverState::atTarget;
}

// ── serialize ────────────────────────────────────────────────────────────────

void eDeliverCartAction::serializeFields(SaveArchive& ar) {
    CartTransporterAction::serializeFields(ar);
    ar.field("deliverState", mDeliverState);
    ar.field("findRetry", mFindRetry);
}

void eDeliverCartAction::resumeFromSavedState() {
    setCurrentAction(nullptr);
    switch(mDeliverState) {
    case eDeliverState::idle:
        enterIdle();
        break;
    case eDeliverState::loading:
    case eDeliverState::waitOutside:
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
