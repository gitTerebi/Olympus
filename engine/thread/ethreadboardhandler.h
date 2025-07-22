#ifndef ETHREADBOARDHANDLER_H
#define ETHREADBOARDHANDLER_H

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "engine/thread/ethreadboard.h"

enum class eStateRelevance;

class eThreadBoardHandler {
public:
    eThreadBoardHandler();

    void initialize(const int w, const int h);

    void update(eGameBoard& board, const eCityId cid,
                const eStateRelevance rel);

    void updateBoard();

    eThreadBoard& board() { return mBoard; }
private:
    void update(const std::vector<eTile*>& tiles);
    void updateAll(eGameBoard& board);

    bool mInitialized = false;
    eThreadBoard mBoard;
    std::atomic_bool mTmpChanged{false};
    eThreadBoard mTmpBoard;
    std::mutex mTmpBoardMutex;
};

#endif // ETHREADBOARDHANDLER_H
