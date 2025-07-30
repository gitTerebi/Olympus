#include "edisembarkpoint.h"

#include "engine/egameboard.h"

eDisembarkPoint::eDisembarkPoint(const int id,
                                 eTile* const tile,
                                 eGameBoard& board) :
    eBanner(eBannerTypeS::disembarkPoint, id, tile, board) {}
