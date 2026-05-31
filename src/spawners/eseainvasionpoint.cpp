#include "eseainvasionpoint.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

eSeaInvasionPoint::eSeaInvasionPoint(const int id,
                                     eTile* const tile,
                                     GameBoard& board) :
    eBanner(eBannerTypeS::seaInvasion, id, tile, board) {}
