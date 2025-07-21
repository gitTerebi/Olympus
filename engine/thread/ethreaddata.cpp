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
    auto& bh = mBoards[cid];
//    std::printf("Access board %p\n", &bh);
    return bh.board();
}

void eThreadData::scheduleUpdate(eGameBoard& board) {
    for(auto& b : mBoards) {
        b.second.scheduleUpdate(board);
    }
}

void eThreadData::iniScheduleUpdate(eGameBoard& board, const eCityId cid) {
    auto& b = mBoards[cid];
    b.scheduleUpdate(board);
    b.updateBoard();
    b.scheduleUpdate(board);
}

void eThreadData::scheduleUpdate(eGameBoard& board, const eCityId cid) {
    auto& b = mBoards[cid];
    b.scheduleUpdate(board, cid);
}
