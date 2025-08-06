#include "epyramidwall.h"

#include "engine/egameboard.h"

ePyramidWall::ePyramidWall(eGameBoard& board,
                           const eOrientation o,
                           const int elevation,
                           const int special,
                           const eCityId cid) :
    ePyramidElement({{0, 2, 0, 1}}, board, eBuildingType::pyramidWall,
                    1, 1, elevation, cid),
    mO(o), mSpecial(special) {}

stdsptr<eTexture> ePyramidWall::getTexture(const eTileSize size) const {
    if(!finished()) return nullptr;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings()[sizeId];
    auto& board = getBoard();
    const auto dir = board.direction();
    const auto o = sRotated(mO, dir);
    const int e = elevation();
    const bool isDark = e % 2;
    int texId = 1;

    switch(o) {
    case eOrientation::top:
        texId = 1;
        break;
    case eOrientation::topRight:
        texId = 2;
        break;
    case eOrientation::right:
        texId = 3;
        break;
    case eOrientation::bottomRight: {
        if(mSpecial == 0) {
            texId = 4;
        } else if(mSpecial == 1) {
            texId = 12;
        } else if(mSpecial == 2) {
            texId = 13;
        } else {
            texId = 10;
        }
    } break;
    case eOrientation::bottom:
        texId = 5;
        break;
    case eOrientation::bottomLeft: {
        if(mSpecial == 0) {
            texId = 6;
        } else if(mSpecial == 1) {
            texId = 15;
        } else if(mSpecial == 2) {
            texId = 14;
        } else {
            texId = 17;
        }
    } break;
    case eOrientation::left:
        texId = 7;
        break;
    case eOrientation::topLeft:
        texId = 8;
        break;
    }

    if(isDark) texId += 17;

    const eTextureCollection* coll = nullptr;
    if(e == 0) {
        if(texId > 9) texId--;
        if(texId > 26) texId--;
        coll = &blds.fPyramid2;
    } else {
        coll = &blds.fPyramid;
    }
    return coll->getTexture(texId - 1);
}
