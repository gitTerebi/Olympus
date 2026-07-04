#include "storage-delivery-cart.h"
#include "buildings/estoragebuilding.h"
#include "engine/game-board.h"
#include "engine/thread/ethreadbuilding.h"
#include "fileIO/save-archive.h"

namespace {
int storageTransportSize(const eResourceType type,
                         const bool doubleCapacity = false) {
    const int base = (type == eResourceType::sculpture) ? 1 : 4;
    return doubleCapacity ? base * 2 : base;
}
}

eStorageDeliveryCartAction::eStorageDeliveryCartAction(
        eCharacter* const c,
        eBuildingWithResource* const b) :
    CartTransporterAction(c, b, eCharActionType::storageDeliveryCartAction) {
    if(c) c->setVisible(false);
}

bool eStorageDeliveryCartAction::decide() {
    if(!building()) return true;
    const auto c = cart();

    switch(mState) {
    case eCartState::idle:
        enterStorageLoadingDeliver();
        break;
    case eCartState::waitOutside: {
        if(c->hasResource()) {
            eCartTask task;
            task.fMaxCount = c->resCount();
            task.fResource = c->resType();
            task.fType = eCartActionType::deliver;
            task.fStoragePush = mTask.fStoragePush;
            // BFS is the single judge — fail lands in onFindTargetFail
            enterStorageMovingToTarget(task);
        } else {
            enterStorageLoadingDeliver();
        }
        break;
    }
    case eCartState::loadingDeliver:
    case eCartState::loadingGet:
        enterStorageLoadingDeliver();
        break;
    case eCartState::movingToTarget:
    case eCartState::atTarget: {
        const int count = c->resCount();
        const auto res = c->resType();
        if(mTask.fType == eCartActionType::deliver && count > 0) {
            enterStorageReturning();
            break;
        }
        bool continuee = false;
        if(mTask.fMaxCount > 0 && mTask.fResource == res) {
            if(mTask.fType == eCartActionType::get) {
                const int max = cartCapacity(res);
                continuee = (max - count) > 0;
            } else {
                continuee = count > 0;
            }
        }
        if(continuee) {
            findTarget(mTask);
        } else {
            enterStorageReturning();
        }
        break;
    }
    case eCartState::idleOutside: {
        if(c->hasResource()) {
            eCartTask task;
            task.fMaxCount = c->resCount();
            task.fResource = c->resType();
            task.fType = eCartActionType::deliver;
            task.fStoragePush = mTask.fStoragePush;
            enterStorageMovingToTarget(task);
        } else {
            enterStorageReturning();
        }
        break;
    }
    case eCartState::returning:
        if(c->hasResource()) {
            const auto supp = support();
            if(supp & eCartActionTypeSupport::get) {
                const auto rt = c->resType();
                const int given = building()->add(rt, c->resCount());
                c->take(rt, given);
                const int leftover = c->resCount();
                if(leftover > 0) {
                    building()->stash(rt, leftover);
                    c->take(rt, leftover);
                }
            }
            if(c->hasResource() && (support() & eCartActionTypeSupport::deliver)) {
                if(!mWaitAfterNoDeliveryTarget) {
                    // one BFS probe for the leftover cargo; fail sets the
                    // flag in onFindTargetFail and comes back here
                    eCartTask task;
                    task.fMaxCount = c->resCount();
                    task.fResource = c->resType();
                    task.fType = eCartActionType::deliver;
                    task.fStoragePush = mTask.fStoragePush;
                    enterStorageMovingToTarget(task);
                    break;
                }
                dumpStockAtHome();
            }
        }
        if(mTask.fMaxCount > 0) {
            c->catchUp();
            finishResourceAction(mTask);
            clearTask();
        }
        if(mWaitAfterNoDeliveryTarget) {
            enterStorageIdle();
            wait(kNoDeliveryTargetWait);
        } else {
            enterStorageLoadingDeliver();
        }
        break;
    }
    return true;
}

int eStorageDeliveryCartAction::cartCapacity(const eResourceType res) const {
    return storageTransportSize(res, board().doubleCartCapacity());
}

