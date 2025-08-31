#ifndef EWARNING_H
#define EWARNING_H

#include "engine/edate.h"

class eGameEvent;

enum class eCityId;

class eWarning {
public:
    eWarning(const int warningMonths,
             const bool initialWarning,
             eGameEvent& parent,
             const eCityId cid,
             eGameBoard& board);

    virtual void trigger() = 0;

    bool isInitialWarning() const { return mInitialWarning; }

    int warningMonths() const { return mWarningMonths; }
    void setWarningMonths(const int ms) { mWarningMonths = ms; }

    const eDate& nextDate() const { return mNextDate; }

    void setNextDate(const eDate& date);
    void handleNewDate(const eDate& date);

    eGameEvent& parent() const { return mParent; }

    eCityId cityId() const { return mCid; }
    eGameBoard& board() const { return mBoard; }

    void setFinished(const bool f) { mFinished = f; }

    virtual void write(eWriteStream& dst) const;
    virtual void read(eReadStream& src);
private:
    eGameEvent& mParent;
    const eCityId mCid;
    eGameBoard& mBoard;
    const bool mInitialWarning;

    int mWarningMonths = 2;
    eDate mNextDate;
    bool mFinished = true;
};

#endif // EWARNING_H
