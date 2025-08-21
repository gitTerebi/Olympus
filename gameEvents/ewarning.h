#ifndef EWARNING_H
#define EWARNING_H

#include "engine/edate.h"

class eGameEvent;

enum class eCityId;

class eWarning {
public:
    eWarning(const int warningDays,
             eGameEvent& parent,
             const eCityId cid,
             eGameBoard& board);

    virtual void trigger() = 0;

    int warningDays() const { return mWarningDays; }
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
    const int mWarningDays;
    eGameEvent& mParent;
    const eCityId mCid;
    eGameBoard& mBoard;

    eDate mNextDate;
    bool mFinished = true;
};

#endif // EWARNING_H
