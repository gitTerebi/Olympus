#include "fire-fighter-action.h"
#include "fileIO/esavearchive.h"

#include "../echaracter.h"
#include "engine/game-board.h"
#include "emovetoaction.h"
#include "ewaitaction.h"

FireFighterAction::FireFighterAction(
        eCharacter* const c,
        ePatrolBuildingBase* const b,
        const std::vector<eOrientation>& path,
        const stdsptr<eDirectionTimes>& dirTimes) :
    ePatrolAction(c, b, path, dirTimes,
                  eCharActionType::fireFighterAction) {}

FireFighterAction::FireFighterAction(eCharacter* const c) :
    ePatrolAction(c, eCharActionType::fireFighterAction) {}

void FireFighterAction::increment(const int by) {
    if(!mFireFighting) {
        const int fireCheckInc = 1000;
        mFireCheck += by;
        if(mFireCheck > fireCheckInc) {
            mFireCheck -= fireCheckInc;
            const auto c = character();
            auto& brd = c->getBoard();
            const auto ct = c->tile();
            const int tx = ct->x();
            const int ty = ct->y();
            const int range = 10;
            bool found = false;
            for(int i = -range; i <= range && !found; i++) {
                for(int j = -range; j <= range && !found; j++) {
                    const int ttx = tx + i;
                    const int tty = ty + j;
                    const auto t = brd.tile(ttx, tty);
                    if(!t) continue;
                    found = t->onFire();
                }
            }
            if(found) lookForFire(false);
        }
    }
    ePatrolAction::increment(by);
}

eTile* neighbourOnFire(eTile* const tile, eOrientation& oo) {
    eTile* n = nullptr;
    for(const auto o : {eOrientation::topRight,
                        eOrientation::right,
                        eOrientation::bottomRight,
                        eOrientation::bottom,
                        eOrientation::bottomLeft,
                        eOrientation::left,
                        eOrientation::topLeft,
                        eOrientation::top}) {
        const auto nt = tile->neighbour<eTile>(o);
        if(!nt) continue;
        if(nt->onFire()) {
            oo = o;
            n = nt;
            break;
        }
    }
    return n;
}

bool FireFighterAction::decide() {
    const auto c = character();
    if(mFireFighting) {
        const auto tile = c->tile();
        eOrientation oo;
        const auto n = neighbourOnFire(tile, oo);
        if(n) {
            c->setOrientation(oo);
            putOutFire(n);
            mUsedWater++;
        } else {
            if(mUsedWater >= 5) {
                c->setActionType(eCharacterActionType::walk);
                goBackDecision(eWalkableObject::sCreateDefault());
            } else {
                c->setActionType(eCharacterActionType::stand);
                lookForFire(true);
            }
        }
        return true;
    } else {
        const bool r = ePatrolAction::decide();
        if(r) return r;
    }

    return true;
}

void FireFighterAction::serializeFields(eSaveArchive& ar) {
    ePatrolAction::serializeFields(ar);
    ar.field("fireFighting", mFireFighting);
    ar.field("fireCheck", mFireCheck);
    ar.field("usedWater", mUsedWater);
    ar.field("fireFighterStage", mStage);
    ar.tileField("fireTile", board(), mFireTile);
}

void FireFighterAction::resumeFromSavedState() {
    switch(mStage) {
    case FireFighterActionStage::idle:
        ePatrolAction::resumeFromSavedState();
        break;
    case FireFighterActionStage::lookingForFire:
        lookForFire(true);
        break;
    case FireFighterActionStage::puttingOutFire:
        if(mFireTile && mFireTile->onFire()) putOutFire(mFireTile);
        else ePatrolAction::resumeFromSavedState();
        break;
    }
}

bool FireFighterAction::lookForFire(const bool second) {
    const auto c = character();
    mStage = FireFighterActionStage::lookingForFire;

    const auto failFunc = std::make_shared<FFA_lookForFireFail>(
                              board(), this);

    const auto onFire = [](eTileBase* const tile) {
        return tile->onFire();
    };

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setFailAction(failFunc);
    a->setMaxFindDistance(50);
    const stdptr<FireFighterAction> tptr(this);
    a->setFoundAction([tptr, a, second, this]() {
        if(!tptr) return;
        if(second) {
            mFireFighting = true;
            const auto c = character();
            c->setActionType(eCharacterActionType::carry);
        } else {
            lookForFire(true);
        }
    });
    a->setRemoveLastTurn(true);

    a->setWait(mFireFighting || second);
    a->start(onFire);
    a->setMaxFindDistance(40);
    if(second) {
        setCurrentAction(a);
    }

    return true;
}

void FireFighterAction::putOutFire(eTile* const tile) {
    mStage = FireFighterActionStage::puttingOutFire;
    mFireTile = tile;
    const auto c = character();
    c->setActionType(eCharacterActionType::firefight);
    const auto finish = std::make_shared<FFA_putOutFireFinish>(
                            board(), c, tile);
    const auto a = e::make_shared<eWaitAction>(c);
    a->setFinishAction(finish);
    if(tile->underBuilding()) {
        a->setTime(1600);
    } else {
        a->setTime(800);
    }
    setCurrentAction(a);
}
