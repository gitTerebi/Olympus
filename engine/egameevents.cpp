#include "egameevents.h"

#include <algorithm>
#include "evectorhelpers.h"
#include "e-game-board.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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
    eSaveArchive ar(dst);
    const_cast<eGameEvents*>(this)->serialize(ar);
}

void eGameEvents::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eGameEvents::serialize(eSaveArchive& ar) {
    int nevs;
    if(ar.writing()) nevs = mGameEvents.size();
    ar.field("nevs", nevs);
    for(int i = 0; i < nevs; i++) {
        eGameEventType type;
        if(ar.writing()) type = mGameEvents[i]->type();
        ar.field("type", type);
        if(ar.reading()) {
            const auto branch = eGameEventBranch::root;
            const auto e = eGameEvent::sCreate(mCid, type, branch, mBoard);
            e->read(ar.readStream());
            addEvent(e);
        } else {
            mGameEvents[i]->write(ar.writeStream());
        }
    }
}

void eGameEvents::serializeJson(eJsonArchive& ar) {
    int nevs = ar.reading() ? 0 : static_cast<int>(mGameEvents.size());
    ar.field("count", nevs);
    for(int i = 0; i < nevs; i++) {
        auto ca = ar.childAt("events", i);
        eGameEventType type{};
        if(!ar.reading()) type = mGameEvents[i]->type();
        ca.field("type", type);
        if(ar.reading()) {
            const auto e = eGameEvent::sCreate(mCid, type, eGameEventBranch::root, mBoard);
            e->serializeJson(ca);
            addEvent(e);
        } else {
            mGameEvents[i]->serializeJson(ca);
        }
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
