#include "eartisanaction.h"

#include "characters/actions/emovetoaction.h"
#include "characters/actions/ewaitaction.h"
#include "characters/actions/ebuildaction.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"
#include "enumbers.h"

eArtisanAction::eArtisanAction(eCharacter* const c,
                               eArtisansGuild* const guild) :
    eActionWithComeback(c, eCharActionType::artisanAction),
    mGuild(guild) {
    setFinishOnComeback(true);
}

eArtisanAction::eArtisanAction(eCharacter* const c) :
    eArtisanAction(c, nullptr) {}

bool hasTarget(eThreadTile* const tile) {
    const auto& b = tile->underBuilding();
    return b.resourcesAvailable() && !b.workedOn() && !tile->busy();
}

bool eArtisanAction::decide() {
    const bool r = eActionWithComeback::decide();
    if(r) return r;

    const auto c = character();
    const auto t = c->tile();

    const bool inGuild = eWalkableHelpers::sTileUnderBuilding(t, mGuild);

    if(inGuild) {
        if(mNoTarget) {
            mNoTarget = false;
            mStage = eArtisanActionStage::waiting;
            mWaitRemaining = 5000;
            wait(mWaitRemaining);
        } else {
            findTargetDecision();
        }
    } else if(mNoTarget) {
        mNoTarget = false;
        goBackDecision();
    } else {
        const auto& brd = board();
        eTile* tt = nullptr;
        for(int i : {0, 1, -1}) {
            for(int j : {0, 1, -1}) {
                const auto ttt = brd.tile(t->x() + i, t->y() + j);
                if(!ttt) continue;
                const auto b = ttt->underBuilding();
                if(const auto bb = dynamic_cast<eSanctBuilding*>(b)) {
                    if(bb->finished()) continue;
                    if(bb->workedOn()) continue;
                    if(!bb->resourcesAvailable()) continue;
                    const auto type = b->type();
                    const bool pyramid = eBuilding::sPyramidBuilding(type);
                    if(pyramid && (i != 0 || j != 0)) continue;
                    tt = ttt;
                    break;
                }
            }
        }
        if(tt) {
            workOnDecision(tt);
        } else {
            findTargetDecision();
        }
    }

    return true;
}

void eArtisanAction::increment(const int by) {
    if(mStage == eArtisanActionStage::working && mWorkRemaining > 0) {
        if(mWorkRemaining <= by) {
            mWorkRemaining = 0;
            setCurrentAction(nullptr);
            finishWork();
            return;
        }
        mWorkRemaining -= by;
    } else if(mStage == eArtisanActionStage::waiting && mWaitRemaining > 0) {
        mWaitRemaining -= by;
        if(mWaitRemaining < 0) mWaitRemaining = 0;
    }
    eActionWithComeback::increment(by);
}

void eArtisanAction::serializeFields(eSaveArchive& ar) {
    eActionWithComeback::serializeFields(ar);
    ar.buildingAsField("guild", &board(), mGuild);
    ar.field("noTarget", mNoTarget);
    ar.field("stage", mStage, eArtisanActionStage::idle);
    ar.field("waitRemaining", mWaitRemaining, 0);
    ar.field("workRemaining", mWorkRemaining, 0);
    ar.tileField("targetTile", board(), mTargetTile);
    ar.buildingAsField("targetBuilding", &board(), mTargetBuilding);
}

void eArtisanAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eArtisanAction::rebuildCurrentStage() {
    switch(mStage) {
    case eArtisanActionStage::findingTarget:
        return static_cast<void>(findTargetDecision());
    case eArtisanActionStage::working:
        if(mTargetBuilding) mTargetBuilding->setWorkedOn(false);
        if(mTargetTile && mTargetBuilding &&
           !mTargetBuilding->finished() &&
           mTargetBuilding->resourcesAvailable()) {
            return workOnDecision(mTargetTile);
        }
        releaseWorkTarget();
        mStage = eArtisanActionStage::idle;
        return static_cast<void>(decide());
    case eArtisanActionStage::goingBack:
        return goBackDecision();
    case eArtisanActionStage::waiting:
        if(mWaitRemaining > 0) {
            wait(mWaitRemaining);
        } else {
            mStage = eArtisanActionStage::idle;
            decide();
        }
        return;
    case eArtisanActionStage::idle:
        return eActionWithComeback::resumeFromSavedState();
    }
}

