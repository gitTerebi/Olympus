#ifndef ETASK_H
#define ETASK_H

#include <atomic>

enum class eCityId;

class eThreadBoard;

#include "engine/estaterelevance.h"

class eTask {
    friend class eThreadPool;
public:
    eTask(const eCityId cid);
    virtual ~eTask();

    eCityId cid() const { return mCid; }

    eStateRelevance relevance() const { return mRelevance; }
    void setRelevance(const eStateRelevance rel) { mRelevance = rel; }

    int expectedState() const { return mExpectedState; }
    void setExpectedState(const int state) { mExpectedState = state; }
protected:
    virtual void run(eThreadBoard& data) { (void)data; }
    virtual void finish() {}
private:
    int mExpectedState;
    const eCityId mCid;
    eStateRelevance mRelevance = eStateRelevance::all;
};

#endif // ETASK_H
