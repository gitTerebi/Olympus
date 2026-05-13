#include "eeventtrigger.h"

#include "egameevent.h"
#include "evectorhelpers.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

#include <cstdio>

eEventTrigger::eEventTrigger(const eCityId cid,
                             const std::string& name,
                             eGameBoard& board) :
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

void eEventTrigger::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eEventTrigger*>(this)->serialize(ar);
}

void eEventTrigger::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eEventTrigger::serialize(eSaveArchive& ar) {
    int ncs = mEvents.size();
    ar.field("ncs", ncs);
    for(int i = 0; i < ncs; i++) {
        eGameEventType type;
        if(ar.writing()) {
            type = mEvents[i]->type();
        }
        ar.field("type", type);
        if(ar.writing()) {
            mEvents[i]->write(ar.writeStream());
            continue;
        }
        const auto branch = eGameEventBranch::trigger;
        const auto e = eGameEvent::sCreate(mCid, type, branch, mBoard);
        printf("Deprecated binary trigger event read skipped; JSON serializeJson should be used\n");
        mEvents.emplace_back(e);
    }
}

void eEventTrigger::addEvent(const stdsptr<eGameEvent>& e) {
    mEvents.push_back(e);
}

void eEventTrigger::removeEvent(const stdsptr<eGameEvent>& e) {
    eVectorHelpers::remove(mEvents, e);
}

eWorldBoard* eEventTrigger::worldBoard() const {
    return &mBoard.world();
}

void eEventTrigger::serializeJson(eJsonArchive& ar) {
    int n = static_cast<int>(mEvents.size());
    ar.field("n", n);
    if(ar.reading()) mEvents.clear();
    for(int i = 0; i < n; i++) {
        eGameEventType type{};
        if(ar.writing()) type = mEvents[i]->type();
        const auto key = std::to_string(i);
        ar.field((key + ".type").c_str(), type);
        auto ca = ar.childAt("events", i);
        if(ar.reading()) {
            const auto e = eGameEvent::sCreate(mCid, type, eGameEventBranch::trigger, mBoard);
            if(!e) continue;
            e->serializeJson(ca);
            mEvents.emplace_back(e);
        } else {
            mEvents[i]->serializeJson(ca);
        }
    }
}
