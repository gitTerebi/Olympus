#include "etradeboat.h"

#include "textures/game-textures.h"

eTradeBoat::eTradeBoat(GameBoard& board) :
    eBoatBase(board, &CharacterTextures::fTradeBoat,
              eCharacterType::tradeBoat) {
    GameTextures::loadTradeBoat();
}
