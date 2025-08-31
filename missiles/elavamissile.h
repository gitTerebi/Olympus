#ifndef ELAVAMISSILE_H
#define ELAVAMISSILE_H

#include "emissile.h"

class eLavaMissile : public eMissile {
public:
    eLavaMissile(eGameBoard& board,
                 const std::vector<ePathPoint>& path = {});

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;

private:
    mutable int mCollId;
    mutable int mTexTimeShift = 0;
};

#endif // ELAVAMISSILE_H
