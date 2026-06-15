#include "eaphroditehelpaction.h"
#include "fileIO/save-archive.h"

#include "tile-helper.h"
#include "engine/eplague.h"
#include "buildings/ehousebase.h"
#include "iterate-square.h"

eAphroditeHelpAction::eAphroditeHelpAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::aphroditeHelpAction) {}

bool eAphroditeHelpAction::decide() {
    switch(mStage) {
    case eAphroditeHelpStage::none:
        mStage = eAphroditeHelpStage::appear;
        appear();
        break;
    case eAphroditeHelpStage::appear:
        goToTarget();
        break;
    case eAphroditeHelpStage::goTo:
        mStage = eAphroditeHelpStage::populate;
        populate();
        break;
    case eAphroditeHelpStage::populate:
        goToTarget();
        break;
    case eAphroditeHelpStage::populating:
        if(!mPopulateTarget) {
            finishPopulating();
            return decide();
        }
        spawnPopulateMissile(mPopulateTarget.get());
        break;
    case eAphroditeHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eAphroditeHelpAction::serializeFields(SaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("prePopulatingStage", mPrePopulatingStage);
    ar.buildingAsField("populateTarget", &board(), mPopulateTarget);
}

void eAphroditeHelpAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eAphroditeHelpAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eAphroditeHelpStage::populating:
        if(!mPopulateTarget) {
            finishPopulating();
            eGodAction::resumeFromSavedState();
            return;
        }
        spawnPopulateMissile(mPopulateTarget.get());
        return;
    case eAphroditeHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eAphroditeHelpStage::appear:
        appear();
        return;
    case eAphroditeHelpStage::goTo:
        goToTarget();
        return;
    case eAphroditeHelpStage::populate:
        decide();
        return;
    case eAphroditeHelpStage::disappear:
        disappear();
        return;
    }
}

void eAphroditeHelpAction::finishPopulating() {
    mStage = mPrePopulatingStage;
    mPrePopulatingStage = eAphroditeHelpStage::none;
    mPopulateTarget = nullptr;
}

bool eAphroditeHelpAction::sHelpNeeded(const eCityId cid,
                                       const GameBoard& board) {
    const auto bs = board.buildings(cid, [](eBuilding* const b) {
        const auto type = b->type();
        return type == eBuildingType::commonHouse ||
               type == eBuildingType::eliteHousing;
    });
    int v = 0;
    for(const auto b : bs) {
        const auto hb = static_cast<eHouseBase*>(b);
        v += hb->vacancies();
    }
    return v > 50;
}

eHouseBase* eAphroditeHelpAction::nearestHouseWithVacancies() {
    const auto c = character();
    const auto cTile = c->tile();
    if(!cTile) return nullptr;
    const int cx = cTile->x();
    const int cy = cTile->y();
    auto& board = this->board();
    const auto cid = cityId();
    const auto bs = board.buildings(cid, [](eBuilding* const b) {
        const auto type = b->type();
        return type == eBuildingType::commonHouse ||
               type == eBuildingType::eliteHousing;
    });
    eHouseBase* nearest = nullptr;
    int minDist = __INT_MAX__;
    for(const auto b : bs) {
        const auto hb = static_cast<eHouseBase*>(b);
        const int v = hb->vacancies();
        if(v <= 0) continue;
        const auto centerTile = b->centerTile();
        const int bx = centerTile->x();
        const int by = centerTile->y();
        const int dx = cx - bx;
        const int dy = cy - by;
        const int dist = dx*dx + dy*dy;
        if(minDist > dist) {
            minDist = dist;
            nearest = hb;
        }
    }
    return nearest;
}

void eAphroditeHelpAction::goToTarget() {
    auto& board = this->board();
    const auto nearest = nearestHouseWithVacancies();
    if(!nearest) {
        mStage = eAphroditeHelpStage::disappear;
        disappear();
    } else {
        mStage = eAphroditeHelpStage::goTo;
        const auto ct = nearest->centerTile();
        const int tx = ct->x();
        const int ty = ct->y();
        const auto tile = TileHelper::closestRoad(tx, ty, board);
        using eGTTT = eGoToTargetTeleport;
        const auto tele = std::make_shared<eGTTT>(board, this);
        goToTile(tile, tele);
    }
}

void eAphroditeHelpAction::populate() {
    const auto house = nearestHouseWithVacancies();
    if(!house) return;
    mPrePopulatingStage = mStage;
    mStage = eAphroditeHelpStage::populating;
    mPopulateTarget = house;
    pauseAction();
    spawnPopulateMissile(house);
}

void eAphroditeHelpAction::spawnPopulateMissile(eHouseBase* const target) {
    const auto c = character();
    const auto targetTile = target->centerTile();
    const auto finishCb = std::make_shared<eAHA_populateFinish>(board(), this);
    const auto act = std::make_shared<eAphroditeHelpAct>(board(), target);
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, act,
                    finishCb);
}
