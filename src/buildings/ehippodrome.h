#ifndef EHIPPODROME_H
#define EHIPPODROME_H

#include "ehippodromepiece.h"
#include "missiles/missile.h"

class SaveArchive;

class eHippodrome {
public:
    eHippodrome(const eCityId cid, GameBoard& board);

    void nextMonth();

    void addPieces(eHippodromePiece* const start);
    bool closed() const;
    bool empty() const { return mPieces.empty(); }

    void clear();
    void spawnHorses();

    void addHorses(const int h);
    int neededHorses() const;
    int hasHorses() const { return mNHorses; }

    int drachmasPerMonth() const;
    bool working() const;
    bool racing() const;
    int length() const;

    void setCart(eCartTransporter* const c);
    bool hasCart() const { return mCart; }

    void nextCleaningPartId();
    int cleaningPartId() const { return mCleaningPartId; }

    int finishPartId() const { return mFinish; }

    void serialize(SaveArchive& ar);
private:

    using eN = eHippodromePiece::eNeighbour;
    eN* addPiece(const eN& n);

    void purgeHorses();
    void updatePaths();

    const eCityId mCid;
    GameBoard& mBoard;
    int mCleaningPartId = 0;

    std::vector<eN> mPieces;
    int mFinish = -1;
    int mNHorses = 0;
    std::vector<PathPoint> mPath1;
    std::vector<PathPoint> mPath2;

    std::vector<stdptr<Missile>> mHorses;

    stdptr<eCartTransporter> mCart;
};

#endif // EHIPPODROME_H
