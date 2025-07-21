#ifndef ETASK_H
#define ETASK_H

#include <atomic>

enum class eCityId;

class eThreadBoard;

class eTask {
    friend class eThreadPool;
public:
    eTask(const eCityId cid);
    virtual ~eTask();

    eCityId cid() const { return mCid; }

    int expectedState() const { return mExpectedState; }
    void setExpectedState(const int state) { mExpectedState = state; }
protected:
    virtual void run(eThreadBoard& data) { (void)data; }
    virtual void finish() {}
private:
    bool mRunInterrupted = true;
    int mExpectedState;
    const eCityId mCid;
};

#endif // ETASK_H
