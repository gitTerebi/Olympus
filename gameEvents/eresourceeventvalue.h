#ifndef ERESOURCEEVENTVALUE_H
#define ERESOURCEEVENTVALUE_H

#include "engine/eresourcetype.h"
#include "fileIO/ejsonarchive.h"

class eWriteStream;
class eReadStream;
class eSaveArchive;

class eResourceEventValue {
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
    void serialize(eSaveArchive& ar);

    void chooseType();

    virtual void longNameReplaceResource(
            const std::string& id,
            std::string& tmpl) const;

    std::vector<eResourceType> mResources = {eResourceType::fleece,
                                             eResourceType::none,
                                             eResourceType::none};

    eResourceType mResource = eResourceType::fleece;
    virtual void serializeJson(eJsonArchive& ar) {
        ar.field("mResource", mResource);
        for(int i = 0; i < 3; i++) {
            ar.field(("mResources" + std::to_string(i)).c_str(), mResources[i]);
        }
    }

};

#endif // ERESOURCEEVENTVALUE_H
