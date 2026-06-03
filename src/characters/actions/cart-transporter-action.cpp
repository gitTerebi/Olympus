#include "cart-transporter-action.h"

#include "../echaracter.h"
#include "buildings/ebuildingwithresource.h"
#include "buildings/estoragebuilding.h"
#include "buildings/horse-ranch-enclosure.h"
#include "buildings/ehorseranch.h"
#include "buildings/evendor.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"
#include "emovetoaction.h"
#include "ewaitaction.h"
#include "ewalkablehelpers.h"

CartTransporterAction::CartTransporterAction(
        eCharacter* const c,
        eBuildingWithResource* const b,
        const eCharActionType type) :
    eActionWithComeback(c, type),
    mBuilding(b) {}

CartTransporterAction::CartTransporterAction(
        eCharacter* const c,
        eBuildingWithResource* const b) :
    CartTransporterAction(c, b, eCharActionType::cartTransporterAction) {}

CartTransporterAction::CartTransporterAction(eCharacter* const c) :
    CartTransporterAction(c, nullptr){}

void CartTransporterAction::increment(const int by) {
    if(!mBuilding) {
        return;
    }
    updateWaiting();
    const bool deliverOnly = (support() & eCartActionTypeSupport::deliver) &&
                             !(support() & eCartActionTypeSupport::get);
    const bool outdoors = deliverOnly &&
                          (mState == eCartState::waitOutside ||
                           mState == eCartState::movingToTarget ||
                           mState == eCartState::atTarget ||
                           mState == eCartState::idleOutside);
    if(outdoors) {
        const auto c = character();
        const auto t = c->tile();
        if(t && !t->hasRoad() && !t->hasAvenue() &&
           !eWalkableHelpers::sTileUnderBuilding(t, mBuilding)) {
            enterReturning();
            return;
        }
    }
    eActionWithComeback::increment(by);
    if(dynamic_cast<eWaitAction*>(currentAction())) {
        character()->setActionType(eCharacterActionType::stand);
    }
}

bool CartTransporterAction::decide() {
    if(!mBuilding) return true;
    const auto c = static_cast<eCartTransporter*>(character());

    switch(mState) {
    case eCartState::idle:
        enterLoadingDeliver();
        break;

    case eCartState::waitOutside: {
        // cart reached road — now path to delivery target
        if(c->hasResource()) {
            eCartTask task;
            task.fMaxCount = c->resCount();
            task.fResource = c->resType();
            task.fType = eCartActionType::deliver;
            enterMovingToTarget(task);
        } else {
            enterLoadingDeliver();
        }
        break;
    }

    case eCartState::loadingDeliver:
    case eCartState::loadingGet:
        // stale state with no current action — restart
        enterLoadingDeliver();
        break;

    case eCartState::movingToTarget:
    case eCartState::atTarget: {
        const int count = c->resCount();
        const auto res = c->resType();
        bool continuee = false;
        if(mTask.fMaxCount > 0 && mTask.fResource == res) {
            if(mTask.fType == eCartActionType::get) {
                const int max = res == eResourceType::sculpture ? 1 : 4;
                continuee = (max - count) > 0;
            } else {
                continuee = count > 0;
            }
        }
        if(continuee) {
            findTarget(mTask);
        } else {
            enterReturning();
        }
        break;
    }

    case eCartState::idleOutside: {
        // standing on road — retry deliver target search
        const auto tasks = mBuilding->cartTasks();
        bool hasDeliver = false;
        for(const auto& task : tasks) {
            if(task.fType != eCartActionType::deliver) continue;
            if(task.fMaxCount > 0) { hasDeliver = true; break; }
        }
        if(hasDeliver) {
            enterLoadingDeliver();
        } else {
            enterReturning();
        }
        break;
    }

    case eCartState::returning: {
        // arrived home — deposit any fetched GET resource
        if(c->hasResource()) {
            const auto supp = support();
            if(supp & eCartActionTypeSupport::get) {
                const auto rt = c->resType();
                const int given = mBuilding->add(rt, c->resCount());
                c->take(rt, given);
                const int leftover = c->resCount();
                if(leftover > 0) {
                    mBuilding->stash(rt, leftover);
                    c->take(rt, leftover);
                }
            }
            // deliver cart came home with leftover — re-try only if a real
            // deliver target exists; else dump back to home to break loop.
            if(c->hasResource() && (support() & eCartActionTypeSupport::deliver)) {
                const auto tks = mBuilding->cartTasks();
                bool hasDeliver = false;
                for(const auto& t : tks) {
                    if(t.fType == eCartActionType::deliver && t.fMaxCount > 0) {
                        hasDeliver = true; break;
                    }
                }
                if(hasDeliver) {
                    eCartTask task;
                    task.fMaxCount = c->resCount();
                    task.fResource = c->resType();
                    task.fType = eCartActionType::deliver;
                    enterMovingToTarget(task);
                    break;
                }
                // no deliver targets — dump back to yard
                const auto rt = c->resType();
                const int n = c->resCount();
                const int added = mBuilding->add(rt, n);
                c->take(rt, added);
                const int leftover = c->resCount();
                if(leftover > 0) {
                    mBuilding->stash(rt, leftover);
                    c->take(rt, leftover);
                }
            }
        }
        if(mTask.fMaxCount > 0) {
            c->catchUp();
            finishResourceAction(mTask);
            clearTask();
        }
        enterLoadingDeliver();
        break;
    }
    }
    return true;
}

