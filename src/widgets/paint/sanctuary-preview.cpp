#include "widgets/paint/sanctuary-preview.h"

#include "buildings/allbuildings.h"
#include "buildings/sanctuaries/esanctuaryblueprint.h"
#include "textures/egametextures.h"
#include "engine/etile.h"
#include "engine/game-board.h"
#include "etilehelper.h"
#include "textures/ebuildingtextures.h"
#include "textures/eterraintextures.h"
#include "widgets/ebuildingmode.h"
#include "widgets/etilepainter.h"

#include <algorithm>

namespace {

static const double gAltarOffsetX[4] = {0, 2, 0, 2};
static const double gAltarOffsetY[4] = {0, -2, 0, -2};

static const double gWomanOffsetX[4] = {1.30, -0.20, 1.80, 1.80};
static const double gWomanOffsetY[4] = {-4.30, -2.70, -0.80, -0.80};

struct PreviewTile
{
    int fX;
    int fY;
    int fAltitude;
    eSanctEleType fType;
    int fId;
};

struct SanctuaryPartTile
{
    eSanctEle fEle;
    int fPreviewX;
    int fPreviewY;
};

bool isStatueOrMonument(const eSanctEleType type)
{
    return type == eSanctEleType::defaultStatue ||
           type == eSanctEleType::aphroditeStatue ||
           type == eSanctEleType::apolloStatue ||
           type == eSanctEleType::aresStatue ||
           type == eSanctEleType::artemisStatue ||
           type == eSanctEleType::athenaStatue ||
           type == eSanctEleType::atlasStatue ||
           type == eSanctEleType::demeterStatue ||
           type == eSanctEleType::dionysusStatue ||
           type == eSanctEleType::hadesStatue ||
           type == eSanctEleType::hephaestusStatue ||
           type == eSanctEleType::heraStatue ||
           type == eSanctEleType::hermesStatue ||
           type == eSanctEleType::poseidonStatue ||
           type == eSanctEleType::zeusStatue ||
           type == eSanctEleType::monument;
}

int stairsId180(const int id)
{
    if(id == 0) return 4;
    if(id == 1) return 5;
    if(id == 2) return 6;
    if(id == 3) return 7;
    if(id == 4) return 0;
    if(id == 5) return 1;
    if(id == 6) return 2;
    if(id == 7) return 3;
    if(id == 8) return 10;
    if(id == 9) return 11;
    if(id == 10) return 8;
    if(id == 11) return 9;
    return id;
}

int statueId180(const int id)
{
    if(id == 0) return 1;
    if(id == 1) return 3;
    if(id == 2) return 0;
    if(id == 3) return 2;
    return id;
}

int templeId180(const int id)
{
    if(id == 0) return 2;
    if(id == 1) return 3;
    if(id == 2) return 0;
    if(id == 3) return 1;
    return id;
}

eSanctEle rotateId180(eSanctEle ele)
{
    if(ele.fType == eSanctEleType::stairs) {
        ele.fId = stairsId180(ele.fId);
    } else if(isStatueOrMonument(ele.fType)) {
        ele.fId = statueId180(ele.fId);
    } else if(ele.fType == eSanctEleType::sanctuary) {
        ele.fId = templeId180(ele.fId);
    }
    return ele;
}

std::vector<SanctuaryPartTile> sanctuaryPreviewTiles(
    const eSanctBlueprint& base,
    const int rotateId)
{
    std::vector<SanctuaryPartTile> result;
    result.reserve(base.fW * base.fH);
    const bool flipped = rotateId >= 2;
    for(const auto& column : base.fTiles) {
        for(auto ele : column) {
            int previewX = ele.fX;
            int previewY = ele.fY;
            if(flipped) {
                ele = rotateId180(ele);
                ele.fX = base.fW - 1 - ele.fX;
                ele.fY = base.fH - 1 - ele.fY;
                previewX = ele.fX;
                previewY = ele.fY;
            }
            if(ele.fType == eSanctEleType::monument ||
               ele.fType == eSanctEleType::altar) {
                if(rotateId == 1 || rotateId == 2) {
                    previewX -= 1;
                    previewY += 1;
                }
            } else if(ele.fType == eSanctEleType::sanctuary) {
                int dx = rotateId == 1 || rotateId == 3 ? -2 : 1;
                int dy = rotateId == 1 || rotateId == 3 ? 2 : -1;
                if(flipped) {
                    dx = -dx - 3;
                    dy = -dy - 3;
                    dx += 2;
                    dy += 4;
                }
                previewX += dx;
                previewY += dy;
                previewX += 0;
                previewY += 0;
            } else if(isStatueOrMonument(ele.fType)) {
                if(ele.fType != eSanctEleType::monument &&
                   (rotateId == 1 || rotateId == 3)) {
                    previewX = base.fW - 1 - previewX;
                }
            }
            result.push_back({ele, previewX, previewY});
        }
    }
    return result;
}

void finishSanctuaryPart(eSanctBuilding* const part)
{
    while(!part->finished()) {
        part->incProgress();
    }
}

eGodType statueGod(const eSanctEleType type, const eGodType fallback)
{
    switch(type) {
    case eSanctEleType::aphroditeStatue: return eGodType::aphrodite;
    case eSanctEleType::apolloStatue: return eGodType::apollo;
    case eSanctEleType::aresStatue: return eGodType::ares;
    case eSanctEleType::artemisStatue: return eGodType::artemis;
    case eSanctEleType::athenaStatue: return eGodType::athena;
    case eSanctEleType::atlasStatue: return eGodType::atlas;
    case eSanctEleType::demeterStatue: return eGodType::demeter;
    case eSanctEleType::dionysusStatue: return eGodType::dionysus;
    case eSanctEleType::hadesStatue: return eGodType::hades;
    case eSanctEleType::hephaestusStatue: return eGodType::hephaestus;
    case eSanctEleType::heraStatue: return eGodType::hera;
    case eSanctEleType::hermesStatue: return eGodType::hermes;
    case eSanctEleType::poseidonStatue: return eGodType::poseidon;
    case eSanctEleType::zeusStatue: return eGodType::zeus;
    default: return fallback;
    }
}

int templeOverlayDirId(const int templeId,
                       const bool rotated,
                       const eWorldDirection dir)
{
    if(dir == eWorldDirection::N) {
        return templeId;
    } else if(dir == eWorldDirection::E) {
        if(templeId == 0) return 1;
        if(templeId == 1) return 0;
        if(templeId == 2) return rotated ? 3 : 1;
        return rotated ? 2 : 0;
    } else if(dir == eWorldDirection::S) {
        if(templeId == 0) return 0;
        if(templeId == 1) return 1;
        if(templeId == 2) return 0;
        return 1;
    } else {
        if(templeId == 0) return 1;
        if(templeId == 1) return 0;
        if(templeId == 2) return rotated ? 1 : 3;
        return rotated ? 0 : 2;
    }
}

int templeTextureDirId(const int rotateId)
{
    if(rotateId == 0) return 1;
    if(rotateId == 1) return 0;
    if(rotateId == 2) return 3;
    return 0;
}

int templeTextureOverlayDirId(const int rotateId)
{
    if(rotateId == 0) return -1;
    if(rotateId == 1) return -1;
    if(rotateId == 2) return 1;
    return -1;
}

int statueTextureIdForRotate(const int rotateId)
{
    if(rotateId == 0) return 1;
    if(rotateId == 1) return 2;
    if(rotateId == 2) return 3;
    return 0;
}

int statueInputIdForFinalId(const int finalId, const eWorldDirection dir)
{
    if(dir == eWorldDirection::N) {
        return finalId;
    } else if(dir == eWorldDirection::E) {
        if(finalId == 0) return 1;
        if(finalId == 1) return 2;
        if(finalId == 2) return 3;
        return 0;
    } else if(dir == eWorldDirection::S) {
        if(finalId == 0) return 2;
        if(finalId == 1) return 0;
        if(finalId == 2) return 3;
        return 1;
    } else {
        if(finalId == 0) return 3;
        if(finalId == 1) return 0;
        if(finalId == 2) return 1;
        return 2;
    }
}

void applyPreviewSpanCameraOffset(double& drawX,
                                  double& drawY,
                                  const int sw,
                                  const int sh,
                                  const eWorldDirection dir)
{
    if (dir == eWorldDirection::E) {
        if ((sw == 4 && sh == 4) || (sw == 2 && sh == 2)) drawX -= 1;
        else if (sw == 6 && sh == 6) drawY -= 1;
    } else if (dir == eWorldDirection::S) {
        if ((sw == 4 && sh == 4) || (sw == 2 && sh == 2)) {
            drawX -= 1;
            drawY += 1;
        } else if (sw == 6 && sh == 6) {
            drawX -= 1;
            drawY -= 1;
        }
    } else if (dir == eWorldDirection::W) {
        if ((sw == 4 && sh == 4) || (sw == 2 && sh == 2)) drawY += 1;
        else if (sw == 6 && sh == 6) drawX -= 1;
    }
}

void sortByDrawOrder(std::vector<PreviewTile>& tiles,
                     const eWorldDirection dir,
                     const int boardw,
                     const int boardh)
{
    std::stable_sort(tiles.begin(), tiles.end(),
                     [&](const PreviewTile& lhs, const PreviewTile& rhs)
                     {
                         int ax;
                         int ay;
                         int bx;
                         int by;
                         eTileHelper::dTileIdToRotatedDTileId(
                             lhs.fX, lhs.fY, ax, ay,
                             dir, boardw, boardh);
                         eTileHelper::dTileIdToRotatedDTileId(
                             rhs.fX, rhs.fY, bx, by,
                             dir, boardw, boardh);
                         if (ay != by)
                             return ay < by;
                         return ax < bx;
                     });
}

void previewDrawXY(GameBoard& board,
                   int tx,
                   int ty,
                   double& rx,
                   double& ry,
                   const int wSpan,
                   const int hSpan,
                   const int altitude)
{
    const auto dir = board.direction();
    if (dir != eWorldDirection::N)
    {
        eTileHelper::tileIdToRotatedTileId(tx, ty, tx, ty,
                                           dir, board.width(), board.height());
    }

    rx = tx + 0.5;
    ry = ty + 1.5;

    if (wSpan == 2 && hSpan == 2)       { rx += 0.5; ry += 0.5; }
    else if (wSpan == 3 && hSpan == 3)  { rx += 0.0; ry += 2.0; }
    else if (wSpan == 4 && hSpan == 4)  { rx += 0.5; ry += 2.5; }
    else if (wSpan == 5 && hSpan == 5)  { rx += 0.0; ry += 4.0; }
    else if (wSpan == 6 && hSpan == 6)  { rx += 0.5; ry += 5.5; }
    rx -= altitude;
    ry -= altitude;
}

int sanctuaryStairTextureId(const int seed,
                            const eTextureCollection& coll,
                            const eWorldDirection dir)
{
    int texId = seed % coll.size();
    if(dir == eWorldDirection::N) {
    } else if(dir == eWorldDirection::E) {
        if(texId == 0) texId = 6;
        else if(texId == 1) texId = 7;
        else if(texId == 2) texId = 0;
        else if(texId == 3) texId = 1;
        else if(texId == 4) texId = 2;
        else if(texId == 5) texId = 3;
        else if(texId == 6) texId = 4;
        else if(texId == 7) texId = 5;
        else if(texId == 8) texId = 11;
        else if(texId == 9) texId = 8;
        else if(texId == 10) texId = 9;
        else if(texId == 11) texId = 10;
    } else if(dir == eWorldDirection::S) {
        if(texId == 0) texId = 4;
        else if(texId == 1) texId = 5;
        else if(texId == 2) texId = 6;
        else if(texId == 3) texId = 7;
        else if(texId == 4) texId = 0;
        else if(texId == 5) texId = 1;
        else if(texId == 6) texId = 2;
        else if(texId == 7) texId = 3;
        else if(texId == 8) texId = 10;
        else if(texId == 9) texId = 11;
        else if(texId == 10) texId = 8;
        else if(texId == 11) texId = 9;
    } else {
        if(texId == 0) texId = 2;
        else if(texId == 1) texId = 3;
        else if(texId == 2) texId = 4;
        else if(texId == 3) texId = 5;
        else if(texId == 4) texId = 6;
        else if(texId == 5) texId = 7;
        else if(texId == 6) texId = 0;
        else if(texId == 7) texId = 1;
        else if(texId == 8) texId = 9;
        else if(texId == 9) texId = 10;
        else if(texId == 10) texId = 11;
        else if(texId == 11) texId = 8;
    }
    return texId;
}

bool isTempleFloorTile(const eSanctEleType type)
{
    return type == eSanctEleType::tile ||
           type == eSanctEleType::copper ||
           type == eSanctEleType::silver ||
           type == eSanctEleType::oliveTree ||
           type == eSanctEleType::vine ||
           type == eSanctEleType::orangeTree;
}

void modPreviewTexture(const std::shared_ptr<eTexture>&,
                       const bool)
{
}

std::shared_ptr<eTexture> sanctuaryTerrainTexture(
    const PreviewTile& tile,
    GameBoard& board,
    const eCityId viewedCityId,
    const eTileSize tileSize,
    const eBuildingTextures& builTexs,
    const eTerrainTextures& trrTexs,
    const eWorldDirection dir)
{
    if (isTempleFloorTile(tile.fType)) {
        (void)board;
        (void)viewedCityId;
        (void)tileSize;
        const auto& coll = builTexs.fSanctuaryTiles;
        if(tile.fId > 9) return coll.getTexture(tile.fId - 10);
        return coll.getTexture(tile.fId);
    }
    if (tile.fType == eSanctEleType::stairs) {
        const auto& coll = trrTexs.fSanctuaryStairs;
        return coll.getTexture(
            sanctuaryStairTextureId(tile.fId, coll, dir));
    }
    const auto& coll = builTexs.fSanctuarySpace;
    return coll.getTexture(tile.fId % coll.size());
}

const eTextureCollection* statueTextureCollection(
    const eBuildingTextures& builTexs,
    const eGodType god)
{
    switch(god) {
    case eGodType::aphrodite: return &builTexs.fAphroditeStatues;
    case eGodType::apollo: return &builTexs.fApolloStatues;
    case eGodType::ares: return &builTexs.fAresStatues;
    case eGodType::artemis: return &builTexs.fArtemisStatues;
    case eGodType::athena: return &builTexs.fAthenaStatues;
    case eGodType::atlas: return &builTexs.fAtlasStatues;
    case eGodType::demeter: return &builTexs.fDemeterStatues;
    case eGodType::dionysus: return &builTexs.fDionysusStatues;
    case eGodType::hades: return &builTexs.fHadesStatues;
    case eGodType::hephaestus: return &builTexs.fHephaestusStatues;
    case eGodType::hera: return &builTexs.fHeraStatues;
    case eGodType::hermes: return &builTexs.fHermesStatues;
    case eGodType::poseidon: return &builTexs.fPoseidonStatues;
    case eGodType::zeus: return &builTexs.fZeusStatues;
    }
    return nullptr;
}

const eTextureCollection* monumentTextureCollection(
    const eBuildingTextures& builTexs,
    const eGodType god)
{
    switch(god) {
    case eGodType::aphrodite: return &builTexs.fAphroditeMonuments;
    case eGodType::apollo: return &builTexs.fApolloMonuments;
    case eGodType::ares: return &builTexs.fAresMonuments;
    case eGodType::artemis: return &builTexs.fArtemisMonuments;
    case eGodType::athena: return &builTexs.fAthenaMonuments;
    case eGodType::atlas: return &builTexs.fAtlasMonuments;
    case eGodType::demeter: return &builTexs.fDemeterMonuments;
    case eGodType::dionysus: return &builTexs.fDionysusMonuments;
    case eGodType::hades: return &builTexs.fHadesMonuments;
    case eGodType::hephaestus: return &builTexs.fHephaestusMonuments;
    case eGodType::hera: return &builTexs.fHeraMonuments;
    case eGodType::hermes: return &builTexs.fHermesMonuments;
    case eGodType::poseidon: return &builTexs.fPoseidonMonuments;
    case eGodType::zeus: return &builTexs.fZeusMonuments;
    }
    return nullptr;
}

}

