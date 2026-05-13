#ifndef EPOINTEVENTVALUE_H
#define EPOINTEVENTVALUE_H

#include <functional>

#include "spawners/ebanner.h"
#include "fileIO/ejsonarchive.h"

class ePointEventValue {
public:
    using eValidator = std::function<bool(eTile*)>;
    ePointEventValue(const eBannerTypeS btype,
                     const eCityId cid,
                     eGameBoard& board,
                     const eValidator& v = nullptr);

    int minPointId() const { return mMinPointId; }
    void setMinPointId(const int p) { mMinPointId = p; }

    int maxPointId() const { return mMaxPointId; }
    void setMaxPointId(const int p) { mMaxPointId = p; }

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);
protected:
    int pointId() const { return mPointId; }

    void choosePointId();
    virtual void serializeJson(eJsonArchive& ar) {
        ar.field("mPointId", mPointId);
        ar.field("mMinPointId", mMinPointId);
        ar.field("mMaxPointId", mMaxPointId);
    }

private:
    const eBannerTypeS mBType;
    const eCityId mCid;
    eGameBoard& mBoard;
    const eValidator mValidator;

    int mPointId = 1;
    int mMinPointId = 1;
    int mMaxPointId = 1;
};

#endif // EPOINTEVENTVALUE_H
