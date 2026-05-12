#include "ezeushelpaction.h"
#include "fileIO/esavearchive.h"

#include "../ewaitaction.h"
#include "gameEvents/invasions/invasion-handler.h"

eZeusHelpAction::eZeusHelpAction(
        eCharacter* const c) :
    eGodAction(c, eCharActionType::zeusHelpAction) {}

bool eZeusHelpAction::decide() {
    switch(mStage) {
    case eZeusHelpStage::none:
        mStage = eZeusHelpStage::appear;
        appear();
        break;
    case eZeusHelpStage::appear:
        mStage = eZeusHelpStage::kill;
        kill();
        break;
    case eZeusHelpStage::kill:
        mStage = eZeusHelpStage::disappear;
        disappear();
        break;
    case eZeusHelpStage::disappear:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

void eZeusHelpAction::read(eReadStream& src) {
    eGodAction::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eZeusHelpAction::write(eWriteStream& dst) const {
    eGodAction::write(dst);
    eSaveArchive ar(dst);
    const_cast<eZeusHelpAction*>(this)->serialize(ar);
}

void eZeusHelpAction::serializeJson(eJsonArchive& ar) {
    eGodAction::serializeJson(ar);
    ar.field("mStage", mStage);
}

void eZeusHelpAction::serialize(eSaveArchive& ar) {
    ar.field("mStage", mStage);
}

bool eZeusHelpAction::sHelpNeeded(const eCityId cid,
                                  const eGameBoard& board) {
    const auto& ivs = board.invasionHandlers(cid);
    return !ivs.empty();
}

void eZeusHelpAction::kill() {
    const auto c = character();
    c->setActionType(eCharacterActionType::fight2);
    const auto a = e::make_shared<eWaitAction>(c);
    using eF = eZHA_killFinish;
    const auto cid = cityId();
    const auto finish = std::make_shared<eF>(board(), cid);
    a->setFailAction(finish);
    a->setFinishAction(finish);
    a->setTime(500);
    setCurrentAction(a);
}

void eZHA_killFinish::call() {
    const auto& b = board();
    const auto& ivs = b.invasionHandlers(mCid);
    for(const auto iv : ivs) {
        iv->killAllWithCorpse();
    }
}
