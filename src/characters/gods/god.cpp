#include "god.h"
#include "fileIO/esavearchive.h"

#include "athena.h"
#include "demeter.h"
#include "dionysus.h"
#include "hermes.h"

#include "elanguage.h"

#include "buildings/ebuilding.h"

#include "textures/egametextures.h"

God::God(GameBoard& board, const GodType gt) :
    eCharacter(board, sGodToCharacterType(gt)),
    mType(gt) {
    setAttack(0.5);
}

int sGodTextId(const GodType type) {
    switch(type) {
    case GodType::aphrodite:
        return 6;
    case GodType::apollo:
        return 3;
    case GodType::ares:
        return 5;
    case GodType::artemis:
        return 4;
    case GodType::athena:
        return 8;
    case GodType::atlas:
        return 13;
    case GodType::demeter:
        return 2;
    case GodType::dionysus:
        return 10;
    case GodType::hades:
        return 11;
    case GodType::hephaestus:
        return 9;
    case GodType::hera:
        return 12;
    case GodType::hermes:
        return 7;
    case GodType::poseidon:
        return 1;
    case GodType::zeus:
    default:
        return 0;
    }
}

GodType God::sCharacterToGodType(const eCharacterType type,
                                   bool* const valid) {
    if(valid) *valid = true;
    switch(type) {
    case eCharacterType::aphrodite: return GodType::aphrodite;
    case eCharacterType::apollo: return GodType::apollo;
    case eCharacterType::ares: return GodType::ares;
    case eCharacterType::artemis: return GodType::artemis;
    case eCharacterType::athena: return GodType::athena;
    case eCharacterType::atlas: return GodType::atlas;
    case eCharacterType::demeter: return GodType::demeter;
    case eCharacterType::dionysus: return GodType::dionysus;
    case eCharacterType::hades: return GodType::hades;
    case eCharacterType::hephaestus: return GodType::hephaestus;
    case eCharacterType::hera: return GodType::hera;
    case eCharacterType::hermes: return GodType::hermes;
    case eCharacterType::poseidon: return GodType::poseidon;
    case eCharacterType::zeus: return GodType::zeus;
    default:
        if(valid) *valid = false;
        return GodType::aphrodite;
    }
}

eCharacterType God::sGodToCharacterType(const GodType type) {
    switch(type) {
    case GodType::aphrodite: return eCharacterType::aphrodite;
    case GodType::apollo: return eCharacterType::apollo;
    case GodType::ares: return eCharacterType::ares;
    case GodType::artemis: return eCharacterType::artemis;
    case GodType::athena: return eCharacterType::athena;
    case GodType::atlas: return eCharacterType::atlas;
    case GodType::demeter: return eCharacterType::demeter;
    case GodType::dionysus: return eCharacterType::dionysus;
    case GodType::hades: return eCharacterType::hades;
    case GodType::hephaestus: return eCharacterType::hephaestus;
    case GodType::hera: return eCharacterType::hera;
    case GodType::hermes: return eCharacterType::hermes;
    case GodType::poseidon: return eCharacterType::poseidon;
    case GodType::zeus: return eCharacterType::zeus;
    default: return eCharacterType::aphrodite;
    }
}

stdsptr<God> God::sCreateGod(const GodType type, GameBoard& board) {
    switch(type) {
    case GodType::aphrodite:
        return e::make_shared<Aphrodite>(board);
    case GodType::apollo:
        return e::make_shared<Apollo>(board);
    case GodType::ares:
        return e::make_shared<Ares>(board);
    case GodType::artemis:
        return e::make_shared<Artemis>(board);
    case GodType::athena:
        return e::make_shared<Athena>(board);
    case GodType::atlas:
        return e::make_shared<Atlas>(board);
    case GodType::demeter:
        return e::make_shared<Demeter>(board);
    case GodType::dionysus:
        return e::make_shared<Dionysus>(board);
    case GodType::hades:
        return e::make_shared<Hades>(board);
    case GodType::hephaestus:
        return e::make_shared<Hephaestus>(board);
    case GodType::hera:
        return e::make_shared<Hera>(board);
    case GodType::hermes:
        return e::make_shared<Hermes>(board);
    case GodType::poseidon:
        return e::make_shared<Poseidon>(board);
    case GodType::zeus:
        return e::make_shared<Zeus>(board);
    }
    return nullptr;
}

