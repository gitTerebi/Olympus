#include "emovepathaction.h"

#include "characters/echaracter.h"
#include "fileIO/esavearchive.h"

eMovePathAction::eMovePathAction(eCharacter* const c,
                                 const std::vector<eOrientation>& path,
                                 const stdsptr<eWalkableObject>& tileWalkable) :
    eMoveAction(c, tileWalkable, eCharActionType::movePathAction),
    mTurns(path) {
}

eMovePathAction::eMovePathAction(eCharacter* const c) :
    eMovePathAction(c, {}, nullptr) {}

eCharacterActionState eMovePathAction::nextTurn(eOrientation& turn) {
    if(mTurns.empty()) return eCharacterActionState::finished;
    if(mWalkedDistance++ > mMaxDistance) {
        return eCharacterActionState::finished;
    }
    turn = mTurns.back();
    mTurns.pop_back();
    const auto c = character();
    const auto t = c->tile();
    if(!t) return eCharacterActionState::failed;
    const auto tt = t->neighbour(turn);
    if(!tt) return eCharacterActionState::failed;
    return eCharacterActionState::running;
}

void eMovePathAction::read(eReadStream& src) {
    eMoveAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eMovePathAction::write(eWriteStream& dst) const {
    eMoveAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eMovePathAction*>(this)->serialize(ar);
}

void eMovePathAction::serialize(eSaveArchive& ar) {
    int n = mTurns.size();
    ar.field("n", n);
    if(ar.reading()) {
        mTurns.clear();
    }
    for(int i = 0; i < n; i++) {
        eOrientation o;
        if(ar.writing()) {
            o = mTurns[i];
        }
        ar.field("o", o);
        if(ar.reading()) {
            mTurns.push_back(o);
        }
    }
    ar.field("mMaxDistance", mMaxDistance);
    ar.field("mWalkedDistance", mWalkedDistance);
}
