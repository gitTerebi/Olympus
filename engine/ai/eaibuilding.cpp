#include "eaibuilding.h"

#include "fileIO/esavearchive.h"

#include <iterator>

void eAIBuilding::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eAIBuilding::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eAIBuilding*>(this)->serialize(ar);
}

void eAIBuilding::serialize(eSaveArchive& ar) {
    ar.value(fType);
    ar.value(fRect);
    ar.value(fGet);
    ar.value(fEmpty);
    ar.value(fAccept);

    int ns;
    if(ar.writing()) ns = fSpace.size();
    ar.value(ns);
    if(ar.reading()) fSpace.clear();
    for(int i = 0; i < ns; i++) {
        eResourceType r;
        int s;
        if(ar.writing()) {
            auto it = fSpace.begin();
            std::advance(it, i);
            r = it->first;
            s = it->second;
        }
        ar.value(r);
        ar.value(s);
        if(ar.reading()) fSpace[r] = s;
    }

    int ng;
    if(ar.writing()) ng = fGuides.size();
    ar.value(ng);
    if(ar.reading()) fGuides.clear();
    for(int i = 0; i < ng; i++) {
        ePatrolGuide pg;
        if(ar.writing()) pg = fGuides[i];
        ar.value(pg.fX);
        ar.value(pg.fY);
        if(ar.reading()) fGuides.push_back(pg);
    }
    ar.value(fGuidesBothDirections);

    ar.value(fO);

    ar.value(fTradingPartner);
    ar.value(fTradePostType);
    ar.value(fOtherRect);
}
