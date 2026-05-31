#include "emoveaction.h"

#include "../echaracter.h"
#include "walkable/ewalkableobject.h"

#include "vec2.h"
#include "fileIO/esavearchive.h"
#include "enumbers.h"

eMoveAction::eMoveAction(eCharacter* const c,
                         const stdsptr<eWalkableObject>& tileWalkable,
                         const eCharActionType type) :
    eCharacterAction(c, type),
    mTileWalkable(tileWalkable) {
    mStartX = c->x();
    mStartY = c->y();
    mOrientation = c->orientation();
}

eMoveAction::eMoveAction(eCharacter* const c,
                         const eCharActionType type) :
    eMoveAction(c, nullptr, type) {}

bool eMoveAction::walkable(eTileBase* const tile) const {
    return mTileWalkable->walkable(tile);
}

void eMoveAction::setObsticleHandler(const stdsptr<eObsticleHandler>& oh) {
    mObstHandler = oh;
}

void orientationToTargetCoords(const eOrientation o,
                               double& targetX,
                               double& targetY) {
    switch(o) {
    case eOrientation::topRight:
        targetX = 0.5;
        targetY = 0;
        break;
    case eOrientation::right:
        targetX = 1;
        targetY = 0;
        break;
    case eOrientation::bottomRight:
        targetX = 1;
        targetY = 0.5;
        break;
    case eOrientation::bottom:
        targetX = 1;
        targetY = 1;
        break;
    case eOrientation::bottomLeft:
        targetX = 0.5;
        targetY = 1;
        break;
    case eOrientation::left:
        targetX = 0;
        targetY = 1;
        break;
    case eOrientation::topLeft:
        targetX = 0;
        targetY = 0.5;
        break;
    case eOrientation::top:
        targetX = 0;
        targetY = 0;
        break;
    }
}

inline bool isEqual(const double x, const double y) {
    const double epsilon = 1e-5;
    return std::abs(x - y) <= epsilon * std::abs(x);
}

void eMoveAction::increment(const int by) {
    switch(state()) {
    case eCharacterActionState::failed:
    case eCharacterActionState::finished:
        return;
    case eCharacterActionState::running:
        break;
    }

    if(!mTargetTile) {
        if(mWait) return;
        if(nextTurn()) increment(by);
        return;
    }

    const auto c = character();
    const double monthMs = 30.0 * eNumbers::sDayLength;
    moveBy(c->speed() * by / monthMs);
}

void eMoveAction::serializeFields(eSaveArchive& ar) {
    eCharacterAction::serializeFields(ar);
    ar.walkableField("tileWalkable", mTileWalkable);
    ar.field("orientation", mOrientation);
    ar.tileField("targetTile", board(), mTargetTile);
    ar.field("wait", mWait);
    ar.field("startX", mStartX);
    ar.field("startY", mStartY);
    ar.field("targetX", mTargetX);
    ar.field("targetY", mTargetY);
}

void eMoveAction::resumeFromSavedState() {
    if(state() != eCharacterActionState::running) return;
    const auto c = character();
    if(c->actionType() == eCharacterActionType::none) {
        c->setActionType(eCharacterActionType::walk);
    }
}

void eMoveAction::moveBy(const double inc) {
    const auto c = character();
    const double x = c->x();
    const double y = c->y();

    vec2d moveVec(mTargetX - x,
                  mTargetY - y);
    const double dist = moveVec.length();
    if(dist > inc) {
        moveVec.normalize();
        moveVec *= inc;
        c->setX(x + moveVec.x);
        c->setY(y + moveVec.y);
    } else {
        c->setX(mTargetX);
        c->setY(mTargetY);
        const stdptr<eMoveAction> tptr(this);
        const auto t = mTargetTile;
        moveToTargetTile();
        if(tptr && t != mTargetTile && !mWait) moveBy(inc - dist);
    }
}

bool eMoveAction::nextTurn() {
    eOrientation turn;
    const auto r = nextTurn(turn);
    if(mWait) {
        mTargetTile = nullptr;
        return false;
    }
    setState(r);
    switch(r) {
    case eCharacterActionState::failed:
    case eCharacterActionState::finished:
        return false;
    case eCharacterActionState::running:
        break;
    }

    const auto c = character();
    c->setOrientation(turn);
    const auto t = c->tile();
    mTargetTile = t->neighbour<eTile>(turn);
    if(!mTargetTile) {
        setState(eCharacterActionState::failed);
        return false;
    }
    if(!walkable(mTargetTile)) {
        // Blocking tile (a building, since the mover uses default walkable while
        // the path was found with attacker walkable). The handler turns the unit
        // onto the building (FightingAction::attackBuilding). It must NOT step
        // onto the tile — stop the move so the fight loop grinds the building
        // down in place; once it dies the unit re-paths. Returning true here and
        // continuing would glide the unit through the building (the old bug).
        if(mObstHandler && mObstHandler->handle(mTargetTile)) {
            mTargetTile = nullptr;
            setState(eCharacterActionState::failed);
            return false;
        } else {
            setState(eCharacterActionState::failed);
            return false;
        }
    }
    mStartX = c->x();
    mStartY = c->y();
    mOrientation = turn;
    orientationToTargetCoords(turn, mTargetX, mTargetY);
    if(isEqual(mStartX, mTargetX) &&
       isEqual(mStartY, mTargetY)) {
        moveToTargetTile();
    }
    return true;
}

void eMoveAction::moveToTargetTile() {
    if(!mTargetTile) {
        setState(eCharacterActionState::failed);
        return;
    }
    const auto c = character();
    if(c->tile() == mTargetTile) {
        if(mWait) {
            mTargetTile = nullptr;
        } else {
            nextTurn();
        }
        return;
    }
    {
        double targetX;
        double targetY;
        orientationToTargetCoords(mOrientation, targetX, targetY);
        mStartX = 1 - targetX;
        mStartY = 1 - targetY;
    }

    const bool prepend = mOrientation == eOrientation::bottomLeft ||
                         mOrientation == eOrientation::bottom ||
                         mOrientation == eOrientation::bottomRight;
    c->changeTile(mTargetTile, prepend);
    mTargetX = 0.5;
    mTargetY = 0.5;

    c->setX(mStartX);
    c->setY(mStartY);

    const auto cs = mTargetTile->characters();
    const auto cc = character();
    for(const auto& c : cs) {
        if(c.get() == character()) continue;
        const bool cf = c->canFight(cc);
        const bool ccf = cc->canFight(c.get());
        if(cf && ccf) {
            cc->fight(c.get());
            c->fight(cc);
            break;
        }
    }
}
