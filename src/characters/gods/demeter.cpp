#include "demeter.h"

#include "textures/egametextures.h"

Aphrodite::Aphrodite(GameBoard& board) :
    BasicGod(board, &eGodTextures::fAphrodite, GodType::aphrodite) {
    eGameTextures::loadAphrodite();
}

Ares::Ares(GameBoard& board) :
    BasicGod(board, &eGodTextures::fAres, GodType::ares) {
    eGameTextures::loadAres();
}

Demeter::Demeter(GameBoard& board) :
    BasicGod(board, &eGodTextures::fDemeter, GodType::demeter) {
    eGameTextures::loadDemeter();
}

Hades::Hades(GameBoard& board) :
    BasicGod(board, &eGodTextures::fHades, GodType::hades) {
    eGameTextures::loadHades();
}

Hephaestus::Hephaestus(GameBoard& board) :
    BasicGod(board, &eGodTextures::fHephaestus, GodType::hephaestus) {
    eGameTextures::loadHephaestus();
}

Hera::Hera(GameBoard& board) :
    BasicGod(board, &eGodTextures::fHera, GodType::hera) {
    eGameTextures::loadHera();
}

Poseidon::Poseidon(GameBoard& board) :
    BasicGod(board, &eGodTextures::fPoseidon, GodType::poseidon) {
    eGameTextures::loadPoseidon();
}