void CartTransporterAction::enterIdle() {
    mState = eCartState::idle;
    mTarget = nullptr;
    clearTask();
}

void CartTransporterAction::enterLoadingDeliver() {
    mState = eCartState::loadingDeliver;
    mTarget = nullptr;
    const auto c = static_cast<eCartTransporter*>(character());
    const auto tasks = mBuilding->cartTasks();
    for(const auto& task : tasks) {
        if(task.fType != eCartActionType::deliver) continue;
        startResourceAction(task);
        if(c->resCount() > 0) {
            enterWaitOutside();
            return;
        }
    }
    // no deliver stock — try GET if supported, else idle
    const auto supp = support();
    if(supp & eCartActionTypeSupport::get) {
        enterLoadingGet();
    } else {
        enterIdle();
    }
}

void CartTransporterAction::enterLoadingGet() {
    mState = eCartState::loadingGet;
    mTarget = nullptr;
    findTarget(); // BFS for GET target
}

void CartTransporterAction::enterWaitOutside() {
    mState = eCartState::waitOutside;
    waitOutside(); // moves cart to adjacent road tile; on arrive → spread() → decide()
}

void CartTransporterAction::enterMovingToTarget(const eCartTask& task) {
    mState = eCartState::movingToTarget;
    findTarget(task);
}

void CartTransporterAction::enterReturning() {
    mState = eCartState::returning;
    goBack(); // sets mTarget = mBuilding internally
}

eCartActionTypeSupport CartTransporterAction::support() const {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    return ct->support();
}

eResourceType CartTransporterAction::supportsResource() const {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    return ct->supportsResource();
}

int CartTransporterAction::cartCapacity(const eResourceType res) const {
    if(!mBuilding) return eResourceTypeHelpers::transportSize(
        res, board().doubleCartCapacity());
    return mBuilding->cartCapacity(res, board().doubleCartCapacity());
}

void CartTransporterAction::findTarget() {
    if(!mBuilding) return;
    const auto tasks = mBuilding->cartTasks();
    const auto supp = support();
    if(supp == eCartActionTypeSupport::both) {
        findTarget(tasks);
    } else {
        std::vector<eCartTask> handled;
        const bool supportGive = supp & eCartActionTypeSupport::deliver;
        const bool supportTake = supp & eCartActionTypeSupport::get;
        for(const auto& t : tasks) {
            if(t.fType == eCartActionType::deliver && !supportGive) continue;
            if(t.fType == eCartActionType::get && !supportTake) continue;
            const auto r = t.fResource;
            const auto sr = supportsResource();
            const bool spprts = static_cast<bool>(r & sr);
            if(spprts) handled.push_back(t);
        }
        findTarget(handled);
    }
}

void CartTransporterAction::findTarget(const eCartTask& task) {
    findTarget(std::vector<eCartTask>{task});
}

