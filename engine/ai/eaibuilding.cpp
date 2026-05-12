#include "eaibuilding.h"

#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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
    ar.field("fType", fType);
    ar.field("fRect", fRect);
    ar.field("fGet", fGet);
    ar.field("fEmpty", fEmpty);
    ar.field("fAccept", fAccept);

    int ns;
    if(ar.writing()) ns = fSpace.size();
    ar.field("ns", ns);
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
        ar.field("r", r);
        ar.field("s", s);
        if(ar.reading()) fSpace[r] = s;
    }

    int ng;
    if(ar.writing()) ng = fGuides.size();
    ar.field("ng", ng);
    if(ar.reading()) fGuides.clear();
    for(int i = 0; i < ng; i++) {
        ePatrolGuide pg;
        if(ar.writing()) pg = fGuides[i];
        ar.field("pg.fX", pg.fX);
        ar.field("pg.fY", pg.fY);
        if(ar.reading()) fGuides.push_back(pg);
    }
    ar.field("fGuidesBothDirections", fGuidesBothDirections);

    ar.field("fO", fO);

    ar.field("fTradingPartner", fTradingPartner);
    ar.field("fTradePostType", fTradePostType);
    ar.field("fOtherRect.x", fOtherRect.x); ar.field("fOtherRect.y", fOtherRect.y);
    ar.field("fOtherRect.w", fOtherRect.w); ar.field("fOtherRect.h", fOtherRect.h);
}

void eAIBuilding::serializeJson(eJsonArchive& ar) {
    ar.field("fType", fType);
    ar.field("fRect.x", fRect.x); ar.field("fRect.y", fRect.y);
    ar.field("fRect.w", fRect.w); ar.field("fRect.h", fRect.h);
    ar.field("fGet", fGet);
    ar.field("fEmpty", fEmpty);
    ar.field("fAccept", fAccept);
    {
        int ns = ar.reading() ? 0 : static_cast<int>(fSpace.size());
        ar.field("spaceCount", ns);
        int si = 0;
        if(ar.reading()) {
            for(int i = 0; i < ns; i++) {
                auto ca = ar.childAt("space", i);
                eResourceType r{}; ca.field("type", r);
                int s = 0; ca.field("amount", s);
                fSpace[r] = s;
            }
        } else {
            for(const auto& [r, s] : fSpace) {
                auto ca = ar.childAt("space", si++);
                auto rv = r; ca.field("type", rv);
                auto sv = s; ca.field("amount", sv);
            }
        }
    }
    {
        int ng = ar.reading() ? 0 : static_cast<int>(fGuides.size());
        ar.field("guideCount", ng);
        for(int i = 0; i < ng; i++) {
            auto ca = ar.childAt("guides", i);
            ePatrolGuide pg = ar.reading() ? ePatrolGuide{} : fGuides[i];
            ca.field("fX", pg.fX);
            ca.field("fY", pg.fY);
            if(ar.reading()) fGuides.push_back(pg);
        }
    }
    ar.field("fGuidesBothDirections", fGuidesBothDirections);
    ar.field("fO", fO);
    ar.field("fTradingPartner", fTradingPartner);
    ar.field("fTradePostType", fTradePostType);
    ar.field("fOtherRect.x", fOtherRect.x); ar.field("fOtherRect.y", fOtherRect.y);
    ar.field("fOtherRect.w", fOtherRect.w); ar.field("fOtherRect.h", fOtherRect.h);
}
