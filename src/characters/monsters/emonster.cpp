#include "emonster.h"

#include "characters/gods/god.h"
#include "characters/heroes/ehero.h"

#include "ecalydonianboar.h"
#include "engine/game-board.h"
#include "engine/ecityid.h"
#include "characters/actions/monster-action.h"

#include "language.h"

eMonster::eMonster(GameBoard& board, const eMonsterType mt) :
    eCharacter(board, sMonsterToCharacterType(mt)) {
    setAttack(0.5);
}

bool eMonster::takeDamage(const double a, eCharacter* const attacker) {
    const bool dead = eCharacter::takeDamage(a, attacker);
    if(dead || !attacker || attacker->dead()) return dead;
    if(!eTeamIdHelpers::isEnemy(attacker->teamId(), teamId())) return dead;
    const auto ma = dynamic_cast<MonsterAction*>(action());
    if(!ma) return dead;
    ma->retaliate(attacker);
    return dead;
}

eMonster::~eMonster() {
    auto& board = getBoard();
    const auto ocid = onCityId();
    board.unregisterMonster(ocid, this);
}

eMonsterType eMonster::sCharacterToMonsterType(const eCharacterType type,
                                               bool* const valid) {
    if(valid) *valid = true;
    switch(type) {
    case eCharacterType::calydonianBoar: return eMonsterType::calydonianBoar;
    case eCharacterType::cerberus: return eMonsterType::cerberus;
    case eCharacterType::chimera: return eMonsterType::chimera;
    case eCharacterType::cyclops: return eMonsterType::cyclops;
    case eCharacterType::dragon: return eMonsterType::dragon;
    case eCharacterType::echidna: return eMonsterType::echidna;
    case eCharacterType::harpies: return eMonsterType::harpies;
    case eCharacterType::hector: return eMonsterType::hector;
    case eCharacterType::hydra: return eMonsterType::hydra;
    case eCharacterType::kraken: return eMonsterType::kraken;
    case eCharacterType::maenads: return eMonsterType::maenads;
    case eCharacterType::medusa: return eMonsterType::medusa;
    case eCharacterType::minotaur: return eMonsterType::minotaur;
    case eCharacterType::scylla: return eMonsterType::scylla;
    case eCharacterType::sphinx: return eMonsterType::sphinx;
    case eCharacterType::talos: return eMonsterType::talos;
    case eCharacterType::satyr: return eMonsterType::satyr;
    default:
        if(valid) *valid = false;
        return eMonsterType::calydonianBoar;
    }
}

eCharacterType eMonster::sMonsterToCharacterType(const eMonsterType type) {
    switch(type) {
    case eMonsterType::calydonianBoar: return eCharacterType::calydonianBoar;
    case eMonsterType::cerberus: return eCharacterType::cerberus;
    case eMonsterType::chimera: return eCharacterType::chimera;
    case eMonsterType::cyclops: return eCharacterType::cyclops;
    case eMonsterType::dragon: return eCharacterType::dragon;
    case eMonsterType::echidna: return eCharacterType::echidna;
    case eMonsterType::harpies: return eCharacterType::harpies;
    case eMonsterType::hector: return eCharacterType::hector;
    case eMonsterType::hydra: return eCharacterType::hydra;
    case eMonsterType::kraken: return eCharacterType::kraken;
    case eMonsterType::maenads: return eCharacterType::maenads;
    case eMonsterType::medusa: return eCharacterType::medusa;
    case eMonsterType::minotaur: return eCharacterType::minotaur;
    case eMonsterType::scylla: return eCharacterType::scylla;
    case eMonsterType::sphinx: return eCharacterType::sphinx;
    case eMonsterType::talos: return eCharacterType::talos;
    case eMonsterType::satyr: return eCharacterType::satyr;
    default: return eCharacterType::calydonianBoar;
    }
}

int eMonster::sMonsterAttackTime(const eMonsterType type) {
    (void)type;
    return 500;
}