void CartTransporterAction::findTarget(const eCartTask& task,
                                        eBuilding* const avoided) {
    findTarget(std::vector<eCartTask>{task}, avoided);
}

void CartTransporterAction::findTarget(const std::vector<eCartTask>& tasks) {
    findTarget(tasks, nullptr);
}

void CartTransporterAction::findTarget(const std::vector<eCartTask>& tasks,
                                        eBuilding* const avoided) {
    findTarget(tasks, avoided, true);
}

void CartTransporterAction::findTarget(const std::vector<eCartTask>& tasks,
                                        eBuilding* const avoided,
                                        const bool preferGranaryFirst,
                                        const bool preferEmptyFirst,
                                        const bool preferGetsFirst) {
    if(!mBuilding) return;
    if(tasks.empty()) { enterIdle(); return; }
    const auto c = character();

    const auto buildingRect = mBuilding->tileRect();
    const bool hasAvoided = avoided;
    const SDL_Rect avoidedRect = hasAvoided ?
                                     avoided->tileRect() : SDL_Rect{0, 0, 0, 0};
    const stdptr<eBuilding> avoidedPtr(avoided);

    // 1. Store target coordinates and task
    const auto bx = std::make_shared<int>(0);
    const auto by = std::make_shared<int>(0);
    const auto ttask = std::make_shared<eCartTask>();

    const auto bType = mBuilding->type();

    // Producers prefer granaries over storehouses/trading posts
    const bool isProducer = bType != eBuildingType::warehouse &&
                            bType != eBuildingType::granary &&
                            bType != eBuildingType::tradePost;
    const bool hasGiveTasks = std::any_of(tasks.begin(), tasks.end(),
        [](const eCartTask& t) {
            return t.fType == eCartActionType::deliver &&
                   static_cast<bool>(t.fResource & eResourceType::food);
        });
    const auto preferGranary = std::make_shared<bool>(preferGranaryFirst &&
                                                    isProducer &&
                                                    hasGiveTasks);
    const bool hasGetTasks = std::any_of(tasks.begin(), tasks.end(),
        [](const eCartTask& t) { return t.fType == eCartActionType::get; });
    const auto preferEmpty = std::make_shared<bool>(preferEmptyFirst && hasGetTasks);

    // EMPTY-mode home: deliver carts prefer targets with GET orders
    const auto homeStorage = dynamic_cast<eStorageBuilding*>(mBuilding.get());
    const bool hasDeliverTasks = std::any_of(tasks.begin(), tasks.end(),
        [](const eCartTask& t) { return t.fType == eCartActionType::deliver; });
    const bool homeEmpties = hasDeliverTasks && homeStorage &&
        std::any_of(tasks.begin(), tasks.end(), [this, homeStorage](const eCartTask& t) {
            return t.fType == eCartActionType::deliver &&
                   homeStorage->empties(t.fResource);
        });
    const auto preferGets = std::make_shared<bool>(preferGetsFirst && homeEmpties);

    // 2. Check each tile for valid target buildings
    const auto finalTile = [this, buildingRect, bType, ttask, tasks, bx, by,
                            preferGranary, preferEmpty, preferGets, hasAvoided, avoidedRect]
                           (eThreadTile* const t) {
        // 2.1 Skip tiles without buildings
        if(!t->isUnderBuilding()) return false;

        // 2.2 Skip tiles part of cart's home building
        const bool r = eWalkableHelpers::sTileUnderBuilding(t, buildingRect);
        if(r) return false;
        if(hasAvoided) {
            const SDL_Point p{t->x(), t->y()};
            if(SDL_PointInRect(&p, &avoidedRect)) return false;
        }

        const auto& ub = t->underBuilding();

        // 2.3 Producers: first pass accepts only granaries
        if(*preferGranary && ub.type() != eBuildingType::granary) return false;

        // 2.3b GET first pass: only buildings set to empty
        if(*preferEmpty) {
            bool anyGetRes = false;
            for(const auto& task : tasks) {
                if(task.fType != eCartActionType::get) continue;
                if(ub.empties(task.fResource)) { anyGetRes = true; break; }
            }
            if(!anyGetRes) return false;
        }

        // 2.3c EMPTY-home first pass: deliver only to buildings with GET orders
        if(*preferGets) {
            bool anyGetsRes = false;
            for(const auto& task : tasks) {
                if(task.fType != eCartActionType::deliver) continue;
                if(ub.gets(task.fResource)) { anyGetsRes = true; break; }
            }
            if(!anyGetsRes) return false;
        }

        // 2.4 Skip trading posts for agora vendors when setting disabled
        if(ub.type() == eBuildingType::tradePost) {
            if(!board().agorasTakeFromTradingPosts()) {
                const auto v = dynamic_cast<eVendor*>(mBuilding.get());
                if(v && v->agora()) return false;
            }
        }

        // 2.5 Check if building can fulfill any cart tasks
        for(const auto& task : tasks) {
            const auto res = task.fResource;
            bool found = false;
            if(!acceptsTargetForTask(task, ub)) continue;

            if(task.fType == eCartActionType::get) {
                const auto city = board().boardCityWithId(t->cityId());
                if(city && city->isStockpiled(res)) continue;
                if(ub.resourceHas(res)) found = true;
            } else { // give
                if(ub.empties(res)) continue;
                if(ub.resourceHasSpace(res)) found = true;
            }

            if(found) {
                // 2.6 Calculate transferable amount
                int space = ub.resourceSpaceLeft(res);
                if(task.fType == eCartActionType::deliver) {
                    // subtract stock other yard carts already heading here
                    const auto realB = board().buildingAt(t->x(), t->y());
                    const int reserved = eStorageBuilding::incomingReservedFor(
                        realB, res, board(), t->cityId());
                    space -= reserved;
                    if(space <= 0) continue;
                }
                int mc = (task.fType == eCartActionType::get) ?
                    std::min(ub.resourceCount(res), task.fMaxCount) :
                    std::min(space, task.fMaxCount);
                if(mc <= 0) continue;

                // 2.7 Valid target found
                *ttask = task;
                *bx = t->x();
                *by = t->y();
                return true;
            }
        }
        return false;
    };
    const stdptr<CartTransporterAction> tptr(this);

    const auto finishAction = std::make_shared<eCTA_findTargetFinish>(
                                  board(), this);

    const auto startSearch = [this, tptr, c, tasks, finalTile, finishAction,
                               ttask, bx, by, preferGranary, preferEmpty, preferGets, avoidedPtr]() {
        const auto a = e::make_shared<eMoveToAction>(c);
        a->setStateRelevance(eStateRelevance::resourcesInBuildings |
                             eStateRelevance::buildings);
        a->setFinishAction(finishAction);

        a->setFoundAction([tptr, this, c, ttask, bx, by, finishAction]() {
            finishAction->setXY(*bx, *by);
            if(!tptr) return;
            const auto& board = this->board();
            const auto b = board.buildingAt(*bx, *by);
            mTarget = b;
            mState = eCartState::movingToTarget;
            mTask = *ttask;
            startResourceAction(mTask);
            c->setActionType(eCharacterActionType::walk);
            onFoundTarget();
        });
        a->setFindFailAction([tptr, this, preferGranary, preferEmpty, preferGets, tasks, avoidedPtr]() {
            if(!tptr) return;
            if(*preferGranary) {
                *preferGranary = false;
                findTarget(tasks, avoidedPtr.get(), false, *preferEmpty, *preferGets);
            } else if(*preferEmpty) {
                *preferEmpty = false;
                findTarget(tasks, avoidedPtr.get(), false, false, *preferGets);
            } else if(*preferGets) {
                *preferGets = false;
                findTarget(tasks, avoidedPtr.get(), false, false, false);
            } else {
                onFindTargetFail();
            }
        });
        a->setRemoveLastTurn(true);
        if(const auto cart = dynamic_cast<eCartTransporter*>(c)) {
            a->setMaxFindDistance(cart->maxDistance());
        }
        // GET carts may cross land, but roads stay cheapest.
        eCartActionType scanType = eCartActionType::deliver;
        for(const auto& tk : tasks) {
            if(tk.fType == eCartActionType::get) { scanType = eCartActionType::get; break; }
        }
        if(scanType == eCartActionType::get) {
            a->setTileDistance([](eTileBase* const tile) {
                const auto type = tile->underBuildingType();
                if(type == eBuildingType::road ||
                   type == eBuildingType::avenue) return 1;
                return 64;
            });
        }
        const auto w = getWalkableForTask(true, scanType);
        setCurrentAction(a);
        a->start(finalTile, w);
    };

    startSearch();
}