SanctuaryPreviewEntry::SanctuaryPreviewEntry(
    const int order,
    const int tx, const int ty, const int altitude,
    const int templeOverlayDirId,
    const eGodType statueGod,
    const int statueTextureId,
    const int monumentTextureId,
    const bool altar,
    const stdsptr<eBuilding>& b) :
    fOrder(order), fTx(tx), fTy(ty), fAltitude(altitude),
    fTempleOverlayDirId(templeOverlayDirId),
    fStatueGod(statueGod), fStatueTextureId(statueTextureId),
    fMonumentTextureId(monumentTextureId), fAltar(altar), fB(b)
{}

std::vector<SanctuaryPreviewEntry> createSanctuaryPreviewEntries(
    GameBoard& board,
    const eBuildingMode mode,
    const int rotateId,
    const int hoverTX,
    const int hoverTY,
    const eCityId viewedCityId,
    SDL_Rect& footprint)
{
    const auto type = eBuildingModeHelpers::toBuildingType(mode);
    const auto god = static_cast<eGodType>(
        static_cast<int>(mode) -
        static_cast<int>(eBuildingMode::templeAphrodite));
    const bool transposed = rotateId == 1 || rotateId == 3;
    const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, transposed);
    const int sw = h->fW;
    const int sh = h->fH;
    const int xMin = hoverTX - sw/2;
    const int yMin = hoverTY - sh/2;
    footprint = {xMin, yMin, sw, sh};

    const auto sanctuary = e::make_shared<eMonument>(
        board, type, sw, sh, 0, viewedCityId);
    sanctuary->setRotated(transposed);
    sanctuary->setTileRect(footprint);
    eGameTextures::loadZeusSanctuary();
    eSanctuary::sLoadMonumentTextures(god);
    eGameTextures::loadSanctuary();

    std::vector<SanctuaryPreviewEntry> result;
    const auto tiles = sanctuaryPreviewTiles(*h, rotateId);
    result.reserve(tiles.size());
    for(const auto& tile : tiles) {
        const auto& te = tile.fEle;
        const int tileOrder = tile.fPreviewX + tile.fPreviewY;
        const int tx = xMin + te.fX;
        const int ty = yMin + te.fY;
        const int previewX = xMin + tile.fPreviewX;
        const int previewY = yMin + tile.fPreviewY;
        const auto boardTile = board.tile(tx, ty);
        const auto previewBoardTile = board.tile(previewX, previewY);
        const int altitude = (boardTile ? boardTile->altitude() : 0) + te.fA;
        const int previewAltitude =
            (previewBoardTile ? previewBoardTile->altitude() : 0) + te.fA;
        switch(te.fType) {
        case eSanctEleType::tile:
        case eSanctEleType::copper:
        case eSanctEleType::silver:
        case eSanctEleType::oliveTree:
        case eSanctEleType::vine:
        case eSanctEleType::orangeTree:
        {
            const auto b = e::make_shared<eTempleTileBuilding>(
                te.fId, board, viewedCityId);
            b->setMonument(sanctuary.get());
            finishSanctuaryPart(b.get());
            sanctuary->registerElement(b);
            auto& floorEntry = result.emplace_back(tileOrder, tx, ty, altitude, -1,
                                                    god, -1, -1, false, b);
            if(isTempleFloorTile(te.fType) && te.fId >= 10) {
                auto& torchEntry = result.emplace_back(tileOrder, tx, ty, altitude, -1,
                                                       god, -1, -1, false, b);
                torchEntry.fTorch = true;
            }
        } break;
        case eSanctEleType::sanctuary:
        {
            const auto b = e::make_shared<eTempleBuilding>(
                te.fId, board, viewedCityId);
            b->setMonument(sanctuary.get());
            finishSanctuaryPart(b.get());
            sanctuary->registerElement(b);
            const int overlayDirId = templeTextureDirId(rotateId);
            const int textureOverlayId = templeTextureOverlayDirId(rotateId);
            const int textureKey = textureOverlayId < 0 ?
                                       overlayDirId :
                                       overlayDirId * 10 + textureOverlayId;
            result.emplace_back(tileOrder, previewX, previewY, altitude,
                                textureKey * 10 + rotateId,
                                god, -1, -1, false, b);
        } break;
        case eSanctEleType::monument:
        {
            const auto b = e::make_shared<eTempleMonumentBuilding>(
                god, te.fId, board, viewedCityId);
            b->setMonument(sanctuary.get());
            finishSanctuaryPart(b.get());
            sanctuary->registerElement(b);
            result.emplace_back(tileOrder, previewX, previewY, altitude, -1,
                                god, -1, te.fId, false, b);
        } break;
        case eSanctEleType::altar:
        {
            const auto b = e::make_shared<eTempleAltarBuilding>(
                board, viewedCityId);
            b->setMonument(sanctuary.get());
            finishSanctuaryPart(b.get());
            sanctuary->registerElement(b);
            result.emplace_back(tileOrder, previewX, previewY, altitude, -1,
                                god, -1, -1, true, b);
        } break;
        case eSanctEleType::defaultStatue:
        case eSanctEleType::aphroditeStatue:
        case eSanctEleType::apolloStatue:
        case eSanctEleType::aresStatue:
        case eSanctEleType::artemisStatue:
        case eSanctEleType::athenaStatue:
        case eSanctEleType::atlasStatue:
        case eSanctEleType::demeterStatue:
        case eSanctEleType::dionysusStatue:
        case eSanctEleType::hadesStatue:
        case eSanctEleType::hephaestusStatue:
        case eSanctEleType::heraStatue:
        case eSanctEleType::hermesStatue:
        case eSanctEleType::poseidonStatue:
        case eSanctEleType::zeusStatue:
        {
            const eGodType sg = statueGod(te.fType, god);
            const int statueTextureId = statueTextureIdForRotate(rotateId);
            const auto b = e::make_shared<eTempleStatueBuilding>(
                sg,
                statueTextureId,
                board, viewedCityId);
            b->setMonument(sanctuary.get());
            finishSanctuaryPart(b.get());
            sanctuary->registerElement(b);
            result.emplace_back(tileOrder, previewX, previewY, previewAltitude, -1,
                                sg, statueTextureId, -1, false, b);
        } break;
        default:
            break;
        }
    }
    return result;
}

