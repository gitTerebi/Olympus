#ifndef ECOUNTEVENT_H
#define ECOUNTEVENT_H

#include "eresourceevent.h"

class eCountEvent {
public:
    int resourceCount() const { return mCount; }

    int minResourceCount() const { return mMinCount; }
    void setMinResourceCount(const int c) { mMinCount = c; }

    int maxResourceCount() const { return mMaxCount; }
    void setMaxResourceCount(const int c) { mMaxCount = c; }

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
