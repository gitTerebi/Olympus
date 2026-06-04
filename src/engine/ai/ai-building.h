#ifndef AIBuilding_H
#define AIBuilding_H

#include "buildings/ebuilding.h"
#include "engine/patrol-waypoint.h"

enum class eResourceType;
enum class eTradePostType;
class eSaveArchive;

struct AIBuilding {
    eBuildingType fType;
    SDL_Rect fRect;
    eResourceType fGet = static_cast<eResourceType>(0);
    eResourceType fEmpty = static_cast<eResourceType>(0);
    eResourceType fAccept = static_cast<eResourceType>(0);
    eResourceType fTradeImports = static_cast<eResourceType>(0);
    eResourceType fTradeExports = static_cast<eResourceType>(0);
    eResourceType fTradeDontAccept = static_cast<eResourceType>(0);
    std::map<eResourceType, int> fSpace;
    std::vector<ePatrolWaypoint> fWaypoints;
    bool fWaypointsBothDirections;
    eDiagonalOrientation fO;
    eCityId fTradingPartner;
    eTradePostType feTradePostType;
    SDL_Rect fOtherRect;

    void serialize(eSaveArchive& ar);
};

#endif // AIBuilding_H
