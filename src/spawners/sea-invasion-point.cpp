#include "sea-invasion-point.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

SeaInvasionPoint::SeaInvasionPoint(const int id,
                                     eTile* const tile,
                                     GameBoard& board) :
    Banner(BannerTypeS::seaInvasion, id, tile, board) {}
