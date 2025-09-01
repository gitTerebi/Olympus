#include "elandslidepoint.h"

#include "engine/egameboard.h"

eLandSlidePoint::eLandSlidePoint(const int id,
                                 eTile* const tile,
                                 eGameBoard& board) :
    eBanner(eBannerTypeS::landSlidePoint, id, tile, board) {}
