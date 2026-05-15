#include "eeventtrigger.h"

#include "egameevent.h"
#include "evectorhelpers.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

eEventTrigger::eEventTrigger(const eCityId cid,
                             const std::string& name,
                             eGameBoard& board) :
    mCid(cid), mBoard(board), mName(name) {}

void eEventTrigger::trigger(eGameEvent& parent,
                            const eDate& date,
                            const std::string& reason) {
    for(const auto& e : mEvents) {
        if(!e) continue;
        const auto c = e->makeCopy();
        if(!c) continue;
        c->setReason(reason);
        c->setRepeat(1);
        c->setupStartDate(date);
        parent.addConsequence(c);
    }
}

void eEventTrigger::loadResources() const {
    for(const auto& c : mEvents) {
        if(!c) continue;
        c->loadResources();
    }
}

void eEventTrigger::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eEventTrigger*>(this)->serialize(ar);
}

void eEventTrigger::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eEventTrigger::serialize(eSaveArchive& ar) {
    if(ar.writing()) {
        eVectorHelpers::removeAll(mEvents, stdsptr<eGameEvent>(nullptr));
    }
    ar.arrayField("events", mEvents, [this](eSaveArchive& ar, auto& e) {
        eGameEventType type;
        if(ar.writing()) {
            type = e->type();
        }
        ar.field("type", type);
        if(ar.writing()) {
            ar.object(e);
        } else {
            const auto branch = eGameEventBranch::trigger;
            e = eGameEvent::sCreate(mCid, type, branch, mBoard);
            ar.object(e);
        }
    });
    if(!ar.writing()) {
        eVectorHelpers::removeAll(mEvents, stdsptr<eGameEvent>(nullptr));
    }
}

void eEventTrigger::addEvent(const stdsptr<eGameEvent>& e) {
    if(!e) return;
    mEvents.push_back(e);
}

void eEventTrigger::removeEvent(const stdsptr<eGameEvent>& e) {
    eVectorHelpers::remove(mEvents, e);
}

eWorldBoard* eEventTrigger::worldBoard() const {
    return &mBoard.world();
}
