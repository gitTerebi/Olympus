#ifndef EDUSTMISSILE_H
#define EDUSTMISSILE_H

#include "emissile.h"

class eDustMissile : public eMissile {
public:
    eDustMissile(eGameBoard& board,
                 const std::vector<ePathPoint>& path = {});

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;
private:
    mutable int mCollId;
    mutable int mTexTimeShift = 0;
};

#endif // EDUSTMISSILE_H