God::eTexPtr God::sGodMissile(const GodType gt) {
    switch(gt) {
    case GodType::aphrodite:
        eGameTextures::loadGodBlueMissile();
        return &eDestructionTextures::fGodBlueMissile;
    case GodType::apollo:
        eGameTextures::loadGodOrangeArrow();
        return &eDestructionTextures::fGodOrangeArrow;
    case GodType::ares:
        eGameTextures::loadGodOrangeMissile();
        return &eDestructionTextures::fGodOrangeMissile;
    case GodType::artemis:
        eGameTextures::loadGodBlueArrow();
        return &eDestructionTextures::fGodBlueArrow;
    case GodType::athena:
        eGameTextures::loadGodRedMissile();
        return &eDestructionTextures::fGodRedMissile;
    case GodType::atlas:
        eGameTextures::loadGodPinkMissile();
        return &eDestructionTextures::fGodPinkMissile;
    case GodType::demeter:
        eGameTextures::loadGodGreenMissile();
        return &eDestructionTextures::fGodGreenMissile;
    case GodType::dionysus:
        eGameTextures::loadGodPurpleMissile();
        return &eDestructionTextures::fGodPurpleMissile;
    case GodType::hades:
        eGameTextures::loadGodOrangeMissile();
        return &eDestructionTextures::fGodOrangeMissile;
    case GodType::hephaestus:
        eGameTextures::loadGodOrangeMissile();
        return &eDestructionTextures::fGodOrangeMissile;
    case GodType::hera:
        eGameTextures::loadGodRedMissile();
        return &eDestructionTextures::fGodRedMissile;
    case GodType::hermes:
        eGameTextures::loadGodBlueMissile();
        return &eDestructionTextures::fGodBlueMissile;
    case GodType::poseidon:
        eGameTextures::loadGodBlueMissile();
        return &eDestructionTextures::fGodBlueMissile;
    case GodType::zeus:
        eGameTextures::loadGodBlueMissile();
        return &eDestructionTextures::fGodBlueMissile;
    }
    return nullptr;
}

