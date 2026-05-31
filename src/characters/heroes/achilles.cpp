#include "achilles.h"

#include "textures/egametextures.h"

Achilles::Achilles(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fAchilles,
               eHeroType::achilles) {
    eGameTextures::loadAchilles();
}

Atalanta::Atalanta(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fAtalanta,
               eHeroType::atalanta) {
    eGameTextures::loadAtalanta();
}

Bellerophon::Bellerophon(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fBellerophon,
               eHeroType::bellerophon) {
    eGameTextures::loadBellerophon();
}

Hercules::Hercules(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fHercules,
               eHeroType::hercules) {
    eGameTextures::loadHeracles();
}

Jason::Jason(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fJason,
               eHeroType::jason) {
    eGameTextures::loadJason();
}

Odysseus::Odysseus(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fOdysseus,
               eHeroType::odysseus) {
    eGameTextures::loadOdysseus();
}

Perseus::Perseus(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fPerseus,
               eHeroType::perseus) {
    eGameTextures::loadPerseus();
}

Theseus::Theseus(GameBoard& board) :
    eBasicHero(board, &eCharacterTextures::fTheseus,
               eHeroType::theseus) {
    eGameTextures::loadTheseus();
}
