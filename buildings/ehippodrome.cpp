#include "ehippodrome.h"

#include "characters/eracinghorse.h"

eHippodrome::eHippodrome(eGameBoard& board) :
    mBoard(board) {}

void eHippodrome::addPieces(eHippodromePiece * const start) {
    mPieces.clear();
    mFinish = -1;
    auto current = &mPieces.emplace_back(eN{start, eDiagonalOrientation::topRight});
    start->setHippodrome(this);
    eN* prev = nullptr;
    while(current) {
        if(mFinish == -1 && current->fPtr->id() == 5) {
            mFinish = mPieces.size() - 1;
        }
        const auto ns = current->fPtr->neighbours();
        if(ns.empty()) break;
        if(prev && ns.size() == 1) break;
        for(const auto& n : ns) {
            if(prev && n.fPtr == prev->fPtr) continue;
            current->fO = n.fO;
            if(n.fPtr == start) {
                current = nullptr;
                break;
            }
            prev = current;
            current = &mPieces.emplace_back(n);
            current->fPtr->setHippodrome(this);
            break;
        }
    }
    updatePaths();
}

bool eHippodrome::closed() const {
    if(mPieces.size() < 4) return false;
    const auto first = mPieces.front().fPtr;
    const auto last = mPieces.back().fPtr;
    const auto ns = last->neighbours();
    for(const auto& n : ns) {
        if(n.fPtr == first) return true;
    }
    return false;
}

void eHippodrome::clear() {
    for(const auto p : mPieces) {
        p.fPtr->setHippodrome(nullptr);
    }
    mPieces.clear();
}

void eHippodrome::spawnHorses() {
    for(int i = 0; i < mNHorses;) {
        for(int j = 0; j < 2 && i < mNHorses; j++, i++) {
            const auto h = e::make_shared<eRacingHorse>(
                mBoard, i, j == 0 ? mPath1 : mPath2);
            h->incTime((i/2)*50);
            h->setSpeed(1 + i*0.1);
        }
    }
}

void eHippodrome::updatePaths() {
    mPath1.clear();
    mPath2.clear();
    const bool c = closed();
    if(!c) return;
    int i = mFinish;
    const eN* start = &mPieces[i];
    const eN* current = start;
    const auto next = [&]() {
        i++;
        if(i >= (int)mPieces.size()) {
            i = 0;
        }
        current = &mPieces[i];
    };

    const auto& r = current->fPtr->tileRect();
    mPath1.push_back(ePathPoint{r.x + 1., (double)r.y, 0.});
    mPath2.push_back(ePathPoint{r.x + 2., (double)r.y, 0.});
    while(current) {
        next();
        const auto ptr = current->fPtr;
        ptr->progressPath(mPath1);
        ptr->progressPath(mPath2);
        if(current == start) break;
    }
}
