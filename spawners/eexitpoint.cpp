#include "eexitpoint.h"

eExitPoint::eExitPoint(const int id, eTile* const tile,
                       eGameBoard& board) :
    eBanner(eBannerTypeS::exitPoint, id, tile, board) {}

eRiverExitPoint::eRiverExitPoint(const int id, eTile * const tile,
                                 eGameBoard &board) :
    eBanner(eBannerTypeS::riverExitPoint, id, tile, board) {}
