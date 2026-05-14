#include "egodvisitevent.h"

#include "engine/e-game-board.h"
#include "engine/eevent.h"
#include "engine/eeventdata.h"
#include "characters/gods/actions/egodvisitaction.h"
#include "elanguage.h"
#include "fileIO/esavearchive.h"

eGodVisitEvent::eGodVisitEvent(const eCityId cid,
                               const eGameEventBranch branch,
                               eGameBoard& board) :
    eGameEvent(cid, eGameEventType::godVisit, branch, board) {}

void eGodVisitEvent::setTypes(const std::vector<eGodType>& types) {
    mTypes = types;
    const int nTypes = mTypes.size();
    if(mNextId >= nTypes) mNextId = 0;
}

void eGodVisitEvent::setRandom(const bool r) {
    mRandom = r;
}

void eGodVisitEvent::loadResources() const {
    eGameEvent::loadResources();
    for(const auto g : mTypes) {
        eGod::sLoadTextures(g);
    }
}

void eGodVisitEvent::trigger() {
    const auto board = gameBoard();
    if(!board) return;
    std::vector<eGodType> types;
    const auto cid = cityId();
    for(const auto t : mTypes) {
        const auto s = board->sanctuary(cid, t);
        if(!s || !s->finished()) {
            types.push_back(t);
        }
    }
    if(types.empty()) return;
    int tid;
    const int nTypes = types.size();
    if(mRandom) {
        tid = eRand::rand() % types.size();
    } else {
        tid = mNextId;
        if(tid >= nTypes) {
            tid = 0;
            mNextId = 0;
        }
        if(++mNextId >= nTypes) {
            mNextId = 0;
        }
    }
    const auto t = types.at(tid);
    const auto god = eGod::sCreateGod(t, *board);
    god->setOnCityId(cityId());
    god->setCityId(eCityId::neutralFriendly);

    const auto a = e::make_shared<eGodVisitAction>(god.get());
    god->setAction(a);
    god->setAttitude(eGodAttitude::friendly);
    a->increment(1);
    eEventData ed(cityId());
    ed.fChar = god.get();
    ed.fTile = god->tile();
    ed.fGod = t;
    board->event(eEvent::godVisit, ed);
}

std::string eGodVisitEvent::longName() const {
    return eLanguage::text("god_visit");
}

void eGodVisitEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    eSaveArchive ar(dst);
    const_cast<eGodVisitEvent*>(this)->serialize(ar);
}

void eGodVisitEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eGodVisitEvent::serialize(eSaveArchive& ar) {
    ar.arrayField("types", mTypes, [](eSaveArchive& ar, eGodType& t) {
        ar.field("t", t);
    });
    ar.field("mRandom", mRandom);
    ar.field("mNextId", mNextId);
}
