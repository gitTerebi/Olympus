#include "competitor.h"

#include "textures/game-textures.h"

Competitor::Competitor(GameBoard& board) :
    eBasicPatroler(board, &CharacterTextures::fCompetitor,
                   eCharacterType::competitor) {
    GameTextures::loadCompetitor();
    setProvide(eProvide::competitorCurator, 10000);
}
