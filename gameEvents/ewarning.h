#ifndef EWARNING_H
#define EWARNING_H

#include "engine/edate.h"

class eGameEvent;
class eSaveArchive;
class eWriteStream;
class eReadStream;

enum class eCityId;

class eWarning {
public:
    eWarning(const int warningMonths,
             const bool initialWarning,
             eGameEvent& parent,
             const eCityId cid,
             GameBoard& board);

    virtual void trigger() = 0;

    bool isInitialWarning() const { return mInitialWarning; }

    int warningMonths() const { return mWarningMonths; }
    void setWarningMonths(const int ms) { mWarningMonths = ms; }

    const eDate& nextDate() const { return mNextDate; }

    void setNextDate(const eDate& date);
    void handleNewDate(const eDate& date);

    eGameEvent& parent() const { return mParent; }

    eCityId cityId() const { return mCid; }
    GameBoard& board() const { return mBoard; }

    void setFinished(const bool f) { mFinished = f; }

    void serialize(eSaveArchive& ar);
private:
    eGameEvent& mParent;
    const eCityId mCid;
    GameBoard& mBoard;
    const bool mInitialWarning;

    int mWarningMonths = 2;
    eDate mNextDate;
    bool mFinished = true;
};

#endif // EWARNING_H
