#include "edisasterpoint.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

eDisasterPoint::eDisasterPoint(const int id,
                              eTile* const tile,
                              GameBoard& board) :
    eBanner(eBannerTypeS::disasterPoint, id, tile, board) {}
