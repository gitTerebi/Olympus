#include "athena.h"

#include "textures/egametextures.h"

Apollo::Apollo(GameBoard& board) :
    ExtendedGod(board, &eGodTextures::fApollo, GodType::apollo) {
    eGameTextures::loadApollo();
}

Artemis::Artemis(GameBoard& board) :
    ExtendedGod(board, &eGodTextures::fArtemis, GodType::artemis) {
    eGameTextures::loadArtemis();
}

Athena::Athena(GameBoard& board) :
    ExtendedGod(board, &eGodTextures::fAthena, GodType::athena) {
    eGameTextures::loadAthena();
}

Atlas::Atlas(GameBoard& board) :
    ExtendedGod(board, &eGodTextures::fAtlas, GodType::atlas) {
    eGameTextures::loadAtlas();
}

Zeus::Zeus(GameBoard& board) :
    ExtendedGod(board, &eGodTextures::fZeus, GodType::zeus) {
    eGameTextures::loadZeus();
}
