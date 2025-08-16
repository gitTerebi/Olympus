#include "eresourcecountcityevent.h"

#include "erand.h"
#include "estringhelpers.h"

void eResourceCountCityEvent::chooseTypeAndCount() {
    chooseType();
    if(mMinCount > mMaxCount) return;
    const int diff = mMaxCount - mMinCount;
    mCount = diff == 0 ? mMinCount : (mMinCount + (eRand::rand() % diff));
}

void eResourceCountCityEvent::longNameReplaceResource(
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
    const auto cStr = std::to_string(mMinCount) + "-" +
                      std::to_string(mMaxCount);
    eStringHelpers::replace(tmpl, id, cStr + " " + resName);
}

void eResourceCountCityEvent::write(eWriteStream& dst) const {
    eResourceCityEvent::write(dst);
    dst << mCount;
    dst << mMinCount;
    dst << mMaxCount;
}

void eResourceCountCityEvent::read(eReadStream& src) {
    eResourceCityEvent::read(src);
    src >> mCount;
    src >> mMinCount;
    src >> mMaxCount;
}
