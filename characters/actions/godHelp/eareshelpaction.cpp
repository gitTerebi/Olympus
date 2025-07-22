#include "eareshelpaction.h"

#include "characters/actions/emovetoaction.h"
#include "characters/actions/ekillcharacterfinishfail.h"

eAresHelpAction::eAresHelpAction(eCharacter* const c) :
    eGodAction(c, eCharActionType::aresHelpAction) {}

bool eAresHelpAction::decide() {
    switch(mStage) {
    case eAresHelpStage::none:
        mStage = eAresHelpStage::appear;
        appear();
        break;
    case eAresHelpStage::appear:
        mStage = eAresHelpStage::go;
        goToTarget();
        break;
    case eAresHelpStage::go:
        const auto c = character();
        c->kill();
        break;
    }
    return true;
}

bool eAresHelpAction::sHelpNeeded(const ePlayerId pid,
                                  const eGameBoard& board) {
    const auto& cs = board.conquests(pid);
    return !cs.empty();
}

void eAresHelpAction::read(eReadStream& src) {
    eGodAction::read(src);
    src >> mStage;
}

void eAresHelpAction::write(eWriteStream& dst) const {
    eGodAction::write(dst);
    dst << mStage;
}

void eAresHelpAction::goToTarget() {
    const auto c = character();
    auto& board = eGodAction::board();
    const auto cid = cityId();
    const auto god = static_cast<eGod*>(c);
    const stdptr<eGod> cptr(god);
    const auto fail = std::make_shared<eKillCharacterFinishFail>(
                          board, god);
    const auto finish = std::make_shared<eKillCharacterFinishFail>(
                            board, god);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::buildings |
                         eStateRelevance::terrain);
    a->setFailAction(fail);
    a->setFinishAction(finish);
    a->setFindFailAction([cptr]() {
        if(cptr) cptr->kill();
    });
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);

    const auto exitPoint = board.exitPoint(cid);
    if(exitPoint) {
        a->start(exitPoint);
    } else {
        const auto edgeTile = [](eTileBase* const tile) {
            return tile->isCityEdge();
        };
        a->start(edgeTile);
    }
}
