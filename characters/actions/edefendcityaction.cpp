#include "edefendcityaction.h"

#include "gameEvents/einvasionevent.h"
#include "egodaction.h"
#include "enumbers.h"

eDefendCityAction::eDefendCityAction(eCharacter* const c) :
    eDefendAttackCityAction(c, eCharActionType::defendCityAction) {
    const auto ctype = c->type();
    if(ctype == eCharacterType::talos) {
        setMaxKilled(eNumbers::sDefendCityTalosMaxKilled);
    } else {
        setMaxKilled(eNumbers::sDefendCityMaxKilled);
    }
}

bool eDefendCityAction::decide() {
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
        goToTarget();
        break;
    case eDefendAttackCityStage::goTo:
    case eDefendAttackCityStage::wait: {
        if(!mEvent || killed() >= maxKilled()) {
            mStage = eDefendAttackCityStage::comeback;
            goBack();
        } else {
            if(mEvent->activeInvasions()) {
                const bool r = goToNearestSoldier();
                if(r) {
                    mStage = eDefendAttackCityStage::fight;
                    moveAround(nullptr, 15000);
                } else {
                    mStage = eDefendAttackCityStage::goTo;
                }
            } else {
                mStage = eDefendAttackCityStage::wait;
                moveAround();
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

void eDefendCityAction::read(eReadStream& src) {
    auto& board = eDefendCityAction::board();
    eDefendAttackCityAction::read(src);
    src.readGameEvent(&board, [this](eGameEvent* const e) {
        mEvent = static_cast<eInvasionEvent*>(e);
    });
}

void eDefendCityAction::write(eWriteStream& dst) const {
    eDefendAttackCityAction::write(dst);
    dst.writeGameEvent(mEvent);
}

void eDefendCityAction::goToTarget() {
    auto& board = eDefendCityAction::board();
    const auto cid = cityId();
    mEvent = board.invasionToDefend(cid);
    if(!mEvent) {
        mStage = eDefendAttackCityStage::comeback;
        return;
    }
    const auto shoreTile = mEvent->shoreTile();
    const auto tile = shoreTile ? shoreTile :
                                  mEvent->invasionTile();
    if(!tile) {
        mStage = eDefendAttackCityStage::comeback;
        return;
    }
    using eGTTT = eGoToTargetTeleport;
    const auto tele = std::make_shared<eGTTT>(board, this);
    goToTile(tile, tele);
}

bool eDefendCityAction::goToNearestSoldier() {
    if(!mEvent) return true;
    const auto c = character();
    const auto tile = c->tile();
    const int tx = tile->x();
    const int ty = tile->y();
    int nX;
    int nY;
    const bool r = mEvent->nearestSoldier(tx, ty, nX, nY);
    if(!r) return true;
    const int range = this->range();
    const bool rr = goTo(nX, nY, range);
    return rr;
}
