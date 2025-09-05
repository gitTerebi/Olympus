#ifndef EHIPPODROME_H
#define EHIPPODROME_H

#include "ehippodromepiece.h"
#include "missiles/emissile.h"

class eHippodrome {
public:
    eHippodrome(const eCityId cid, eGameBoard& board);

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

    void setCart(eCartTransporter* const c);
    bool hasCart() const { return mCart; }

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);
private:
    void purgeHorses();
    void updatePaths();

    const eCityId mCid;
    eGameBoard& mBoard;

    using eN = eHippodromePiece::eNeighbour;
    std::vector<eN> mPieces;
    int mFinish = -1;
    int mNHorses = 0;
    std::vector<ePathPoint> mPath1;
    std::vector<ePathPoint> mPath2;

    std::vector<stdptr<eMissile>> mHorses;

    stdptr<eCartTransporter> mCart;
};

#endif // EHIPPODROME_H
