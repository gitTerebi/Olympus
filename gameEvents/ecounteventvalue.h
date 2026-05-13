#ifndef ECOUNTEVENTVALUE_H
#define ECOUNTEVENTVALUE_H

#include "eresourceeventvalue.h"
#include "fileIO/ejsonarchive.h"

class eSaveArchive;

class eCountEventValue {
public:
    int count() const { return mCount; }

    int minCount() const { return mMinCount; }
    void setMinCount(const int c) { mMinCount = c; }

    int maxCount() const { return mMaxCount; }
    void setMaxCount(const int c) { mMaxCount = c; }

    void write(eWriteStream& dst) const;
    void serialize(eSaveArchive& ar);
    void read(eReadStream& src);
    void serializeJson(eJsonArchive& ar) {
        ar.field("mMinCount", mMinCount);
        ar.field("mMaxCount", mMaxCount);
        ar.field("mCount", mCount);
    }
protected:
    void chooseCount();

    void longNameReplaceCount(
            const std::string& id,
            std::string& tmpl) const;

    int mMinCount = 8;
    int mMaxCount = 16;

    int mCount = 16;
};

#endif // ECOUNTEVENTVALUE_H
