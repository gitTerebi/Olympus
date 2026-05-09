#include "ethreadpool.h"

#include <chrono>

#include "evectorhelpers.h"
#include "engine/e-game-board.h"

eThreadPool::eThreadPool(eGameBoard &board) : mBoard(board) {}

eThreadPool::~eThreadPool()
{
    {
        mQuit = true;
        for (const auto d : mThreadData)
        {
            {
                std::unique_lock<std::mutex> lock(d->fTasksMutex);
            }
            d->fCv.notify_all();
        }
    }
    for (auto &thread : mThreads)
    {
        thread.join();
    }
}

void eThreadPool::initialize(const int w, const int h)
{
    if (mThreads.empty())
    {
        const int hc = std::thread::hardware_concurrency();
        const int threads = std::clamp(hc, 2, 4);
        for (int i = 0; i < threads; i++)
        {
            const auto b = new eThreadData;
            b->initialize(w, h);
            mThreadData.push_back(b);
            mThreads.emplace_back(std::bind(&eThreadPool::threadEntry, this, b));
        }
    }
    else
    {
        for (auto &t : mThreadData)
        {
            t->initialize(w, h);
        }
    }
}

void eThreadPool::addBoard(const eCityId cid)
{
    waitFinished();
    for (auto &t : mThreadData)
    {
        t->addBoard(cid);
    }
}

void eThreadPool::removeBoard(const eCityId cid)
{
    waitFinished();
    for (auto &t : mThreadData)
    {
        t->removeBoard(cid);
    }
}

// double gThreadedTime = 0.;

void eThreadPool::threadEntry(eThreadData *data)
{
    eTask *task = nullptr;
    while (!mQuit)
    {
        {
            std::unique_lock<std::mutex> lock(data->fTasksMutex);

            while (!mQuit && data->fTasks.empty())
            {
                data->fCv.wait(lock);
            }

            if (data->fTasks.empty())
                return;

            data->fBusy = true;
            task = data->fTasks.front();
            data->fTasks.pop();
        }
        if (task)
        {
            {
                const auto cid = task->cid();
                data->updateBoard(cid);
                auto &b = data->board(cid);
                task->run(b);
            }
            {
                std::lock_guard lock(mFinishedTasksMutex);
                mFinishedTasks.push_back(task);
            }
            {
                std::unique_lock<std::mutex> lock(data->fTasksMutex);
                data->fBusy = false;
            }
            data->fCvFinished.notify_one();
        }
    }
}

void eThreadPool::queueTask(eTask *const task)
{
    task->setExpectedState(mBoard.state());
    const auto cid = task->cid();
    const int threadId = mTaskId++ % mThreadData.size();
    const auto d = mThreadData[threadId];
    d->update(mBoard, cid, task->relevance());

    std::unique_lock<std::mutex> lock(d->fTasksMutex);
    d->fTasks.emplace(task);
    d->fCv.notify_one();
}

void eThreadPool::handleFinished()
{
    std::vector<eTask *> tasks;
    {
        std::lock_guard lock(mFinishedTasksMutex);
        std::swap(tasks, mFinishedTasks);
    }
    for (const auto t : tasks)
    {
        t->finish();
        delete t;
    }
}

void eThreadPool::scheduleDataUpdate()
{
    mBoard.incState();
}

bool eThreadPool::finished()
{
    for (const auto d : mThreadData)
    {
        std::lock_guard lock(d->fTasksMutex);
        const bool f = d->fTasks.empty() && !d->fBusy;
        if (!f)
            return false;
    }
    return true;
}

void eThreadPool::waitFinished()
{
    for (const auto d : mThreadData)
    {
        std::unique_lock<std::mutex> lock(d->fTasksMutex);
        d->fCvFinished.wait(lock, [d]()
                            { return d->fTasks.empty() && !d->fBusy; });
    }
}
