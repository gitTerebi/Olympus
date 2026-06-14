#include "egodattackevent.h"

#include "engine/game-board.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "characters/gods/actions/god-attack-action.h"
#include "gameEvents/gods/egodtraderesumesevent.h"
#include "fileIO/esavearchive.h"

eGodAttackEvent::eGodAttackEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        GameBoard& board) :
    eGameEvent(cid, eGameEventType::godAttack, branch, board) {}

void eGodAttackEvent::setSanctuary(const stdptr<eSanctuary>& s) {
    mSanctuary = s;
}

void eGodAttackEvent::setTypes(const std::vector<GodType>& types) {
    mTypes = types;
    const int nTypes = mTypes.size();
    if(mNextId >= nTypes) mNextId = 0;
}

void eGodAttackEvent::setRandom(const bool r) {
    mRandom = r;
}

void eGodAttackEvent::loadResources() const {
    eGameEvent::loadResources();
    for(const auto g : mTypes) {
        God::sLoadTextures(g);
    }
}

void eGodAttackEvent::trigger() {
    if(mTypes.empty()) return;
    const auto board = gameBoard();
    if(!board) return;
    int tid;
    const int nTypes = mTypes.size();
    if(mRandom) {
        tid = eRand::rand() % mTypes.size();
    } else {
        tid = mNextId;
        if(++mNextId >= nTypes) mNextId = 0;
    }
    const auto t = mTypes.at(tid);
    const auto god = God::sCreateGod(t, *board);
    god->setOnCityId(cityId());
    god->setCityId(eCityId::neutralAggresive);

    const auto a = e::make_shared<GodAttackAction>(god.get());
    god->setAttitude(GodAttitude::hostile);
    god->setAction(a);
    a->increment(1);
    const auto cid = cityId();
    eEventData ed(cid);
    ed.fChar = god.get();
    ed.fTile = god->tile();
    ed.fGod = t;
    board->registerAttackingGod(cid, god.get());
    board->event(eEvent::godInvasion, ed);
    if(mSanctuary) {
        a->setSanctuary(mSanctuary);
        const auto sCid = mSanctuary->cityId();
        const auto sPid = board->cityIdToPlayerId(sCid);
        const auto ppid = board->personPlayer();
        if(sPid == ppid) {
            eEventData ed(sCid);
            ed.fChar = god.get();
            ed.fTile = god->tile();
            board->event(eEvent::playerGodAttack, ed);
        }
    }
    if(t == GodType::zeus) {
        board->setLandTradeShutdown(cid, true);
        board->setSeaTradeShutdown(cid, true);
    } else if(t == GodType::poseidon) {
        board->setSeaTradeShutdown(cid, true);
    } else if(t == GodType::hermes) {
        board->setLandTradeShutdown(cid, true);
    }
    if(t == GodType::zeus ||
       t == GodType::poseidon ||
       t == GodType::hermes) {
        const auto e = e::make_shared<eGodTradeResumesEvent>(
                           cityId(), eGameEventBranch::child, *board);
        e->setGod(t);
        e->initializeDate(board->date() + 365);
        addConsequence(e);
    }
}

std::string eGodAttackEvent::longName() const {
    return eLanguage::zeusText(156, 27);
}

void eGodAttackEvent::serializeFields(eSaveArchive& ar) {
    eGameEvent::serializeFields(ar);
    ar.arrayField("types", mTypes, [](eSaveArchive& ar, GodType& t) {
        ar.field("t", t);
    });
    ar.field("random", mRandom, false);
    ar.field("nextId", mNextId, 0);
    ar.buildingAsField("sanctuary", gameBoard(), mSanctuary);
}
