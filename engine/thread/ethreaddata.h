#ifndef ETHREADDATA_H
#define ETHREADDATA_H

#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "engine/thread/ethreadboardhandler.h"

class eGameBoard;
class eTask;

struct eTrueBool {
    bool fIni = true;
    bool fV = true;
};

class eThreadData {
public:
    eThreadData() {}

    void initialize(const int w, const int h);
    void addBoard(const eCityId cid);

    void updateBoard(const eCityId cid);
    eThreadBoard& board(const eCityId cid);

    void scheduleUpdate(eGameBoard& board);
    void iniScheduleUpdate(eGameBoard& board, const eCityId cid);
    void scheduleUpdate(eGameBoard& board, const eCityId cid);

    void setRunning(const bool r);

    std::map<eCityId, eTrueBool> fDataUpdateScheduled;
    std::atomic_bool fBusy{false};
    std::mutex fTasksMutex;
    std::condition_variable fCv;
    std::condition_variable fCvFinished;
    std::queue<eTask*> fTasks;
private:
    int mW = 0;
    int mH = 0;
    std::atomic_bool mRunning{false};

    std::map<eCityId, eThreadBoardHandler> mBoards;
};

#endif // ETHREADDATA_H
