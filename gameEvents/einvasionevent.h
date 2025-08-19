#ifndef EINVASIONEVENT_H
#define EINVASIONEVENT_H

#include "egameevent.h"
#include "epointeventbase.h"
#include "ecityevent.h"
#include "ecountevent.h"

#include "engine/eworldcity.h"
#include "characters/eenlistedforces.h"

class eInvasionHandler;
class ePlayerConquestEvent;

class eInvasionEvent : public eGameEvent,
                       public ePointEventBase,
                       public eCityEvent,
                       public eCountEvent {
public:
    eInvasionEvent(const eCityId cid,
                   const eGameEventBranch branch,
                   eGameBoard& board);
    ~eInvasionEvent();

    void pointerCreated() override;

    void initialize(const stdsptr<eWorldCity>& city,
                    const int count);
    void initialize(const stdsptr<eWorldCity>& city,
                    const eEnlistedForces& forces,
                    ePlayerConquestEvent* const conquestEvent);

    void trigger() override;
    std::string longName() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;

    bool finished() const override;

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
    void defeated();

    void updateWarnings();
private:
    void soldiersByType(int& infantry,
                        int& cavalry,
                        int& archers) const;
    int bribeCost() const;
    void updateDisembarkAndShoreTile();

    std::vector<eInvasionHandler*> mHandlers;

    bool mHardcoded = true;

    stdptr<ePlayerConquestEvent> mConquestEvent;
    eEnlistedForces mForces;

    eTile* mDisembarkTile = nullptr;
    eTile* mShoreTile = nullptr;

    bool mWarned = false;
    eDate mFirstWarning;
};

#endif // EINVASIONEVENT_H
