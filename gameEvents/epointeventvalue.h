#ifndef EPOINTEVENTVALUE_H
#define EPOINTEVENTVALUE_H

#include <functional>

#include "spawners/ebanner.h"

class eWriteStream;
class eReadStream;
class eSaveArchive;

class ePointEventValue {
public:
    using eValidator = std::function<bool(eTile*)>;
    ePointEventValue(const eBannerTypeS btype,
                     const eCityId cid,
                     GameBoard& board,
                     const eValidator& v = nullptr);

    int minPointId() const { return mMinPointId; }
    void setMinPointId(const int p) { mMinPointId = p; }

    int maxPointId() const { return mMaxPointId; }
    void setMaxPointId(const int p) { mMaxPointId = p; }

    void serialize(eSaveArchive& ar);
protected:
    int pointId() const { return mPointId; }

    void choosePointId();
private:
    const eBannerTypeS mBType;
    const eCityId mCid;
    GameBoard& mBoard;
    const eValidator mValidator;

    int mPointId = 1;
    int mMinPointId = 1;
    int mMaxPointId = 1;
};

#endif // EPOINTEVENTVALUE_H
