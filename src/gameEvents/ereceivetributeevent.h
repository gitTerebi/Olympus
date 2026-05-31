#ifndef ERECEIVETRIBUTEEVENT_H
#define ERECEIVETRIBUTEEVENT_H

#include "egameevent.h"

#include "engine/e-worldcity.h"

class eSaveArchive;

class eReceiveTributeEvent : public eGameEvent {
public:
    eReceiveTributeEvent(const eCityId cid,
                         const eGameEventBranch branch,
                         GameBoard& board);

    void initialize(const stdsptr<eWorldCity>& c);

    void trigger() override;
    void respond(int response, eCityId city = eCityId::neutralAggresive) override;
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

    stdsptr<eWorldCity> mCity;
};

#endif // ERECEIVETRIBUTEEVENT_H