void CartTransporterAction::throttleDropoffRetry() {
    if(!mBuilding) return;
    if(mRetryCount >= kMaxDropoffRetries) {
        mRetryCount = 0;
        enterIdle();
        return;
    }
    mRetryCount++;
    wait(kRetryWaitTicks);
}

void CartTransporterAction::onFindTargetFail() {
    const auto c = cart();
    if(c->hasResource()) {
        enterReturning();
    } else {
        enterIdle();
    }
}

void CartTransporterAction::goBack() {
    if(!mBuilding) return;
    const auto w = getWalkable();
    eActionWithComeback::goBack(w);
    mTarget = mBuilding;
}

void CartTransporterAction::targetResourceAction(const int bx, const int by) {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    auto& brd = ct->getBoard();
    const auto t = brd.tile(bx, by);
    const auto b = t->underBuilding();
    if(!b) return;
    const auto rb = dynamic_cast<eBuildingWithResource*>(b);
    targetResourceAction(rb);
}

void CartTransporterAction::targetResourceAction(eBuildingWithResource* const rb) {
    if(!mBuilding) return;
    if(!rb) return;
    mState = eCartState::atTarget;
    onAtTarget();
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    const int startCount = ct->resCount();
    const int takenGiven = targetProcessTask(rb, mTask);
    mTask.fMaxCount -= takenGiven;

    auto tasks = mBuilding->cartTasks();
    const auto supp = support();
    const bool supportGive = supp & eCartActionTypeSupport::deliver;
    const bool supportTake = supp & eCartActionTypeSupport::get;
    for(auto& task : tasks) {
        if(task.fType == eCartActionType::deliver && !supportGive) continue;
        if(task.fType == eCartActionType::get && !supportTake) continue;
        if(task.fType == eCartActionType::get) {
            if(ct->resType() == task.fResource) {
                task.fMaxCount -= ct->resCount();
            }
        }
        targetProcessTask(rb, task);
    }
    const int count = ct->resCount();
    if(count > 0 && mTask.fType == eCartActionType::deliver) {
        const auto res = ct->resType();
        const int added = rb->add(res, count);
        ct->setResource(res, count - added);
        if(added > 0) mRetryCount = 0;
    }
    // subclass FSM drives next transition via decide()
}

