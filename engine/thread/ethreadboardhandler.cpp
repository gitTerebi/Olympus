#include "ethreadboardhandler.h"

#include "engine/egameboard.h"

eThreadBoardHandler::eThreadBoardHandler() {}

void eThreadBoardHandler::initialize(const int w, const int h) {
    mBoard.initialize(w, h);
    mTmpBoard.initialize(w, h);
}

void eThreadBoardHandler::scheduleUpdate(eGameBoard& board) {
    //    using std::chrono::high_resolution_clock;
//    using std::chrono::duration_cast;
//    using std::chrono::duration;
//    using std::chrono::milliseconds;

//    const auto t1 = high_resolution_clock::now();

    std::printf("Update tmp board %p to %d\n", this, board.state());

    std::lock_guard l(mTmpBoardMutex);
    mTmpChanged = true;
    for(int i = 0; i < board.width(); i++) {
        for(int j = 0; j < board.height(); j++) {
            const auto src = board.dtile(i, j);
            const auto dst = mTmpBoard.dtile(i, j);
            if(!src || !dst) continue;
            dst->load(src);
        }
    }
    mTmpBoard.setState(board.state());

//    const auto t2 = high_resolution_clock::now();

//    const duration<double, std::milli> ms = t2 - t1;
    //    printf("update board: %f ms\n", ms.count());
}

void eThreadBoardHandler::scheduleUpdate(eGameBoard& board, const eCityId cid) {
    std::printf("Update tmp board %p to %d\n", this, board.state());

    const auto c = board.boardCityWithId(cid);
    const auto& tiles = c->tiles();

    std::lock_guard l(mTmpBoardMutex);
    mTmpChanged = true;
    for(const auto src : tiles) {
        const int dx = src->dx();
        const int dy = src->dy();
        const auto dst = mTmpBoard.dtile(dx, dy);
        if(!dst) continue;
        dst->load(src);
    }
    mTmpBoard.setState(board.state());
}

void eThreadBoardHandler::updateBoard() {
    if(mTmpChanged) {
        std::printf("Swap boards %p from %d to %d\n", this, mBoard.state(), mTmpBoard.state());
        std::lock_guard l(mTmpBoardMutex);
        std::swap(mBoard, mTmpBoard);
        mTmpChanged = false;
    }
}
