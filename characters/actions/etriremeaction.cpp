#include "etriremeaction.h"

#include "buildings/ebuilding.h"
#include "ekillcharacterfinishfail.h"
#include "characters/actions/emovetoaction.h"
#include "buildings/etriremewharf.h"
#include "characters/etrireme.h"
#include "engine/egameboard.h"

eTriremeAction::eTriremeAction(eTriremeWharf* const home,
                               eCharacter* const trireme) :
    eFightingAction(trireme, eCharActionType::triremeAction),
    mHome(home) {}

bool eTriremeAction::decide() {
    return true;
}

void eTriremeAction::increment(const int by) {
    const auto r = lookForEnemy(by);
    if(r == eLookForEnemyState::dead) return;
    eComplexAction::increment(by);
}

void eTriremeAction::read(eReadStream& src) {
    eFightingAction::read(src);
    auto& board = eTriremeAction::board();
    src.readBuilding(&board, [this](eBuilding* const b) {
        mHome = static_cast<eTriremeWharf*>(b);
    });
}

void eTriremeAction::write(eWriteStream& dst) const {
    eFightingAction::write(dst);
    dst.writeBuilding(mHome);
}

void eTriremeAction::goHome() {
    const auto c = character();

    const auto tile = mHome->triremeTile();
    const stdptr<eCharacter> cptr(c);

    const auto a = e::make_shared<eMoveToAction>(cptr.get());
    a->setStateRelevance(eStateRelevance::terrain);
    a->setFoundAction([cptr]() {
        if(!cptr) return;
        cptr->setActionType(eCharacterActionType::walk);
    });
    a->start(tile, eWalkableObject::sCreateDeepWater());
    setCurrentAction(a);
}

eTile* eTriremeAction::exitPoint() const {
    auto& board = eTriremeAction::board();
    const auto cid = cityId();
    return board.riverExitPoint(cid);
}

void eTriremeAction::goAbroad() {
    const auto c = character();
    auto& board = eFightingAction::board();
    const auto trireme = static_cast<eTrireme*>(c);
    board.deselectTrireme(trireme);
    mHome->triremeLeaving();
    const stdptr<eTrireme> cptr(trireme);
    const auto fail = std::make_shared<eKillCharacterFinishFail>(
        board, trireme);
    const auto finish = std::make_shared<eKillCharacterFinishFail>(
        board, trireme);

    const auto a = e::make_shared<eMoveToAction>(c);
    a->setStateRelevance(eStateRelevance::terrain);
    a->setFailAction(fail);
    a->setFinishAction(finish);
    a->setFindFailAction([cptr]() {
        if(cptr) cptr->kill();
    });
    setCurrentAction(a);
    c->setActionType(eCharacterActionType::walk);

    const auto exitPoint = eTriremeAction::exitPoint();
    a->start(exitPoint, eWalkableObject::sCreateDeepWater());
}

eTriremeWharf *eTriremeAction::home() const {
    return mHome.get();
}
