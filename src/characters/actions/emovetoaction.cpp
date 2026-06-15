#include "emovetoaction.h"

#include <cstdio>

#include "emovepathaction.h"

#include "characters/echaracter.h"
#include "engine/game-board.h"
#include "engine/thread/ethreadboard.h"
#include "engine/ethreadpool.h"
#include "path-find-task.h"
#include "ewaitaction.h"
#include "earcheraction.h"
#include "fileIO/save-archive.h"
#include "buildings/epatroltarget.h"

eMoveToAction::eMoveToAction(eCharacter* const c) :
    eComplexAction(c, eCharActionType::moveToAction) {}

void eMoveToAction::start(const eTileFinal& final,
                          stdsptr<WalkableObject> pathFindWalkable,
                          stdsptr<WalkableObject> moveWalkable,
                          const eTileGetter& endTile) {
    mSavedGoal = eSavedGoal::custom;
    mSavedTile = nullptr;
    mSavedRect = {};
    mSavedBuilding.clear();
    mSavedBuildingType = eBuildingType{};
    mSavedPathFindWalkable = pathFindWalkable;
    mSavedMoveWalkable = moveWalkable;
    startInternal(final, pathFindWalkable, moveWalkable, endTile);
}

void eMoveToAction::startInternal(const eTileFinal& final,
                                  stdsptr<WalkableObject> pathFindWalkable,
                                  stdsptr<WalkableObject> moveWalkable,
                                  const eTileGetter& endTile) {
    const auto c = character();
    const auto t = c->tile();
    auto& brd = c->getBoard();
    auto& tp = brd.threadPool();

    if(!pathFindWalkable->walkable(t)) {
        if(const auto b = t->underBuilding()) {
            pathFindWalkable = WalkableObject::sCreateRect(
                b, pathFindWalkable);
        }
    }

    if(moveWalkable && !moveWalkable->walkable(t)) {
        if(const auto b = t->underBuilding()) {
            moveWalkable = WalkableObject::sCreateRect(
                b, moveWalkable);
        }
    }

    if(!moveWalkable) {
        moveWalkable = pathFindWalkable;
    }

    const int tx = t->x();
    const int ty = t->y();

    const auto startTile = [tx, ty](eThreadBoard& board) {
        return board.tile(tx, ty);
    };

    const stdptr<eCharacter> cptr(c);
    const stdptr<eMoveToAction> tptr(this);
    const auto finishFunc = [tptr, this, cptr, moveWalkable](
                            std::vector<eOrientation> path) {
        if(!cptr) return;
        if(!tptr) return;
        const auto r = ref<eMoveToAction>();
        const auto failFunc = std::make_shared<eAA_patrolFail>(
                                  board(), this);
        const auto finishAction = std::make_shared<eAA_patrolFinish>(
                                  board(), this);

        if(mRemoveLastTurn && !path.empty()) {
            path.erase(path.begin());
        }
        mPathLength = path.size();
        if(mFoundAction) {
            mFoundAction();
            mFoundAction = nullptr;
        }
        if(path.empty()) {
            finishAction->call();
            return;
        }
        const auto a  = e::make_shared<eMovePathAction>(
                            cptr, path, moveWalkable);
        a->setFailAction(failFunc);
        a->setFinishAction(finishAction);
        a->setObsticleHandler(mObstHandler);
        a->setMaxDistance(mMaxWalkDistance);
        setCurrentAction(a);
    };

    const auto findFailFunc = [tptr, this]() {
        if(!tptr) return;
        const auto r = ref<eMoveToAction>();
        if(mFindFailAction) {
            mFindFailAction();
            mFindFailAction = nullptr;
        }
        setState(eCharacterActionState::failed);
    };

    const auto cid = onCityId();
    const auto tileBRect = brd.boardCityTileBRect(cid);
    const auto pft = new PathFindTask(cid, tileBRect,
                                       startTile,
                                       pathFindWalkable,
                                       final, finishFunc,
                                       findFailFunc, mDiagonalOnly,
                                       mMaxFindDistance,
                                       mDistance,
                                       endTile);
    pft->setRelevance(mRelevance);
    tp.queueTask(pft);

    if(mWait) {
        const auto w = e::make_shared<eWaitAction>(c);
        setCurrentAction(w);
    }
}

