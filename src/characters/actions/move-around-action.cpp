#include "move-around-action.h"
#include "fileIO/save-archive.h"

#include "characters/echaracter.h"
#include "engine/etile.h"

MoveAroundAction::MoveAroundAction(eCharacter* const c,
                                     const int startX, const int startY,
                                     const stdsptr<WalkableObject>& walkable) :
    eMoveAction(c, walkable, eCharActionType::moveAroundAction) {
    mStartTX = startX;
    mStartTY = startY;
}

MoveAroundAction::MoveAroundAction(eCharacter* const c) :
    MoveAroundAction(c, 0, 0, nullptr) {}

void MoveAroundAction::increment(const int by) {
    mRemTime -= by;
    eMoveAction::increment(by);
}

void MoveAroundAction::setTime(const int t) {
    mRemTime = t;
}

void MoveAroundAction::setMaxDistance(const int md) {
    mMaxDist = md;
}

void MoveAroundAction::serializeFields(SaveArchive& ar) {
    eMoveAction::serializeFields(ar);
    ar.field("startTX", mStartTX);
    ar.field("startTY", mStartTY);
    ar.field("maxDist", mMaxDist);
    ar.field("remTime", mRemTime);
}

eCharacterActionState MoveAroundAction::nextTurn(eOrientation& turn) {
    if(mRemTime <= 0) {
        return eCharacterActionState::finished;
    }
    std::vector<eOrientation> os{eOrientation::topRight,
                                 eOrientation::right,
                                 eOrientation::bottomRight,
                                 eOrientation::bottom,
                                 eOrientation::bottomLeft,
                                 eOrientation::left,
                                 eOrientation::topLeft,
                                 eOrientation::top};
    std::random_shuffle(os.begin(), os.end());
    const auto c = character();
    const bool keepO = (Rand::rand() % (mMaxDist/2)) != 0;
    if(keepO) os.insert(os.begin(), c->orientation());
    const auto t = c->tile();
    if(!t) return eCharacterActionState::failed;
    const auto cid = t->cityId();
    const int tx = t->x() - mStartTX;
    const int ty = t->y() - mStartTY;
    const int oldDist = std::sqrt(tx*tx + ty*ty);
    const auto cType = c->type();
    for(const auto o : os) {
        const auto tt = t->neighbour<eTile>(o);
        if(!tt || !walkable(tt) || tt->cityId() != cid) continue;
        bool occupied = false;
        for(const auto& oc : tt->characters()) {
            if(oc.get() != c && oc->type() == cType) {
                occupied = true;
                break;
            }
        }
        if(occupied) continue;
        const int ttx = tt->x() - mStartTX;
        const int tty = tt->y() - mStartTY;
        const int dist = std::sqrt(ttx*ttx + tty*tty);
        if(dist >= mMaxDist && dist > oldDist) continue;
        turn = o;
        return eCharacterActionState::running;
    }
    return eCharacterActionState::failed;
}