void printSanctuaryPreviewTiles(
    const eBuildingMode mode,
    const int rotateId,
    const int hoverTX,
    const int hoverTY)
{
    const auto type = eBuildingModeHelpers::toBuildingType(mode);
    if(!eBuilding::sSanctuaryBuilding(type)) {
        return;
    }

    const bool transposed = rotateId == 1 || rotateId == 3;
    const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, transposed);
    const int xMin = hoverTX - h->fW/2;
    const int yMin = hoverTY - h->fH/2;
    printf("sanctuary preview rotate after-key mode=%d rotateId=%d templeTex=%05d hover=(%d,%d) origin=(%d,%d) size=%dx%d\n",
           static_cast<int>(mode), rotateId,
           templeTextureDirId(rotateId) + 1,
           hoverTX, hoverTY, xMin, yMin, h->fW, h->fH);

    const auto tiles = sanctuaryPreviewTiles(*h, rotateId);
    for(const auto& tile : tiles) {
        const auto& ele = tile.fEle;
        if(ele.fType == eSanctEleType::none) {
            continue;
        }
        printf("  type=%d id=%d rel=(%d,%d) tile=(%d,%d) previewRel=(%d,%d) preview=(%d,%d) alt=%d\n",
               static_cast<int>(ele.fType), ele.fId,
               ele.fX, ele.fY,
               xMin + ele.fX, yMin + ele.fY,
               tile.fPreviewX, tile.fPreviewY,
               xMin + tile.fPreviewX, yMin + tile.fPreviewY,
               ele.fA);
    }
}

