#include "emonsterpoint.h"

#include "engine/e-game-board.h"

eMonsterPoint::eMonsterPoint(const int id,
                             eTile* const tile,
                             eGameBoard& board) :
    eBanner(eBannerTypeS::monsterPoint, id, tile, board) {}
