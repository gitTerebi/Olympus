#ifndef EPOINTEVENTBASE_H
#define EPOINTEVENTBASE_H

#include "spawners/ebanner.h"

class ePointEventBase {
public:
    ePointEventBase(const eBannerTypeS btype,
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

#endif // EPOINTEVENTBASE_H
