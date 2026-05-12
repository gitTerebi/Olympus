#include "epatrolmoveaction.h"

#include <string>
#include "../echaracter.h"
#include "engine/etile.h"
#include "emovepathaction.h"
#include "engine/epathfinder.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

ePatrolMoveAction::ePatrolMoveAction(eCharacter* const c,
                                     const bool diagonalOnly,
                                     const stdsptr<eWalkableObject>& walkable,
                                     const stdsptr<eDirectionTimes>& os) :
    eMoveAction(c, walkable, eCharActionType::patrolMoveAction),
    mDiagonalOnly(diagonalOnly),
    mWalkable(walkable),
    mOs(os) {
    mO = c->orientation();
}

void ePatrolMoveAction::read(eReadStream& src) {
    eMoveAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void ePatrolMoveAction::write(eWriteStream& dst) const {
    eMoveAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<ePatrolMoveAction*>(this)->serialize(ar);
}

void ePatrolMoveAction::serialize(eSaveArchive& ar) {
    ar.field("mDiagonalOnly", mDiagonalOnly);
    if(ar.reading()) {
        mWalkable = ar.readStream().readWalkable();
        mOs = ar.readStream().readDirectionTimes(board());
    } else {
        ar.writeStream().writeWalkable(mWalkable.get());
        ar.writeStream().writeDirectionTimes(mOs.get());
    }
    ar.field("mO", mO);
    ar.field("mMaxWalkDistance", mMaxWalkDistance);
    ar.field("mWalkedDistance", mWalkedDistance);
}

void ePatrolMoveAction::serializeJson(eJsonArchive& ar) {
    eMoveAction::serializeJson(ar);
    ar.field("mDiagonalOnly", mDiagonalOnly);
    ar.walkableRef("mWalkable", mWalkable);
    ar.field("mO", mO);
    ar.field("mMaxWalkDistance", mMaxWalkDistance);
    ar.field("mWalkedDistance", mWalkedDistance);
    if(ar.writing()) {
        int n = static_cast<int>(mOs->size());
        ar.field("mOs.n", n);
        int i = 0;
        for(auto& kv : *mOs) {
            auto sub = ar.child(("mOs." + std::to_string(i++)).c_str());
            int tx = kv.first->x(), ty = kv.first->y();
            sub.field("x", tx);
            sub.field("y", ty);
            for(int j = 0; j < 8; j++) {
                auto o = static_cast<eOrientation>(j);
                int t = kv.second.time(o);
                sub.field(("t" + std::to_string(j)).c_str(), t);
            }
        }
    } else {
        if(!mOs) mOs = std::make_shared<eDirectionTimes>();
        int n = 0;
        ar.field("mOs.n", n);
        auto& brd = board();
        for(int i = 0; i < n; i++) {
            auto sub = ar.child(("mOs." + std::to_string(i)).c_str());
            int x = 0, y = 0;
            sub.field("x", x);
            sub.field("y", y);
            auto* tile = brd.tile(x, y);
            if(tile) {
                auto& entry = (*mOs)[tile];
                for(int j = 0; j < 8; j++) {
                    int t = 0;
                    sub.field(("t" + std::to_string(j)).c_str(), t);
                    entry.time(static_cast<eOrientation>(j)) = t;
                }
            }
        }
    }
}

eCharacterActionState ePatrolMoveAction::nextTurn(eOrientation& t) {
    if(mWalkedDistance++ > mMaxWalkDistance) {
        return eCharacterActionState::finished;
    }
    const auto c = character();
    const auto tile = c->tile();
    if(!tile) return eCharacterActionState::failed;

    const auto neighVer = [&](eTileBase* const t) {
        return t && mWalkable->walkable(t) && t->neighbour(mO) != tile;
    };
    auto options = mDiagonalOnly ? tile->diagonalNeighbours(neighVer) :
                                   tile->neighbours(neighVer);
    auto& uses = (*mOs)[tile];
    if(options.empty()) {
        mO = !mO;
    } else {
        int min = __INT_MAX__;
        std::vector<eOrientation> minOs;
        minOs.reserve(options.size());
        for(const auto& o : options) {
            const auto oo = o.first;
            int& u = uses.time(oo);
            if(u < min) {
                minOs.clear();
                minOs.push_back(oo);
                min = u;
            } else if(u == min) {
                minOs.push_back(oo);
            }
        }
        if(!minOs.empty()) {
            const int id = eRand::rand() % minOs.size();
            mO = minOs[id];
        }
    }
    t = mO;
    const auto tt = tile->neighbour<eTile>(mO);
    if(!tt) {
        return eCharacterActionState::failed;
    }
    const auto& board = c->getBoard();
    const int time = board.totalTime();
    if(options.size() > 1) {
        uses.time(mO) = time;
        (*mOs)[tt].time(!mO) = time;
    }
    return eCharacterActionState::running;
}