void eStorageDeliveryCartAction::serializeFields(SaveArchive& ar) {
    CartTransporterAction::serializeFields(ar);
    ar.field("taskStoragePush", mTask.fStoragePush, false);
}

bool eStorageDeliveryCartAction::acceptsTargetForTask(
        const eCartTask& task,
        const eThreadBuilding& target) const {
    const auto type = target.type();
    if(task.fType == eCartActionType::deliver &&
       type == eBuildingType::tradePost) return false;
    if(!task.fStoragePush) return true;
    return type != eBuildingType::warehouse &&
           type != eBuildingType::granary;
}

void eStorageDeliveryCartAction::onFindTargetFail() {
    const auto c = cart();
    if(c->hasResource()) {
        // no reachable target for the cargo — go home, dump it there
        mWaitAfterNoDeliveryTarget = true;
        enterStorageReturning();
        return;
    }
    if(mState == eCartState::loadingGet) {
        enterStorageIdle();
        wait(kNoDeliveryTargetWait);
        return;
    }
    // deliver probe failed with nothing loaded — cart never left home
    if(support() & eCartActionTypeSupport::get) {
        enterStorageLoadingGet();
    } else {
        enterStorageIdle();
        wait(kNoDeliveryTargetWait);
    }
}

void eStorageDeliveryCartAction::onFoundTarget() {
    // stock was just loaded by the BFS found-callback; step out
    character()->setVisible(true);
}

void eStorageDeliveryCartAction::startResourceAction(const eCartTask& task) {
    if(!building()) return;
    const auto c = cart();
    if(task.fMaxCount <= 0) return;
    if(task.fType == eCartActionType::get) {
        if(c->resCount() == 0) c->setResource(task.fResource, 0);
        return;
    }
    if(c->resCount() > 0 && c->resType() != task.fResource) return;
    const int space = cartCapacity(task.fResource) - c->resCount();
    if(space <= 0) return;
    const int toTake = std::min(space, task.fMaxCount);
    const int t = building()->take(task.fResource, toTake);
    if(t > 0) {
        c->setResource(task.fResource, c->resCount() + t);
    }
}

void eStorageDeliveryCartAction::enterStorageIdle() {
    mWaitAfterNoDeliveryTarget = false;
    enterIdle();
    character()->setVisible(false);
}

void eStorageDeliveryCartAction::enterStorageLoadingDeliver() {
    mWaitAfterNoDeliveryTarget = false;
    mState = eCartState::loadingDeliver;
    mTarget = nullptr;
    // probe with the real road BFS — stock is only taken in its
    // found-callback (startResourceAction), so a failed probe never
    // leaves the building
    std::vector<eCartTask> dtasks;
    const auto tasks = building()->cartTasks();
    for(const auto& task : tasks) {
        if(task.fType != eCartActionType::deliver) continue;
        if(task.fMaxCount <= 0) continue;
        dtasks.push_back(task);
    }
    if(!dtasks.empty()) {
        findTarget(dtasks);
        return;
    }
    if(support() & eCartActionTypeSupport::get) {
        enterStorageLoadingGet();
    } else {
        enterStorageIdle();
    }
}

void eStorageDeliveryCartAction::enterStorageLoadingGet() {
    character()->setVisible(true);
    enterLoadingGet();
}

void eStorageDeliveryCartAction::enterStorageWaitOutside() {
    character()->setVisible(true);
    enterWaitOutside();
}

void eStorageDeliveryCartAction::enterStorageMovingToTarget(
        const eCartTask& task) {
    character()->setVisible(true);
    enterMovingToTarget(task);
}

void eStorageDeliveryCartAction::enterStorageReturning() {
    character()->setVisible(true);
    enterReturning();
}

void eStorageDeliveryCartAction::dumpStockAtHome() {
    const auto c = cart();
    if(!c->hasResource()) return;
    const auto res = c->resType();
    const int n = c->resCount();
    const int added = building()->add(res, n);
    c->take(res, added);
    const int leftover = c->resCount();
    if(leftover > 0) {
        building()->stash(res, leftover);
        c->take(res, leftover);
    }
}
