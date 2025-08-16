#ifndef ERESOURCECOUNTCITYEVENT_H
#define ERESOURCECOUNTCITYEVENT_H

#include "eresourcecityevent.h"

class eResourceCountCityEvent : public eResourceCityEvent {
public:
    using eResourceCityEvent::eResourceCityEvent;

    int resourceCount() const { return mCount; }

    int minResourceCount() const { return mMinCount; }
    void setMinResourceCount(const int c) { mMinCount = c; }

    int maxResourceCount() const { return mMaxCount; }
    void setMaxResourceCount(const int c) { mMaxCount = c; }

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
protected:
    void chooseTypeAndCount();

    void longNameReplaceResource(
            const std::string& id,
            std::string& tmpl) const override;

    int mMinCount = 8;
    int mMaxCount = 16;

    int mCount = 16;
};

#endif // ERESOURCECOUNTCITYEVENT_H
