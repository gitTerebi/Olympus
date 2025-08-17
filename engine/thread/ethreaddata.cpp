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

void eThreadData::removeBoard(const eCityId cid) {
    mBoards.erase(cid);
}

void eThreadData::updateBoard(const eCityId cid) {
    mBoards[cid].updateBoard();
}

eThreadBoard& eThreadData::board(const eCityId cid) {
    auto& bh = mBoards[cid];
//    std::printf("Access board %p\n", &bh);
    return bh.board();
}

void eThreadData::update(eGameBoard& board, const eCityId cid,
                         const eStateRelevance rel) {
    auto& b = mBoards[cid];
    b.update(board, cid, rel);
}
