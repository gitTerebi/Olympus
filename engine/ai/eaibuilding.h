#ifndef EAIBUILDING_H
#define EAIBUILDING_H

#include "buildings/ebuilding.h"
#include "engine/epatrolguide.h"

enum class eResourceType;

struct eAIBuilding {
    eBuildingType fType;
    SDL_Rect fRect;
    eResourceType fGet = static_cast<eResourceType>(0);
    eResourceType fEmpty = static_cast<eResourceType>(0);
    eResourceType fAccept = static_cast<eResourceType>(0);
    std::map<eResourceType, int> fSpace;
    std::vector<ePatrolGuide> fGuides;
    eDiagonalOrientation fO;

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;
};

#endif // EAIBUILDING_H
