#include "elandinvasionpoint.h"

#include "engine/e-game-board.h"

eLandInvasionPoint::eLandInvasionPoint(const int id,
                                       eTile* const tile,
                                       eGameBoard& board) :
    eBanner(eBannerTypeS::landInvasion, id, tile, board) {}
