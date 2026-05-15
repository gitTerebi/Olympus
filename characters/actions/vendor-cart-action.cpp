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
    case eVendorCartState::idle:        toFindTarget();  break;
    case eVendorCartState::findTarget:  toFindTarget();  break;
    case eVendorCartState::moving:      toAtOrReturn();  break;
    case eVendorCartState::atTarget:    toAtOrReturn();  break;
    case eVendorCartState::returning:   toDeposit();     break;
    }
    return true;
}

// ── transitions ───────────────────────────────────────────────────────────────

void eVendorCartAction::toFindTarget() {
    mState = eVendorCartState::findTarget;
    findTarget();
}

void eVendorCartAction::toAtOrReturn() {
    const auto c = cart();
    const int count = c->resCount();
    const auto res = c->resType();
    const int max = res == eResourceType::sculpture ? 1 : 4;
    const bool canTakeMore = mTask.fMaxCount > 0 &&
                             mTask.fResource == res &&
                             mTask.fType == eCartActionType::get &&
                             (max - count) > 0;
    if(canTakeMore) {
        mState = eVendorCartState::findTarget;
        findTarget(mTask);
    } else {
        enterReturning();
    }
}

void eVendorCartAction::toDeposit() {
    // arrived home — deposit to vendor building
    const auto c = cart();
    const auto b = building();
    if(c && b && c->hasResource()) {
        const auto rt = c->resType();
        const int crc = c->resCount();
        const int given = b->add(rt, crc);
        c->setResource(rt, crc - given);
        const int leftover = c->resCount();
        if(leftover > 0) {
            b->stash(rt, leftover);
            c->setResource(rt, 0);
        }
    }
    enterIdle();
}

// ── enter ─────────────────────────────────────────────────────────────────────

void eVendorCartAction::enterIdle() {
    mState = eVendorCartState::idle;
    clearTask();
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

void eVendorCartAction::serializeVendor(eSaveArchive& ar) {
    ar.field("mState", mState);
}

void eVendorCartAction::read(eReadStream& src) {
    eCartTransporterAction::read(src);
    eSaveArchive ar(src);
    serializeVendor(ar);
}

void eVendorCartAction::write(eWriteStream& dst) const {
    eCartTransporterAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eVendorCartAction*>(this)->serializeVendor(ar);
}
