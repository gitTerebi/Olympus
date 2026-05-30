#include "edisembarkpoint.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

eDisembarkPoint::eDisembarkPoint(const int id,
                                 eTile* const tile,
                                 GameBoard& board) :
    eBanner(eBannerTypeS::disembarkPoint, id, tile, board) {}
