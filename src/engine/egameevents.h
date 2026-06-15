#ifndef EGAMEEVENTS_H
#define EGAMEEVENTS_H

#include "gameEvents/egameevent.h"

class SaveArchive;

class eGameEvents {
public:
    eGameEvents(const eCityId cid, GameBoard& board);

    void addEvent(const stdsptr<eGameEvent>& e);
    void removeEvent(const stdsptr<eGameEvent>& e);
    void setupStartDate(const eDate& date);
    void fastForward(const eDate& date);
    void handleNewDate(const eDate& date);
    void clearAfterLastEpisode();
    bool handleEpisodeCompleteEvents();

    void loadResources();

    void clear();
    void serialize(SaveArchive& ar);
private:

    const eCityId mCid;
    GameBoard& mBoard;
    std::vector<stdsptr<eGameEvent>> mGameEvents;
};

#endif // EGAMEEVENTS_H
