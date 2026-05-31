#include "epyramidbuildingpart.h"

#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

ePyramidBuildingPart::ePyramidBuildingPart(
        const std::vector<eSanctCost>& cost,
        GameBoard& board, const int elevation,
        const eCityId cid) :
    ePyramidElement(cost, board,
                    eBuildingType::pyramidPart,
                    elevation, 1, cid) {}

ePyramidBuildingPart::ePyramidBuildingPart(
        ePyramid* const pyramid,
        GameBoard& board, const int elevation,
        const eCityId cid) :
    ePyramidElement(pyramid, {}, board,
                    eBuildingType::pyramidPart,
                    elevation, 1, cid) {}

eTextureSpace ePyramidBuildingPart::getTextureSpace(
        const int tx, const int ty,
        const eTileSize size) const {
    if(mPaint && mPaint->renderBuilding()) {
        if(mPaintDir == eWorldDirection::N) return {nullptr};
        auto& board = getBoard();
        const auto dir = board.direction();
        if(dir == mPaintDir) {
            const auto tile = mPaint->centerTile();
            const int tx = tile->x();
            const int ty = tile->y();
            auto tex = mPaint->getBuildingTextureSpace(
                                 tx, ty, size);
            {
                const auto tile = centerTile();
                tex.fRect.x = tile->x();
                tex.fRect.y = tile->y();
            }
            return tex;
        }
        return {nullptr};
    }
    return ePyramidElement::getTextureSpace(tx, ty, size);
}

void ePyramidBuildingPart::setPaint(ePyramidElement* const paint,
                                    const eWorldDirection dir){
    mPaint = paint;
    mPaintDir = dir;
}

void ePyramidBuildingPart::serializeFields(eSaveArchive& ar) {
    ePyramidElement::serializeFields(ar);
    ar.buildingAsField("paint", &getBoard(), mPaint);
    ar.field("mPaintDir", mPaintDir);
}
