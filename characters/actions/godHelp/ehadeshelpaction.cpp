#include "ehadeshelpaction.h"

#include "etilehelper.h"
#include "buildings/epalace.h"

eHadesHelpAction::eHadesHelpAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::hadesHelpAction) {}

bool eHadesHelpAction::decide() {
    switch(mStage) {
    case eHadesHelpStage::none:
        mStage = eHadesHelpStage::appear;
        appear();
        break;
    case eHadesHelpStage::appear:
        mStage = eHadesHelpStage::goTo;
        goToTarget();
        break;
    case eHadesHelpStage::goTo:
        mStage = eHadesHelpStage::give;
        give();
        break;
    case eHadesHelpStage::give:
        mStage = eHadesHelpStage::disappear;
        disappear();
        break;
    case eHadesHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eHadesHelpAction::read(eReadStream& src) {
    eGodAction::read(src);
    src >> mStage;
    src.readBuilding(&board(), [this](eBuilding* const b) {
        mTarget = b;
    });
}

void eHadesHelpAction::write(eWriteStream& dst) const {
    eGodAction::write(dst);
    dst << mStage;
    dst.writeBuilding(mTarget);
}

bool eHadesHelpAction::sHelpNeeded(const eCityId cid,
                                   const eGameBoard& board) {
    return board.hasPalace(cid);
}

void eHadesHelpAction::goToTarget() {
    auto& board = this->board();
    using eGTTT = eGoToTargetTeleport;
    const auto tele = std::make_shared<eGTTT>(board, this);
    const auto cid = cityId();
    mTarget = board.palace(cid);
    if(mTarget) {
        const auto ct = mTarget->centerTile();
        const int tx = ct->x();
        const int ty = ct->y();
        const auto tile = eTileHelper::closestRoad(tx, ty, board);
        goToTile(tile, tele);
    } else {
        disappear();
    }
}

void eHadesHelpAction::give() {
    if(!mTarget) return;
    const auto c = character();
    const auto targetTile = mTarget->centerTile();
    using eGA_LFRAF = eGA_lookForRangeActionFinish;
    const auto finishAttackA =
            std::make_shared<eGA_LFRAF>(
                board(), this);
    using eGPDA = eGodProvideDrachmasAct;
    const auto cid = mTarget->cityId();
    const auto act = std::make_shared<eGPDA>(board(), cid);
    pauseAction();
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, act,
                    finishAttackA);
}
