#include "epatrolmoveaction.h"

#include "../echaracter.h"
#include "engine/etile.h"
#include "emovepathaction.h"
#include "engine/epathfinder.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

void eDirectionLastUseTime::serialize(eSaveArchive& ar) {
    ar.field("time", mTime);
}

namespace {

struct eNextTurnChoice {
    eOrientation fOrientation;
    bool fRecordUse;
};

eCharacterActionState nextTurnChoices(
        eTile* const tile,
        const bool diagonalOnly,
        const WalkableObject& walkable,
        eDirectionTimes& times,
        const eOrientation orientation,
        std::vector<eNextTurnChoice>& choices) {
    choices.clear();
    if(!tile) return eCharacterActionState::failed;

    const auto neighVer = [&](eTileBase* const t) {
        return t && walkable.walkable(t) && t->neighbour(orientation) != tile;
    };
    auto options = diagonalOnly ? tile->diagonalNeighbours(neighVer) :
                                  tile->neighbours(neighVer);
    auto& uses = times[tile];
    if(options.empty()) {
        choices.push_back({!orientation, false});
    } else {
        int min = __INT_MAX__;
        choices.reserve(options.size());
        for(const auto& o : options) {
            const auto oo = o.first;
            int& u = uses.time(oo);
            if(u < min) {
                choices.clear();
                choices.push_back({oo, options.size() > 1});
                min = u;
            } else if(u == min) {
                choices.push_back({oo, options.size() > 1});
            }
        }
    }
    return choices.empty() ? eCharacterActionState::failed :
                             eCharacterActionState::running;
}

}

ePatrolMoveAction::ePatrolMoveAction(eCharacter* const c,
                                     const bool diagonalOnly,
                                     const stdsptr<WalkableObject>& walkable,
                                     const stdsptr<eDirectionTimes>& os) :
    eMoveAction(c, walkable, eCharActionType::patrolMoveAction),
    mDiagonalOnly(diagonalOnly),
    mWalkable(walkable),
    mOs(os) {
    mO = c->orientation();
}

void ePatrolMoveAction::serializeFields(eSaveArchive& ar) {
    eMoveAction::serializeFields(ar);
    ar.field("diagonalOnly", mDiagonalOnly);
    ar.walkableField("walkable", mWalkable);
    ar.directionTimesField("directionTimes", board(), mOs);
    ar.field("orientation", mO);
    ar.field("maxWalkDistance", mMaxWalkDistance);
    ar.field("walkedDistance", mWalkedDistance);
}

eCharacterActionState ePatrolMoveAction::sNextTurn(
        eTile* const tile,
        const bool diagonalOnly,
        const WalkableObject& walkable,
        eDirectionTimes& times,
        eOrientation& orientation,
        const int time,
        const eOrientationPicker& pickOrientation) {
    std::vector<eNextTurnChoice> choices;
    const auto r = nextTurnChoices(tile, diagonalOnly, walkable, times,
                                   orientation, choices);
    if(r != eCharacterActionState::running) return r;

    std::vector<eOrientation> orientations;
    orientations.reserve(choices.size());
    for(const auto& c : choices) orientations.push_back(c.fOrientation);
    orientation = pickOrientation(orientations, orientation);

    const auto tt = tile->neighbour<eTile>(orientation);
    if(!tt) {
        return eCharacterActionState::failed;
    }
    bool recordUse = false;
    for(const auto& c : choices) {
        if(c.fOrientation == orientation) {
            recordUse = c.fRecordUse;
            break;
        }
    }
    if(recordUse) {
        auto& uses = times[tile];
        uses.time(orientation) = time;
        times[tt].time(!orientation) = time;
    }
    return eCharacterActionState::running;
}

eCharacterActionState ePatrolMoveAction::nextTurn(eOrientation& t) {
    if(mWalkedDistance++ > mMaxWalkDistance) {
        return eCharacterActionState::finished;
    }
    const auto c = character();
    const auto picker = [](const std::vector<eOrientation>& options,
                           const eOrientation) {
        const int id = eRand::rand() % options.size();
        return options[id];
    };
    const auto r = sNextTurn(c->tile(), mDiagonalOnly, *mWalkable, *mOs,
                             mO, c->getBoard().totalTime(), picker);
    t = mO;
    return r;
}
