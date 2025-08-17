#ifndef ETHREADDATA_H
#define ETHREADDATA_H

#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <set>

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
    void removeBoard(const eCityId cid);

    void updateBoard(const eCityId cid);
    eThreadBoard& board(const eCityId cid);

    void update(eGameBoard& board, const eCityId cid,
                const eStateRelevance rel);

    std::atomic_bool fBusy{false};
    std::mutex fTasksMutex;
    std::condition_variable fCv;
    std::condition_variable fCvFinished;
    std::queue<eTask*> fTasks;
private:    
    int mW = 0;
    int mH = 0;

    std::map<eCityId, eThreadBoardHandler> mBoards;
};

#endif // ETHREADDATA_H
