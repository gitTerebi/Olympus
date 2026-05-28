#include "edemeter.h"

#include "textures/egametextures.h"

eAphrodite::eAphrodite(GameBoard& board) :
    eBasicGod(board, &eGodTextures::fAphrodite, eGodType::aphrodite) {
    eGameTextures::loadAphrodite();
}

eAres::eAres(GameBoard& board) :
    eBasicGod(board, &eGodTextures::fAres, eGodType::ares) {
    eGameTextures::loadAres();
}

eDemeter::eDemeter(GameBoard& board) :
    eBasicGod(board, &eGodTextures::fDemeter, eGodType::demeter) {
    eGameTextures::loadDemeter();
}

eHades::eHades(GameBoard& board) :
    eBasicGod(board, &eGodTextures::fHades, eGodType::hades) {
    eGameTextures::loadHades();
}

eHephaestus::eHephaestus(GameBoard& board) :
    eBasicGod(board, &eGodTextures::fHephaestus, eGodType::hephaestus) {
    eGameTextures::loadHephaestus();
}

eHera::eHera(GameBoard& board) :
    eBasicGod(board, &eGodTextures::fHera, eGodType::hera) {
    eGameTextures::loadHera();
}

ePoseidon::ePoseidon(GameBoard& board) :
    eBasicGod(board, &eGodTextures::fPoseidon, eGodType::poseidon) {
    eGameTextures::loadPoseidon();
}