std::map<std::pair<GodType, GodType>, GodType> gFightWinner {
    {{GodType::zeus, GodType::zeus}, GodType::zeus},
    {{GodType::zeus, GodType::poseidon}, GodType::zeus},
    {{GodType::zeus, GodType::demeter}, GodType::zeus},
    {{GodType::zeus, GodType::apollo}, GodType::zeus},
    {{GodType::zeus, GodType::artemis}, GodType::zeus},
    {{GodType::zeus, GodType::ares}, GodType::zeus},
    {{GodType::zeus, GodType::aphrodite}, GodType::zeus},
    {{GodType::zeus, GodType::hermes}, GodType::zeus},
    {{GodType::zeus, GodType::athena}, GodType::zeus},
    {{GodType::zeus, GodType::hephaestus}, GodType::zeus},
    {{GodType::zeus, GodType::dionysus}, GodType::zeus},
    {{GodType::zeus, GodType::hades}, GodType::zeus},
    {{GodType::zeus, GodType::hera}, GodType::hera},
    {{GodType::zeus, GodType::atlas}, GodType::zeus},

    {{GodType::poseidon, GodType::zeus}, GodType::zeus},
    {{GodType::poseidon, GodType::poseidon}, GodType::poseidon},
    {{GodType::poseidon, GodType::demeter}, GodType::poseidon},
    {{GodType::poseidon, GodType::apollo}, GodType::poseidon},
    {{GodType::poseidon, GodType::artemis}, GodType::poseidon},
    {{GodType::poseidon, GodType::ares}, GodType::poseidon},
    {{GodType::poseidon, GodType::aphrodite}, GodType::poseidon},
    {{GodType::poseidon, GodType::hermes}, GodType::poseidon},
    {{GodType::poseidon, GodType::athena}, GodType::poseidon},
    {{GodType::poseidon, GodType::hephaestus}, GodType::poseidon},
    {{GodType::poseidon, GodType::dionysus}, GodType::poseidon},
    {{GodType::poseidon, GodType::hades}, GodType::poseidon},
    {{GodType::poseidon, GodType::hera}, GodType::poseidon},
    {{GodType::poseidon, GodType::atlas}, GodType::poseidon},

    {{GodType::demeter, GodType::zeus}, GodType::zeus},
    {{GodType::demeter, GodType::poseidon}, GodType::poseidon},
    {{GodType::demeter, GodType::demeter}, GodType::demeter},
    {{GodType::demeter, GodType::apollo}, GodType::demeter},
    {{GodType::demeter, GodType::artemis}, GodType::demeter},
    {{GodType::demeter, GodType::ares}, GodType::demeter},
    {{GodType::demeter, GodType::aphrodite}, GodType::demeter},
    {{GodType::demeter, GodType::hermes}, GodType::demeter},
    {{GodType::demeter, GodType::athena}, GodType::demeter},
    {{GodType::demeter, GodType::hephaestus}, GodType::demeter},
    {{GodType::demeter, GodType::dionysus}, GodType::demeter},
    {{GodType::demeter, GodType::hades}, GodType::hades},
    {{GodType::demeter, GodType::hera}, GodType::hera},
    {{GodType::demeter, GodType::atlas}, GodType::demeter},

    {{GodType::apollo, GodType::zeus}, GodType::zeus},
    {{GodType::apollo, GodType::poseidon}, GodType::poseidon},
    {{GodType::apollo, GodType::demeter}, GodType::demeter},
    {{GodType::apollo, GodType::apollo}, GodType::apollo},
    {{GodType::apollo, GodType::artemis}, GodType::artemis},
    {{GodType::apollo, GodType::ares}, GodType::apollo},
    {{GodType::apollo, GodType::aphrodite}, GodType::apollo},
    {{GodType::apollo, GodType::hermes}, GodType::apollo},
    {{GodType::apollo, GodType::athena}, GodType::athena},
    {{GodType::apollo, GodType::hephaestus}, GodType::apollo},
    {{GodType::apollo, GodType::dionysus}, GodType::apollo},
    {{GodType::apollo, GodType::hades}, GodType::hades},
    {{GodType::apollo, GodType::hera}, GodType::hera},
    {{GodType::apollo, GodType::atlas}, GodType::apollo},

    {{GodType::artemis, GodType::zeus}, GodType::zeus},
    {{GodType::artemis, GodType::poseidon}, GodType::poseidon},
    {{GodType::artemis, GodType::demeter}, GodType::demeter},
    {{GodType::artemis, GodType::apollo}, GodType::artemis},
    {{GodType::artemis, GodType::artemis}, GodType::artemis},
    {{GodType::artemis, GodType::ares}, GodType::artemis},
    {{GodType::artemis, GodType::aphrodite}, GodType::artemis},
    {{GodType::artemis, GodType::hermes}, GodType::artemis},
    {{GodType::artemis, GodType::athena}, GodType::athena},
    {{GodType::artemis, GodType::hephaestus}, GodType::artemis},
    {{GodType::artemis, GodType::dionysus}, GodType::artemis},
    {{GodType::artemis, GodType::hades}, GodType::hades},
    {{GodType::artemis, GodType::hera}, GodType::hera},
    {{GodType::artemis, GodType::atlas}, GodType::artemis},

    {{GodType::ares, GodType::zeus}, GodType::zeus},
    {{GodType::ares, GodType::poseidon}, GodType::poseidon},
    {{GodType::ares, GodType::demeter}, GodType::demeter},
    {{GodType::ares, GodType::apollo}, GodType::apollo},
    {{GodType::ares, GodType::artemis}, GodType::artemis},
    {{GodType::ares, GodType::ares}, GodType::ares},
    {{GodType::ares, GodType::aphrodite}, GodType::aphrodite},
    {{GodType::ares, GodType::hermes}, GodType::ares},
    {{GodType::ares, GodType::athena}, GodType::athena},
    {{GodType::ares, GodType::hephaestus}, GodType::ares},
    {{GodType::ares, GodType::dionysus}, GodType::ares},
    {{GodType::ares, GodType::hades}, GodType::hades},
    {{GodType::ares, GodType::hera}, GodType::hera},
    {{GodType::ares, GodType::atlas}, GodType::atlas},

    {{GodType::aphrodite, GodType::zeus}, GodType::zeus},
    {{GodType::aphrodite, GodType::poseidon}, GodType::poseidon},
    {{GodType::aphrodite, GodType::demeter}, GodType::demeter},
    {{GodType::aphrodite, GodType::apollo}, GodType::apollo},
    {{GodType::aphrodite, GodType::artemis}, GodType::artemis},
    {{GodType::aphrodite, GodType::ares}, GodType::aphrodite},
    {{GodType::aphrodite, GodType::aphrodite}, GodType::aphrodite},
    {{GodType::aphrodite, GodType::hermes}, GodType::aphrodite},
    {{GodType::aphrodite, GodType::athena}, GodType::athena},
    {{GodType::aphrodite, GodType::hephaestus}, GodType::aphrodite},
    {{GodType::aphrodite, GodType::dionysus}, GodType::aphrodite},
    {{GodType::aphrodite, GodType::hades}, GodType::hades},
    {{GodType::aphrodite, GodType::hera}, GodType::hera},
    {{GodType::aphrodite, GodType::atlas}, GodType::atlas},

    {{GodType::hermes, GodType::zeus}, GodType::zeus},
    {{GodType::hermes, GodType::poseidon}, GodType::poseidon},
    {{GodType::hermes, GodType::demeter}, GodType::demeter},
    {{GodType::hermes, GodType::apollo}, GodType::apollo},
    {{GodType::hermes, GodType::artemis}, GodType::artemis},
    {{GodType::hermes, GodType::ares}, GodType::ares},
    {{GodType::hermes, GodType::aphrodite}, GodType::aphrodite},
    {{GodType::hermes, GodType::hermes}, GodType::hermes},
    {{GodType::hermes, GodType::athena}, GodType::athena},
    {{GodType::hermes, GodType::hephaestus}, GodType::hephaestus},
    {{GodType::hermes, GodType::dionysus}, GodType::hermes},
    {{GodType::hermes, GodType::hades}, GodType::hades},
    {{GodType::hermes, GodType::hera}, GodType::hera},
    {{GodType::hermes, GodType::atlas}, GodType::atlas},

    {{GodType::athena, GodType::zeus}, GodType::zeus},
    {{GodType::athena, GodType::poseidon}, GodType::poseidon},
    {{GodType::athena, GodType::demeter}, GodType::demeter},
    {{GodType::athena, GodType::apollo}, GodType::athena},
    {{GodType::athena, GodType::artemis}, GodType::athena},
    {{GodType::athena, GodType::ares}, GodType::athena},
    {{GodType::athena, GodType::aphrodite}, GodType::athena},
    {{GodType::athena, GodType::hermes}, GodType::athena},
    {{GodType::athena, GodType::athena}, GodType::athena},
    {{GodType::athena, GodType::hephaestus}, GodType::athena},
    {{GodType::athena, GodType::dionysus}, GodType::athena},
    {{GodType::athena, GodType::hades}, GodType::hades},
    {{GodType::athena, GodType::hera}, GodType::hera},
    {{GodType::athena, GodType::atlas}, GodType::athena},

    {{GodType::hephaestus, GodType::zeus}, GodType::zeus},
    {{GodType::hephaestus, GodType::poseidon}, GodType::poseidon},
    {{GodType::hephaestus, GodType::demeter}, GodType::demeter},
    {{GodType::hephaestus, GodType::apollo}, GodType::apollo},
    {{GodType::hephaestus, GodType::artemis}, GodType::artemis},
    {{GodType::hephaestus, GodType::ares}, GodType::ares},
    {{GodType::hephaestus, GodType::aphrodite}, GodType::aphrodite},
    {{GodType::hephaestus, GodType::hermes}, GodType::hephaestus},
    {{GodType::hephaestus, GodType::athena}, GodType::athena},
    {{GodType::hephaestus, GodType::hephaestus}, GodType::hephaestus},
    {{GodType::hephaestus, GodType::dionysus}, GodType::hephaestus},
    {{GodType::hephaestus, GodType::hades}, GodType::hades},
    {{GodType::hephaestus, GodType::hera}, GodType::hera},
    {{GodType::hephaestus, GodType::atlas}, GodType::atlas},

    {{GodType::dionysus, GodType::zeus}, GodType::zeus},
    {{GodType::dionysus, GodType::poseidon}, GodType::poseidon},
    {{GodType::dionysus, GodType::demeter}, GodType::demeter},
    {{GodType::dionysus, GodType::apollo}, GodType::apollo},
    {{GodType::dionysus, GodType::artemis}, GodType::artemis},
    {{GodType::dionysus, GodType::ares}, GodType::ares},
    {{GodType::dionysus, GodType::aphrodite}, GodType::aphrodite},
    {{GodType::dionysus, GodType::hermes}, GodType::hermes},
    {{GodType::dionysus, GodType::athena}, GodType::athena},
    {{GodType::dionysus, GodType::hephaestus}, GodType::hephaestus},
    {{GodType::dionysus, GodType::dionysus}, GodType::dionysus},
    {{GodType::dionysus, GodType::hades}, GodType::hades},
    {{GodType::dionysus, GodType::hera}, GodType::hera},
    {{GodType::dionysus, GodType::atlas}, GodType::atlas},

    {{GodType::hades, GodType::zeus}, GodType::zeus},
    {{GodType::hades, GodType::poseidon}, GodType::poseidon},
    {{GodType::hades, GodType::demeter}, GodType::hades},
    {{GodType::hades, GodType::apollo}, GodType::hades},
    {{GodType::hades, GodType::artemis}, GodType::hades},
    {{GodType::hades, GodType::ares}, GodType::hades},
    {{GodType::hades, GodType::aphrodite}, GodType::hades},
    {{GodType::hades, GodType::hermes}, GodType::hades},
    {{GodType::hades, GodType::athena}, GodType::hades},
    {{GodType::hades, GodType::hephaestus}, GodType::hades},
    {{GodType::hades, GodType::dionysus}, GodType::hades},
    {{GodType::hades, GodType::hades}, GodType::hades},
    {{GodType::hades, GodType::hera}, GodType::hades},
    {{GodType::hades, GodType::atlas}, GodType::hades},

    {{GodType::hera, GodType::zeus}, GodType::hera},
    {{GodType::hera, GodType::poseidon}, GodType::poseidon},
    {{GodType::hera, GodType::demeter}, GodType::hera},
    {{GodType::hera, GodType::apollo}, GodType::hera},
    {{GodType::hera, GodType::artemis}, GodType::hera},
    {{GodType::hera, GodType::ares}, GodType::hera},
    {{GodType::hera, GodType::aphrodite}, GodType::hera},
    {{GodType::hera, GodType::hermes}, GodType::hera},
    {{GodType::hera, GodType::athena}, GodType::hera},
    {{GodType::hera, GodType::hephaestus}, GodType::hera},
    {{GodType::hera, GodType::dionysus}, GodType::hera},
    {{GodType::hera, GodType::hades}, GodType::hades},
    {{GodType::hera, GodType::hera}, GodType::hera},
    {{GodType::hera, GodType::atlas}, GodType::hera},

    {{GodType::atlas, GodType::zeus}, GodType::zeus},
    {{GodType::atlas, GodType::poseidon}, GodType::poseidon},
    {{GodType::atlas, GodType::demeter}, GodType::demeter},
    {{GodType::atlas, GodType::apollo}, GodType::apollo},
    {{GodType::atlas, GodType::artemis}, GodType::artemis},
    {{GodType::atlas, GodType::ares}, GodType::atlas},
    {{GodType::atlas, GodType::aphrodite}, GodType::atlas},
    {{GodType::atlas, GodType::hermes}, GodType::atlas},
    {{GodType::atlas, GodType::athena}, GodType::athena},
    {{GodType::atlas, GodType::hephaestus}, GodType::atlas},
    {{GodType::atlas, GodType::dionysus}, GodType::atlas},
    {{GodType::atlas, GodType::hades}, GodType::hades},
    {{GodType::atlas, GodType::hera}, GodType::hera},
    {{GodType::atlas, GodType::atlas}, GodType::atlas},
};

