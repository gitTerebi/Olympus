#include "ethreaddata.h"

#include "engine/egameboard.h"

#include <chrono>

void eThreadData::initialize(const int w, const int h) {
    for(auto& b : mBoards) {
        b.second.initialize(w, h);
    }
    mW = w;
    mH = h;
}

void eThreadData::addBoard(const eCityId cid) {
    auto& b = mBoards[cid];
    b.initialize(mW, mH);
}

void eThreadData::updateBoard(const eCityId cid) {
    mBoards[cid].updateBoard();
}

eThreadBoard& eThreadData::board(const eCityId cid) {
    return mBoards[cid].board();
}

void eThreadData::scheduleUpdate(eGameBoard& board) {
    for(auto& b : mBoards) {
        b.second.scheduleUpdate(board);
    }
}

void eThreadData::scheduleUpdate(eGameBoard& board, const eCityId cid) {
    auto& b = mBoards[cid];
    b.scheduleUpdate(board, cid);
}

void eThreadData::setRunning(const bool r) {
    mRunning = r;
}
