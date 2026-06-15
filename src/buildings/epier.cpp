#include "epier.h"

#include "textures/game-textures.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"

// index: 0=bottomRight 1=bottomLeft 2=topLeft 3=topRight
double ePier::sOvX[4] = {0.30, -0.20, -0.40, 0.00};
double ePier::sOvY[4] = {-1.90, -1.60, -1.80, -2.40};

double ePier::sLoadOvX[4] = {0.20, -0.10, -0.60, 0.20};
double ePier::sLoadOvY[4] = {-1.90, -2.10, -2.40, -2.10};

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
    GameTextures::loadPier();
    setOverlayEnabledFunc([]()
                          { return true; });
}

std::shared_ptr<eTexture> ePier::getTexture(const eTileSize size) const
{
    const int sizeId = static_cast<int>(size);
    const auto &blds = GameTextures::buildings();
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
    const auto &blds = GameTextures::buildings();
    const auto &coll = mLoading ? blds[sizeId].fPierLoadOverlay
                                : blds[sizeId].fPierOverlay;
    const int per = mLoading ? 34 : 32;
    if (coll.size() < 4 * per)
        return os;
    auto &board = getBoard();
    const auto dir = board.direction();
    const auto o = sRotated(mO, dir);
    const int t = textureTime();
    int dirIdx;
    switch(o) {
    case eDiagonalOrientation::bottomRight: dirIdx = 0; break;
    case eDiagonalOrientation::bottomLeft:  dirIdx = 1; break;
    case eDiagonalOrientation::topLeft:     dirIdx = 2; break;
    case eDiagonalOrientation::topRight:    dirIdx = 3; break;
    default:                                dirIdx = 0; break;
    }
    const int texId = dirIdx * per + (t % per);
    auto &ov = os.emplace_back();
    ov.fTex = coll.getTexture(texId);
    const int oi = sOrientIndex(o);
    ov.fX = mLoading ? sLoadOvX[oi] : sOvX[oi];
    ov.fY = mLoading ? sLoadOvY[oi] : sOvY[oi];
    // dockworker must draw over the docked boat (boat is a "big" char drawn after overlays)
    ov.fOnTop = mLoading;
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
