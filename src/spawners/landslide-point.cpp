#include "landslide-point.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

LandSlidePoint::LandSlidePoint(const int id,
                                 eTile* const tile,
                                 GameBoard& board) :
    Banner(BannerTypeS::landSlidePoint, id, tile, board) {}
