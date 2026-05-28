#include "edisembarkpoint.h"

#include "engine/e-game-board.h"

eDisembarkPoint::eDisembarkPoint(const int id,
                                 eTile* const tile,
                                 GameBoard& board) :
    eBanner(eBannerTypeS::disembarkPoint, id, tile, board) {}
