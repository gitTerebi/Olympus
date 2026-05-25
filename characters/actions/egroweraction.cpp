#include "egroweraction.h"

#include "characters/actions/emovetoaction.h"
#include "characters/actions/ewaitaction.h"
#include "buildings/eresourcebuilding.h"
#include "engine/e-game-board.h"
#include "enumbers.h"
#include "fileIO/esavearchive.h"

eGrowerAction::eGrowerAction(const eGrowerType type,
                             eGrowersLodge* const lodge,
                             eCharacter* const c,
                             const eGrowerActionMode mode) :
    eActionWithComeback(c, eCharActionType::growerAction),
    mType(type), mGrower(static_cast<eGrower*>(c)),
    mLodge(lodge), mMode(mode) {}

eGrowerAction::eGrowerAction(eCharacter* const c) :
    eGrowerAction(eGrowerType::grapesAndOlives, nullptr, c) {}

static bool isOliveHarvestMonth(const eMonth m) {
    return m == eMonth::january ||
           m == eMonth::february ||
           m == eMonth::march;
}

static bool isGrapeHarvestMonth(const eMonth m) {
    return m == eMonth::october ||
           m == eMonth::november ||
           m == eMonth::december;
}

static bool isOrangeHarvestMonth(const eMonth m) {
    return m == eMonth::january ||
           m == eMonth::february ||
           m == eMonth::march;
}

bool hasResource(eThreadTile* const tile, const eGrowerType gt,
                 const bool grapesDisabled, const bool olivesDisabled,
                 const eGrowerActionMode mode,
                 const bool canHarvestOlives,
                 const bool canHarvestGrapes,
                 const bool canHarvestOranges) {
    if(mode != eGrowerActionMode::oliveGroomer && eRand::rand() % 2) return false;
    const auto ub = tile->underBuildingType();
    bool r;
    switch(gt) {
    case eGrowerType::grapesAndOlives:
        r = (ub == eBuildingType::vine && !grapesDisabled) ||
            (ub == eBuildingType::oliveTree && !olivesDisabled);
        break;
    case eGrowerType::oranges:
        r = ub == eBuildingType::orangeTree;
        break;
    }

    if(!r) return false;
    if(tile->busy()) return false;
    const auto& b = tile->underBuilding();
    const bool hasRes = b.treeResource() > 0;
    if(mode == eGrowerActionMode::oliveHarvester) {
        return ub == eBuildingType::oliveTree && hasRes && canHarvestOlives;
    }
    if(mode == eGrowerActionMode::oliveGroomer) {
        const bool plant = ub == eBuildingType::oliveTree ||
                           ub == eBuildingType::vine;
        return plant && !hasRes && !b.workedOn();
    }
    if(b.workedOn()) return false;
    if(hasRes) {
        if(ub == eBuildingType::vine && !canHarvestGrapes) return false;
        if(ub == eBuildingType::oliveTree && !canHarvestOlives) return false;
        if(ub == eBuildingType::orangeTree && !canHarvestOranges) return false;
    }
    return true;
}

enum class eCollectType {
    groom,
    collect
};

eResourceBuilding* tryToCollect(eTile* const tile,
                                const eGrowerType type,
                                const eGrowerActionMode mode,
                                const bool canHarvestOlives,
                                const bool canHarvestGrapes,
                                const bool canHarvestOranges,
                                eCollectType& collType) {
    if(tile->busy()) return nullptr;
    const auto b = tile->underBuilding();
    if(b) {
        const auto s = static_cast<eResourceBuilding*>(b);
        const auto t = b->type();
        switch(type) {
        case eGrowerType::grapesAndOlives:
            if(t != eBuildingType::vine &&
               t != eBuildingType::oliveTree) {
                return nullptr;
            }
           break;
        case eGrowerType::oranges:
            if(t != eBuildingType::orangeTree) {
                return nullptr;
            }
           break;
        }

        const bool hasRes = s->resource() > 0;
        if(mode == eGrowerActionMode::oliveHarvester) {
            if(t != eBuildingType::oliveTree ||
               !hasRes ||
               !canHarvestOlives) return nullptr;
        } else if(mode == eGrowerActionMode::oliveGroomer) {
            const bool plant = t == eBuildingType::oliveTree ||
                               t == eBuildingType::vine;
            if(!plant || hasRes || s->workedOn()) {
                return nullptr;
            }
        } else if(s->workedOn()) {
            return nullptr;
        }

        if(hasRes && mode == eGrowerActionMode::normal) {
            if(t == eBuildingType::vine && !canHarvestGrapes) return nullptr;
            if(t == eBuildingType::oliveTree && !canHarvestOlives) return nullptr;
            if(t == eBuildingType::orangeTree && !canHarvestOranges) return nullptr;
        }

        if(hasRes) {
            collType = eCollectType::collect;
        } else {
            collType = eCollectType::groom;
        }
        return s;
    }
    return nullptr;
}

