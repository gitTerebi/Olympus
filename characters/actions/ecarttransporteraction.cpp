#include "ecarttransporteraction.h"

#include "../echaracter.h"
#include "buildings/ebuildingwithresource.h"
#include "buildings/ehorseranchenclosure.h"
#include "buildings/ehorseranch.h"
#include "buildings/evendor.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"
#include "emovetoaction.h"

eCartTransporterAction::eCartTransporterAction(
        eCharacter* const c,
        eBuildingWithResource* const b) :
    eActionWithComeback(c, eCharActionType::cartTransporterAction),
    mBuilding(b) {}

eCartTransporterAction::eCartTransporterAction(eCharacter* const c) :
    eCartTransporterAction(c, nullptr){}

void eCartTransporterAction::increment(const int by) {
    if(!mBuilding) {
        return;
    }
    updateWaiting();
    eActionWithComeback::increment(by);
}

bool eCartTransporterAction::decide() {
    if(!mBuilding) {
        return true;
    }
    const auto c = static_cast<eCartTransporter*>(character());
    const bool r = eWalkableHelpers::sTileUnderBuilding(
                       c->tile(), mBuilding);
    const int count = c->resCount();
    const auto res = c->resType();
    bool continuee = false;
    if(mTask.fMaxCount > 0 && mTask.fResource == res) {
        if(mTask.fType == eCartActionType::take) {
            const int max = res == eResourceType::sculpture ? 1 : 4;
            const int space = max - count;
            continuee = space > 0;
        } else { // give
            continuee = count > 0;
        }
    }
    if(r || mWaitOutside) {
        if(!mWaitOutside) c->catchUp();
        if(mTask.fMaxCount > 0) {
            finishResourceAction(mTask);
            clearTask();
        }
        if(mNoTarget) {
            mNoTarget = false;
            const bool hr = c->hasResource();
            if(!hr) { wait(1000); mTarget = nullptr; }
            else if(mWaitOutside) { waitOutside(); }
            else if(hr) {
                const auto rt = c->resType();
                const int cc = c->resCount();
                eCartTask task;
                task.fMaxCount = cc;
                task.fResource = rt;
                task.fType = eCartActionType::give;
                findTarget(task);
            }
        } else {
            int cc = c->resCount();
            if(cc > 0) {
                const auto supp = support();
                if(supp & eCartActionTypeSupport::take) {
                    const auto rt = c->resType();
                    const int r = mBuilding->add(rt, cc);
                    c->take(rt, r);
                    cc = c->resCount();
                    if(cc > 0) {
                        const int s = mBuilding->stash(rt, cc);
                        c->take(rt, s);
                        cc = c->resCount();
                    }
                }
                if(cc > 0 && supp & eCartActionTypeSupport::give) {
                    eCartTask task;
                    task.fMaxCount = cc;
                    task.fResource = c->resType();
                    task.fType = eCartActionType::give;
                    findTarget(task);
                }
            } else {
                findTarget();
            }
        }
    } else if(continuee && !mNoTarget) {
        findTarget(mTask);
    } else {
        goBack();
    }
    return true;
}

eCartActionTypeSupport eCartTransporterAction::support() const {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    return ct->support();
}

eResourceType eCartTransporterAction::supportsResource() const {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    return ct->supportsResource();
}

void eCartTransporterAction::findTarget() {
    if(!mBuilding) return;
    const auto tasks = mBuilding->cartTasks();
    const auto supp = support();
    if(supp == eCartActionTypeSupport::both) {
        findTarget(tasks);
    } else {
        std::vector<eCartTask> handled;
        const bool supportGive = supp & eCartActionTypeSupport::give;
        const bool supportTake = supp & eCartActionTypeSupport::take;
        for(const auto& t : tasks) {
            if(t.fType == eCartActionType::give && !supportGive) continue;
            if(t.fType == eCartActionType::take && !supportTake) continue;
            const auto r = t.fResource;
            const auto sr = supportsResource();
            const bool spprts = static_cast<bool>(r & sr);
            if(spprts) handled.push_back(t);
        }
        findTarget(handled);
    }
}

void eCartTransporterAction::findTarget(const eCartTask& task) {
    findTarget(std::vector<eCartTask>{task});
}

