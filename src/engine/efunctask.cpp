#include "efunctask.h"

eFuncTask::eFuncTask(const eCityId cid, const eRunFunc& runFunc,
                     const eFunc& finishFunc) :
    eTask(cid), mRun(runFunc), mFinish(finishFunc) {}

void eFuncTask::run(eThreadBoard& data) {
    if(mRun) mRun(data);
}

void eFuncTask::finish() {
    if(mFinish) mFinish();
}
