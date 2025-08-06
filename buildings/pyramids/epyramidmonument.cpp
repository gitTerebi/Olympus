#include "epyramidmonument.h"

#include "textures/egametextures.h"
#include "epyramid.h"

#include "characters/gods/egod.h"
#include "engine/egameboard.h"

#include "../sanctuaries/etemplemonumentbuilding.h"

ePyramidMonument::ePyramidMonument(
        const std::vector<eSanctCost>& cost,
        eGameBoard& board,
        const int elevation,
        const eGodType type,
        const int id,
        const eCityId cid) :
    ePyramidElement(cost, board, eBuildingType::pyramidMonument,
                    2, 2, elevation, cid),
    mType(type), mId(id) {
    eGameTextures::loadSanctuary();
}

ePyramidMonument::ePyramidMonument(
        ePyramid* const pyramid,
        eGameBoard& board,
        const int elevation,
        const eGodType type,
        const int id,
        const eCityId cid) :
    ePyramidElement(pyramid,
                    {{0, 2, 0}, {0, 3, 0}},
                    board, eBuildingType::pyramidMonument,
                    2, 2, elevation, cid),
    mType(type), mId(id) {
    eGameTextures::loadSanctuary();
}

std::shared_ptr<eTexture>
ePyramidMonument::getTexture(const eTileSize size) const {
    const int p = 2 - maxProgress() + progress();
    if(p <= 0) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    if(p == 1) {
        return blds.fBlankMonument;
    }
    const auto coll = eTempleMonumentBuilding::sGodMonumentTextureCollection(size, mType);

    if(!coll) return nullptr;
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
    return coll->getTexture(dirId);
}
