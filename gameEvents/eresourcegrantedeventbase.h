#ifndef ERESOURCEGRANTEDEVENTBASE_H
#define ERESOURCEGRANTEDEVENTBASE_H

#include "egameevent.h"
#include "ecityeventvalue.h"
#include "eresourceeventvalue.h"
#include "ecounteventvalue.h"

#include "engine/eevent.h"

class eSaveArchive;

class eResourceGrantedEventBase : public eGameEvent,
                                  public eCityEventValue,
                                  public eResourceEventValue,
                                  public eCountEventValue {
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
    void respond(int response, eCityId city = eCityId::neutralAggresive) override;
    bool finished() const override;

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
protected:
    bool mPostpone = true;
private:
    enum class eResponse {
        accept,
        postpone,
        decline
    };

    void accept(eCityId city);
    void postpone();
    void decline();
    void serialize(eSaveArchive& ar);

    eEvent mGiftCashAccepted;
    eEvent mGiftAccepted;
    eEvent mGiftPostponed;
    eEvent mGiftRefused;
    eEvent mGiftForfeited;
    eEvent mGiftGranted;
    eEvent mGiftLastChance;
    eEvent mGiftInsufficientSpace;
    eEvent mGiftPartialSpace;
    bool mAwaitingResponse = false;
};

#endif // ERESOURCEGRANTEDEVENTBASE_H
