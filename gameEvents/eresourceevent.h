#ifndef ERESOURCEEVENT_H
#define ERESOURCEEVENT_H

#include "engine/eresourcetype.h"

class eWriteStream;
class eReadStream;

class eResourceEvent {
public:
    eResourceType resourceType() const { return mResource; }

    const std::vector<eResourceType>& resourceTypes() const
    { return mResources; }
    eResourceType resourceType(const int id) const
    { return mResources[id]; }
    void setResourceType(const int id, const eResourceType type)
    { mResources[id] = type; }

    void write(eWriteStream& dst) const;
    void read(eReadStream& src);
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

#endif // ERESOURCEEVENT_H