void eMoveToAction::start(eTile* const final,
                          const stdsptr<WalkableObject>& pathFindWalkable,
                          const stdsptr<WalkableObject>& moveWalkable,
                          const eTileGetter& endTile) {
    mSavedGoal = eSavedGoal::tile;
    mSavedTile = final;
    mSavedRect = {};
    mSavedBuilding.clear();
    mSavedBuildingType = eBuildingType{};
    mSavedPathFindWalkable = pathFindWalkable;
    mSavedMoveWalkable = moveWalkable;
    const int tx = final->x();
    const int ty = final->y();
    const auto finalFunc = [tx, ty](eTileBase* const t) {
        return t->x() == tx && t->y() == ty;
    };
    startInternal(finalFunc, pathFindWalkable, moveWalkable, endTile);
}

void eMoveToAction::start(const SDL_Rect& rect,
                          stdsptr<WalkableObject> pathFindWalkable,
                          stdsptr<WalkableObject> moveWalkable,
                          const eTileGetter& endTile) {
    mSavedGoal = eSavedGoal::rect;
    mSavedTile = nullptr;
    mSavedRect = rect;
    mSavedBuilding.clear();
    mSavedBuildingType = eBuildingType{};
    mSavedPathFindWalkable = pathFindWalkable;
    mSavedMoveWalkable = moveWalkable;
    const auto finalFunc = [rect](eTileBase* const t) {
        const SDL_Point p{t->x(), t->y()};
        return SDL_PointInRect(&p, &rect);
    };
    pathFindWalkable = WalkableObject::sCreateRect(
                              rect, pathFindWalkable);
    if(moveWalkable) {
        moveWalkable = WalkableObject::sCreateRect(
                                  rect, moveWalkable);
    }
    startInternal(finalFunc, pathFindWalkable, moveWalkable, endTile);
}

void eMoveToAction::start(eBuilding* const final,
                          const stdsptr<WalkableObject>& pathFindWalkable,
                          const stdsptr<WalkableObject>& moveWalkable,
                          const eTileGetter& endTile) {
    mSavedGoal = eSavedGoal::building;
    mSavedTile = nullptr;
    mSavedRect = {};
    mSavedBuilding = final;
    mSavedBuildingType = eBuildingType{};
    mSavedPathFindWalkable = pathFindWalkable;
    mSavedMoveWalkable = moveWalkable;
    const auto rect = final->tileRect();
    const auto finalFunc = [rect](eTileBase* const t) {
        const SDL_Point p{t->x(), t->y()};
        return SDL_PointInRect(&p, &rect);
    };
    auto pfw = WalkableObject::sCreateRect(rect, pathFindWalkable);
    auto mw = moveWalkable;
    if(mw) mw = WalkableObject::sCreateRect(rect, mw);
    startInternal(finalFunc, pfw, mw, endTile);
}

void eMoveToAction::start(const eBuildingType final,
                          const stdsptr<WalkableObject>& pathFindWalkable,
                          const stdsptr<WalkableObject>& moveWalkable,
                          const eTileGetter& endTile) {
    mSavedGoal = eSavedGoal::buildingType;
    mSavedTile = nullptr;
    mSavedRect = {};
    mSavedBuilding.clear();
    mSavedBuildingType = final;
    mSavedPathFindWalkable = pathFindWalkable;
    mSavedMoveWalkable = moveWalkable;
    const auto finalFunc = [final](eTileBase* const t) {
        return t->underBuildingType() == final;
    };
    startInternal(finalFunc, pathFindWalkable, moveWalkable, endTile);
}

void eMoveToAction::setObsticleHandler(const stdsptr<eObsticleHandler>& oh) {
    mObstHandler = oh;
}

void eMoveToAction::setTileDistance(const eTileDistance& dist) {
    mDistance = dist;
}