int CartTransporterAction::targetProcessTask(eBuildingWithResource* const rb,
                                              const eCartTask& task) {
    if(task.fMaxCount <= 0) return 0;
    const auto c = static_cast<eCartTransporter*>(character());
    const auto res = c->resType();
    const int count = c->resCount();
    const auto tres = task.fResource;
    const int max = tres == eResourceType::sculpture ? 1 : 4;
    if(task.fType == eCartActionType::get) {
        const auto city = board().boardCityWithId(rb->cityId());
        if(city && city->isStockpiled(tres)) return 0;
        if(count > 0 && res != tres) return 0;
        int destinationSpace = mBuilding->spaceLeft(tres);
        if(const auto storage = dynamic_cast<eStorageBuilding*>(mBuilding.get())) {
            for(int i = 0; i < eStorageBuilding::sMaxCarts; i++) {
                eCartTransporter* const other = storage->cart(i);
                if(!other || other == c) continue;
                if(!other->hasResource()) continue;
                if(other->resType() != tres) continue;
                destinationSpace -= other->resCount();
            }
        }
        const int space = std::min(max - count, std::max(0, destinationSpace));
        if(space <= 0) return 0;
        const int toTake = std::min(space, task.fMaxCount);
        const int taken = rb->take(tres, toTake);
        c->setResource(tres, taken + count);
        if(taken > 0) return taken;
    } else { // give
        if(count == 0) return 0;
        if(res != tres) return 0;
        const int toAdd = std::min(count, task.fMaxCount);
        const int added = rb->add(tres, toAdd);
        c->setResource(tres, count - added);
        return added;
    }
    return 0;
}

