#include "get-cart-action.h"

#include "fileIO/esavearchive.h"

eGetCartAction::eGetCartAction(eCharacter* const c,
                               eBuildingWithResource* const b)
    : CartTransporterAction(c, b, eCharActionType::getCartAction) {}

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
    const auto c = cart();
    if(c->hasResource()) {
        const auto rt = c->resType();
        const int given = building()->add(rt, c->resCount());
        c->take(rt, given);
        const int leftover = c->resCount();
        if(leftover > 0) {
            building()->stash(rt, leftover);
            c->take(rt, leftover);
        }
    }
    enterIdle();
}

void eGetCartAction::toFindTarget() {
    mGetState = eGetState::findTarget;
    findTarget(); // BFS all GET tasks
}

void eGetCartAction::toAtOrReturn() {
    enterReturning();
}

// ── enter: actions (side effects) ────────────────────────────────────────────

void eGetCartAction::enterIdle() {
    mGetState = eGetState::idle;
    cart()->setSupportResource(eResourceType::allTransportable);
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

void eGetCartAction::onAtTarget() {
    mGetState = eGetState::atTarget;
}

void eGetCartAction::onFindTargetFail() {
    if(cart()->hasResource()) {
        enterReturning();
        return;
    }
    wait(kFindRetryWait); // sleep → decide() → toFindTarget() retry forever
}

// ── serialize ────────────────────────────────────────────────────────────────

void eGetCartAction::serializeFields(eSaveArchive& ar) {
    CartTransporterAction::serializeFields(ar);
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
