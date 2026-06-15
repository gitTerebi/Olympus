#include "disaster-point.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

DisasterPoint::DisasterPoint(const int id,
                              eTile* const tile,
                              GameBoard& board) :
    Banner(BannerTypeS::disasterPoint, id, tile, board) {}
