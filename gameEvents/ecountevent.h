#ifndef ECOUNTEVENT_H
#define ECOUNTEVENT_H

#include "eresourceevent.h"

class eCountEvent {
public:
    int count() const { return mCount; }

    int minCount() const { return mMinCount; }
    void setMinCount(const int c) { mMinCount = c; }

    int maxCount() const { return mMaxCount; }
    void setMaxCount(const int c) { mMaxCount = c; }

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);
protected:
    void chooseCount();

    void longNameReplaceCount(
            const std::string& id,
            std::string& tmpl) const;

    int mMinCount = 8;
    int mMaxCount = 16;

    int mCount = 16;
};

#endif // ECOUNTEVENT_H
