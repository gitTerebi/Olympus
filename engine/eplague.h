#ifndef EPLAGUE_H
#define EPLAGUE_H

#include <vector>

#include "pointers/estdselfref.h"

class eGameBoard;
class eSmallHouse;

class eReadStream;
class eWriteStream;

enum class eCityId;

class ePlague {
public:
    ePlague(const eCityId cid, eGameBoard& board);

    eCityId cityId() const { return mCityId; }

    void randomSpread();
    void spreadFrom(eSmallHouse* const h);
    void healAll();
    void healHouse(eSmallHouse* const h);
    bool hasHouse(eSmallHouse* const h) const;
    void removeHouse(eSmallHouse* const h);
    int houseCount() const { return mHouses.size(); }
    using eHouses = std::vector<eSmallHouse*>;
    const eHouses& houses() const { return mHouses; }

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
private:
    eGameBoard& mBoard;
    eCityId mCityId;
    eHouses mHouses;
};

#endif // EPLAGUE_H
