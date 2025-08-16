#include "eresourcecityevent.h"

#include "erand.h"
#include "estringhelpers.h"

void eResourceCityEvent::chooseType() {
    std::vector<eResourceType> types;
    for(const auto r : mResources) {
        if(r == eResourceType::none) continue;
        types.push_back(r);
    }
    if(types.empty()) return;
    const int typeId = eRand::rand() % types.size();
    mResource = types[typeId];
}

void eResourceCityEvent::longNameReplaceResource(
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

void eResourceCityEvent::write(eWriteStream& dst) const {
    eBasicCityEvent::write(dst);
    dst << mResource;

    for(const auto r : mResources) {
        dst << r;
    }
}

void eResourceCityEvent::read(eReadStream& src) {
    eBasicCityEvent::read(src);
    src >> mResource;

    for(int i = 0; i < 3; i++) {
        src >> mResources[i];
    }
}
