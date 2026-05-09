#include "edisembarkpoint.h"

#include "engine/e-game-board.h"

eDisembarkPoint::eDisembarkPoint(const int id,
                                 eTile* const tile,
                                 eGameBoard& board) :
    eBanner(eBannerTypeS::disembarkPoint, id, tile, board) {}
