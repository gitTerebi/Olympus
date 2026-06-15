#include "achilles.h"

#include "textures/game-textures.h"

Achilles::Achilles(GameBoard& board) :
    eBasicHero(board, &CharacterTextures::fAchilles,
               eHeroType::achilles) {
    GameTextures::loadAchilles();
}

Atalanta::Atalanta(GameBoard& board) :
    eBasicHero(board, &CharacterTextures::fAtalanta,
               eHeroType::atalanta) {
    GameTextures::loadAtalanta();
}

Bellerophon::Bellerophon(GameBoard& board) :
    eBasicHero(board, &CharacterTextures::fBellerophon,
               eHeroType::bellerophon) {
    GameTextures::loadBellerophon();
}

Hercules::Hercules(GameBoard& board) :
    eBasicHero(board, &CharacterTextures::fHercules,
               eHeroType::hercules) {
    GameTextures::loadHeracles();
}

Jason::Jason(GameBoard& board) :
    eBasicHero(board, &CharacterTextures::fJason,
               eHeroType::jason) {
    GameTextures::loadJason();
}

Odysseus::Odysseus(GameBoard& board) :
    eBasicHero(board, &CharacterTextures::fOdysseus,
               eHeroType::odysseus) {
    GameTextures::loadOdysseus();
}

Perseus::Perseus(GameBoard& board) :
    eBasicHero(board, &CharacterTextures::fPerseus,
               eHeroType::perseus) {
    GameTextures::loadPerseus();
}

Theseus::Theseus(GameBoard& board) :
    eBasicHero(board, &CharacterTextures::fTheseus,
               eHeroType::theseus) {
    GameTextures::loadTheseus();
}
