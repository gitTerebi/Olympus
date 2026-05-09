#include "elandslidepoint.h"

#include "engine/e-game-board.h"

eLandSlidePoint::eLandSlidePoint(const int id,
                                 eTile* const tile,
                                 eGameBoard& board) :
    eBanner(eBannerTypeS::landSlidePoint, id, tile, board) {}
