#ifndef EAIBUILDING_H
#define EAIBUILDING_H

#include "buildings/ebuilding.h"
#include "engine/epatrolguide.h"

enum class eResourceType;
enum class eTradePostType;
class eSaveArchive;

struct eAIBuilding {
    eBuildingType fType;
    SDL_Rect fRect;
    eResourceType fGet = static_cast<eResourceType>(0);
    eResourceType fEmpty = static_cast<eResourceType>(0);
    eResourceType fAccept = static_cast<eResourceType>(0);
    eResourceType fTradeImports = static_cast<eResourceType>(0);
    eResourceType fTradeExports = static_cast<eResourceType>(0);
    eResourceType fTradeDontAccept = static_cast<eResourceType>(0);
    std::map<eResourceType, int> fSpace;
    std::vector<ePatrolGuide> fGuides;
    bool fGuidesBothDirections;
    eDiagonalOrientation fO;
    eCityId fTradingPartner;
    eTradePostType fTradePostType;
    SDL_Rect fOtherRect;

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
    void serialize(eSaveArchive& ar);
};

#endif // EAIBUILDING_H
