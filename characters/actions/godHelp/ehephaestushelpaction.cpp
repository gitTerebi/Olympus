#include "ehephaestushelpaction.h"
#include "fileIO/esavearchive.h"

#include "etilehelper.h"
#include "buildings/epalace.h"
#include "gameEvents/invasions/invasion-event.h"
#include "characters/monsters/ecalydonianboar.h"
#include "characters/actions/edefendcityaction.h"

eHephaestusHelpAction::eHephaestusHelpAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::hephaestusHelpAction) {}

bool eHephaestusHelpAction::decide() {
    switch(mStage) {
    case eHephaestusHelpStage::none:
        mStage = eHephaestusHelpStage::appear;
        appear();
        break;
    case eHephaestusHelpStage::appear:
        mStage = eHephaestusHelpStage::provide;
        provide();
        break;
    case eHephaestusHelpStage::provide:
        mStage = eHephaestusHelpStage::disappear;
        disappear();
        break;
    case eHephaestusHelpStage::providing:
        spawnProvideMissile();
        break;
    case eHephaestusHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eHephaestusHelpAction::serializeFields(eSaveArchive& ar) {
    eGodAction::serializeFields(ar);
    ar.field("stage", mStage);
    ar.field("preProvidingStage", mPreProvidingStage);
    ar.field("talosSpawned", mTalosSpawned);
}

void eHephaestusHelpAction::resumeFromSavedState() {
    rebuildCurrentStage();
}

void eHephaestusHelpAction::rebuildCurrentStage() {
    if(state() != eCharacterActionState::running) return;
    switch(mStage) {
    case eHephaestusHelpStage::providing:
        spawnProvideMissile();
        return;
    case eHephaestusHelpStage::none:
        eGodAction::resumeFromSavedState();
        return;
    case eHephaestusHelpStage::appear:
        appear();
        return;
    case eHephaestusHelpStage::provide:
        decide();
        return;
    case eHephaestusHelpStage::disappear:
        disappear();
        return;
    }
}

void eHephaestusHelpAction::finishProviding() {
    mStage = mPreProvidingStage;
    mPreProvidingStage = eHephaestusHelpStage::none;
}

bool eHephaestusHelpAction::sHelpNeeded(const eCityId cid,
                                        const eGameBoard& board) {
    return board.invasionToDefend(cid);
}

void eHephaestusHelpAction::provide() {
    mPreProvidingStage = mStage;
    mStage = eHephaestusHelpStage::providing;
    pauseAction();
    if(!mTalosSpawned) {
        auto& board = eHephaestusHelpAction::board();
        const auto c = character();
        const auto talos = e::make_shared<eTalos>(board);
        const auto tile = c->tile();
        talos->changeTile(tile);
        const auto da = e::make_shared<eDefendCityAction>(talos.get());
        talos->setAction(da);
        mTalosSpawned = true;
    }
    spawnProvideMissile();
}

void eHephaestusHelpAction::spawnProvideMissile() {
    auto& board = eHephaestusHelpAction::board();
    const auto c = character();
    const auto cid = cityId();
    const auto p = board.palace(cid);
    const int bw = board.width();
    const int bh = board.height();
    const auto centerTile = board.dtile(bw/2, bh/2);
    const auto targetTile = p ? p->centerTile() : centerTile;
    const auto finishCb = std::make_shared<eHfHA_provideFinish>(board, this);
    spawnGodMissile(eCharacterActionType::bless,
                    c->type(), targetTile,
                    eGodSound::santcify, nullptr,
                    finishCb);
}