void CartTransporterAction::startResourceAction(const eCartTask& task) {
    if(!mBuilding) return;
    const auto c = static_cast<eCartTransporter*>(character());
    if(task.fMaxCount <= 0) return;
    if(task.fType == eCartActionType::get) {
        if(c->resCount() == 0) c->setResource(task.fResource, 0);
        return;
    } else { // deliver — top-up to capacity
        if(c->resCount() > 0 && c->resType() != task.fResource) return;
        const int max = cartCapacity(task.fResource);
        const int space = max - c->resCount();
        if(space <= 0) return;
        const int toTake = std::min(space, task.fMaxCount);
        const int t = mBuilding->take(task.fResource, toTake);
        if(t > 0) {
            c->setResource(task.fResource, c->resCount() + t);
        }
    }
}

void CartTransporterAction::finishResourceAction(const eCartTask& task) {
    if(!mBuilding) return disappear();
    const auto c = static_cast<eCartTransporter*>(character());
    if(c->resCount() <= 0) return disappear();
    if(task.fMaxCount <= 0) return;
    if(task.fResource != c->resType()) return;
    if(task.fType == eCartActionType::get) {
        const int crc = c->resCount();
        const int a = mBuilding->add(task.fResource, crc);
        c->setResource(task.fResource, crc - a);
        if(c->resCount() <= 0) return disappear();
    } else { //give
        return;
    }
}

bool CartTransporterAction::acceptsTargetForTask(
        const eCartTask& task,
        const eThreadBuilding& target) const {
    const bool storageHome = dynamic_cast<eStorageBuilding*>(mBuilding.get());
    const auto targetType = target.type();
    const bool storageTarget = targetType == eBuildingType::warehouse ||
                               targetType == eBuildingType::granary;
    if(storageHome && task.fType == eCartActionType::get) {
        // allow pulling imported goods from a trade post too
        return storageTarget || targetType == eBuildingType::tradePost;
    }
    if(storageHome &&
       task.fType == eCartActionType::deliver &&
       targetType == eBuildingType::tradePost) return false;
    return true;
}

void CartTransporterAction::serializeFields(eSaveArchive& ar) {
    // skip mCurrentAction (walk/wait "feet") on write; rebuilt from FSM state on load.
    const bool writing = ar.writing();
    stdsptr<eCharacterAction> savedFeet;
    if(writing) {
        savedFeet = currentActionPtr();
        setCurrentAction(nullptr);
    }
    eActionWithComeback::serializeFields(ar);
    if(writing) {
        setCurrentAction(savedFeet);
    }

    ar.buildingAsField("sourceBuilding", &board(), mBuilding);

    ar.field("taskMaxCount", mTask.fMaxCount);
    ar.field("taskResource", mTask.fResource);
    ar.field("taskType", mTask.fType);

    ar.field("updateWaiting", mUpdateWaiting);

    if(savesCartState()) {
        ar.field("cartState", mState);
    }

    ar.buildingField("targetBuilding", &board(), mTarget);
}

void CartTransporterAction::resumeFromSavedState() {
    setCurrentAction(nullptr);
    switch(mState) {
    case eCartState::idle:
        break;
    case eCartState::loadingDeliver:
    case eCartState::loadingGet:
    case eCartState::atTarget:
    case eCartState::idleOutside:
        decide();
        break;
    case eCartState::waitOutside:
        enterWaitOutside();
        break;
    case eCartState::movingToTarget:
        if(mTask.fMaxCount > 0) enterMovingToTarget(mTask);
        else decide();
        break;
    case eCartState::returning:
        enterReturning();
        break;
    }
}

bool CartTransporterAction::savesCartState() const {
    return true;
}

