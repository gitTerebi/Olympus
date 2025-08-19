#ifndef ERESOURCEGRANTEDEVENTBASE_H
#define ERESOURCEGRANTEDEVENTBASE_H

#include "egameevent.h"
#include "ecityevent.h"
#include "eresourceevent.h"
#include "ecountevent.h"

#include "engine/eevent.h"

class eResourceGrantedEventBase : public eGameEvent,
                                  public eCityEvent,
                                  public eResourceEvent,
                                  public eCountEvent {
public:
    eResourceGrantedEventBase(const eCityId cid,
                              const eEvent giftCashAccepted,
                              const eEvent giftAccepted,
                              const eEvent giftPostponed,
                              const eEvent giftRefused,
                              const eEvent giftForfeited,
                              const eEvent giftGranted,
                              const eEvent giftLastChance,
                              const eEvent giftInsufficientSpace,
                              const eEvent giftPartialSpace,
                              const eGameEventType type,
                              const eGameEventBranch branch,
                              eGameBoard& board);

    void initialize(const bool postpone,
                    const eResourceType res,
                    const int count,
                    const stdsptr<eWorldCity>& c);

    void trigger() override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
protected:
    bool mPostpone = true;
private:
    eEvent mGiftCashAccepted;
    eEvent mGiftAccepted;
    eEvent mGiftPostponed;
    eEvent mGiftRefused;
    eEvent mGiftForfeited;
    eEvent mGiftGranted;
    eEvent mGiftLastChance;
    eEvent mGiftInsufficientSpace;
    eEvent mGiftPartialSpace;
};

#endif // ERESOURCEGRANTEDEVENTBASE_H
