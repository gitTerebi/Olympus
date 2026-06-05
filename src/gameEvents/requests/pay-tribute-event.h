#ifndef PAY_TRIBUTE_EVENT_H
#define PAY_TRIBUTE_EVENT_H

#include "../egameevent.h"

#include "engine/world-city.h"

class eSaveArchive;

class PayTributeEvent : public eGameEvent {
public:
    PayTributeEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        GameBoard& board);

    void initialize(const stdsptr<WorldCity>& c);

    void trigger() override;
    void respond(int response, eCityId city = eCityId::neutralAggresive) override;
    bool finished() const override;
    std::string longName() const override;
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    enum class eResponse {
        accept,
        postpone,
        decline
    };

    void accept(eCityId city);
    void postpone();
    void decline();

    stdsptr<WorldCity> mCity;
    bool mAwaitingResponse = false;
    bool mPostponed = false;
};

#endif // PAY_TRIBUTE_EVENT_H
