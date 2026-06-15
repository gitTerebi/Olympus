#include "athena.h"

#include "textures/game-textures.h"

Apollo::Apollo(GameBoard& board) :
    ExtendedGod(board, &GodTextures::fApollo, GodType::apollo) {
    GameTextures::loadApollo();
}

Artemis::Artemis(GameBoard& board) :
    ExtendedGod(board, &GodTextures::fArtemis, GodType::artemis) {
    GameTextures::loadArtemis();
}

Athena::Athena(GameBoard& board) :
    ExtendedGod(board, &GodTextures::fAthena, GodType::athena) {
    GameTextures::loadAthena();
}

Atlas::Atlas(GameBoard& board) :
    ExtendedGod(board, &GodTextures::fAtlas, GodType::atlas) {
    GameTextures::loadAtlas();
}

Zeus::Zeus(GameBoard& board) :
    ExtendedGod(board, &GodTextures::fZeus, GodType::zeus) {
    GameTextures::loadZeus();
}