stdsptr<eMonster> eMonster::sCreateMonster(const eMonsterType type, GameBoard& board) {
    switch(type) {
    case eMonsterType::calydonianBoar:
        return e::make_shared<eCalydonianBoar>(board);
    case eMonsterType::cerberus:
        return e::make_shared<eCerberus>(board);
    case eMonsterType::chimera:
        return e::make_shared<eChimera>(board);
    case eMonsterType::cyclops:
        return e::make_shared<eCyclops>(board);
    case eMonsterType::dragon:
        return e::make_shared<eDragon>(board);
    case eMonsterType::echidna:
        return e::make_shared<eEchidna>(board);
    case eMonsterType::harpies:
        return e::make_shared<eHarpies>(board);
    case eMonsterType::hector:
        return e::make_shared<eHector>(board);
    case eMonsterType::hydra:
        return e::make_shared<eHydra>(board);
    case eMonsterType::kraken:
        return e::make_shared<eKraken>(board);
    case eMonsterType::maenads:
        return e::make_shared<eMaenads>(board);
    case eMonsterType::medusa:
        return e::make_shared<eMedusa>(board);
    case eMonsterType::minotaur:
        return e::make_shared<eMinotaur>(board);
    case eMonsterType::scylla:
        return e::make_shared<eScylla>(board);
    case eMonsterType::sphinx:
        return e::make_shared<eSphinx>(board);
    case eMonsterType::talos:
        return e::make_shared<eTalos>(board);
    case eMonsterType::satyr:
        return e::make_shared<eSatyr>(board);
    }
    return nullptr;
}

GodType eMonster::sMonsterSender(const eMonsterType type, bool* const valid) {
    if(valid) *valid = true;
    switch(type) {
    case eMonsterType::calydonianBoar: return GodType::artemis;
    case eMonsterType::cerberus: return GodType::hades;
    case eMonsterType::chimera: return GodType::atlas;
    case eMonsterType::cyclops: return GodType::zeus;
    case eMonsterType::dragon: return GodType::ares;

    case eMonsterType::hector: return GodType::aphrodite;
    case eMonsterType::hydra: return GodType::athena;
    case eMonsterType::kraken: return GodType::poseidon;
    case eMonsterType::maenads: return GodType::dionysus;
    case eMonsterType::medusa: return GodType::demeter;
    case eMonsterType::minotaur: return GodType::hermes;
    case eMonsterType::scylla: return GodType::apollo;
    case eMonsterType::sphinx: return GodType::hera;
    case eMonsterType::talos: return GodType::hephaestus;
    case eMonsterType::satyr: return GodType::dionysus;

    case eMonsterType::echidna:
    case eMonsterType::harpies:
        if(valid) *valid = false;
        return GodType::artemis;
    }
    return GodType::athena;
}

eMonsterType eMonster::sGodsMinion(const GodType type) {
    switch(type) {
    case GodType::artemis: return eMonsterType::calydonianBoar;
    case GodType::hades: return eMonsterType::cerberus;
    case GodType::atlas: return eMonsterType::chimera;
    case GodType::zeus: return eMonsterType::cyclops;
    case GodType::ares: return eMonsterType::dragon;

    case GodType::aphrodite: return eMonsterType::hector;
    case GodType::athena: return eMonsterType::hydra;
    case GodType::poseidon: return eMonsterType::kraken;
    case GodType::dionysus: return eMonsterType::maenads;
    case GodType::demeter: return eMonsterType::medusa;
    case GodType::hermes: return eMonsterType::minotaur;
    case GodType::apollo: return eMonsterType::scylla;
    case GodType::hera: return eMonsterType::sphinx;
    case GodType::hephaestus: return eMonsterType::talos;
    }
    return eMonsterType::talos;
}

eHeroType eMonster::sSlayer(const eMonsterType type) {
    switch(type) {
    case eMonsterType::calydonianBoar: return eHeroType::theseus;
    case eMonsterType::cerberus: return eHeroType::hercules;
    case eMonsterType::chimera: return eHeroType::bellerophon;
    case eMonsterType::cyclops: return eHeroType::odysseus;
    case eMonsterType::dragon: return eHeroType::jason;
    case eMonsterType::echidna: return eHeroType::bellerophon;
    case eMonsterType::harpies: return eHeroType::atalanta;
    case eMonsterType::hector: return eHeroType::achilles;
    case eMonsterType::hydra: return eHeroType::hercules;
    case eMonsterType::kraken: return eHeroType::perseus;
    case eMonsterType::maenads: return eHeroType::achilles;
    case eMonsterType::medusa: return eHeroType::perseus;
    case eMonsterType::minotaur: return eHeroType::theseus;
    case eMonsterType::scylla: return eHeroType::odysseus;
    case eMonsterType::sphinx: return eHeroType::atalanta;
    case eMonsterType::talos: return eHeroType::jason;
    default: return eHeroType::jason;
    }
}

