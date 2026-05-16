#include "vendor-cart-action.h"

#include "fileIO/esavearchive.h"
#include "buildings/ebuildingwithresource.h"
#include "characters/ecarttransporter.h"

eVendorCartAction::eVendorCartAction(eCharacter* const c,
                                     eBuildingWithResource* const b)
    : eCartTransporterAction(c, b, eCharActionType::vendorCartAction) {}

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
    const auto tasks = building() ? building()->cartTasks() : std::vector<eCartTask>{};
    if(tasks.empty()) { enterWaitAtHome(); return; }
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

void eVendorCartAction::onFindTargetFail() {
    wait(kFindRetryWait);
}

// ── serialize ─────────────────────────────────────────────────────────────────

void eVendorCartAction::serializeFields(eSaveArchive& ar) {
    eCartTransporterAction::serializeFields(ar);
    ar.field("mState", mState);
}