GodType God::sFightWinner(const GodType g1, const GodType g2) {
    return gFightWinner[{g1, g2}];
}

std::string God::sFightResultString(const GodType g1, const GodType g2) {
    const auto w = sFightWinner(g1, g2);
    const auto l = g1 == w ? g2 : g1;
    const int wi = sGodTextId(w);
    const int li = sGodTextId(l);
    const int group = 332;
    const int string = 14*wi + li;
    return eLanguage::zeusText(group, string);
}

int sGodAttackSpriteLength(const GodType gt) {
    switch(gt) {
    case GodType::aphrodite: return eGodTextures::sAphroditeFBTime;
    case GodType::apollo: return eGodTextures::sApolloFTime;
    case GodType::ares: return eGodTextures::sAresFBTime;
    case GodType::artemis: return eGodTextures::sArtemisFTime;
    case GodType::athena: return eGodTextures::sAthenaFTime;
    case GodType::atlas: return eGodTextures::sAtlasFTime;
    case GodType::demeter: return eGodTextures::sDemeterFBTime;
    case GodType::dionysus: return eGodTextures::sDionysusFBTime;
    case GodType::hades: return eGodTextures::sHadesFBTime;
    case GodType::hephaestus: return eGodTextures::sHephaestusFBTime;
    case GodType::hera: return eGodTextures::sHeraFBTime;
    case GodType::hermes: return eGodTextures::sHermesFBTime;
    case GodType::poseidon: return eGodTextures::sPoseidonFBTime;
    case GodType::zeus: return eGodTextures::sZeusFTime;
    }
    return 0;
}

