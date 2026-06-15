#ifndef INVASION_EVENT_H
#define INVASION_EVENT_H

#include "../egameevent.h"
#include "../epointeventvalue.h"
#include "../ecityeventvalue.h"
#include "../ecounteventvalue.h"

#include "engine/world-city.h"
#include "characters/eenlistedforces.h"

class eInvasionHandler;
class PlayerConquestEvent;
class eInvasionWarning;
class SaveArchive;

enum class eInvasionResult {
    invaderWonOrPlayerSurrendered,
    invaderDefeated
};

class eInvasionEvent : public eGameEvent,
                       public ePointEventValue,
                       public eCityEventValue,
                       public eCountEventValue {
public:
    eInvasionEvent(const eCityId cid,
                   const eGameEventBranch branch,
                   GameBoard& board);
    ~eInvasionEvent();

    void pointerCreated() override;

    void initialize(const stdsptr<WorldCity>& city,
                    const int count, const ePlayerId sentBy =
                        ePlayerId::neutralFriendly);
    void initialize(const stdsptr<WorldCity>& city,
                    const eEnlistedForces& forces,
                    PlayerConquestEvent* const conquestEvent);
    static bool tryCreateCityInvasion(WorldCity& attacker,
                                      GameBoard& board);

    void trigger() override;
    void respond(int response, eCityId city = eCityId::neutralAggresive) override;
    std::string longName() const override;

    bool finished() const override;

    void setWarningMonths(const int ms) override;

    eTile* invasionTile() const;
    eTile* shoreTile() const { return mShoreTile; }
    eTile* landInvasionTile() const;

    void setFirstWarning(const eDate& w);
    eDate firstWarning() const { return mFirstWarning; }
    bool warned() const { return mWarned; }

    bool hardcoded() const { return mHardcoded; }
    void setHardcoded(const bool h) { mHardcoded = h; }

    bool activeInvasions() const;
    void addInvasionHandler(eInvasionHandler* const i);
    void removeInvasionHandler(eInvasionHandler* const i);

    bool nearestSoldier(const int fromX, const int fromY,
                        int& toX,int& toY) const;

    PlayerConquestEvent* conquestEvent() const;

    const eEnlistedForces& forces() const
    { return mForces; }

    void invadersWon();
    void invadersDefeated();
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    enum class eResponse {
        surrender,
        bribe,
        fight
    };

    void surrender();
    void bribe();
    void fight();
    void useGeneratedCityWarnings();
    void sendInitialAnnouncement();

    void soldiersByType(int& infantry,
                        int& cavalry,
                        int& archers) const;
    void restoreAttitudeAfterInvasion(const eInvasionResult result);
    int bribeCost() const;
    void updateDisembarkAndShoreTile();

    eInvasionWarning* mInitialWarning = nullptr;

    std::vector<eInvasionHandler*> mHandlers;

    bool mHardcoded = true;

    stdptr<PlayerConquestEvent> mConquestEvent;
    eEnlistedForces mForces;

    eTile* mDisembarkTile = nullptr;
    eTile* mShoreTile = nullptr;

    bool mWarned = false;
    bool mWaitingForResponse = false;
    bool mInvadersWon = false;
    eDate mFirstWarning;

    ePlayerId mSentByPlayer = ePlayerId::neutralFriendly;
};

#endif // INVASION_EVENT_H
