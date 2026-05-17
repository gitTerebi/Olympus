#include "emovearoundaction.h"
#include "fileIO/esavearchive.h"

#include "characters/echaracter.h"

eMoveAroundAction::eMoveAroundAction(eCharacter* const c,
                                     const int startX, const int startY,
                                     const stdsptr<eWalkableObject>& walkable) :
    eMoveAction(c, walkable, eCharActionType::moveAroundAction) {
    mStartTX = startX;
    mStartTY = startY;
}

eMoveAroundAction::eMoveAroundAction(eCharacter* const c) :
    eMoveAroundAction(c, 0, 0, nullptr) {}

void eMoveAroundAction::increment(const int by) {
    mRemTime -= by;
    eMoveAction::increment(by);
}

void eMoveAroundAction::setTime(const int t) {
    mRemTime = t;
}

void eMoveAroundAction::setMaxDistance(const int md) {
    mMaxDist = md;
}

void eMoveAroundAction::serializeFields(eSaveArchive& ar) {
    eMoveAction::serializeFields(ar);
    ar.field("startTX", mStartTX);
    ar.field("startTY", mStartTY);
    ar.field("maxDist", mMaxDist);
    ar.field("remTime", mRemTime);
}

eCharacterActionState eMoveAroundAction::nextTurn(eOrientation& turn) {
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
    const bool keepO = (eRand::rand() % (mMaxDist/2)) != 0;
    if(keepO) os.insert(os.begin(), c->orientation());
    const auto t = c->tile();
    if(!t) return eCharacterActionState::failed;
    const auto cid = t->cityId();
    const int tx = t->x() - mStartTX;
    const int ty = t->y() - mStartTY;
    const int oldDist = std::sqrt(tx*tx + ty*ty);
    for(const auto o : os) {
        const auto tt = t->neighbour(o);
        if(!tt || !walkable(tt) || tt->cityId() != cid) continue;
        const int ttx = tt->x() - mStartTX;
        const int tty = tt->y() - mStartTY;
        const int dist = std::sqrt(ttx*ttx + tty*tty);
        if(dist >= mMaxDist && dist > oldDist) continue;
        turn = o;
        return eCharacterActionState::running;
    }
    return eCharacterActionState::failed;
}
