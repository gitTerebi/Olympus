#include "etask.h"

eTask::eTask(const eCityId cid) : mCid(cid) {}

eTask::~eTask() {}

void eTask::setRunInterrupted(const bool r) {
    mRunInterrupted = r;
}