void eCartTransporterAction::findTarget(const std::vector<eCartTask>& tasks) {
    if(!mBuilding) return;
    if(tasks.empty()) return;
    const auto c = character();

    const auto buildingRect = mBuilding->tileRect();

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
            return t.fType == eCartActionType::give &&
                   static_cast<bool>(t.fResource & eResourceType::food);
        });
    const auto granaryOnly = std::make_shared<bool>(isProducer && hasGiveTasks);

    // 2. Check each tile for valid target buildings
    const auto finalTile = [this, buildingRect, bType, ttask, tasks, bx, by, granaryOnly]
                           (eThreadTile* const t) {
        // 2.1 Skip tiles without buildings
        if(!t->isUnderBuilding()) return false;

        // 2.2 Skip tiles part of cart's home building
        const bool r = eWalkableHelpers::sTileUnderBuilding(t, buildingRect);
        if(r) return false;

        bool found = false;
        const auto& ub = t->underBuilding();

        // 2.3 Producers: first pass accepts only granaries
        if(*granaryOnly && ub.type() != eBuildingType::granary) return false;

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

            if(task.fType == eCartActionType::take) {
                if(ub.resourceHas(res)) found = true;
            } else { // give
                if(ub.empties(res)) continue;
                if(ub.resourceHasSpace(res)) found = true;
            }

            if(found) {
                // 2.6 Calculate transferable amount
                int mc = (task.fType == eCartActionType::take) ?
                    std::min(ub.resourceCount(res), task.fMaxCount) :
                    std::min(ub.resourceSpaceLeft(res), task.fMaxCount);
                if(mc <= 0) continue;

                // 2.7 Valid target found
                *ttask = task;
                *bx = t->x();
                *by = t->y();
                break;
            }
        }
        return found;
    };
    const stdptr<eCartTransporterAction> tptr(this);

    const auto finishAction = std::make_shared<eCTA_findTargetFinish>(
                                  board(), this);

    const auto startSearch = [this, tptr, c, tasks, finalTile, finishAction,
                               ttask, bx, by, granaryOnly]() {
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
            mWaitOutside = false;
            mTask = *ttask;
            startResourceAction(mTask);
            c->setActionType(eCharacterActionType::walk);
        });
        a->setFindFailAction([tptr, this, granaryOnly, tasks]() {
            if(!tptr) return;
            if(*granaryOnly) {
                // No granary found — retry without granary restriction
                *granaryOnly = false;
                findTarget(tasks);
            } else {
                mNoTarget = true;
            }
        });
        a->setRemoveLastTurn(true);
        if(const auto cart = dynamic_cast<eCartTransporter*>(c)) {
            a->setMaxFindDistance(cart->maxDistance());
        }
        const auto w = getWalkable();
        a->start(finalTile, w);
        setCurrentAction(a);
    };

    startSearch();
}

void eCartTransporterAction::goBack() {
    if(!mBuilding) return;
    const auto w = getWalkable();
    eActionWithComeback::goBack(w);
    mTarget = mBuilding;
}

void eCartTransporterAction::targetResourceAction(const int bx, const int by) {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    auto& brd = ct->getBoard();
    const auto t = brd.tile(bx, by);
    const auto b = t->underBuilding();
    if(!b) return;
    const auto rb = dynamic_cast<eBuildingWithResource*>(b);
    targetResourceAction(rb);
}

void eCartTransporterAction::targetResourceAction(eBuildingWithResource* const rb) {
    if(!mBuilding) return;
    if(!rb) return;
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    const int takenGiven = targetProcessTask(rb, mTask);
    mTask.fMaxCount -= takenGiven;

    auto tasks = mBuilding->cartTasks();
    const auto supp = support();
    const bool supportGive = supp & eCartActionTypeSupport::give;
    const bool supportTake = supp & eCartActionTypeSupport::take;
    for(auto& task : tasks) {
        if(task.fType == eCartActionType::give && !supportGive) continue;
        if(task.fType == eCartActionType::take && !supportTake) continue;
        if(task.fType == eCartActionType::take) {
            if(ct->resType() == task.fResource) {
                task.fMaxCount -= ct->resCount();
            }
        }
        targetProcessTask(rb, task);
    }
    const int count = ct->resCount();
    if(count > 0) {
        const auto res = ct->resType();
        const int added = rb->add(res, count);
        ct->setResource(res, count - added);
    }
}

