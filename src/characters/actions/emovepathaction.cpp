#include "emovepathaction.h"

#include "characters/echaracter.h"
#include "engine/etile.h"
#include "fileIO/save-archive.h"

eMovePathAction::eMovePathAction(eCharacter* const c,
                                 const std::vector<eOrientation>& path,
                                 const stdsptr<WalkableObject>& tileWalkable) :
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

void eMovePathAction::serializeFields(SaveArchive& ar) {
    eMoveAction::serializeFields(ar);
    ar.arrayField("turns", mTurns, [](SaveArchive& itemAr, eOrientation& o) {
        itemAr.field("orientation", o);
    });
    ar.field("maxDistance", mMaxDistance);
    ar.field("walkedDistance", mWalkedDistance);
}
