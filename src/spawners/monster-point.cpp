#include "emonsterpoint.h"

#include "engine/game-board.h"
#include "characters/soldier-banner.h"

eMonsterPoint::eMonsterPoint(const int id,
                             eTile* const tile,
                             GameBoard& board) :
    eBanner(eBannerTypeS::monsterPoint, id, tile, board) {}