void adjustSanctuaryTemplePreviewDebugOffset(
    const eBuildingMode mode,
    const int rotateId,
    const int dx,
    const int dy)
{
    (void)mode; (void)rotateId; (void)dx; (void)dy;
}

void drawSanctuaryTempleBuildingPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    const int tx,
    const int ty,
    const int altitude,
    const int templeOverlayDirId,
    const eWorldDirection dir,
    const int animFrame,
    const bool canBuild)
{
    if(templeOverlayDirId < 0) {
        return;
    }
    const int rotateId = templeOverlayDirId % 10;
    const int textureKey = templeOverlayDirId / 10;
    const bool splitTexture = textureKey >= 10;
    const int baseDirId = splitTexture ?
                              textureKey / 10 :
                              textureKey;
    const int textureOverlayDirId = splitTexture ?
                                        textureKey % 10 :
                                        -1;

    double drawX;
    double drawY;
    previewDrawXY(board, tx, ty, drawX, drawY, 4, 4, altitude);
    applyPreviewSpanCameraOffset(drawX, drawY, 4, 4, dir);

    const auto drawLayer = [&](const std::shared_ptr<eTexture>& tex) {
        if(!tex) return;
        modPreviewTexture(tex, canBuild);
        tp.drawTexture(drawX, drawY, tex, eAlignment::top);
        tex->clearColorMod();
    };

    if(static_cast<int>(builTexs.fSanctuary.size()) > baseDirId) {
        const auto& coll = builTexs.fSanctuary[baseDirId];
        const int maxLayer = splitTexture ? 2 : coll.size();
        for(int i = 0; i < maxLayer; i++) {
            drawLayer(coll.getTexture(i));
        }
    }

    if(splitTexture &&
       static_cast<int>(builTexs.fSanctuary.size()) > textureOverlayDirId) {
        const auto& coll = builTexs.fSanctuary[textureOverlayDirId];
        if(coll.size() > 2) drawLayer(coll.getTexture(2));
    }

    if(rotateId != 0 && rotateId != 1) {
        return;
    }

    std::shared_ptr<eTexture> tex;
    const double offX = gWomanOffsetX[rotateId % 4];
    const double offY = gWomanOffsetY[rotateId % 4];
    if(baseDirId == 0) {
        const auto& coll = builTexs.fSanctuaryHOverlay;
        tex = coll.getTexture(animFrame % coll.size());
    } else {
        const auto& coll = builTexs.fSanctuaryWOverlay;
        tex = coll.getTexture(animFrame % coll.size());
    }
    if(!tex) return;
    modPreviewTexture(tex, canBuild);
    tp.drawTexture(drawX + offX, drawY + offY, tex);
    tex->clearColorMod();
}

void drawSanctuaryStatuePreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    const eGodType god,
    const int statueTextureId,
    const int tx,
    const int ty,
    const int altitude,
    const eWorldDirection dir,
    const bool canBuild)
{
    (void)dir;
    const auto coll = statueTextureCollection(builTexs, god);
    if(!coll || statueTextureId < 0 || statueTextureId >= coll->size()) {
        return;
    }
    double drawX;
    double drawY;
    previewDrawXY(board, tx, ty, drawX, drawY, 1, 1, altitude);
    const auto tex = coll->getTexture(statueTextureId);
    modPreviewTexture(tex, canBuild);
    tp.drawTexture(drawX, drawY, tex, eAlignment::top);
    tex->clearColorMod();
}

void drawSanctuaryMonumentPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    const eGodType god,
    const int monumentTextureId,
    const int tx,
    const int ty,
    const int altitude,
    const eWorldDirection dir,
    const bool canBuild)
{
    (void)dir;
    const auto coll = monumentTextureCollection(builTexs, god);
    if(!coll || monumentTextureId < 0 || monumentTextureId >= coll->size()) {
        return;
    }
    double drawX;
    double drawY;
    previewDrawXY(board, tx, ty, drawX, drawY, 2, 2, altitude);
    applyPreviewSpanCameraOffset(drawX, drawY, 2, 2, dir);
    const auto tex = coll->getTexture(monumentTextureId);
    modPreviewTexture(tex, canBuild);
    tp.drawTexture(drawX, drawY, tex, eAlignment::top);
    tex->clearColorMod();
}

void drawSanctuaryAltarPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    const int tx,
    const int ty,
    const int altitude,
    const eWorldDirection dir,
    const int rotateId,
    const bool canBuild)
{
    double drawX;
    double drawY;
    previewDrawXY(board, tx, ty, drawX, drawY, 2, 2, altitude);
    applyPreviewSpanCameraOffset(drawX, drawY, 2, 2, dir);
    drawX += gAltarOffsetX[rotateId % 4];
    drawY += gAltarOffsetY[rotateId % 4];
    const auto tex = (rotateId % 2 == 1) ? builTexs.fSanctuaryAltarFlipped
                                          : builTexs.fSanctuaryAltar;
    if(!tex) return;
    modPreviewTexture(tex, canBuild);
    tp.drawTexture(drawX, drawY, tex, eAlignment::top);
    tex->clearColorMod();
}

void drawSanctuaryTorchPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    const int tx,
    const int ty,
    const int altitude,
    const int animFrame)
{
    double rx;
    double ry;
    previewDrawXY(board, tx, ty, rx, ry, 1, 1, altitude);
    const auto& coll = builTexs.fSanctuaryFire;
    if(coll.size() == 0) return;
    const int texId = (animFrame / 4) % coll.size();
    const auto& tex = coll.getTexture(texId);
    tp.drawTexture(rx + 0.5, ry - 0.5, tex, eAlignment::bottom);
}

