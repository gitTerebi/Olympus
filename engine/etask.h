#ifndef ETASK_H
#define ETASK_H

enum class eCityId;

class eThreadBoard;

class eTask {
    friend class eThreadPool;
public:
    eTask(const eCityId cid);
    virtual ~eTask();

    eCityId cid() const { return mCid; }
protected:
    virtual void run(eThreadBoard& data) { (void)data; }
    virtual void finish() {}
private:
    const eCityId mCid;
};

#endif // ETASK_H
