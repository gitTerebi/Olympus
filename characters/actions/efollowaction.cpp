#include "efollowaction.h"

#include "../echaracter.h"

#include "walkable/ewalkableobject.h"
#include "fileIO/esavearchive.h"

eFollowAction::eFollowAction(eCharacter* const f,
                             eCharacter* const c,
                             const eCharActionType type) :
    eMoveAction(c, eWalkableObject::sCreateAll(), type),
    mFollow(f) {
    if(f) c->setSpeed(f->speed());
    c->setActionType(eCharacterActionType::stand);
}

eFollowAction::eFollowAction(eCharacter* const f,
                             eCharacter* const c) :
    eFollowAction(f, c, eCharActionType::followAction) {}

eFollowAction::eFollowAction(eCharacter* const c) :
    eFollowAction(nullptr, c) {}

void eFollowAction::catchUp() {
    mCatchUp = true;
}

void eFollowAction::setDistance(const int d) {
    mDistance = d;
}

void eFollowAction::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eFollowAction::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eFollowAction*>(this)->serialize(ar);
}

void eFollowAction::serialize(eSaveArchive& ar) {
    eMoveAction::serialize(ar);
    if(ar.reading()) {
        ar.readStream().readCharacter(&board(), [this](eCharacter* const c) {
            mFollow = c;
            if(c) {
                const auto cc = character();
                cc->setSpeed(c->speed());
            }
        });
    } else {
        ar.writeStream().writeCharacter(mFollow);
    }
    ar.field("mCatchUp", mCatchUp);
    ar.field("mDistance", mDistance);
    ar.dequeField("tiles", mTiles, [this](eSaveArchive& ar, ePathNode& n) {
        ar.tile(n.fTile, board());
        ar.field("n.fO", n.fO);
    });
}

eOrientation sOrientation(eTile* const from,
                          eTile* const to) {
    const int dx = to->x() - from->x();
    const int dy = to->y() - from->y();
    if(dx < 0 && dy < 0) return eOrientation::top;
    else if(dx > 0 && dy > 0) return eOrientation::bottom;
    else if(dx > 0 && dy < 0) return eOrientation::right;
    else if(dx < 0 && dy > 0) return eOrientation::left;
    else if(dy < 0) return eOrientation::topRight;
    else if(dx > 0) return eOrientation::bottomRight;
    else if(dy > 0) return eOrientation::bottomLeft;
    else if(dx < 0) return eOrientation::topLeft;
    return eOrientation::topLeft;
}

void eFollowAction::increment(const int by) {
    const auto ft = mFollow ? mFollow->tile() : nullptr;
    const bool dead = mFollow ? mFollow->dead() : false;
    const auto c = character();
    const auto ctile = c->tile();
    if(!ft || dead) {
        if(mTiles.empty()) {
            return setState(eCharacterActionState::finished);
        } else {
            setWait(false);
            return eMoveAction::increment(by);
        }
    }
    if(!mTiles.empty()) {
        auto& b = mTiles.back();
        if(ft == b.fTile) {
            const auto at = mFollow->actionType();
            if(at == eCharacterActionType::walk ||
               at == eCharacterActionType::carry) {
                b.fO = mFollow->orientation();
            }
        } else {
            b.fO = sOrientation(b.fTile, ft);
            mTiles.push_back({ft, mFollow->orientation()});
        }
    } else if(ctile != ft) {
        mTiles.push_back({ft, mFollow->orientation()});
    }
    const int nt = mTiles.size();
    if((nt < mDistance + 1 && !mCatchUp) || mTiles.empty()) {
        setWait(true);
        mCatchUp = false;
        return;
    } else {
        setWait(false);
    }
    eMoveAction::increment(by);
}

eCharacterActionState eFollowAction::nextTurn(eOrientation& turn) {
    if(mTiles.empty()) {
        setWait(true);
        mCatchUp = false;
        return eCharacterActionState::running;
    }
    const auto c = character();
    const auto pn = mTiles.front();
    mTiles.pop_front();
    c->setActionType(eCharacterActionType::walk);
    c->changeTile(pn.fTile);
    c->setOrientation(pn.fO);
    turn = pn.fO;
    return eCharacterActionState::running;
}