void drawSanctuaryTerrainPreview(
    GameBoard& board,
    eTilePainter& tp,
    const eBuildingTextures& builTexs,
    const eTerrainTextures& trrTexs,
    const eBuildingMode mode,
    const int rotateId,
    const int hoverTX,
    const int hoverTY,
    const eCityId viewedCityId,
    const SDL_Rect footprint,
    const eWorldDirection dir,
    const int boardw,
    const int boardh,
    const int animFrame,
    const bool canBuild)
{
    if (footprint.w <= 0 || footprint.h <= 0)
        return;

    const auto type = eBuildingModeHelpers::toBuildingType(mode);
    if(!eBuilding::sSanctuaryBuilding(type)) {
        return;
    }

    const bool transposed = rotateId == 1 || rotateId == 3;
    const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, transposed);
    const int xMin = hoverTX - h->fW/2;
    const int yMin = hoverTY - h->fH/2;
    const auto sanctuaryTiles = sanctuaryPreviewTiles(*h, rotateId);
    std::vector<PreviewTile> tiles;
    tiles.reserve(sanctuaryTiles.size());

    for (const auto& tile : sanctuaryTiles) {
        const auto& ele = tile.fEle;
        const int x = xMin + ele.fX;
        const int y = yMin + ele.fY;
        const auto boardTile = board.tile(x, y);
        if (!boardTile)
            continue;
        tiles.push_back({x, y, boardTile->altitude() + ele.fA,
                         ele.fType, ele.fId});
    }

    sortByDrawOrder(tiles, dir, boardw, boardh);
    for (const auto& tile : tiles)
    {
        const auto tex = sanctuaryTerrainTexture(
            tile, board, viewedCityId, tp.size(), builTexs, trrTexs, dir);
        if(!tex) continue;
        double rx;
        double ry;
        previewDrawXY(board, tile.fX, tile.fY, rx, ry, 1, 1,
                      tile.fAltitude);
        tp.drawTexture(rx, ry, tex, eAlignment::top);
    }
}
