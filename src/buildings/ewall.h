#ifndef EWALL_H
#define EWALL_H

#include "ebuilding.h"

class eWall : public eBuilding {
public:
    eWall(GameBoard& board, const eCityId cid);
    ~eWall();

    std::shared_ptr<Texture>
        getTexture(const eTileSize size) const;

    void setDeleteArchers(const bool d) { mDeleteArchers = d; }
private:
    bool mDeleteArchers = true;
};

#endif // EWALL_H
