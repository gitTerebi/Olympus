#include "edisasterpoint.h"

#include "engine/e-game-board.h"

eDisasterPoint::eDisasterPoint(const int id,
                              eTile* const tile,
                              eGameBoard& board) :
    eBanner(eBannerTypeS::disasterPoint, id, tile, board) {}
