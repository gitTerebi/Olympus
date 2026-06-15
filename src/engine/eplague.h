#ifndef EPLAGUE_H
#define EPLAGUE_H

#include <vector>

#include "pointers/estdselfref.h"

class GameBoard;
class SmallHouse;

class SaveArchive;

enum class eCityId;

class ePlague {
public:
    ePlague(const eCityId cid, GameBoard& board);

    eCityId cityId() const { return mCityId; }

    void randomSpread();
    void spreadFrom(SmallHouse* const h);
    void healAll();
    void healHouse(SmallHouse* const h);
    bool hasHouse(SmallHouse* const h) const;
    void removeHouse(SmallHouse* const h);
    int houseCount() const { return mHouses.size(); }
    using eHouses = std::vector<SmallHouse*>;
    const eHouses& houses() const { return mHouses; }

    void serialize(SaveArchive& ar);
private:

    GameBoard& mBoard;
    eCityId mCityId;
    eHouses mHouses;
};

#endif // EPLAGUE_H
