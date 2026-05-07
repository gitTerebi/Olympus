#include "eresourceeventvalue.h"

#include "erand.h"
#include "estringhelpers.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"
#include "fileIO/esavearchive.h"

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
    const int typeId = eRand::rand() % types.size();
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
    eStringHelpers::replace(tmpl, id, resName);
}

void eResourceEventValue::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eResourceEventValue*>(this)->serialize(ar);
}

void eResourceEventValue::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eResourceEventValue::serialize(eSaveArchive& ar) {
    ar.value(mResource);
    for(int i = 0; i < 3; i++) {
        ar.value(mResources[i]);
    }
}