bool eArtisanAction::findTargetDecision() {
    mStage = eArtisanActionStage::findingTarget;
    const stdptr<eArtisanAction> tptr(this);

    const auto c = character();
    const auto a = e::make_shared<eMoveToAction>(c);
    const auto aptr = eStdPointer<eMoveToAction>(a);
    const auto hha = [aptr](eThreadTile* const tile) {
        const bool final = hasTarget(tile);
        if(final && aptr) {
            const auto type = tile->underBuildingType();
            const bool pyramid = eBuilding::sPyramidBuilding(type);
            if(pyramid) aptr->setRemoveLastTurn(false);
        }
        return final;
    };

    a->setStateRelevance(eStateRelevance::sanctBuildings |
                         eStateRelevance::buildings |
                         eStateRelevance::terrain);
    const stdptr<eCharacter> cptr(c);
    a->setFoundAction([cptr]() {
        if(!cptr) return;
        cptr->setActionType(eCharacterActionType::walk);
    });
    const auto findFailFunc = [tptr, this]() {
        if(tptr) mNoTarget = true;
    };

    a->setFindFailAction(findFailFunc);
    a->setRemoveLastTurn(true);
    a->start(hha, WalkableObject::sCreateArtisan());
    setCurrentAction(a);
    return true;
}

void eArtisanAction::workOnDecision(eTile* const tile) {
    const auto b = tile->underBuilding();
    const auto bb = dynamic_cast<eSanctBuilding*>(b);
    if(!bb || bb->workedOn() || bb->finished() ||
       !bb->resourcesAvailable()) return;
    bb->setWorkedOn(true);
    mStage = eArtisanActionStage::working;
    mTargetTile = tile;
    mTargetBuilding = bb;
    if(mWorkRemaining <= 0) {
        mWorkRemaining = eNumbers::sArtisanBuildTime;
    }
    const auto c = character();
    c->setActionType(eCharacterActionType::build);
    const auto t = c->tile();
    const int dx = t->x() - tile->x();
    const int dy = t->y() - tile->y();
    eOrientation o;
    if(dx > 0 && dy == 0) {
        o = eOrientation::topLeft;
    } else if(dx < 0 && dy == 0) {
        o = eOrientation::bottomRight;
    } else if(dx == 0 && dy > 0) {
        o = eOrientation::topRight;
    } else if(dx == 0 && dy < 0) {
        o = eOrientation::bottomLeft;
    } else if(dx < 0 && dy < 0) {
        o = eOrientation::bottom;
    } else if(dx > 0 && dy > 0) {
        o = eOrientation::top;
    } else if(dx < 0 && dy > 0) {
        o = eOrientation::right;
    } else if(dx > 0 && dy < 0) {
        o = eOrientation::left;
    } else {
        o = static_cast<eOrientation>(eRand::rand() % 8);
    }
    c->setOrientation(o);
    const auto w = e::make_shared<eBuildAction>(c);
    const auto fail = std::make_shared<eArtA_buildDelete>(board(), bb);
    w->setFailAction(fail);
    const auto deleteFail = std::make_shared<eArtA_buildDelete>(board(), bb);
    w->setDeleteFailAction(deleteFail);
    setCurrentAction(w);
}

void eArtisanAction::finishWork() {
    const auto b = mTargetBuilding.get();
    releaseWorkTarget();
    mStage = eArtisanActionStage::idle;
    if(!b) return;
    if(b->resourcesAvailable()) {
        b->incProgress();
    }
}

void eArtisanAction::releaseWorkTarget() {
    if(mTargetBuilding) mTargetBuilding->setWorkedOn(false);
    mTargetBuilding = nullptr;
    mTargetTile = nullptr;
    mWorkRemaining = 0;
}

void eArtisanAction::goBackDecision() {
    mStage = eArtisanActionStage::goingBack;
    const auto c = character();
    c->setActionType(eCharacterActionType::walk);
    goBack(mGuild, WalkableObject::sCreateArtisan());
}
