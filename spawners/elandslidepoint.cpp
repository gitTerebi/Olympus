#include "elandslidepoint.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

eLandSlidePoint::eLandSlidePoint(const int id,
                                 eTile* const tile,
                                 GameBoard& board) :
    eBanner(eBannerTypeS::landSlidePoint, id, tile, board) {}
