#ifndef EURCHINGATHERER_H
#define EURCHINGATHERER_H

#include "eresourcecollectorbase.h"

class eUrchinGatherer : public eResourceCollectorBase {
public:
    eUrchinGatherer(GameBoard& board);
    std::shared_ptr<eTexture> getTexture(const eTileSize size) const;
};

#endif // EURCHINGATHERER_H
