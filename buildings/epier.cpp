#include "epier.h"

#include "textures/egametextures.h"
#include "engine/e-game-board.h"
#include "fileIO/esavearchive.h"

// index: 0=bottomRight 1=bottomLeft 2=topLeft 3=topRight
double ePier::sOvX[4] = {0.30, -0.20, -0.40, 0.00};
double ePier::sOvY[4] = {-1.90, -1.60, -1.80, -2.40};

int ePier::sOrientIndex(const eDiagonalOrientation o) {
    switch(o) {
    case eDiagonalOrientation::bottomRight: return 0;
    case eDiagonalOrientation::bottomLeft:  return 1;
    case eDiagonalOrientation::topLeft:     return 2;
    case eDiagonalOrientation::topRight:    return 3;
    default:                                return 0;
    }
}

ePier::ePier(GameBoard &board, const eDiagonalOrientation o,
             const eCityId cid) : eBuilding(board, eBuildingType::pier, 2, 2, cid),
                                  mO(o)
{
    eGameTextures::loadPier();
    setOverlayEnabledFunc([]()
                          { return true; });
}

std::shared_ptr<eTexture> ePier::getTexture(const eTileSize size) const
{
    const int sizeId = static_cast<int>(size);
    const auto &blds = eGameTextures::buildings();
    const auto &coll = blds[sizeId].fPier1;
    auto &board = getBoard();
    const auto dir = board.direction();
    const auto o = sRotated(mO, dir);
    int id = 3;
    switch (o)
    {
    case eDiagonalOrientation::topRight:
        id = 0;
        break;
    case eDiagonalOrientation::bottomRight:
        id = 1;
        break;
    case eDiagonalOrientation::bottomLeft:
        id = 2;
        break;
    case eDiagonalOrientation::topLeft:
        id = 3;
        break;
    default:
        id = 0;
        break;
    }

    return coll.getTexture(id);
}

std::vector<eOverlay> ePier::getOverlays(const eTileSize size) const
{
    std::vector<eOverlay> os;
    const int sizeId = static_cast<int>(size);
    const auto &blds = eGameTextures::buildings();
    const auto &coll = blds[sizeId].fPierOverlay;
    if (coll.size() < 128)
        return os;
    auto &board = getBoard();
    const auto dir = board.direction();
    const auto o = sRotated(mO, dir);
    const int t = textureTime();
    int base;
    switch(o) {
    case eDiagonalOrientation::bottomRight: base = 0;  break;
    case eDiagonalOrientation::bottomLeft:  base = 32; break;
    case eDiagonalOrientation::topLeft:     base = 64; break;
    case eDiagonalOrientation::topRight:    base = 96; break;
    default:                                base = 0;  break;
    }
    const int texId = base + (t % 32);
    auto &ov = os.emplace_back();
    ov.fTex = coll.getTexture(texId);
    const int oi = sOrientIndex(o);
    ov.fX = sOvX[oi];
    ov.fY = sOvY[oi];
    return os;
}

void ePier::collapse()
{
    if (mTradePost)
    {
        mTradePost->collapse();
    }
    else
    {
        eBuilding::collapse();
    }
}

void ePier::erase()
{
    if (mTradePost)
    {
        mTradePost->eBuilding::erase();
    }
    eBuilding::erase();
}

void ePier::setTradePost(eBuilding *const b)
{
    mTradePost = b;
}

void ePier::serializeFields(eSaveArchive &ar)
{
    eBuilding::serializeFields(ar);
    ar.buildingAsField("tradePost", &getBoard(), mTradePost);
}
