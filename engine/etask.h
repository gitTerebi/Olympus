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

    bool ai() const { return mAI; }
    void setAI(const bool ai) { mAI = ai; }

    bool runInterrupted() const { return mRunInterrupted; }
    void setRunInterrupted(const bool r);

    bool shouldDelete() const { return mShouldDelete; }
    void setShouldDelete(const bool s) { mShouldDelete = s; }

    int expectedState() const { return mExpectedState; }
    void setExpectedState(const int state) { mExpectedState = state; }
protected:
    virtual void run(eThreadBoard& data,
                     const std::atomic_bool& interrupt) {
        (void)interrupt;
        run(data);
    }
    virtual void run(eThreadBoard& data) { (void)data; }
    virtual void finish() {}
private:
    bool mAI = false;
    bool mShouldDelete = true;
    bool mRunInterrupted = true;
    int mExpectedState;
    const eCityId mCid;
};

#endif // ETASK_H
