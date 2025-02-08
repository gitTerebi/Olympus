#ifndef ETHREADBOARDHANDLER_H
#define ETHREADBOARDHANDLER_H

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "engine/thread/ethreadboard.h"

class eThreadBoardHandler {
public:
    eThreadBoardHandler();

    void initialize(const int w, const int h);

    void scheduleUpdate(eGameBoard& board);
    void scheduleUpdate(eGameBoard& board, const eCityId cid);

    void updateBoard();

    eThreadBoard& board() { return mBoard; }
private:
    eThreadBoard mBoard;
    std::atomic_bool mTmpChanged{false};
    eThreadBoard mTmpBoard;
    std::mutex mTmpBoardMutex;
};

#endif // ETHREADBOARDHANDLER_H
