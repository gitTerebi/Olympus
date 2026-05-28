#include "eachilles.h"

#include "textures/egametextures.h"

eAchilles::eAchilles(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fAchilles,
               eHeroType::achilles) {
    eGameTextures::loadAchilles();
}

eAtalanta::eAtalanta(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fAtalanta,
               eHeroType::atalanta) {
    eGameTextures::loadAtalanta();
}

eBellerophon::eBellerophon(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fBellerophon,
               eHeroType::bellerophon) {
    eGameTextures::loadBellerophon();
}

eHercules::eHercules(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fHercules,
               eHeroType::hercules) {
    eGameTextures::loadHeracles();
}

eJason::eJason(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fJason,
               eHeroType::jason) {
    eGameTextures::loadJason();
}

eOdysseus::eOdysseus(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fOdysseus,
               eHeroType::odysseus) {
    eGameTextures::loadOdysseus();
}

ePerseus::ePerseus(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fPerseus,
               eHeroType::perseus) {
    eGameTextures::loadPerseus();
}

eTheseus::eTheseus(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fTheseus,
               eHeroType::theseus) {
    eGameTextures::loadTheseus();
}
