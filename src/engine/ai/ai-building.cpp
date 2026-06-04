#include "ai-building.h"

#include "fileIO/esavearchive.h"

#include <iterator>

void AIBuilding::serialize(eSaveArchive& ar) {
    ar.field("fType", fType);
    ar.field("fRect", fRect);
    ar.field("fGet", fGet);
    ar.field("fEmpty", fEmpty);
    ar.field("fAccept", fAccept);
    ar.field("fTradeImports", fTradeImports, static_cast<eResourceType>(0)); // SAVE_COMPAT_OPTIONAL_FIELD
    ar.field("fTradeExports", fTradeExports, static_cast<eResourceType>(0)); // SAVE_COMPAT_OPTIONAL_FIELD
    ar.field("fTradeDontAccept", fTradeDontAccept, static_cast<eResourceType>(0)); // SAVE_COMPAT_OPTIONAL_FIELD

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
    if(ar.writing()) ng = fWaypoints.size();
    ar.field("ng", ng);
    if(ar.reading()) fWaypoints.clear();
    for(int i = 0; i < ng; i++) {
        ePatrolWaypoint waypoint;
        if(ar.writing()) waypoint = fWaypoints[i];
        ar.field("pg.fX", waypoint.fX);
        ar.field("pg.fY", waypoint.fY);
        if(ar.reading()) fWaypoints.push_back(waypoint);
    }
    ar.field("fGuidesBothDirections", fWaypointsBothDirections);

    ar.field("fO", fO);

    ar.field("fTradingPartner", fTradingPartner);
    ar.field("feTradePostType", feTradePostType);
    ar.field("fOtherRect", fOtherRect);
}
