#include "vendor-cart-action.h"

#include "fileIO/esavearchive.h"
#include "buildings/ebuildingwithresource.h"
#include "characters/ecarttransporter.h"
#include "engine/game-board.h"

eVendorCartAction::eVendorCartAction(eCharacter* const c,
                                     eBuildingWithResource* const b)
    : CartTransporterAction(c, b, eCharActionType::vendorCartAction) {}

// ── decide ────────────────────────────────────────────────────────────────────

bool eVendorCartAction::decide() {
    if(!building()) return true;
    switch(mState) {
    case eVendorCartState::waitAtHome:  toFindTarget();   break;
    case eVendorCartState::findTarget:  toFindTarget();   break;
    case eVendorCartState::moving:      mState = eVendorCartState::atTarget; break;
    case eVendorCartState::atTarget:    enterReturning(); break;
    case eVendorCartState::returning:   toDeposit();      break;
    }
    return true;
}

// ── transitions ───────────────────────────────────────────────────────────────

void eVendorCartAction::toFindTarget() {
    const auto b = building();
    const auto tasks = b ? b->cartTasks() : std::vector<eCartTask>{};
    if(tasks.empty()) { enterWaitAtHome(); return; }
    // don't leave home unless the city actually holds stock to fetch
    if(b) {
        auto& board = b->getBoard();
        const auto cid = b->cityId();
        bool anyStock = false;
        for(const auto& t : tasks) {
            if(t.fType != eCartActionType::get) continue;
            if(board.resourceCount(cid, t.fResource) > 0) { anyStock = true; break; }
        }
        if(!anyStock) { enterWaitAtHome(); return; }
    }
    mState = eVendorCartState::findTarget;
    findTarget();
}

void eVendorCartAction::toDeposit() {
    const auto c = cart();
    const auto b = building();
    if(c && b) {
        const auto rt = c->resType();
        const int crc = c->resCount();
        if(crc > 0) {
            const int given = b->add(rt, crc);
            c->setResource(rt, crc - given);
            const int leftover = c->resCount();
            if(leftover > 0) {
                b->stash(rt, leftover);
                c->setResource(rt, 0);
            }
        }
    }
    enterWaitAtHome();
}

// ── enter ─────────────────────────────────────────────────────────────────────

void eVendorCartAction::enterWaitAtHome() {
    mState = eVendorCartState::waitAtHome;
    clearTask();
    const auto b = building();
    const auto c = cart();
    // park inside the building, not on the outside road start — no loitering
    if(b && c) c->changeTile(b->centerTile());
    wait(kFindRetryWait);
}

void eVendorCartAction::enterReturning() {
    mState = eVendorCartState::returning;
    goBack();
}

// ── hooks ─────────────────────────────────────────────────────────────────────

void eVendorCartAction::onFoundTarget() {
    mState = eVendorCartState::moving;
}

void eVendorCartAction::onAtTarget() {
    mState = eVendorCartState::atTarget;
}

void eVendorCartAction::onFindTargetFail() {
    if(cart()->hasResource()) {
        enterReturning();
    } else {
        enterWaitAtHome();
    }
}

// ── serialize ─────────────────────────────────────────────────────────────────

void eVendorCartAction::serializeFields(eSaveArchive& ar) {
    CartTransporterAction::serializeFields(ar);
    ar.field("vendorState", mState);
}

void eVendorCartAction::resumeFromSavedState() {
    setCurrentAction(nullptr);
    switch(mState) {
    case eVendorCartState::waitAtHome:
        enterWaitAtHome();
        break;
    case eVendorCartState::findTarget:
    case eVendorCartState::moving:
        toFindTarget();
        break;
    case eVendorCartState::atTarget:
    case eVendorCartState::returning:
        enterReturning();
        break;
    }
}

bool eVendorCartAction::savesCartState() const {
    return false;
}
