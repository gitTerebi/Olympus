#include "epyramidstatue.h"

#include "textures/egametextures.h"
#include "epyramid.h"

#include "characters/gods/god.h"
#include "engine/game-board.h"

ePyramidStatue::ePyramidStatue(const std::vector<eSanctCost>& cost,
                               GameBoard& board,
                               const int elevation,
                               const GodType type,
                               const int id,
                               const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidStatue,
                    elevation, 1, cid),
    mType(type), mId(id) {
    eGameTextures::loadSanctuary();
}

ePyramidStatue::ePyramidStatue(ePyramid* const pyramid,
                               GameBoard& board,
                               const int elevation,
                               const GodType type,
                               const int id,
                               const eCityId cid) :
    ePyramidElement(pyramid,
                    {{0, 0, 1}},
                    board, eBuildingType::pyramidStatue,
                    elevation, 1, cid),
    mType(type), mId(id) {
    eGameTextures::loadSanctuary();
}

stdsptr<eTexture> ePyramidStatue::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    const eTextureCollection* coll = nullptr;
    switch(mType) {
    case GodType::aphrodite:
        coll = &blds.fAphroditeStatues;
        break;
    case GodType::apollo:
        coll = &blds.fApolloStatues;
        break;
    case GodType::ares:
        coll = &blds.fAresStatues;
        break;
    case GodType::artemis:
        coll = &blds.fArtemisStatues;
        break;
    case GodType::athena:
        coll = &blds.fAthenaStatues;
        break;
    case GodType::atlas:
        coll = &blds.fAtlasStatues;
        break;
    case GodType::demeter:
        coll = &blds.fDemeterStatues;
        break;
    case GodType::dionysus:
        coll = &blds.fDionysusStatues;
        break;
    case GodType::hades:
        coll = &blds.fHadesStatues;
        break;
    case GodType::hephaestus:
        coll = &blds.fHephaestusStatues;
        break;
    case GodType::hera:
        coll = &blds.fHeraStatues;
        break;
    case GodType::hermes:
        coll = &blds.fHermesStatues;
        break;
    case GodType::poseidon:
        coll = &blds.fPoseidonStatues;
        break;
    case GodType::zeus:
        coll = &blds.fZeusStatues;
        break;
    }
    auto& board = getBoard();
    const auto dir = board.direction();
    int dirId;
    if(dir == eWorldDirection::N) {
        dirId = mId;
    } else if(dir == eWorldDirection::E) {
        if(mId == 0) {
            dirId = 3;
        } else if(mId == 1) {
            dirId = 0;
        } else if(mId == 2) {
            dirId = 1;
        } else { // if(mId == 3) {
            dirId = 2;
        }
    } else if(dir == eWorldDirection::S) {
        if(mId == 0) {
            dirId = 1;
        } else if(mId == 1) {
            dirId = 3;
        } else if(mId == 2) {
            dirId = 0;
        } else { // if(mId == 3) {
            dirId = 2;
        }
    } else { // if(dir == eWorldDirection::W) {
        if(mId == 0) {
            dirId = 1;
        } else if(mId == 1) {
            dirId = 2;
        } else if(mId == 2) {
            dirId = 3;
        } else { // if(mId == 3) {
            dirId = 0;
        }
    }
    if(!coll) return nullptr;
    return coll->getTexture(dirId);
}
