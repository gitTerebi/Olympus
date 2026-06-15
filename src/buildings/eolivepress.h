#ifndef EOLIVEPRESS_H
#define EOLIVEPRESS_H

#include "eprocessingbuilding.h"

class eOlivePress : public eProcessingBuilding {
public:
    eOlivePress(GameBoard& board, const eCityId cid);

    std::vector<Overlay> getOverlays(const eTileSize size) const override;
};

#endif // EOLIVEPRESS_H