int God::sGodAttackTime(const GodType gt) {
    const int sl = sGodAttackSpriteLength(gt);
    return sl*eCharacter::sTextureTimeDivisor;
}

int sGodBlessSpriteLength(const GodType gt) {
    switch(gt) {
    case GodType::aphrodite: return eGodTextures::sAphroditeFBTime;
    case GodType::apollo: return eGodTextures::sApolloBTime;
    case GodType::ares: return eGodTextures::sAresFBTime;
    case GodType::artemis: return eGodTextures::sArtemisBTime;
    case GodType::athena: return eGodTextures::sAthenaBTime;
    case GodType::atlas: return eGodTextures::sAtlasBTime;
    case GodType::demeter: return eGodTextures::sDemeterFBTime;
    case GodType::dionysus: return eGodTextures::sDionysusFBTime;
    case GodType::hades: return eGodTextures::sHadesFBTime;
    case GodType::hephaestus: return eGodTextures::sHephaestusFBTime;
    case GodType::hera: return eGodTextures::sHeraFBTime;
    case GodType::hermes: return eGodTextures::sHermesFBTime;
    case GodType::poseidon: return eGodTextures::sPoseidonFBTime;
    case GodType::zeus: return eGodTextures::sZeusBTime;
    }
    return 0;
}

