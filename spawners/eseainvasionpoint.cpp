#include "eseainvasionpoint.h"

#include "engine/egameboard.h"

eSeaInvasionPoint::eSeaInvasionPoint(const int id,
                                     eTile* const tile,
                                     eGameBoard& board) :
    eBanner(eBannerTypeS::seaInvasion, id, tile, board) {}
