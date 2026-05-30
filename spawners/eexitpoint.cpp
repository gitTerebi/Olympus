#include "eexitpoint.h"
#include "characters/soldier-banner.h"

eExitPoint::eExitPoint(const int id, eTile* const tile,
                       GameBoard& board) :
    eBanner(eBannerTypeS::exitPoint, id, tile, board) {}

eRiverExitPoint::eRiverExitPoint(const int id, eTile * const tile,
                                 GameBoard &board) :
    eBanner(eBannerTypeS::riverExitPoint, id, tile, board) {}
