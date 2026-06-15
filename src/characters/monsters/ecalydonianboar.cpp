#include "ecalydonianboar.h"

#include "textures/game-textures.h"

eCalydonianBoar::eCalydonianBoar(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fCalydonianBoar,
                  eMonsterType::calydonianBoar) {
    GameTextures::loadCalydonianBoar();
}

eCerberus::eCerberus(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fCerberus,
                  eMonsterType::cerberus) {
    GameTextures::loadCerberus();
}

eChimera::eChimera(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fChimera,
                  eMonsterType::chimera) {
    GameTextures::loadChimera();
}

eCyclops::eCyclops(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fCyclops,
                  eMonsterType::cyclops) {
    GameTextures::loadCyclops();
}

eDragon::eDragon(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fDragon,
                  eMonsterType::dragon) {
    GameTextures::loadDragon();
}

eEchidna::eEchidna(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fEchidna,
                  eMonsterType::echidna) {
    GameTextures::loadEchidna();
}

eHarpies::eHarpies(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fHarpies,
                  eMonsterType::harpies) {
    GameTextures::loadHarpie();
}

eHector::eHector(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fHector,
                  eMonsterType::hector) {
    GameTextures::loadHector();
}

eHydra::eHydra(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fHydra,
                  eMonsterType::hydra) {
    GameTextures::loadHydra();
}

eKraken::eKraken(GameBoard& board) :
    eWaterMonster(board, &CharacterTextures::fKraken,
                  eMonsterType::kraken) {
    GameTextures::loadKraken();
}

eMaenads::eMaenads(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fMaenads,
                  eMonsterType::maenads) {
    GameTextures::loadMaenads();
}

eMedusa::eMedusa(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fMedusa,
                  eMonsterType::medusa) {
    GameTextures::loadMedusa();
}

eMinotaur::eMinotaur(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fMinotaur,
                  eMonsterType::minotaur) {
    GameTextures::loadMinotaur();
}

eScylla::eScylla(GameBoard& board) :
    eWaterMonster(board, &CharacterTextures::fScylla,
                  eMonsterType::scylla) {
    GameTextures::loadScylla();
}

eSphinx::eSphinx(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fSphinx,
                  eMonsterType::sphinx) {
    GameTextures::loadSphinx();
}

eTalos::eTalos(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fTalos,
                  eMonsterType::talos) {
    GameTextures::loadTalos();
}

eSatyr::eSatyr(GameBoard& board) :
    eBasicMonster(board, &CharacterTextures::fSatyr,
                  eMonsterType::satyr) {
    GameTextures::loadSatyr();
}

