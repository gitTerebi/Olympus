#ifndef ASK_FOR_STRIKE_EVENT_H
#define ASK_FOR_STRIKE_EVENT_H

#include "../egameevent.h"

class SaveArchive;

class AskForStrikeEvent : public eGameEvent {
public:
    AskForStrikeEvent(const eCityId cid,
                        const eGameEventBranch branch,
                        GameBoard& board);

    void trigger() override;
    std::string longName() const override;

    void setEnd(const int e) { mEnd = e; }
    bool end() const { return mEnd; }

    void setCity(const stdsptr<WorldCity>& c) { mCity = c; }
    const stdsptr<WorldCity>& city() const { return mCity; }

    void setRivalCity(const stdsptr<WorldCity>& c) { mRivalCity = c; }
    const stdsptr<WorldCity>& rivalCity() const { return mRivalCity; }
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    bool mEnd = false;
    stdsptr<WorldCity> mCity;
    stdsptr<WorldCity> mRivalCity;
};

#endif // ASK_FOR_STRIKE_EVENT_H
