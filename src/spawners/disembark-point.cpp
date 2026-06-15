#include "disembark-point.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

DisembarkPoint::DisembarkPoint(const int id,
                                 eTile* const tile,
                                 GameBoard& board) :
    Banner(BannerTypeS::disembarkPoint, id, tile, board) {}
