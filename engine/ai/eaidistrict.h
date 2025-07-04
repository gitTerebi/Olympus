#ifndef EAIDISTRICT_H
#define EAIDISTRICT_H

#include "eaibuilding.h"

class eAIDistrict {
public:
    void build(eGameBoard& board,
               const ePlayerId pid,
               const eCityId cid,
               const bool editorDisplay) const;

    bool road(int& x, int& y) const;

    void addBuilding(const eAIBuilding& a);

    void read(eReadStream& src);
    void write(eWriteStream& dst) const;

    std::vector<eAIBuilding> fBuildings;
};

#endif // EAIDISTRICT_H
