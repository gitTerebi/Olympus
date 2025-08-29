#include "egameevents.h"

#include "evectorhelpers.h"
#include "egameboard.h"

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

void eGameEvents::handleNewDate(const eDate& date) {
    for(int i = 0; i < (int)mGameEvents.size(); i++) {
        const auto& e = mGameEvents[i];
        if(e->finished() && !e->hasActiveConsequences()) {
            eVectorHelpers::remove(mGameEvents, e);
            i--;
        } else {
            e->handleNewDate(date);
        }
    }
}

void eGameEvents::clearAfterLastEpisode() {
    for(int i = 0; i < static_cast<int>(mGameEvents.size()); i++) {
        const auto& e = mGameEvents[i];
        const auto type = e->type();
        if(type == eGameEventType::godQuest) continue;
        e->startingNewEpisode();
        if(e->finished()) {
            if(!e->hasActiveConsequences()) {
                mGameEvents.erase(mGameEvents.begin() + i);
                i--;
            }
        }
    }
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
