#include "egodattackevent.h"

#include "engine/egameboard.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "characters/actions/egodattackaction.h"
#include "egodtraderesumesevent.h"

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
    if(mSanctuary) a->setSanctuary(mSanctuary);
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
    dst << mTypes.size();
    for(const auto t : mTypes) {
        dst << t;
    }
    dst << mRandom;
    dst << mNextId;
    dst.writeBuilding(mSanctuary);
}

void eGodAttackEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    int n;
    src >> n;
    for(int i = 0; i < n; i++) {
        eGodType t;
        src >> t;
        mTypes.push_back(t);
    }
    src >> mRandom;
    src >> mNextId;
    const auto board = gameBoard();
    src.readBuilding(board, [this](eBuilding* const b) {
        mSanctuary = static_cast<eSanctuary*>(b);
    });
}
