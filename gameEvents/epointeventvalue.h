#ifndef EPOINTEVENTVALUE_H
#define EPOINTEVENTVALUE_H

#include "spawners/ebanner.h"

class ePointEventValue {
public:
    ePointEventValue(const eBannerTypeS btype,
                    const eCityId cid,
                    eGameBoard& board);

    int minPointId() const { return mMinPointId; }
    void setMinPointId(const int p) { mMinPointId = p; }

    int maxPointId() const { return mMaxPointId; }
    void setMaxPointId(const int p) { mMaxPointId = p; }

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);
protected:
    int pointId() const { return mPointId; }

    void choosePointId();
private:
    const eBannerTypeS mBType;
    const eCityId mCid;
    eGameBoard& mBoard;

    int mPointId = 1;
    int mMinPointId = 1;
    int mMaxPointId = 1;
};

#endif // EPOINTEVENTVALUE_H
