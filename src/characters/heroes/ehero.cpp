#include "ehero.h"

#include "achilles.h"

#include "elanguage.h"

eHero::eHero(GameBoard& board, const eHeroType mt) :
    eCharacter(board, sHeroToCharacterType(mt)) {
    setAttack(0.5);
}

eHeroType eHero::sCharacterToHeroType(const eCharacterType type,
                                      bool* const valid) {
    if(valid) *valid = true;
    switch(type) {
    case eCharacterType::achilles: return eHeroType::achilles;
    case eCharacterType::atalanta: return eHeroType::atalanta;
    case eCharacterType::bellerophon: return eHeroType::bellerophon;
    case eCharacterType::hercules: return eHeroType::hercules;
    case eCharacterType::jason: return eHeroType::jason;
    case eCharacterType::odysseus: return eHeroType::odysseus;
    case eCharacterType::perseus: return eHeroType::perseus;
    case eCharacterType::theseus: return eHeroType::theseus;
    default:
        if(valid) *valid = false;
        return eHeroType::achilles;
    }
}

bool eHero::sRangedHero(const eHeroType ht) {
    return ht == eHeroType::atalanta;
}

int eHero::sHeroAttackTime(const eHeroType ht) {
    if(ht == eHeroType::atalanta) return 360;
    return 0;
}

eHero::eTexPtr eHero::sHeroMissile(const eHeroType gt) {
    switch(gt) {
    case eHeroType::atalanta:
        GameTextures::loadGodBlueArrow();
        return &DestructionTextures::fGodBlueArrow;
    default:
        return nullptr;
    }
    return nullptr;
}

eCharacterType eHero::sHeroToCharacterType(const eHeroType type) {
    switch(type) {
    case eHeroType::achilles: return eCharacterType::achilles;
    case eHeroType::atalanta: return eCharacterType::atalanta;
    case eHeroType::bellerophon: return eCharacterType::bellerophon;
    case eHeroType::hercules: return eCharacterType::hercules;
    case eHeroType::jason: return eCharacterType::jason;
    case eHeroType::odysseus: return eCharacterType::odysseus;
    case eHeroType::perseus: return eCharacterType::perseus;
    case eHeroType::theseus: return eCharacterType::theseus;
    default: return eCharacterType::achilles;
    }
}

stdsptr<eHero> eHero::sCreateHero(const eHeroType type, GameBoard& board) {
    switch(type) {
    case eHeroType::achilles:
        return e::make_shared<Achilles>(board);
    case eHeroType::atalanta:
        return e::make_shared<Atalanta>(board);
    case eHeroType::bellerophon:
        return e::make_shared<Bellerophon>(board);
    case eHeroType::hercules:
        return e::make_shared<Hercules>(board);
    case eHeroType::jason:
        return e::make_shared<Jason>(board);
    case eHeroType::odysseus:
        return e::make_shared<Odysseus>(board);
    case eHeroType::perseus:
        return e::make_shared<Perseus>(board);
    case eHeroType::theseus:
        return e::make_shared<Theseus>(board);
    }
}

std::string eHero::sHeroName(const eHeroType ht) {
    const int group = 163;
    int string = -1;
    switch(ht) {
    case eHeroType::achilles:
        string = 0;
        break;
    case eHeroType::atalanta:
        string = 6;
        break;
    case eHeroType::bellerophon:
        string = 7;
        break;
    case eHeroType::hercules:
        string = 1;
        break;
    case eHeroType::jason:
        string = 2;
        break;
    case eHeroType::odysseus:
        string = 3;
        break;
    case eHeroType::perseus:
        string = 4;
        break;
    case eHeroType::theseus:
        string = 5;
        break;
    }
    return eLanguage::zeusText(group, string);
}

void eHero::sHeroStrings(std::vector<eHeroType>& heros,
                       std::vector<std::string>& heroNames) {
    heros.push_back(eHeroType::achilles);
    heros.push_back(eHeroType::atalanta);
    heros.push_back(eHeroType::bellerophon);
    heros.push_back(eHeroType::hercules);
    heros.push_back(eHeroType::jason);
    heros.push_back(eHeroType::odysseus);
    heros.push_back(eHeroType::perseus);
    heros.push_back(eHeroType::theseus);
    for(const auto h : heros) {
        heroNames.push_back(sHeroName(h));
    }
}
