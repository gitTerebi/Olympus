#include "eresourceeventvalue.h"

#include "rand.h"
#include "string-helpers.h"

#include "fileIO/write-stream.h"
#include "fileIO/read-stream.h"
#include "fileIO/save-archive.h"

void eResourceEventValue::chooseType() {
    std::vector<eResourceType> types;
    for(const auto r : mResources) {
        if(r == eResourceType::none) continue;
        types.push_back(r);
    }
    if(types.empty()) {
        printf("No resource types to choose from\n");
        return;
    }
    const int typeId = Rand::rand() % types.size();
    mResource = types[typeId];
}

void eResourceEventValue::longNameReplaceResource(
        const std::string& id, std::string& tmpl) const {
    std::string resName;
    if(mResources[0] != eResourceType::none) {
        resName += eResourceTypeHelpers::typeName(mResources[0]);
    }
    if(mResources[1] != eResourceType::none) {
        if(!resName.empty()) resName += "/";
        resName += eResourceTypeHelpers::typeName(mResources[1]);
    }
    if(mResources[2] != eResourceType::none) {
        if(!resName.empty()) resName += "/";
        resName += eResourceTypeHelpers::typeName(mResources[2]);
    }
    StringHelpers::replace(tmpl, id, resName);
}

void eResourceEventValue::serialize(SaveArchive& ar) {
    ar.field("resource", mResource, eResourceType::fleece);
    const char* const names[] = {"resource0", "resource1", "resource2"};
    const eResourceType defaults[] = {eResourceType::fleece,
                                       eResourceType::none,
                                       eResourceType::none};
    for(int i = 0; i < 3; i++) {
        ar.field(names[i], mResources[i], defaults[i]);
    }
}
