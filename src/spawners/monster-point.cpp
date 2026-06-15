#include "monster-point.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

MonsterPoint::MonsterPoint(const int id,
                             eTile* const tile,
                             GameBoard& board) :
    Banner(BannerTypeS::monsterPoint, id, tile, board) {}
