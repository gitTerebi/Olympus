#include "eaibuilding.h"

eBuildingType fType;
SDL_Rect fRect;
eResourceType fGet = static_cast<eResourceType>(0);
eResourceType fEmpty = static_cast<eResourceType>(0);
eResourceType fAccept = static_cast<eResourceType>(0);
std::map<eResourceType, int> fSpace;
std::vector<ePatrolGuide> fGuides;
eDiagonalOrientation fO;

void eAIBuilding::read(eReadStream& src) {
    src >> fType;
    src >> fRect;
    src >> fGet;
    src >> fEmpty;
    src >> fAccept;

    int ns;
    src >> ns;
    for(int i = 0; i < ns; i++) {
        eResourceType r;
        src >> r;
        int s;
        src >> s;
        fSpace[r] = s;
    }

    int ng;
    src >> ng;
    for(int i = 0; i < ng; i++) {
        auto& pg = fGuides.emplace_back();
        src >> pg.fX;
        src >> pg.fY;
    }

    src >> fO;
}

void eAIBuilding::write(eWriteStream& dst) const {
    dst << fType;
    dst << fRect;
    dst << fGet;
    dst << fEmpty;
    dst << fAccept;

    dst << fSpace.size();
    for(const auto& s : fSpace) {
        dst << s.first;
        dst << s.second;
    }

    dst << fGuides.size();
    for(const auto& pg : fGuides) {
        dst << pg.fX;
        dst << pg.fY;
    }

    dst << fO;
}