std::string eMonster::sMonsterName(const eMonsterType type) {
    const int group = 190;
    int string = -1;
    switch(type) {
    case eMonsterType::calydonianBoar:
        string = 6;
        break;
    case eMonsterType::cerberus:
        string = 2;
        break;
    case eMonsterType::chimera:
        string = 13;
        break;
    case eMonsterType::cyclops:
        string = 3;
        break;
    case eMonsterType::dragon:
        string = 7;
        break;
    case eMonsterType::echidna:
        string = 15;
        break;
    case eMonsterType::harpies:
        string = 14;
        break;
    case eMonsterType::hector:
        string = 9;
        break;
    case eMonsterType::hydra:
        string = 0;
        break;
    case eMonsterType::kraken:
        string = 1;
        break;
    case eMonsterType::maenads:
        string = 11;
        break;
    case eMonsterType::medusa:
        string = 5;
        break;
    case eMonsterType::minotaur:
        string = 4;
        break;
    case eMonsterType::scylla:
        string = 10;
        break;
    case eMonsterType::sphinx:
        string = 12;
        break;
    case eMonsterType::talos:
        string = 8;
        break;
    case eMonsterType::satyr:
        string = 17;
        break;
    }
    return Language::zeusText(group, string);
}

void eMonster::sMonsterStrings(
        std::vector<eMonsterType>& mnstrs,
        std::vector<std::string>& monsterStrs,
        const bool withGodsOnly,
        const bool showGodNames) {
    const int iMin = static_cast<int>(eMonsterType::calydonianBoar);
    const int iMax = static_cast<int>(eMonsterType::talos);
    std::vector<eMonsterType> allMonsters;
    for(int i = iMin; i <= iMax; i++) {
        const auto mi = static_cast<eMonsterType>(i);
        allMonsters.push_back(mi);
    }

    const std::vector<std::string> monsterGods {
        God::sGodName(GodType::artemis),
        God::sGodName(GodType::hades),
        God::sGodName(GodType::atlas),
        God::sGodName(GodType::zeus),
        God::sGodName(GodType::ares),
        "-",
        "-",
        God::sGodName(GodType::aphrodite),
        God::sGodName(GodType::athena),
        God::sGodName(GodType::poseidon),
        God::sGodName(GodType::dionysus),
        God::sGodName(GodType::demeter),
        God::sGodName(GodType::hermes),
        God::sGodName(GodType::apollo),
        God::sGodName(GodType::hera),
        God::sGodName(GodType::hephaestus)
    };
    const std::vector<std::string> monsterHeroes {
        eHero::sHeroName(eHeroType::theseus),
        eHero::sHeroName(eHeroType::hercules),
        eHero::sHeroName(eHeroType::bellerophon),
        eHero::sHeroName(eHeroType::odysseus),
        eHero::sHeroName(eHeroType::jason),
        eHero::sHeroName(eHeroType::bellerophon),
        eHero::sHeroName(eHeroType::atalanta),
        eHero::sHeroName(eHeroType::achilles),
        eHero::sHeroName(eHeroType::hercules),
        eHero::sHeroName(eHeroType::perseus),
        eHero::sHeroName(eHeroType::achilles),
        eHero::sHeroName(eHeroType::perseus),
        eHero::sHeroName(eHeroType::theseus),
        eHero::sHeroName(eHeroType::odysseus),
        eHero::sHeroName(eHeroType::atalanta),
        eHero::sHeroName(eHeroType::jason)
    };

    const int iMax2 = allMonsters.size();
    for(int i = 0; i < iMax2; i++) {
        const auto m = allMonsters[i];
        if(withGodsOnly) {
            if(m == eMonsterType::echidna || m == eMonsterType::harpies) {
                continue;
            }
        }
        const auto& g = monsterGods[i];
        const auto& h = monsterHeroes[i];
        mnstrs.push_back(m);
        std::string str = eMonster::sMonsterName(m);
        if(showGodNames) {
            str = str + " (" + g + ", " + h + ")";
        } else {
            str = str + " (" + h + ")";
        }
        monsterStrs.push_back(str);
    }
}
