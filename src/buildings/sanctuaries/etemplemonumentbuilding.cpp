#include "etemplemonumentbuilding.h"
#include "textures/game-textures.h"

eTempleMonumentBuilding::eTempleMonumentBuilding(
        const GodType god,
        const int id, GameBoard& board,
        const eCityId cid) :
    eSanctBuilding({{0, 2, 0}, {0, 3, 0}}, board,
                   eBuildingType::templeMonument,
                   2, 2, cid),
    mGod(god), mId(id) {

}

const TextureCollection* eTempleMonumentBuilding::sGodMonumentTextureCollection(
        const eTileSize size, const GodType god) {
    const int sizeId = static_cast<int>(size);
    const auto& blds = GameTextures::buildings()[sizeId];
    switch(god) {
    case GodType::aphrodite:
        return &blds.fAphroditeMonuments;
    case GodType::apollo:
        return &blds.fApolloMonuments;
    case GodType::ares:
        return &blds.fAresMonuments;
    case GodType::artemis:
        return &blds.fArtemisMonuments;
    case GodType::athena:
        return &blds.fAthenaMonuments;
    case GodType::atlas:
        return &blds.fAtlasMonuments;
    case GodType::demeter:
        return &blds.fDemeterMonuments;
    case GodType::dionysus:
        return &blds.fDionysusMonuments;
    case GodType::hades:
        return &blds.fHadesMonuments;
    case GodType::hephaestus:
        return &blds.fHephaestusMonuments;
    case GodType::hera:
        return &blds.fHeraMonuments;
    case GodType::hermes:
        return &blds.fHermesMonuments;
    case GodType::poseidon:
        return &blds.fPoseidonMonuments;
    case GodType::zeus:
        return &blds.fZeusMonuments;
    }
    return nullptr;
}

