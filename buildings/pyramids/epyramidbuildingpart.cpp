#include "epyramidbuildingpart.h"

#include "engine/egameboard.h"

ePyramidBuildingPart::ePyramidBuildingPart(
        const std::vector<eSanctCost>& cost,
        eGameBoard& board, const int elevation,
        const eCityId cid) :
    ePyramidElement(cost, board,
                    eBuildingType::pyramidPart,
                    elevation, 1, cid) {}

ePyramidBuildingPart::ePyramidBuildingPart(
        ePyramid* const pyramid,
        eGameBoard& board, const int elevation,
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

void ePyramidBuildingPart::read(eReadStream& src) {
    ePyramidElement::read(src);
    auto& board = getBoard();
    src.readBuilding(&board, [this](eBuilding* const b) {
        mPaint = static_cast<ePyramidElement*>(b);
    });
    src >> mPaintDir;
}

void ePyramidBuildingPart::write(eWriteStream& dst) const {
    ePyramidElement::write(dst);
    dst.writeBuilding(mPaint);
    dst << mPaintDir;
}
