#include "eresourcecityevent.h"

#include "erand.h"

void eResourceCityEvent::setCity(const stdsptr<eWorldCity>& c) {
    mCity = c;
}

void eResourceCityEvent::chooseTypeAndCount() {
    std::vector<eResourceType> types;
    for(const auto r : mResources) {
        if(r == eResourceType::none) continue;
        types.push_back(r);
    }
    if(types.empty()) return;
    if(mMinCount > mMaxCount) return;
    const int typeId = eRand::rand() % types.size();
    mResource = types[typeId];
    const int diff = mMaxCount - mMinCount;
    mCount = diff == 0 ? mMinCount : (mMinCount + (eRand::rand() % diff));
}

void eResourceCityEvent::write(eWriteStream& dst) const {
    eGameEvent::write(dst);
    dst << mResource;
    dst << mCount;
    dst << mMinCount;
    dst << mMaxCount;

    dst << mResources.size();
    for(const auto r : mResources) {
        dst << r;
    }

    dst.writeCity(mCity.get());
}

void eResourceCityEvent::read(eReadStream& src) {
    eGameEvent::read(src);
    src >> mResource;
    src >> mCount;
    src >> mMinCount;
    src >> mMaxCount;

    int nr;
    src >> nr;
    for(int i = 0; i < nr; i++) {
        eResourceType type;
        src >> type;
        mResources.push_back(type);
    }

    src.readCity(worldBoard(), [this](const stdsptr<eWorldCity>& c) {
        mCity = c;
    });
}
