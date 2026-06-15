#include "eapollohelpaction.h"
#include "fileIO/save-archive.h"

#include "tile-helper.h"
#include "engine/eplague.h"
#include "buildings/small-house.h"
#include "iterate-square.h"

eApolloHelpAction::eApolloHelpAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::apolloHelpAction) {}

bool eApolloHelpAction::decide() {
    switch(mStage) {
    case eApolloHelpStage::none:
        mStage = eApolloHelpStage::appear;
        appear();
        break;
    case eApolloHelpStage::appear:
        goToTarget();
        break;
    case eApolloHelpStage::goTo:
        mStage = eApolloHelpStage::heal;
        heal();
        break;
    case eApolloHelpStage::heal:
        goToTarget();
        break;
    case eApolloHelpStage::healing:
        if(!mHealTarget) {
            finishHealing();
            return decide();
        }
        spawnHealMissile(mHealTarget.get());
        break;
    case eApolloHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eApolloHelpAction::serializeFields(SaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("preHealingStage", mPreHealingStage);
    ar.buildingAsField("healTarget", &board(), mHealTarget);
}

void eApolloHelpAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eApolloHelpAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eApolloHelpStage::healing:
        if(!mHealTarget) {
            finishHealing();
            eGodAction::resumeFromSavedState();
            return;
        }
        spawnHealMissile(mHealTarget.get());
        return;
    case eApolloHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eApolloHelpStage::appear:
        appear();
        return;
    case eApolloHelpStage::goTo:
        goToTarget();
        return;
    case eApolloHelpStage::heal:
        decide();
        return;
    case eApolloHelpStage::disappear:
        disappear();
        return;
    }
}

void eApolloHelpAction::finishHealing() {
    mStage = mPreHealingStage;
    mPreHealingStage = eApolloHelpStage::none;
    mHealTarget = nullptr;
}

bool eApolloHelpAction::sHelpNeeded(const eCityId cid,
                                    const GameBoard& board) {
    const auto& ps = board.plagues(cid);
    return !ps.empty();
}

void eApolloHelpAction::goToTarget() {
    auto& board = this->board();
    const auto cid = cityId();
    const auto& plagues = board.plagues(cid);
    if(plagues.empty()) {
        mStage = eApolloHelpStage::disappear;
        disappear();
    } else {
        const auto p = plagues[0];
        const auto& hs = p->houses();
        if(hs.empty()) {
            mStage = eApolloHelpStage::disappear;
            disappear();
        } else {
            mStage = eApolloHelpStage::goTo;
            const auto h = hs[0];
            const auto ct = h->centerTile();
            const int tx = ct->x();
            const int ty = ct->y();
            const auto tile = TileHelper::closestRoad(tx, ty, board);
            using eGTTT = eGoToTargetTeleport;
            const auto tele = std::make_shared<eGTTT>(board, this);
            goToTile(tile, tele);
        }
    }
}

SmallHouse* sClosestPlagueHouseTile(
        eTile* const to, GameBoard& board) {
    if(!to) return nullptr;
    const int rdx = to->x();
    const int rdy = to->y();
    SmallHouse* result = nullptr;
    const auto prcsTile = [&](const int i, const int j) {
        const int tx = rdx + i;
        const int ty = rdy + j;
        const auto tt = board.tile(tx, ty);
        if(!tt) return false;
        const auto ub = tt->underBuilding();
        if(const auto ch = dynamic_cast<SmallHouse*>(ub)) {
            if(ch->plague()) {
                result = ch;
                return true;
            }
        }
        return false;
    };

    for(int k = 0; k < 1000; k++) {
        IterateSquare::iterateSquare(k, prcsTile, 1);
        if(result) break;
    }
    return result;
}

void eApolloHelpAction::heal() {
    const auto c = character();
    const auto house = sClosestPlagueHouseTile(c->tile(), board());
    if(!house) return;
    mPreHealingStage = mStage;
    mStage = eApolloHelpStage::healing;
    mHealTarget = house;
    pauseAction();
    spawnHealMissile(house);
}

void eApolloHelpAction::spawnHealMissile(SmallHouse* const target) {
    const auto c = character();
    const auto targetTile = target->centerTile();
    const auto finishCb = std::make_shared<eApHA_healFinish>(board(), this);
    const auto act = std::make_shared<eApolloHelpAct>(board(), target);
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, act,
                    finishCb);
}
