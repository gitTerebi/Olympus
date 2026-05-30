#include "ecollectresourceaction.h"

#include "characters/eresourcecollector.h"
#include "buildings/eresourcecollectbuilding.h"
#include "emovetoaction.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

eCollectResourceAction::eCollectResourceAction(
        eResourceCollectBuildingBase* const b,
        eCharacter* const c,
        const stdsptr<eHasResourceObject>& hr) :
    eActionWithComeback(c, eCharActionType::collectResourceAction),
    mHasResource(hr),
    mBuilding(b) {}

eCollectResourceAction::eCollectResourceAction(eCharacter* const c) :
    eCollectResourceAction(nullptr, c, nullptr) {}

bool eCollectResourceAction::decide() {
    const bool r = eActionWithComeback::decide();
    if(r) return r;

    if(mDisabled) {
        goBackDecision();
        return true;
    }

    const auto c = character();
    const auto cc = static_cast<eResourceCollectorBase*>(c);

    const auto t = c->tile();
    const int coll = cc->collected();

    const bool inside = eWalkableHelpers::sTileUnderBuilding(t, mBuilding) ||
                        t == startTile();

    if(coll > 0) {
        if(inside) {
            if(mAddResource) mBuilding->addRaw();
            cc->incCollected(-coll);
            if(mFinishOnce) {
                setState(eCharacterActionState::finished);
                return true;
            } else {
                const auto ct = c->type();
                if(ct == eCharacterType::urchinGatherer) {
                    c->setActionType(eCharacterActionType::deposit);
                }
                waitDecision();
            }
        } else {
            goBackDecision();
        }
    } else {
        const auto& brd = c->getBoard();
        eTile* tt = nullptr;
        if(mGetAtTile) {
            const auto t = c->tile();
            if(mHasResource->has(t) && !t->busy()) {
                tt = t;
            }
        } else {
            for(int i = -1; i < 2; i++) {
                for(int j = -1; j < 2; j++) {
                    const auto ttt = brd.tile(t->x() + i, t->y() + j);
                    if(!ttt) continue;
                    if(mHasResource->has(ttt) && !ttt->busy()) {
                        tt = ttt;
                        break;
                    }
                }
            }
        }
        if(tt) collect(tt);
        else if(inside) {
            const int res = mBuilding->resource();
            if(res >= mBuilding->maxResource() ||
               !mBuilding->enabled()) {
                waitDecision();
            } else {
                if(mNoTarget) {
                    mNoTarget = false;
                    waitDecision();
                } else {
                    findResourceDecision();
                }
            }
        } else if(mNoTarget) {
            mNoTarget = false;
            goBackDecision();
        } else {
            findResourceDecision();
        }
    }

    return true;
}

void eCollectResourceAction::setCollectedAction(const eTileActionType a) {
    mCollectedAction = a;
}

void eCollectResourceAction::callCollectedAction(eTile* const tile) const {
    eResourceType marbleType = eResourceType::marble;
    switch(mCollectedAction) {
    case eTileActionType::none:
        break;
    case eTileActionType::blackMasonry:
        marbleType = eResourceType::blackMarble;
        [[fallthrough]];
    case eTileActionType::masonry: {
        auto& board = eCollectResourceAction::board();
        const auto r = e::make_shared<eCartTransporter>(board);
        const auto cid = tile->cityId();
        r->setAtlantean(board.atlantean(cid));
        r->setBothCityIds(cid);
        r->changeTile(tile);
        r->setBigTrailer(true);
        r->setResource(marbleType, 1);

        const auto finish = std::make_shared<eCRA_callCollectedActionFinish>(
                                board, mBuilding);
        const auto a = e::make_shared<eMoveToAction>(r.get());
        a->setStateRelevance(eStateRelevance::terrain |
                             eStateRelevance::buildings);
        a->setFinishAction(finish);
        a->start(mBuilding);
        r->setAction(a);
        r->setActionType(eCharacterActionType::walk);
    } break;
    }
}

void eCollectResourceAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.hasResourceField("hasResource", mHasResource);
    ar.buildingAsField("building", &board(), mBuilding);
    ar.field("collectedAction", mCollectedAction);
    ar.walkableField("walkable", mWalkable);
    ar.field("disabled", mDisabled);
    ar.field("waitTime", mWaitTime);
    ar.field("finishOnce", mFinishOnce);
    ar.field("addResource", mAddResource);
    ar.field("getAtTile", mGetAtTile);
    ar.field("noTarget", mNoTarget);
    ar.field("stage", mStage);
    ar.tileField("targetTile", board(), mTargetTile);
}

