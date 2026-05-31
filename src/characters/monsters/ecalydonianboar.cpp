#include "ecalydonianboar.h"

#include "textures/egametextures.h"

eCalydonianBoar::eCalydonianBoar(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fCalydonianBoar,
                  eMonsterType::calydonianBoar) {
    eGameTextures::loadCalydonianBoar();
}

eCerberus::eCerberus(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fCerberus,
                  eMonsterType::cerberus) {
    eGameTextures::loadCerberus();
}

eChimera::eChimera(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fChimera,
                  eMonsterType::chimera) {
    eGameTextures::loadChimera();
}

eCyclops::eCyclops(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fCyclops,
                  eMonsterType::cyclops) {
    eGameTextures::loadCyclops();
}

eDragon::eDragon(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fDragon,
                  eMonsterType::dragon) {
    eGameTextures::loadDragon();
}

eEchidna::eEchidna(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fEchidna,
                  eMonsterType::echidna) {
    eGameTextures::loadEchidna();
}

eHarpies::eHarpies(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fHarpies,
                  eMonsterType::harpies) {
    eGameTextures::loadHarpie();
}

eHector::eHector(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fHector,
                  eMonsterType::hector) {
    eGameTextures::loadHector();
}

eHydra::eHydra(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fHydra,
                  eMonsterType::hydra) {
    eGameTextures::loadHydra();
}

eKraken::eKraken(GameBoard& board) :
    eWaterMonster(board, &eCharacterTextures::fKraken,
                  eMonsterType::kraken) {
    eGameTextures::loadKraken();
}

eMaenads::eMaenads(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fMaenads,
                  eMonsterType::maenads) {
    eGameTextures::loadMaenads();
}

eMedusa::eMedusa(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fMedusa,
                  eMonsterType::medusa) {
    eGameTextures::loadMedusa();
}

eMinotaur::eMinotaur(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fMinotaur,
                  eMonsterType::minotaur) {
    eGameTextures::loadMinotaur();
}

eScylla::eScylla(GameBoard& board) :
    eWaterMonster(board, &eCharacterTextures::fScylla,
                  eMonsterType::scylla) {
    eGameTextures::loadScylla();
}

eSphinx::eSphinx(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fSphinx,
                  eMonsterType::sphinx) {
    eGameTextures::loadSphinx();
}

eTalos::eTalos(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fTalos,
                  eMonsterType::talos) {
    eGameTextures::loadTalos();
}

eSatyr::eSatyr(GameBoard& board) :
    eBasicMonster(board, &eCharacterTextures::fSatyr,
                  eMonsterType::satyr) {
    eGameTextures::loadSatyr();
}

