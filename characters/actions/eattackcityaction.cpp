#include "eattackcityaction.h"

#include "engine/egameboard.h"
#include "enumbers.h"

eAttackCityAction::eAttackCityAction(eCharacter* const c) :
    eDefendAttackCityAction(c, eCharActionType::attackCityAction) {
    const auto ctype = c->type();
    if(ctype == eCharacterType::ares) {
        setMaxKilled(eNumbers::sAttackCityAresMaxKilled);
    } else {
        setMaxKilled(eNumbers::sAttackCityMaxKilled);
    }
}

bool eAttackCityAction::decide() {
    const auto c = character();
    switch(mStage) {
    case eDefendAttackCityStage::none:
        mStage = eDefendAttackCityStage::appear;
        if(!c->tile()) randomPlaceOnBoard();
        if(!c->tile()) {
            c->kill();
        } else {
            appear();
        }
        break;
    case eDefendAttackCityStage::appear:
        mStage = eDefendAttackCityStage::goTo;
        mStartTile = c->tile();
        goToNearestSoldier();
        break;
    case eDefendAttackCityStage::goTo:
    case eDefendAttackCityStage::wait: {
        if(killed() >= maxKilled()) {
            mStage = eDefendAttackCityStage::comeback;
            goBack();
        } else {
            const bool r = goToNearestSoldier();
            if(r) {
                mStage = eDefendAttackCityStage::wait;
                moveAround(nullptr, 15000);
            } else {
                mStage = eDefendAttackCityStage::goTo;
            }
        }
    }   break;
    case eDefendAttackCityStage::fight:
        mStage = eDefendAttackCityStage::comeback;
        goBack();
        break;
    case eDefendAttackCityStage::comeback:
        mStage = eDefendAttackCityStage::disappear;
        disappear();
        break;
    case eDefendAttackCityStage::disappear:
        c->kill();
        break;
    }
    return true;
}

void eAttackCityAction::invasionFinished() {
    mStage = eDefendAttackCityStage::comeback;
    goBack();
}

bool eAttackCityAction::goToNearestSoldier() {
    auto& board = eAttackCityAction::board();
    const auto onCid = onCityId();
    const auto city = board.boardCityWithId(onCid);
    const auto cid = cityId();
    const auto tid = board.cityIdToTeamId(cid);
    const auto c = character();
    const auto tile = c->tile();
    const int tx = tile->x();
    const int ty = tile->y();
    int nX;
    int nY;
    const bool r = city->nearestEnemySoldier(tid, tx, ty, nX, nY);
    if(!r) return true;
    const int range = this->range();
    const bool rr = goTo(nX, nY, range);
    return rr;
}