bool eGrowerAction::decide() {
    const bool r = eActionWithComeback::decide();
    if(r) return r;

    const auto t = mGrower->tile();

    const int grapes = mGrower->grapes();
    const int olives = mGrower->olives();
    const int oranges = mGrower->oranges();

    const bool inLodge = eWalkableHelpers::sTileUnderBuilding(t, mLodge);
    const auto curMonth = board().date().month();
    const bool canHarvestOlives = isOliveHarvestMonth(curMonth);
    const bool canHarvestGrapes = isGrapeHarvestMonth(curMonth);
    const bool canHarvestOranges = isOrangeHarvestMonth(curMonth);

    if(grapes > 0 || olives > 0 || oranges > 0) {
        if(inLodge) {
            mLodge->growerDelivered(eResourceType::grapes, grapes);
            mLodge->growerDelivered(eResourceType::olives, olives);
            mLodge->growerDelivered(eResourceType::oranges, oranges);

            mGrower->incGrapes(-grapes);
            mGrower->incOlives(-olives);
            mGrower->incOranges(-oranges);

            if(mFinishOnce) {
                setState(eCharacterActionState::finished);
                return true;
            } else {
                waitDecision();
            }
        } else {
            goBackDecision();
        }
    } else {
        eCollectType collType;
        if(const auto a = tryToCollect(t, mType, mMode,
                                       canHarvestOlives,
                                       canHarvestGrapes,
                                       canHarvestOranges, collType)) {
            workOnDecision(t);
        } else if(inLodge) {
            int space = 0;
            switch(mType) {
            case eGrowerType::grapesAndOlives:
                space = mLodge->spaceLeft(eResourceType::grapes);
                break;
            case eGrowerType::oranges:
                space = mLodge->spaceLeft(eResourceType::oranges);
                break;
            }

            const bool oliveGroomingDone =
                mMode == eGrowerActionMode::oliveGroomer &&
                mOliveGroomsThisMonth >= 4;
            if(oliveGroomingDone) {
                setState(eCharacterActionState::finished);
                return true;
            }
            const bool spaceBlocked =
                mMode != eGrowerActionMode::oliveGroomer && space <= 0;
            if(spaceBlocked || !mLodge->enabled()) {
                waitDecision();
            } else {
                if(mNoResource) {
                    mNoResource = false;
                    waitDecision();
                } else {
                    findResourceDecision();
                }
            }
        } else if(mNoResource) {
            mNoResource = false;
            goBackDecision();
        } else {
            if(mMode == eGrowerActionMode::oliveGroomer &&
               mOliveGroomsThisMonth >= 4) {
                setFinishOnComeback(true);
                goBackDecision();
                return true;
            }
            if(mMode != eGrowerActionMode::oliveGroomer &&
               mGroomed > eNumbers::sGrowerMaxGroom) {
                mGroomed = 0;
                goBackDecision();
            } else {
                findResourceDecision();
            }
        }
    }
    return true;
}

