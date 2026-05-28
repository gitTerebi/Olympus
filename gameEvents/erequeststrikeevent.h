#ifndef EREQUESTSTRIKEEVENT_H
#define EREQUESTSTRIKEEVENT_H

#include "egameevent.h"

class eSaveArchive;

class eRequestStrikeEvent : public eGameEvent {
public:
    eRequestStrikeEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void setEnd(const int e) { mEnd = e; }
    bool end() const { return mEnd; }

    void setCity(const stdsptr<eWorldCity>& c) { mCity = c; }
    const stdsptr<eWorldCity>& city() const { return mCity; }

    void setRivalCity(const stdsptr<eWorldCity>& c) { mRivalCity = c; }
    const stdsptr<eWorldCity>& rivalCity() const { return mRivalCity; }
protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    bool mEnd = false;
    stdsptr<eWorldCity> mCity;
    stdsptr<eWorldCity> mRivalCity;
};

#endif // EREQUESTSTRIKEEVENT_H
