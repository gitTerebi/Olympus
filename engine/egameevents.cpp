#include "egameevents.h"

#include <algorithm>
#include "evectorhelpers.h"
#include "egameboard.h"
#include "fileIO/esavearchive.h"

eGameEvents::eGameEvents(const eCityId cid, eGameBoard& board) :
    mCid(cid), mBoard(board) {}

void eGameEvents::addEvent(const stdsptr<eGameEvent>& e) {
    mGameEvents.push_back(e);
}

void eGameEvents::removeEvent(const stdsptr<eGameEvent>& e) {
    eVectorHelpers::remove(mGameEvents, e);
}

void eGameEvents::setupStartDate(const eDate& date) {
    for(const auto& e : mGameEvents) {
        e->setupStartDate(date);
    }
}

void eGameEvents::fastForward(const eDate& date) {
    for(const auto& e : mGameEvents) {
        e->fastForward(date);
    }
}

void eGameEvents::handleNewDate(const eDate& date) {
    for(const auto& e : mGameEvents) {
        if(!e->finished() || e->hasActiveConsequences()) {
            e->handleNewDate(date);
        }
    }
    const auto end = std::remove_if(mGameEvents.begin(), mGameEvents.end(),
        [](const stdsptr<eGameEvent>& e) {
            return e->finished() && !e->hasActiveConsequences();
        });
    mGameEvents.erase(end, mGameEvents.end());
}

void eGameEvents::clearAfterLastEpisode() {
    for(const auto& e : mGameEvents) {
        if(e->type() == eGameEventType::godQuest) continue;
        e->startingNewEpisode();
    }
    const auto end = std::remove_if(mGameEvents.begin(), mGameEvents.end(),
        [](const stdsptr<eGameEvent>& e) {
            return e->finished() && !e->hasActiveConsequences();
        });
    mGameEvents.erase(end, mGameEvents.end());
}

bool eGameEvents::handleEpisodeCompleteEvents() {
    bool result = false;
    for(const auto& e : mGameEvents) {
        const bool r = e->episodeCompleteEvent();
        if(!r) continue;
        const int rr = e->repeat();
        if(rr <= 0) continue;
        e->trigger();
        e->setRepeat(0);
        result = true;
    }
    return result;
}

void eGameEvents::write(eWriteStream& dst) const {
    dst << mGameEvents.size();
    for(const auto& e : mGameEvents) {
        dst << e->type();
        e->write(dst);
    }
}

void eGameEvents::read(eReadStream& src) {
    int nevs;
    src >> nevs;
    for(int i = 0; i < nevs; i++) {
        eGameEventType type;
        src >> type;
        const auto branch = eGameEventBranch::root;
        const auto e = eGameEvent::sCreate(mCid, type, branch, mBoard);
        e->read(src);
        addEvent(e);
    }
}

void eGameEvents::loadResources() {
    for(const auto& e : mGameEvents) {
        e->loadResources();
    }
}

void eGameEvents::clear() {
    mGameEvents.clear();
}