int God::sGodBlessTime(const GodType gt) {
    const int sl = sGodBlessSpriteLength(gt);
    return sl*eCharacter::sTextureTimeDivisor;
}

int sGodAppearSpriteLength(const GodType gt) {
    switch(gt) {
    case GodType::aphrodite: return eGodTextures::sAphroditeDTime;
    case GodType::apollo: return eGodTextures::sApolloDTime;
    case GodType::ares: return eGodTextures::sAresDTime;
    case GodType::artemis: return eGodTextures::sArtemisDTime;
    case GodType::athena: return eGodTextures::sAthenaDTime;
    case GodType::atlas: return eGodTextures::sAtlasDTime;
    case GodType::demeter: return eGodTextures::sDemeterDTime;
    case GodType::dionysus: return eGodTextures::sDionysusATime;
    case GodType::hades: return eGodTextures::sHadesDTime;
    case GodType::hephaestus: return eGodTextures::sHephaestusDTime;
    case GodType::hera: return eGodTextures::sHeraDTime;
    case GodType::hermes: return 0;
    case GodType::poseidon: return eGodTextures::sPoseidonDTime;
    case GodType::zeus: return eGodTextures::sZeusDTime;
    }
    return 0;
}

int God::sGodAppearTime(const GodType gt) {
    const int sl = sGodAppearSpriteLength(gt);
    return sl*eCharacter::sTextureTimeDivisor;
}

