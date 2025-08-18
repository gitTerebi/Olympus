#ifndef EWAVEMISSILE_H
#define EWAVEMISSILE_H

#include "emissile.h"

class eWaveMissile : public eMissile {
public:
    eWaveMissile(eGameBoard& board,
                 const std::vector<ePathPoint>& path = {});

    std::shared_ptr<eTexture>
    getTexture(const eTileSize size) const override;
};

#endif // EWAVEMISSILE_H
