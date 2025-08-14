#include "eeventtrigger.h"

#include "egameevent.h"
#include "evectorhelpers.h"
#include "engine/egameboard.h"

eEventTrigger::eEventTrigger(const eCityId cid,
                             const std::string& name,
                             eGameBoard& board) :
    mCid(cid), mBoard(board), mName(name) {}

void eEventTrigger::trigger(eGameEvent& parent,
                            const eDate& date,
                            const std::string& reason) {
    for(const auto& e : mEvents) {
        const auto c = e->makeCopy();
        c->setReason(reason);
        if(!c) continue;
        const int delay = e->period();
        const auto d = date + delay;
        const int period = e->period();
        const int repeat = e->repeat();
        c->initializeDate(d, period, repeat);
        parent.addConsequence(c);
    }
}

void eEventTrigger::write(eWriteStream& dst) const {
    dst << mEvents.size();
    for(const auto& c : mEvents) {
        dst << c->type();
        c->write(dst);
    }
}

void eEventTrigger::read(eReadStream& src) {
    int ncs;
    src >> ncs;
    for(int i = 0; i < ncs; i++) {
        eGameEventType type;
        src >> type;
        const auto branch = eGameEventBranch::trigger;
        const auto e = eGameEvent::sCreate(mCid, type, branch, mBoard);
        e->read(src);
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
