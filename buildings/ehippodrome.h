#ifndef EHIPPODROME_H
#define EHIPPODROME_H

#include "ehippodromepiece.h"
#include "missiles/emissile.h"

class eHippodrome {
public:
    eHippodrome(eGameBoard& board);

    void addPieces(eHippodromePiece* const start);
    bool closed() const;
    bool empty() const { return mPieces.empty(); }

    void clear();
    void spawnHorses();
private:
    void updatePaths();

    eGameBoard& mBoard;

    using eN = eHippodromePiece::eNeighbour;
    std::vector<eN> mPieces;
    int mFinish = -1;
    int mNHorses = 4;
    std::vector<ePathPoint> mPath1;
    std::vector<ePathPoint> mPath2;
};

#endif // EHIPPODROME_H
