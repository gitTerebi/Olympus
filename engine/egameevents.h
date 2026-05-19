#ifndef EGAMEEVENTS_H
#define EGAMEEVENTS_H

#include "gameEvents/egameevent.h"

class eSaveArchive;

class eGameEvents {
public:
    eGameEvents(const eCityId cid, eGameBoard& board);

    void addEvent(const stdsptr<eGameEvent>& e);
    void removeEvent(const stdsptr<eGameEvent>& e);
    void setupStartDate(const eDate& date);
    void fastForward(const eDate& date);
    void handleNewDate(const eDate& date);
    void clearAfterLastEpisode();
    bool handleEpisodeCompleteEvents();

    void loadResources();

    void clear();
    void serialize(eSaveArchive& ar);
private:

    const eCityId mCid;
    eGameBoard& mBoard;
    std::vector<stdsptr<eGameEvent>> mGameEvents;
};

#endif // EGAMEEVENTS_H
