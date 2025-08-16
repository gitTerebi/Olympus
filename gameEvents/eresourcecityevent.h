#ifndef ERESOURCECITYEVENT_H
#define ERESOURCECITYEVENT_H

#include "ebasiccityevent.h"

#include "engine/eresourcetype.h"

class eResourceCityEvent : public eBasicCityEvent {
public:
    using eBasicCityEvent::eBasicCityEvent;

    eResourceType resourceType() const { return mResource; }

    const std::vector<eResourceType>& resourceTypes() const
    { return mResources; }
    eResourceType resourceType(const int id) const
    { return mResources[id]; }
    void setResourceType(const int id, const eResourceType type)
    { mResources[id] = type; }

    void write(eWriteStream& dst) const override;
    void read(eReadStream& src) override;
protected:
    void chooseType();

    virtual void longNameReplaceResource(
            const std::string& id,
            std::string& tmpl) const;

    std::vector<eResourceType> mResources = {eResourceType::fleece,
                                             eResourceType::none,
                                             eResourceType::none};

    eResourceType mResource = eResourceType::fleece;
};

#endif // ERESOURCECITYEVENT_H
