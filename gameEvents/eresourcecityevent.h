#ifndef ERESOURCECITYEVENT_H
#define ERESOURCECITYEVENT_H

#include "egameevent.h"

#include "engine/eworldcity.h"

class eResourceCityEvent : public eGameEvent {
public:
    using eGameEvent::eGameEvent;

    const stdsptr<eWorldCity>& city() const { return mCity; }
    void setCity(const stdsptr<eWorldCity>& c);

    eResourceType resourceType() const { return mResource; }
    int resourceCount() const { return mCount; }

    int minResourceCount() const { return mMinCount; }
    void setMinResourceCount(const int c) { mMinCount = c; }

    int maxResourceCount() const { return mMaxCount; }
    void setMaxResourceCount(const int c) { mMaxCount = c; }

    const std::vector<eResourceType>& resourceTypes() const
    { return mResources; }
    eResourceType resourceType(const int id) const
    { return mResources[id]; }
    void setResourceType(const int id, const eResourceType type)
    { mResources[id] = type; }

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
protected:
    void chooseTypeAndCount();

    std::vector<eResourceType> mResources = {eResourceType::fleece,
                                             eResourceType::fleece,
                                             eResourceType::fleece};
    int mMinCount = 8;
    int mMaxCount = 16;

    eResourceType mResource = eResourceType::fleece;
    int mCount = 16;

    stdsptr<eWorldCity> mCity;
};

#endif // ERESOURCECITYEVENT_H