void eMoveToAction::serializeFields(SaveArchive& ar) {
    eComplexAction::serializeFields(ar);
    ar.field("stateRelevance", mRelevance, eStateRelevance::all);
    ar.field("removeLastTurn", mRemoveLastTurn, false);
    ar.field("waitForPath", mWait, true);
    ar.field("diagonalOnly", mDiagonalOnly, false);
    ar.field("maxFindDistance", mMaxFindDistance, 10000);
    ar.field("maxWalkDistance", mMaxWalkDistance, 10000);
    ar.field("pathLength", mPathLength, 0);
    ar.field("savedGoal", mSavedGoal, eSavedGoal::none);
    if(mSavedGoal == eSavedGoal::tile) {
        ar.tileField("goalTile", board(), mSavedTile);
    } else if(ar.reading()) {
        mSavedTile = nullptr;
    }
    if(mSavedGoal == eSavedGoal::rect) {
        ar.field("goalRect", mSavedRect, SDL_Rect{});
    } else if(ar.reading()) {
        mSavedRect = {};
    }
    if(mSavedGoal == eSavedGoal::building) {
        ar.buildingField("goalBuilding", &board(), mSavedBuilding);
    } else if(ar.reading()) {
        mSavedBuilding.clear();
    }
    if(mSavedGoal == eSavedGoal::buildingType) {
        ar.field("goalBuildingType", mSavedBuildingType, eBuildingType{});
    } else if(ar.reading()) {
        mSavedBuildingType = eBuildingType{};
    }
    bool hasPathFindWalkable = mSavedPathFindWalkable != nullptr;
    if(mSavedGoal != eSavedGoal::none) {
        ar.field("pathFindWalkable.has", hasPathFindWalkable, false);
        if(hasPathFindWalkable) {
            ar.walkableField("pathFindWalkable", mSavedPathFindWalkable);
        } else if(ar.reading()) {
            mSavedPathFindWalkable = nullptr;
        }
    } else if(ar.reading()) {
        mSavedPathFindWalkable = nullptr;
    }
    bool hasMoveWalkable = mSavedMoveWalkable != nullptr;
    if(mSavedGoal != eSavedGoal::none) {
        ar.field("moveWalkable.has", hasMoveWalkable, false);
        if(hasMoveWalkable) {
            ar.walkableField("moveWalkable", mSavedMoveWalkable);
        } else if(ar.reading()) {
            mSavedMoveWalkable = nullptr;
        }
    } else if(ar.reading()) {
        mSavedMoveWalkable = nullptr;
    }
}

void eMoveToAction::resumeFromSavedState() {
    eComplexAction::resumeFromSavedState();
    if(state() != eCharacterActionState::running) return;
    if(currentAction()) return;
    if(restartSavedGoal()) return;
    if(mSavedGoal != eSavedGoal::none) {
        printf("[saveLoad] moveToAction saved goal unsupported; failing. action=%p char=%p goal=%d\n",
               (void*)this, (void*)character(), (int)mSavedGoal);
    } else {
        printf("[saveLoad] moveToAction has no saved goal; failing. action=%p char=%p\n",
               (void*)this, (void*)character());
    }
    setState(eCharacterActionState::failed);
}

bool eMoveToAction::restartSavedGoal() {
    if(!mSavedPathFindWalkable) {
        mSavedPathFindWalkable = WalkableObject::sCreateDefault();
    }
    switch(mSavedGoal) {
    case eSavedGoal::tile:
        if(!mSavedTile) return false;
        start(mSavedTile, mSavedPathFindWalkable, mSavedMoveWalkable);
        return true;
    case eSavedGoal::rect:
        if(SDL_RectEmpty(&mSavedRect)) return false;
        start(mSavedRect, mSavedPathFindWalkable, mSavedMoveWalkable);
        return true;
    case eSavedGoal::building:
        if(!mSavedBuilding) return false;
        start(mSavedBuilding.get(), mSavedPathFindWalkable, mSavedMoveWalkable);
        return true;
    case eSavedGoal::buildingType:
        start(mSavedBuildingType, mSavedPathFindWalkable, mSavedMoveWalkable);
        return true;
    case eSavedGoal::none:
    case eSavedGoal::custom:
        if(const auto f = dynamic_cast<ePT_spawnGetActorFinish*>(finishAction())) {
            const auto t = f->target();
            if(!t) return false;
            start(t, WalkableObject::sCreateRoadAvenue());
            return true;
        }
        return false;
    }
    return false;
}
