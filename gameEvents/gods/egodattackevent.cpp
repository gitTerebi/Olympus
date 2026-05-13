#include "egodattackevent.h"

#include "engine/e-game-board.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "characters/gods/actions/egodattackaction.h"
#include "gameEvents/gods/egodtraderesumesevent.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

eGodAttackEvent::eGodAttackEvent(
        const eCityId cid,
        const eGameEventBranch branch,
        eGameBoard& board) :
    eGameEvent(cid, eGameEventType::godAttack, branch, board) {}

void eGodAttackEvent::setSanctuary(const stdptr<eSanctuary>& s) {
    mSanctuary = s;
}

void eGodAttackEvent::setTypes(const std::vector<eGodType>& types) {
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
        eGod::sLoadTextures(g);
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
    const auto god = eGod::sCreateGod(t, *board);
    god->setOnCityId(cityId());
    god->setCityId(eCityId::neutralAggresive);

    const auto a = e::make_shared<eGodAttackAction>(god.get());
    god->setAttitude(eGodAttitude::hostile);
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
    if(t == eGodType::zeus) {
        board->setLandTradeShutdown(cid, true);
        board->setSeaTradeShutdown(cid, true);
    } else if(t == eGodType::poseidon) {
        board->setSeaTradeShutdown(cid, true);
    } else if(t == eGodType::hermes) {
        board->setLandTradeShutdown(cid, true);
    }
    if(t == eGodType::zeus ||
       t == eGodType::poseidon ||
       t == eGodType::hermes) {
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

void eGodAttackEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eGodAttackEvent*>(this)->serialize(ar);
}

void eGodAttackEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eGodAttackEvent::serialize(eSaveArchive& ar) {
    int n = mTypes.size();
    ar.field("n", n);
    if(ar.reading()) mTypes.clear();
    for(int i = 0; i < n; i++) {
        eGodType t;
        if(ar.writing()) t = mTypes[i];
        ar.field("t", t);
        if(ar.reading()) mTypes.push_back(t);
    }
    ar.field("mRandom", mRandom);
    ar.field("mNextId", mNextId);
    if(ar.reading()) {
        const auto board = gameBoard();
        ar.readStream().readBuilding(board, [this](eBuilding* const b) {
            mSanctuary = static_cast<eSanctuary*>(b);
        });
    } else {
        ar.writeStream().writeBuilding(mSanctuary);
    }
}

void eGodAttackEvent::serializeJson(eJsonArchive& ar) {
    eGameEvent::serializeJson(ar);
    int n = ar.writing() ? static_cast<int>(mTypes.size()) : 0;
    ar.field("n", n);
    if(ar.reading()) mTypes.clear();
    for(int i = 0; i < n; i++) {
        const auto key = "t" + std::to_string(i);
        if(ar.writing()) {
            eGodType t = mTypes[i];
            ar.field(key.c_str(), t);
        } else {
            eGodType t;
            ar.field(key.c_str(), t);
            mTypes.push_back(t);
        }
    }
    ar.field("mRandom", mRandom);
    ar.field("mNextId", mNextId);
    eBuilding* raw = mSanctuary;
    ar.buildingRef("mSanctuary", raw, *gameBoard());
    if(ar.reading()) mSanctuary = static_cast<eSanctuary*>(raw);
}
