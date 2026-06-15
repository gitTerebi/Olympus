#include "land-invasion-point.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

LandInvasionPoint::LandInvasionPoint(const int id,
                                       eTile* const tile,
                                       GameBoard& board) :
    Banner(BannerTypeS::landInvasion, id, tile, board) {}
