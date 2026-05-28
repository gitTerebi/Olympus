#include "vanilla-stats.h"

#include "characters/echaracterbase.h"
#include "engine/e-game-board.h"
#include "engine/edifficulty.h"
#include "engine/model-data.h"

namespace VanillaStats {

const char* figureName(const eCharacterType type) {
    switch(type) {
    case eCharacterType::rockThrower:
    case eCharacterType::archer:
    case eCharacterType::archerPoseidon:
        return "Rabble/Archer";
    case eCharacterType::hoplite:
    case eCharacterType::hoplitePoseidon:
        return "Hoplite/Spearman";
    case eCharacterType::horseman:
    case eCharacterType::chariot:
    case eCharacterType::chariotPoseidon:
        return "Horseman/Chariot";
    case eCharacterType::trireme:
    case eCharacterType::enemyBoat:
        return "Trireme";
    case eCharacterType::wolf:
        return "Wolf";
    case eCharacterType::boar:
        return "Boar";
    case eCharacterType::deer:
        return "Sheep";
    case eCharacterType::hunter:
        return "Hunter";
    case eCharacterType::watchman:
        return "Watchman";
    case eCharacterType::aphrodite:
    case eCharacterType::apollo:
    case eCharacterType::ares:
    case eCharacterType::artemis:
    case eCharacterType::athena:
    case eCharacterType::atlas:
    case eCharacterType::demeter:
    case eCharacterType::dionysus:
    case eCharacterType::hades:
    case eCharacterType::hephaestus:
    case eCharacterType::hera:
    case eCharacterType::hermes:
    case eCharacterType::poseidon:
    case eCharacterType::zeus:
        return "God";
    case eCharacterType::calydonianBoar:
    case eCharacterType::cerberus:
    case eCharacterType::chimera:
    case eCharacterType::cyclops:
    case eCharacterType::dragon:
    case eCharacterType::echidna:
    case eCharacterType::harpies:
    case eCharacterType::hector:
    case eCharacterType::hydra:
    case eCharacterType::kraken:
    case eCharacterType::maenads:
    case eCharacterType::medusa:
    case eCharacterType::minotaur:
    case eCharacterType::scylla:
    case eCharacterType::sphinx:
    case eCharacterType::talos:
    case eCharacterType::satyr:
        return "Monster";
    case eCharacterType::achilles:
    case eCharacterType::atalanta:
    case eCharacterType::bellerophon:
    case eCharacterType::hercules:
    case eCharacterType::jason:
    case eCharacterType::odysseus:
    case eCharacterType::perseus:
    case eCharacterType::theseus:
        return "Hero";
    default:
        return nullptr;
    }
}

int greekEnemyFigureId(const eCharacterType type) {
    switch(type) {
    case eCharacterType::rockThrower:
    case eCharacterType::greekRockThrower:
        return 1;
    case eCharacterType::hoplite:
    case eCharacterType::hoplitePoseidon:
    case eCharacterType::greekHoplite:
        return 0;
    case eCharacterType::horseman:
    case eCharacterType::greekHorseman:
        return 2;
    case eCharacterType::trireme:
        return 3;
    default:
        return -1;
    }
}

void apply(eCharacterBase& c, const eDifficulty d) {
    const char* name = figureName(c.type());
    const int greekId = greekEnemyFigureId(c.type());
    if(!name && greekId < 0) return;
    const auto& md = ModelData::instance();
    if(!md.loaded()) return;
    const FigureStats* s = greekId >= 0 ? md.enemyFigure(d, greekId) :
                                          md.figure(d, name);
    if(!s) return;
    c.setHP(s->hp);
    c.setAttack(s->att);
    c.setArmor(s->arm);
    c.setArmorVsMissiles(s->avsm);
    c.setMissileAttack(s->ma);
    c.setAttackFreq(s->frq);
    c.setMissileFreq(s->mrf);
    if(s->spd > 0) c.setSpeed(s->spd * 9.067);
    if(s->mr > 0) c.setRange(s->mr);
}

void applyForCity(eCharacterBase& c, GameBoard& board) {
    const auto cid = c.cityId();
    const auto pid = board.cityIdToPlayerId(cid);
    const auto d = board.difficulty(pid);
    apply(c, d);
}

}