stdsptr<WalkableObject> CartTransporterAction::getWalkableForTask(
        bool excludeHomeRect, eCartActionType taskType) const {
    if(!mBuilding) return WalkableObject::sCreateRoadAvenue();
    const auto supp = support();
    // storage-yard rule: GET fallback = open ground, DELIVER/EMPTY = road
    const bool isStorageHome = dynamic_cast<eStorageBuilding*>(mBuilding.get());
    if(isStorageHome && taskType != eCartActionType::get) {
        const auto buildingRect = mBuilding->tileRect();
        auto w = WalkableObject::sCreateRoadAvenue();
        if(!excludeHomeRect) w = WalkableObject::sCreateRect(buildingRect, w);
        return w;
    }
    if(supp & eCartActionTypeSupport::get) {
        const auto buildingRect = mBuilding->tileRect();
        const auto type = mBuilding->type();
        const bool isVendor = type == eBuildingType::foodVendor ||
                              type == eBuildingType::fleeceVendor ||
                              type == eBuildingType::oilVendor ||
                              type == eBuildingType::wineVendor ||
                              type == eBuildingType::armsVendor ||
                              type == eBuildingType::horseTrainer ||
                              type == eBuildingType::chariotVendor;
        if(isVendor) {
            auto w = WalkableObject::sCreateRoadAvenue();
            w = WalkableObject::sCreateRect(buildingRect, w);
            return w;
        }
        auto w = WalkableObject::sCreateDefault();
        w = WalkableObject::sCreateRect(buildingRect, w);
        if(type == eBuildingType::horseRanch) {
            const auto hr = static_cast<eHorseRanch*>(mBuilding.get());
            const auto e = hr->enclosure();
            const auto eRect = e->tileRect();
            w = WalkableObject::sCreateRect(eRect, w);
        }
        return w;
    }
    const auto buildingRect = mBuilding->tileRect();
    auto w = WalkableObject::sCreateRoadAvenue();
    if(!excludeHomeRect) w = WalkableObject::sCreateRect(buildingRect, w);
    return w;
}

stdsptr<WalkableObject> CartTransporterAction::getWalkable(bool excludeHomeRect) const {
    return getWalkableForTask(excludeHomeRect, mTask.fType);
}

void CartTransporterAction::updateWaiting() {
    if(!mBuilding) return;
    const auto c = static_cast<eCartTransporter*>(character());
    const bool r = eWalkableHelpers::sTileUnderBuilding(
                       c->tile(), mBuilding);
    c->setWaiting(mState == eCartState::waitOutside || r);
}

void CartTransporterAction::waitOutside() {
    if(!mBuilding) return;
    // guard removed — enterWaitOutside() is the sole caller; it sets state first
    const auto neighs = mBuilding->neighbours();
    if(neighs.empty()) return;
    const auto c = character();
    eTile* tt = nullptr;
    for(const auto t : neighs) {
        if(!t->hasRoad()) continue;
        tt = t;
        break;
    }
    if(!tt) return;

    const auto stand = std::make_shared<eCTA_waitOutsideFinish>(
                           board(), this);
    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings);
    a->setFinishAction(stand);
    a->setFailAction(stand);

    const auto w = getWalkable();
    a->start(tt, w);

    setCurrentAction(a);
}

void CartTransporterAction::spread() {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    if(!ct->isOx()) {
        c->setActionType(eCharacterActionType::stand);
        return;
    }

    const auto stand = std::make_shared<eCTA_spreadFinish>(
                           board(), ct);
    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings);
    a->setFailAction(stand);
    a->setFinishAction(stand);

    const auto ctt = ct->tile();
    const int tx = ctt->x();
    const int ty = ctt->y();

    const auto finalFunc = [tx, ty](eTileBase* const t) {
        const int ttx = t->x();
        const int tty = t->y();
        const int dx = tx - ttx;
        const int dy = ty - tty;
        return sqrt(dx*dx + dy*dy) > 4;
    };
    a->setRemoveLastTurn(true);
    a->start(finalFunc, WalkableObject::sCreateRoadAvenue());

    setCurrentAction(a);
}

void CartTransporterAction::clearTask() {
    mTask.fMaxCount = 0;
    setCurrentAction(nullptr);
    mTarget = nullptr;
    character()->setActionType(eCharacterActionType::stand);
}

void CartTransporterAction::disappear() {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    if(ct->resCount() == 0) ct->setResource(eResourceType::wine, 0);
}
