#include "elandinvasionpoint.h"

#include "engine/e-game-board.h"

eLandInvasionPoint::eLandInvasionPoint(const int id,
                                       eTile* const tile,
                                       GameBoard& board) :
    eBanner(eBannerTypeS::landInvasion, id, tile, board) {}
