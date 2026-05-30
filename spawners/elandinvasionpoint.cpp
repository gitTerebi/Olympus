#include "elandinvasionpoint.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

eLandInvasionPoint::eLandInvasionPoint(const int id,
                                       eTile* const tile,
                                       GameBoard& board) :
    eBanner(eBannerTypeS::landInvasion, id, tile, board) {}