void eCollectResourceAction::resumeFromSavedState() {
    switch(mStage) {
    case eCollectResourceActionStage::idle:
        eActionWithComeback::resumeFromSavedState();
        break;
    case eCollectResourceActionStage::findingResource:
        findResourceDecision();
        break;
    case eCollectResourceActionStage::collecting:
        if(mTargetTile) collect(mTargetTile);
        else eActionWithComeback::resumeFromSavedState();
        break;
    case eCollectResourceActionStage::goingBack:
        goBackDecision();
        break;
    case eCollectResourceActionStage::waiting:
        waitDecision();
        break;
    }
}

void eCollectResourceAction::finishCollecting(eTile* const tile) {
    if(!tile) return;
    tile->setBusy(false);
    if(mTargetTile == tile) mTargetTile = nullptr;
    mStage = eCollectResourceActionStage::idle;
    callCollectedAction(tile);
}

bool eCollectResourceAction::findResourceDecision() {
    const auto c = character();
    mStage = eCollectResourceActionStage::findingResource;

    const stdptr<eCollectResourceAction> tptr(this);

    const auto tileWalkable = eWalkableObject::sCreateHasResource(
                                  mHasResource, mWalkable);

    const auto hr = mHasResource;
    const auto hubr = [hr](eTileBase* const t) {
        return !t->busy() && hr->has(t);
    };

    const auto a = e::make_shared<eMoveToAction>(c);
    const auto type = hr->type();
    if(type == eHasResourceObjectType::forest) {
        a->setStateRelevance(eStateRelevance::forests |
                             eStateRelevance::buildings |
                             eStateRelevance::terrain);
    } else {
        a->setStateRelevance(eStateRelevance::resources |
                             eStateRelevance::buildings |
                             eStateRelevance::terrain);
    }

    const stdptr<eCharacter> cptr(c);
    a->setFoundAction([cptr, tptr, this]() {
        if(cptr) {
            cptr->setActionType(eCharacterActionType::walk);
        }
        if(tptr) {
            mNoTarget = false;
            mBuilding->setNoTarget(false);
        }
    });

    const auto findFailFunc = [tptr, this]() {
        if(tptr) {
            mNoTarget = true;
            mBuilding->setNoTarget(true);
        }
    };
    a->setFindFailAction(findFailFunc);
    a->setRemoveLastTurn(!mGetAtTile);

    a->start(hubr, tileWalkable);
    setCurrentAction(a);

    return false;
}

bool eCollectResourceAction::collect(eTile* const tile) {
    mStage = eCollectResourceActionStage::collecting;
    mTargetTile = tile;
    tile->setBusy(true);
    const auto c = character();
    c->setActionType(eCharacterActionType::collect);

    const auto failAction = std::make_shared<eCRA_collectFail>(
                                board(), tile);
    const auto finishAction = std::make_shared<eCRA_collectFinish>(
                                  board(), this, tile);

    eTranformFunc func = eTranformFunc::none;
    const auto terr = tile->terrain();
    if(terr == eTerrain::forest) {
        func = eTranformFunc::tree;
    } else if(terr == eTerrain::marble) {
        func = eTranformFunc::marble;
    } else if(terr == eTerrain::blackMarble) {
        func = eTranformFunc::blackMarble;
    }
    const auto a = e::make_shared<eCollectAction>(c, func);
    a->setFailAction(failAction);
    a->setFinishAction(finishAction);
    a->setDeleteFailAction(failAction);
    setCurrentAction(a);
    return false;
}

void eCollectResourceAction::goBackDecision() {
    mStage = eCollectResourceActionStage::goingBack;
    const auto c = character();
    const auto cc = static_cast<eResourceCollectorBase*>(c);

    if(cc->collected()) {
        c->setActionType(eCharacterActionType::carry);
    } else {
        c->setActionType(eCharacterActionType::walk);
    }
    const auto tileWalkable = eWalkableObject::sCreateHasResource(
                                  mHasResource, mWalkable);
    goBack(mBuilding, tileWalkable);
}

void eCollectResourceAction::waitDecision() {
    mStage = eCollectResourceActionStage::waiting;
    const auto c = character();
    c->setActionType(eCharacterActionType::stand);
    wait(mWaitTime);
}