void eGrowerAction::increment(const int by) {
    if(mStage == eGrowerActionStage::working && mWorkRemaining > 0) {
        if(mWorkRemaining <= by) {
            mWorkRemaining = 0;
            setCurrentAction(nullptr);
            finishWorkOn(mTargetTile, mTargetBuildingType);
            return;
        }
        mWorkRemaining -= by;
    } else if(mStage == eGrowerActionStage::waiting && mWaitRemaining > 0) {
        mWaitRemaining -= by;
        if(mWaitRemaining < 0) mWaitRemaining = 0;
    }
    eActionWithComeback::increment(by);
}

void eGrowerAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.field("growerType", mType);
    ar.characterAsField("grower", &board(), mGrower);
    ar.buildingAsField("lodge", &board(), mLodge);
    ar.field("finishOnce", mFinishOnce);
    ar.field("groomed", mGroomed);
    ar.field("noResource", mNoResource);
    ar.field("mode", mMode, eGrowerActionMode::normal);
    ar.field("oliveGroomsThisMonth", mOliveGroomsThisMonth, 0);
    ar.field("oliveGroomMonth", mOliveGroomMonth, -1);
    ar.field("stage", mStage, eGrowerActionStage::idle);
    ar.field("waitRemaining", mWaitRemaining, 0);
    ar.field("workRemaining", mWorkRemaining, 0);
    ar.tileField("targetTile", board(), mTargetTile);
    ar.field("targetBuildingType", mTargetBuildingType, eBuildingType::none);
}

void eGrowerAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eGrowerAction::rebuildCurrentStage() {
    switch(mStage) {
    case eGrowerActionStage::findingResource:
        return static_cast<void>(findResourceDecision());
    case eGrowerActionStage::working:
        if(mTargetTile) mTargetTile->setBusy(false);
        if(mTargetTile && mTargetTile->underBuilding()) {
            return workOnDecision(mTargetTile);
        }
        releaseWorkTile();
        mStage = eGrowerActionStage::idle;
        return static_cast<void>(decide());
    case eGrowerActionStage::goingBack:
        return goBackDecision();
    case eGrowerActionStage::waiting:
        if(mWaitRemaining > 0) {
            wait(mWaitRemaining);
        } else {
            mStage = eGrowerActionStage::idle;
            decide();
        }
        return;
    case eGrowerActionStage::idle:
        return eActionWithComeback::resumeFromSavedState();
    }
}

