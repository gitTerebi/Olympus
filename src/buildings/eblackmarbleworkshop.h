#ifndef EBLACKMARBLEWORKSHOP_H
#define EBLACKMARBLEWORKSHOP_H

#include "eresourcecollectbuilding.h"

class eBlackMarbleWorkshop : public eResourceCollectBuilding {
public:
    eBlackMarbleWorkshop(GameBoard& board, const eCityId cid);

    std::vector<Overlay> getOverlays(const eTileSize size) const;

    void timeChanged(const int by);
private:
    void setCollectAction();

    bool mCollectActionSet = false;
};

#endif // EBLACKMARBLEWORKSHOP_H
