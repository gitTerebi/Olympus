#ifndef EINVASIONEVENT_H
#define EINVASIONEVENT_H

#include "egameevent.h"
#include "epointeventvalue.h"
#include "ecityeventvalue.h"
#include "ecounteventvalue.h"

#include "engine/eworldcity.h"
#include "characters/eenlistedforces.h"

class eInvasionHandler;
class ePlayerConquestEvent;
class eInvasionWarning;

class eInvasionEvent : public eGameEvent,
                       public ePointEventValue,
                       public eCityEventValue,
                       public eCountEventValue {
public:
    eInvasionEvent(const eCityId cid,
                   const eGameEventBranch branch,
                   eGameBoard& board);
    ~eInvasionEvent();

    void pointerCreated() override;

    void initialize(const stdsptr<eWorldCity>& city,
                    const int count, const ePlayerId sentBy =
                        ePlayerId::neutralFriendly);
    void initialize(const stdsptr<eWorldCity>& city,
                    const eEnlistedForces& forces,
                    ePlayerConquestEvent* const conquestEvent);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

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

    ePlayerConquestEvent* conquestEvent() const;

    const eEnlistedForces& forces() const
    { return mForces; }

    void invadersWon();
    void invadersDefeated();
private:
    void soldiersByType(int& infantry,
                        int& cavalry,
                        int& archers) const;
    int bribeCost() const;
    void updateDisembarkAndShoreTile();

    eInvasionWarning* mInitialWarning = nullptr;

    std::vector<eInvasionHandler*> mHandlers;

    bool mHardcoded = true;

    stdptr<ePlayerConquestEvent> mConquestEvent;
    eEnlistedForces mForces;

    eTile* mDisembarkTile = nullptr;
    eTile* mShoreTile = nullptr;

    bool mWarned = false;
    eDate mFirstWarning;

    ePlayerId mSentByPlayer = ePlayerId::neutralFriendly;
};

#endif // EINVASIONEVENT_H
