#include "exit-point.h"
#include "characters/soldier-banner.h"

ExitPoint::ExitPoint(const int id, eTile* const tile,
                       GameBoard& board) :
    Banner(BannerTypeS::exitPoint, id, tile, board) {}

RiverExitPoint::RiverExitPoint(const int id, eTile * const tile,
                                 GameBoard &board) :
    Banner(BannerTypeS::riverExitPoint, id, tile, board) {}