bool eGrowerAction::findResourceDecision() {
    mStage = eGrowerActionStage::findingResource;
    const stdptr<eGrowerAction> tptr(this);

    const auto gt = mType;
    auto& board = this->board();
    const auto cid = cityId();
    const bool gd = board.isShutDown(cid, eResourceType::grapes);
    const bool od = board.isShutDown(cid, eResourceType::olives);
    const auto mode = mMode;
    const auto curMonth = board.date().month();
    const bool canHarvestOlives = isOliveHarvestMonth(curMonth);
    const bool canHarvestGrapes = isGrapeHarvestMonth(curMonth);
    const bool canHarvestOranges = isOrangeHarvestMonth(curMonth);
    const auto hha = [gt, gd, od, mode, canHarvestOlives,
                      canHarvestGrapes, canHarvestOranges](eThreadTile* const tile) {
        return hasResource(tile, gt, gd, od, mode, canHarvestOlives,
                           canHarvestGrapes, canHarvestOranges);
    };

    const auto a = e::make_shared<eMoveToAction>(mGrower);
    a->setStateRelevance(eStateRelevance::treesAndVines |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFoundAction([tptr, this]() {
        if(!tptr) return;
        if(mLodge) mLodge->setNoTarget(false);
        if(!mGrower) return;
        mGrower->setActionType(eCharacterActionType::walk);
    });
    const auto findFailFunc = [tptr, this]() {
        if(tptr) {
            mNoResource = true;
            if(mLodge) mLodge->setNoTarget(true);
        }
    };
    a->setFindFailAction(findFailFunc);
    a->setMaxFindDistance(eNumbers::sGrowerMaxDistance);
    a->start(hha);
    setCurrentAction(a);
    return true;
}

void eGrowerAction::workOnDecision(eTile* const tile) {
    const auto type = tile->underBuildingType();
    switch(mType) {
    case eGrowerType::grapesAndOlives:
        if(type != eBuildingType::vine &&
           type != eBuildingType::oliveTree) return;
        break;
    case eGrowerType::oranges:
        if(type != eBuildingType::orangeTree) return;
        break;
    }
    mStage = eGrowerActionStage::working;
    mTargetTile = tile;
    mTargetBuildingType = type;
    if(mWorkRemaining <= 0) {
        mWorkRemaining = eNumbers::sGrowerWorkTime;
    }
    tile->setBusy(true);
    const auto b = tile->underBuilding();
    const auto bb = dynamic_cast<eResourceBuilding*>(b);
    if(bb->resource() > 0) {
        if(type == eBuildingType::vine) {
            mGrower->setActionType(eCharacterActionType::collectGrapes);
        } else if(type == eBuildingType::oliveTree) {
            mGrower->setActionType(eCharacterActionType::collectOlives);
        } else if(type == eBuildingType::orangeTree) {
            mGrower->setActionType(eCharacterActionType::collectOranges);
        }
    } else {
        if(type == eBuildingType::vine) {
            mGrower->setActionType(eCharacterActionType::workOnGrapes);
        } else if(type == eBuildingType::oliveTree) {
            mGrower->setActionType(eCharacterActionType::workOnOlives);
        } else if(type == eBuildingType::orangeTree) {
            mGrower->setActionType(eCharacterActionType::workOnOranges);
        }
    }

    const auto w = e::make_shared<eWaitAction>(mGrower);
    const auto fail = std::make_shared<eGRA_workOnDecisionDeleteFail>(
                            board(), tile);
    w->setFailAction(fail);
    const auto deleteFail = std::make_shared<eGRA_workOnDecisionDeleteFail>(
                            board(), tile);
    w->setDeleteFailAction(deleteFail);
    w->setTime(mWorkRemaining);
    setCurrentAction(w);
}

void eGrowerAction::finishWorkOn(eTile* const tile, const eBuildingType type) {
    if(!tile) {
        releaseWorkTile();
        mStage = eGrowerActionStage::idle;
        return;
    }
    tile->setBusy(false);
    if(const auto b = tile->underBuilding()) {
        if(const auto bb = dynamic_cast<eResourceBuilding*>(b)) {
            const bool collecting = bb->resource() > 0;
            if(!collecting) {
                bb->workOn();
            }
            const int ripeStage = bb->ripe();
            const int units = ripeStage * 20;
            const int took = bb->takeResource(1);
            if(took > 0) {
                if(type == eBuildingType::vine) {
                    mGrower->incGrapes(units);
                } else if(type == eBuildingType::oliveTree) {
                    mGrower->incOlives(units);
                } else if(type == eBuildingType::orangeTree) {
                    mGrower->incOranges(units);
                }
                mGroomed += 5;
            } else if(mMode == eGrowerActionMode::oliveGroomer &&
                      (type == eBuildingType::oliveTree ||
                       type == eBuildingType::vine)) {
                mOliveGroomsThisMonth++;
            }
        }
    }
    mGroomed++;
    releaseWorkTile();
    mStage = eGrowerActionStage::idle;
}

void eGrowerAction::releaseWorkTile() {
    if(mTargetTile) mTargetTile->setBusy(false);
    mTargetTile = nullptr;
    mTargetBuildingType = eBuildingType::none;
    mWorkRemaining = 0;
}

void eGrowerAction::goBackDecision() {
    mStage = eGrowerActionStage::goingBack;
    mGrower->setActionType(eCharacterActionType::carry);
    goBack(mLodge, eWalkableObject::sCreateDefault());
}

void eGrowerAction::waitDecision() {
    mStage = eGrowerActionStage::waiting;
    if(mWaitRemaining <= 0) {
        mWaitRemaining = eNumbers::sGrowerSpawnWaitTime;
    }
    wait(mWaitRemaining);
}