int eCartTransporterAction::targetProcessTask(eBuildingWithResource* const rb,
                                              const eCartTask& task) {
    if(task.fMaxCount <= 0) return 0;
    const auto c = static_cast<eCartTransporter*>(character());
    const auto res = c->resType();
    const int count = c->resCount();
    const auto tres = task.fResource;
    const int max = tres == eResourceType::sculpture ? 1 : 4;
    if(task.fType == eCartActionType::take) {
        if(count > 0 && res != tres) return 0;
        const int space = max - count;
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

void eCartTransporterAction::startResourceAction(const eCartTask& task) {
    if(!mBuilding) return;
    const auto c = static_cast<eCartTransporter*>(character());
    if(c->resCount() > 0) return;
    if(task.fMaxCount <= 0) return;
    if(task.fType == eCartActionType::take) {
        if(c->resCount() == 0) c->setResource(task.fResource, 0);
        return;
    } else { //give
        const int max = eResourceTypeHelpers::transportSize(task.fResource, board().doubleCartCapacity());
        const int mmax = std::min(max, task.fMaxCount);
        const int t = mBuilding->take(task.fResource, mmax);
        if(t <= 0) {
            clearTask();
        } else {
            c->setResource(task.fResource, t);
        }
    }
}

void eCartTransporterAction::finishResourceAction(const eCartTask& task) {
    if(!mBuilding) return disappear();
    const auto c = static_cast<eCartTransporter*>(character());
    if(c->resCount() <= 0) return disappear();
    if(task.fMaxCount <= 0) return;
    if(task.fResource != c->resType()) return;
    if(task.fType == eCartActionType::take) {
        const int crc = c->resCount();
        const int a = mBuilding->add(task.fResource, crc);
        c->setResource(task.fResource, crc - a);
        if(c->resCount() <= 0) return disappear();
    } else { //give
        return;
    }
}

void eCartTransporterAction::serialize(eSaveArchive& ar) {
    if(ar.reading()) {
        ar.readStream().readBuilding(&board(), [this](eBuilding* const b) {
            mBuilding = static_cast<eBuildingWithResource*>(b);
        });
    } else {
        ar.writeStream().writeBuilding(mBuilding);
    }

    ar.field("mTask.fMaxCount", mTask.fMaxCount);
    ar.field("mTask.fResource", mTask.fResource);
    ar.field("mTask.fType", mTask.fType);

    ar.field("mUpdateWaiting", mUpdateWaiting);
    ar.field("mNoTarget", mNoTarget);
    ar.field("mWaitOutside", mWaitOutside);

    if(ar.reading()) {
        ar.readStream().readBuilding(&board(), [this](eBuilding* const b) {
            mTarget = b;
        });
    } else {
        ar.writeStream().writeBuilding(mTarget);
    }
}

void eCartTransporterAction::read(eReadStream& src) {
    eActionWithComeback::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eCartTransporterAction::write(eWriteStream& dst) const {
    eActionWithComeback::write(dst);
    eSaveArchive ar(dst);
    const_cast<eCartTransporterAction*>(this)->serialize(ar);
}

void eCartTransporterAction::serializeJson(eJsonArchive& ar) {
    eActionWithComeback::serializeJson(ar);

    if(ar.writing()) {
        eBuilding* raw = mBuilding;
        ar.buildingRef("mBuilding", raw, board());
    } else {
        ar.buildingRef("mBuilding", [this](eBuilding* b) {
            mBuilding = dynamic_cast<eBuildingWithResource*>(b);
        }, board());
    }

    ar.field("mTask.fMaxCount", mTask.fMaxCount);
    ar.field("mTask.fResource", mTask.fResource);
    ar.field("mTask.fType", mTask.fType);
    ar.field("mUpdateWaiting", mUpdateWaiting);
    ar.field("mNoTarget", mNoTarget);
    ar.field("mWaitOutside", mWaitOutside);

    if(ar.writing()) {
        eBuilding* raw = mTarget;
        ar.buildingRef("mTarget", raw, board());
    } else {
        ar.buildingRef("mTarget", [this](eBuilding* b) {
            mTarget = b;
        }, board());
    }
}

stdsptr<eWalkableObject> eCartTransporterAction::getWalkable() const {
    if(!mBuilding) return eWalkableObject::sCreateRoadAvenue();
    const auto buildingRect = mBuilding->tileRect();
    auto w = eWalkableObject::sCreateRoadAvenue();
    w = eWalkableObject::sCreateRect(buildingRect, w);
    const auto type = mBuilding->type();
    if(type == eBuildingType::horseRanch) {
        const auto hr = static_cast<eHorseRanch*>(mBuilding.get());
        const auto e = hr->enclosure();
        const auto eRect = e->tileRect();
        w = eWalkableObject::sCreateRect(eRect, w);
    }
    return w;
}

void eCartTransporterAction::updateWaiting() {
    if(!mBuilding) return;
    const auto c = static_cast<eCartTransporter*>(character());
    const bool r = eWalkableHelpers::sTileUnderBuilding(
                       c->tile(), mBuilding);
    c->setWaiting(mWaitOutside || r);
}

void eCartTransporterAction::waitOutside() {
    if(!mBuilding) return;
    if(mWaitOutside) return;
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
    mWaitOutside = true;

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

void eCartTransporterAction::spread() {
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
    a->start(finalFunc, eWalkableObject::sCreateRoadAvenue());

    setCurrentAction(a);
}

void eCartTransporterAction::clearTask() {
    mTask.fMaxCount = 0;
    setCurrentAction(nullptr);
    mTarget = nullptr;
}

void eCartTransporterAction::disappear() {
    const auto c = character();
    const auto ct = static_cast<eCartTransporter*>(c);
    if(ct->resCount() == 0) ct->setResource(eResourceType::wine, 0);
}
