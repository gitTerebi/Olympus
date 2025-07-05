#include "ethreadpool.h"

#include <chrono>

#include "evectorhelpers.h"
#include "engine/egameboard.h"

eThreadPool::eThreadPool(eGameBoard& board) :
    mBoard(board) {}

eThreadPool::~eThreadPool() {
    {
        mQuit = true;
        for(const auto d : mThreadData) {
            d->fCv.notify_all();
        }
    }
    for(auto& thread : mThreads) {
        thread.join();
    }
}

void eThreadPool::initialize(const int w, const int h) {
    if(mThreads.empty()) {
        const int hc = std::thread::hardware_concurrency();
        const int threads = std::clamp(hc, 2, 4);
        for(int i = 0; i < threads; i++) {
            const auto b = new eThreadData;
            b->initialize(w, h);
            mThreadData.push_back(b);
            mThreads.emplace_back(std::bind(&eThreadPool::threadEntry, this, b));
        }
    } else {
        for(auto& t : mThreadData) {
            t->initialize(w, h);
        }
    }
}

void eThreadPool::addBoard(const eCityId cid) {
    for(auto& t : mThreadData) {
        t->addBoard(cid);
    }
}

void eThreadPool::threadEntry(eThreadData* data) {
    eTask* task = nullptr;
    while(!mQuit) {
        {
            std::unique_lock<std::mutex> lock(data->fTasksMutex);

            while(!mQuit && data->fTasks.empty()) {
                data->fCv.wait(lock);
            }

            if(data->fTasks.empty()) return;

            data->fBusy = true;
            task = data->fTasks.front();
            data->fTasks.pop();
        }
        if(task) {
            {
//                using std::chrono::high_resolution_clock;
//                using std::chrono::duration_cast;
//                using std::chrono::duration;
//                using std::chrono::milliseconds;

//                const auto t1 = high_resolution_clock::now();

                const auto cid = task->cid();
                data->setRunning(true);
                data->updateBoard(cid);
//                std::printf("Run task %p\n", task);
                auto& b = data->board(cid);
                if(task->expectedState() > b.state()) {
                    printf("Old state!\n");
                }
                task->run(b, data->fInterrupted);
                data->setRunning(false);
                if(task->ai()) data->fAI--;

//                const auto t2 = high_resolution_clock::now();

//                const duration<double, std::milli> ms = t2 - t1;
//                printf("run task: %f ms\n", ms.count());
            }
            std::lock_guard lock(mFinishedTasksMutex);
            mFinishedTasks.push_back(task);
            data->fBusy = false;
            data->fCvFinished.notify_one();
        }
    }
}

void eThreadPool::queueTask(eTask* const task) {
    if(mInterrupted && !task->runInterrupted()) {
        mWaitingTasks.push_back(task);
        return;
    }
    task->setExpectedState(mBoard.state());
    const auto cid = task->cid();
    if(cid == eCityId::neutralAggresive ||
       cid == eCityId::neutralFriendly) {
        printf("Wrong city id\n");
    }
    eThreadData* d = nullptr;
    const bool ai = task->ai();
    if(ai && mAI > 0) {
        while(!d || d->fAI == 0) {
            const int threadId = mTaskId++ % mThreadData.size();
            d = mThreadData[threadId];
        }
    } else if(!ai && mAI > 0) {
        while(!d || d->fAI > 0) {
            const int threadId = mTaskId++ % mThreadData.size();
            d = mThreadData[threadId];
        }
    } else {
        const int threadId = mTaskId++ % mThreadData.size();
        d = mThreadData[threadId];
    }
    auto& cidV = d->fDataUpdateScheduled[cid];
//    std::printf("Que task %p in %p\n", task, d);
    if(cidV.fV) {
        cidV.fV = false;
        if(cidV.fIni) {
            cidV.fIni = false;
            d->iniScheduleUpdate(mBoard, cid);
        } else {
            d->scheduleUpdate(mBoard, cid);
        }
    }
    if(task->ai()) {
        d->fAI++;
        mAI++;
    }
    std::unique_lock<std::mutex> lock(d->fTasksMutex);
    d->fTasks.emplace(task);
    d->fCv.notify_one();
}

void eThreadPool::handleFinished() {
    std::vector<eTask*> tasks;
    {
        std::lock_guard lock(mFinishedTasksMutex);
        std::swap(tasks, mFinishedTasks);
    }
    for(const auto t : tasks) {
        if(t->ai()) mAI--;
        t->finish();
        if(t->shouldDelete()) delete t;
    }
}

void eThreadPool::scheduleDataUpdate() {
    mBoard.incState();
    for(const auto d : mThreadData) {
//        for(auto& f : d->fDataUpdateScheduled) {
//            f.second.fV = true;
//        }
        std::vector<eCityId> taskCid;
        std::queue<eTask*> tasks;
        {
            std::lock_guard lock(d->fTasksMutex);
            tasks = d->fTasks;
        }
        while(!tasks.empty()) {
            const auto task = tasks.front();
            const auto cid = task->cid();
            const bool r = eVectorHelpers::contains(taskCid, cid);
            if(!r) taskCid.push_back(cid);
            tasks.pop();
        }
        for(auto& f : d->fDataUpdateScheduled) {
            const auto cid = f.first;
            const bool r = eVectorHelpers::contains(taskCid, cid);
            if(r) {
                d->scheduleUpdate(mBoard, cid);
            } else {
                f.second.fV = true;
            }
        }
    }
}

bool eThreadPool::finished() {
    for(const auto d : mThreadData) {
        std::lock_guard lock(d->fTasksMutex);
        const bool f = d->fTasks.empty() && !d->fBusy;
        if(!f) return false;
    }
    return true;
}

void eThreadPool::waitFinished() {
    for(const auto d : mThreadData) {
        std::unique_lock<std::mutex> lock(d->fTasksMutex);
        d->fCvFinished.wait(lock, [d]() {
            return d->fTasks.empty() && !d->fBusy;
        });
    }
}

void eThreadPool::interrupt() {
    mInterrupted = true;
    for(const auto d : mThreadData) {
        d->fInterrupted = true;
    }
}

void eThreadPool::resume() {
    if(!mInterrupted) return;
    mInterrupted = false;
    for(const auto d : mThreadData) {
        d->fInterrupted = false;
    }
    for(const auto task : mWaitingTasks) {
        queueTask(task);
    }
    mWaitingTasks.clear();
}
