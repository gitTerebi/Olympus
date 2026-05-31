#include "eathena.h"

#include "textures/egametextures.h"

eApollo::eApollo(GameBoard& board) :
    eExtendedGod(board, &eGodTextures::fApollo, eGodType::apollo) {
    eGameTextures::loadApollo();
}

eArtemis::eArtemis(GameBoard& board) :
    eExtendedGod(board, &eGodTextures::fArtemis, eGodType::artemis) {
    eGameTextures::loadArtemis();
}

eAthena::eAthena(GameBoard& board) :
    eExtendedGod(board, &eGodTextures::fAthena, eGodType::athena) {
    eGameTextures::loadAthena();
}

eAtlas::eAtlas(GameBoard& board) :
    eExtendedGod(board, &eGodTextures::fAtlas, eGodType::atlas) {
    eGameTextures::loadAtlas();
}

eZeus::eZeus(GameBoard& board) :
    eExtendedGod(board, &eGodTextures::fZeus, eGodType::zeus) {
    eGameTextures::loadZeus();
}
