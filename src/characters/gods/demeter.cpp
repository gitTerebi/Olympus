#include "demeter.h"

#include "textures/game-textures.h"

Aphrodite::Aphrodite(GameBoard& board) :
    BasicGod(board, &GodTextures::fAphrodite, GodType::aphrodite) {
    GameTextures::loadAphrodite();
}

Ares::Ares(GameBoard& board) :
    BasicGod(board, &GodTextures::fAres, GodType::ares) {
    GameTextures::loadAres();
}

Demeter::Demeter(GameBoard& board) :
    BasicGod(board, &GodTextures::fDemeter, GodType::demeter) {
    GameTextures::loadDemeter();
}

Hades::Hades(GameBoard& board) :
    BasicGod(board, &GodTextures::fHades, GodType::hades) {
    GameTextures::loadHades();
}

Hephaestus::Hephaestus(GameBoard& board) :
    BasicGod(board, &GodTextures::fHephaestus, GodType::hephaestus) {
    GameTextures::loadHephaestus();
}

Hera::Hera(GameBoard& board) :
    BasicGod(board, &GodTextures::fHera, GodType::hera) {
    GameTextures::loadHera();
}

Poseidon::Poseidon(GameBoard& board) :
    BasicGod(board, &GodTextures::fPoseidon, GodType::poseidon) {
    GameTextures::loadPoseidon();
}