bool God::sTarget(const GodType gt, const eBuildingType bt) {
    switch(gt) {
    case GodType::aphrodite:
        return bt == eBuildingType::commonHouse ||
               bt == eBuildingType::fountain ||
               bt == eBuildingType::eliteHousing;
    case GodType::apollo:
        return bt == eBuildingType::podium ||
               bt == eBuildingType::college ||
               bt == eBuildingType::dramaSchool ||
               bt == eBuildingType::theater ||
               bt == eBuildingType::stadium ||
               bt == eBuildingType::gymnasium ||
               bt == eBuildingType::bibliotheke ||
               bt == eBuildingType::inventorsWorkshop ||
               bt == eBuildingType::laboratory ||
               bt == eBuildingType::university ||
               bt == eBuildingType::observatory ||
               bt == eBuildingType::museum;
    case GodType::ares:
        return false;
    case GodType::artemis:
        return bt == eBuildingType::huntingLodge ||
               bt == eBuildingType::granary;
    case GodType::athena:
        return bt == eBuildingType::growersLodge ||
               bt == eBuildingType::oliveTree ||
               bt == eBuildingType::olivePress ||
               bt == eBuildingType::oilVendor;
    case GodType::atlas:
        return bt == eBuildingType::masonryShop ||
               bt == eBuildingType::artisansGuild ||
               bt == eBuildingType::timberMill ||
               bt == eBuildingType::sculptureStudio;
    case GodType::demeter:
        return bt == eBuildingType::wheatFarm ||
               bt == eBuildingType::onionsFarm ||
               bt == eBuildingType::carrotsFarm ||
               bt == eBuildingType::cardingShed ||
               bt == eBuildingType::dairy ||
               bt == eBuildingType::corral ||
               bt == eBuildingType::growersLodge ||
               bt == eBuildingType::orangeTendersLodge ||
               bt == eBuildingType::foodVendor ||
               bt == eBuildingType::granary;
    case GodType::dionysus:
        return bt == eBuildingType::growersLodge ||
               bt == eBuildingType::vine ||
               bt == eBuildingType::winery ||
               bt == eBuildingType::wineVendor;
    case GodType::hades:
        return bt == eBuildingType::foundry ||
               bt == eBuildingType::refinery ||
               bt == eBuildingType::mint;
    case GodType::hephaestus:
        return bt == eBuildingType::foundry ||
               bt == eBuildingType::refinery ||
               bt == eBuildingType::blackMarbleWorkshop ||
               bt == eBuildingType::mint ||
               bt == eBuildingType::armory ||
               bt == eBuildingType::sculptureStudio;
    case GodType::hera:
        return bt == eBuildingType::foodVendor ||
               bt == eBuildingType::fleeceVendor ||
               bt == eBuildingType::oilVendor ||
               bt == eBuildingType::wineVendor ||
               bt == eBuildingType::armsVendor ||
               bt == eBuildingType::horseTrainer ||
               bt == eBuildingType::chariotFactory ||
               bt == eBuildingType::orangeTendersLodge ||
               bt == eBuildingType::orangeTree;
    case GodType::hermes:
        return bt == eBuildingType::tradePost ||
               bt == eBuildingType::pier;
    case GodType::poseidon:
        return bt == eBuildingType::urchinQuay ||
               bt == eBuildingType::fishery ||
               bt == eBuildingType::pier ||
               bt == eBuildingType::triremeWharf;
    case GodType::zeus:
        return true;
    }
    return false;
}

std::string God::sGodName(const GodType gt) {
    const int group = 157;
    const int string = sGodTextId(gt);
    return eLanguage::zeusText(group, string);
}

void God::sGodStrings(std::vector<GodType>& gods,
                       std::vector<std::string>& godNames) {
    gods.push_back(GodType::aphrodite);
    gods.push_back(GodType::apollo);
    gods.push_back(GodType::ares);
    gods.push_back(GodType::artemis);
    gods.push_back(GodType::athena);
    gods.push_back(GodType::atlas);
    gods.push_back(GodType::demeter);
    gods.push_back(GodType::dionysus);
    gods.push_back(GodType::hades);
    gods.push_back(GodType::hephaestus);
    gods.push_back(GodType::hera);
    gods.push_back(GodType::hermes);
    gods.push_back(GodType::poseidon);
    gods.push_back(GodType::zeus);
    for(const auto g : gods) {
        godNames.push_back(sGodName(g));
    }
}

void God::sLoadTextures(const GodType g) {
    switch(g) {
    case GodType::aphrodite:
        eGameTextures::loadAphrodite();
        break;
    case GodType::apollo:
        eGameTextures::loadApollo();
        break;
    case GodType::ares:
        eGameTextures::loadAres();
        break;
    case GodType::artemis:
        eGameTextures::loadArtemis();
        break;
    case GodType::athena:
        eGameTextures::loadAthena();
        break;
    case GodType::atlas:
        eGameTextures::loadAtlas();
        break;
    case GodType::demeter:
        eGameTextures::loadDemeter();
        break;
    case GodType::dionysus:
        eGameTextures::loadDionysus();
        break;
    case GodType::hades:
        eGameTextures::loadHades();
        break;
    case GodType::hephaestus:
        eGameTextures::loadHephaestus();
        break;
    case GodType::hera:
        eGameTextures::loadHera();
        break;
    case GodType::hermes:
        eGameTextures::loadHermes();
        break;
    case GodType::poseidon:
        eGameTextures::loadPoseidon();
        break;
    case GodType::zeus:
        eGameTextures::loadZeus();
        break;
    }
}

void God::serializeFields(eSaveArchive& ar) {
    eCharacter::serializeFields(ar);
    ar.field("mAttitude", mAttitude);
}
