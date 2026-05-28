#include "eeventtrigger.h"

#include "egameevent.h"
#include "evectorhelpers.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

eEventTrigger::eEventTrigger(const eCityId cid,
                             const std::string& name,
                             GameBoard& board) :
    mCid(cid), mBoard(board), mName(name) {}

void eEventTrigger::trigger(eGameEvent& parent,
                            const eDate& date,
                            const std::string& reason) {
    for(const auto& e : mEvents) {
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
        c->loadResources();
    }
}

void eEventTrigger::serialize(eSaveArchive& ar) {
    ar.arrayField("events", mEvents, [this](eSaveArchive& ar, auto& e) {
        eGameEventType type;
        if(ar.writing()) {
            type = e->type();
        }
        ar.field("type", type);
        if(ar.reading()) {
            const auto branch = eGameEventBranch::trigger;
            e = eGameEvent::sCreate(mCid, type, branch, mBoard);
        }
        ar.archiveField("state", [&e](eSaveArchive& childAr) {
            e->serialize(childAr);
        });
    });
    if(ar.reading()) {
        eVectorHelpers::removeAll(mEvents, stdsptr<eGameEvent>(nullptr));
    }
}

void eEventTrigger::addEvent(const stdsptr<eGameEvent>& e) {
    if(!e) {
        printf("Ignoring null trigger game event.\n");
        return;
    }
    mEvents.push_back(e);
}

void eEventTrigger::removeEvent(const stdsptr<eGameEvent>& e) {
    eVectorHelpers::remove(mEvents, e);
}

eWorldBoard* eEventTrigger::worldBoard() const {
    return &mBoard.world();
}
