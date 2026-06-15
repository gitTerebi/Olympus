#include "widgets/paint/sanctuary-preview.h"

#include "buildings/allbuildings.h"
#include "buildings/sanctuaries/esanctbuilding.h"
#include "buildings/sanctuaries/esanctuaryblueprint.h"
#include "textures/game-textures.h"
#include "engine/etile.h"
#include "engine/game-board.h"
#include "etilehelper.h"
#include "textures/building-textures.h"
#include "textures/terrain-textures.h"
#include "widgets/ebuildingmode.h"
#include "widgets/etilepainter.h"

#include <algorithm>

namespace {

static const double gAltarOffsetX[4] = {0, 2, 0, 2};
static const double gAltarOffsetY[4] = {0, -2, 0, -2};

// Draw-space nudge after world tile -> drawXY. Tunable via JKLI.
// dirIdx: N=0, W=1, S=2, E=3
static double gWomanTileDX[4][4] = {
    {3.00, -1.50, 0.00, 0.00},
    {1.50,  0.00, 0.00, 3.00},
    {0.00,  0.00, 0.00, 1.50},
    {0.00,  0.00, -1.50, 0.00}
};
static double gWomanTileDY[4][4] = {
    {0.50,  2.00, 0.00,  0.00},
    {2.00,  0.00, 0.00, -3.00},
    {0.00,  0.00, -2.50, -1.00},
    {0.00,  0.50, -1.00,  0.00}
};

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

GodType statueGod(const eSanctEleType type, const GodType fallback)
{
    switch(type) {
    case eSanctEleType::aphroditeStatue: return GodType::aphrodite;
    case eSanctEleType::apolloStatue: return GodType::apollo;
    case eSanctEleType::aresStatue: return GodType::ares;
    case eSanctEleType::artemisStatue: return GodType::artemis;
    case eSanctEleType::athenaStatue: return GodType::athena;
    case eSanctEleType::atlasStatue: return GodType::atlas;
    case eSanctEleType::demeterStatue: return GodType::demeter;
    case eSanctEleType::dionysusStatue: return GodType::dionysus;
    case eSanctEleType::hadesStatue: return GodType::hades;
    case eSanctEleType::hephaestusStatue: return GodType::hephaestus;
    case eSanctEleType::heraStatue: return GodType::hera;
    case eSanctEleType::hermesStatue: return GodType::hermes;
    case eSanctEleType::poseidonStatue: return GodType::poseidon;
    case eSanctEleType::zeusStatue: return GodType::zeus;
    default: return fallback;
    }
}


void applyPreviewSpanCameraOffset(double& drawX,
                                  double& drawY,
                                  const int tileSpanW,
                                  const int tileSpanH,
                                  const eWorldDirection dir)
{
    if (dir == eWorldDirection::E) {
        if ((tileSpanW == 4 && tileSpanH == 4) || (tileSpanW == 2 && tileSpanH == 2)) drawX -= 1;
        else if (tileSpanW == 6 && tileSpanH == 6) drawY -= 1;
    } else if (dir == eWorldDirection::S) {
        if ((tileSpanW == 4 && tileSpanH == 4) || (tileSpanW == 2 && tileSpanH == 2)) {
            drawX -= 1;
            drawY += 1;
        } else if (tileSpanW == 6 && tileSpanH == 6) {
            drawX -= 1;
            drawY -= 1;
        }
    } else if (dir == eWorldDirection::W) {
        if ((tileSpanW == 4 && tileSpanH == 4) || (tileSpanW == 2 && tileSpanH == 2)) drawY += 1;
        else if (tileSpanW == 6 && tileSpanH == 6) drawX -= 1;
    }
}

void sortByDrawOrder(std::vector<PreviewTile>& tiles,
                     const eWorldDirection dir,
                     const int boardWidth,
                     const int boardHeight)
{
    std::stable_sort(tiles.begin(), tiles.end(),
                     [&](const PreviewTile& lhs, const PreviewTile& rhs)
                     {
                         int lhsViewTileX;
                         int lhsViewTileY;
                         int rhsViewTileX;
                         int rhsViewTileY;
                         eTileHelper::tileIdToRotatedTileId(
                             lhs.fX, lhs.fY, lhsViewTileX, lhsViewTileY,
                             dir, boardWidth, boardHeight);
                         eTileHelper::tileIdToRotatedTileId(
                             rhs.fX, rhs.fY, rhsViewTileX, rhsViewTileY,
                             dir, boardWidth, boardHeight);
                         if (lhsViewTileY != rhsViewTileY)
                             return lhsViewTileY < rhsViewTileY;
                         return lhsViewTileX < rhsViewTileX;
                     });
}

void previewDrawXY(GameBoard& board,
                   int worldTileX,
                   int worldTileY,
                   double& drawX,
                   double& drawY,
                   const int tileSpanW,
                   const int tileSpanH,
                   const int altitude)
{
    const auto dir = board.direction();
    int viewTileX = worldTileX;
    int viewTileY = worldTileY;
    if (dir != eWorldDirection::N)
    {
        eTileHelper::tileIdToRotatedTileId(worldTileX, worldTileY,
                                           viewTileX, viewTileY,
                                           dir, board.width(), board.height());
    }

    drawX = viewTileX + 0.5;
    drawY = viewTileY + 1.5;

    if (tileSpanW == 2 && tileSpanH == 2)       { drawX += 0.5; drawY += 0.5; }
    else if (tileSpanW == 3 && tileSpanH == 3)  { drawX += 0.0; drawY += 2.0; }
    else if (tileSpanW == 4 && tileSpanH == 4)  { drawX += 0.5; drawY += 2.5; }
    else if (tileSpanW == 5 && tileSpanH == 5)  { drawX += 0.0; drawY += 4.0; }
    else if (tileSpanW == 6 && tileSpanH == 6)  { drawX += 0.5; drawY += 5.5; }
    drawX -= altitude;
    drawY -= altitude;
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


std::shared_ptr<eTexture> sanctuaryTerrainTexture(
    const PreviewTile& tile,
    GameBoard& board,
    const eCityId viewedCityId,
    const eTileSize tileSize,
    const BuildingTextures& builTexs,
    const TerrainTextures& trrTexs,
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
    const BuildingTextures& builTexs,
    const GodType god)
{
    switch(god) {
    case GodType::aphrodite: return &builTexs.fAphroditeStatues;
    case GodType::apollo: return &builTexs.fApolloStatues;
    case GodType::ares: return &builTexs.fAresStatues;
    case GodType::artemis: return &builTexs.fArtemisStatues;
    case GodType::athena: return &builTexs.fAthenaStatues;
    case GodType::atlas: return &builTexs.fAtlasStatues;
    case GodType::demeter: return &builTexs.fDemeterStatues;
    case GodType::dionysus: return &builTexs.fDionysusStatues;
    case GodType::hades: return &builTexs.fHadesStatues;
    case GodType::hephaestus: return &builTexs.fHephaestusStatues;
    case GodType::hera: return &builTexs.fHeraStatues;
    case GodType::hermes: return &builTexs.fHermesStatues;
    case GodType::poseidon: return &builTexs.fPoseidonStatues;
    case GodType::zeus: return &builTexs.fZeusStatues;
    }
    return nullptr;
}

const eTextureCollection* monumentTextureCollection(
    const BuildingTextures& builTexs,
    const GodType god)
{
    switch(god) {
    case GodType::aphrodite: return &builTexs.fAphroditeMonuments;
    case GodType::apollo: return &builTexs.fApolloMonuments;
    case GodType::ares: return &builTexs.fAresMonuments;
    case GodType::artemis: return &builTexs.fArtemisMonuments;
    case GodType::athena: return &builTexs.fAthenaMonuments;
    case GodType::atlas: return &builTexs.fAtlasMonuments;
    case GodType::demeter: return &builTexs.fDemeterMonuments;
    case GodType::dionysus: return &builTexs.fDionysusMonuments;
    case GodType::hades: return &builTexs.fHadesMonuments;
    case GodType::hephaestus: return &builTexs.fHephaestusMonuments;
    case GodType::hera: return &builTexs.fHeraMonuments;
    case GodType::hermes: return &builTexs.fHermesMonuments;
    case GodType::poseidon: return &builTexs.fPoseidonMonuments;
    case GodType::zeus: return &builTexs.fZeusMonuments;
    }
    return nullptr;
}

}

SanctuaryPreviewEntry::SanctuaryPreviewEntry(
    const int order,
    const int worldTileX, const int worldTileY, const int altitude,
    const int templeOverlayDirId,
    const GodType statueGod,
    const int statueTextureId,
    const int monumentTextureId,
    const bool altar,
    const stdsptr<eBuilding>& b) :
    fOrder(order), fTx(worldTileX), fTy(worldTileY), fAltitude(altitude),
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
    const auto god = static_cast<GodType>(
        static_cast<int>(mode) -
        static_cast<int>(eBuildingMode::templeAphrodite));
    const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, rotateId);
    const int tileSpanW = h->fW;
    const int tileSpanH = h->fH;
    const int xMin = hoverTX - tileSpanW/2;
    const int yMin = hoverTY - tileSpanH/2;
    footprint = {xMin, yMin, tileSpanW, tileSpanH};

    const auto sanctuary = e::make_shared<eMonument>(
        board, type, tileSpanW, tileSpanH, 0, viewedCityId);
    sanctuary->setRotateId(rotateId);
    sanctuary->setTileRect(footprint);
    GameTextures::loadZeusSanctuary();
    eSanctuary::sLoadMonumentTextures(god);
    GameTextures::loadSanctuary();

    std::vector<SanctuaryPreviewEntry> result;
    const auto tiles = sanctuaryPreviewTiles(*h, rotateId);
    result.reserve(tiles.size());
    for(const auto& tile : tiles) {
        const auto& te = tile.fEle;
        const int tileOrder = tile.fPreviewX + tile.fPreviewY;
        const int worldTileX = xMin + te.fX;
        const int worldTileY = yMin + te.fY;
        const int previewX = xMin + tile.fPreviewX;
        const int previewY = yMin + tile.fPreviewY;
        const auto boardTile = board.tile(worldTileX, worldTileY);
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
            auto& floorEntry = result.emplace_back(tileOrder, worldTileX, worldTileY, altitude, -1,
                                                    god, -1, -1, false, b);
            if(isTempleFloorTile(te.fType) && te.fId >= 10) {
                auto& torchEntry = result.emplace_back(tileOrder, worldTileX, worldTileY, altitude, -1,
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
            result.emplace_back(tileOrder, previewX, previewY, altitude,
                                rotateId,
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
                                god, -1, rotateId, false, b);
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
            const GodType sg = statueGod(te.fType, god);
            const int statueTextureId = rotateId;
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

    const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, rotateId);
    const int xMin = hoverTX - h->fW/2;
    const int yMin = hoverTY - h->fH/2;
    const int templeTex = (rotateId == 0 ? 1 : rotateId == 2 ? 3 : 0) + 1;
    printf("sanctuary preview rotate after-key mode=%d rotateId=%d templeTex=%05d hover=(%d,%d) origin=(%d,%d) size=%dx%d\n",
           static_cast<int>(mode), rotateId,
           templeTex,
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
    const int dirIdx,
    const int dx,
    const int dy)
{
    (void)mode;
    adjustWomanDebugOffset(rotateId, dirIdx, dx, dy);
}

void adjustWomanDebugOffset(
    const int rotateId,
    const int dirIdx,
    const int dx,
    const int dy)
{
    const int r = rotateId % 4;
    const int d = dirIdx % 4;
    gWomanTileDX[r][d] += dx * 0.5;
    gWomanTileDY[r][d] += dy * 0.5;
    printf("womanTile rot=%d dir=%d tile=(%.2f,%.2f)\n",
           r, d, gWomanTileDX[r][d], gWomanTileDY[r][d]);
}

double sanctuaryWomanTileDX(const int rotateId, const int dirIdx) {
    return gWomanTileDX[rotateId % 4][dirIdx % 4];
}

double sanctuaryWomanTileDY(const int rotateId, const int dirIdx) {
    return gWomanTileDY[rotateId % 4][dirIdx % 4];
}

SanctuaryTempleTextures sanctuaryTempleGetTextures(
    const BuildingTextures& builTexs,
    const int rotateId,
    const eWorldDirection dir,
    const int animFrame,
    const int stage)
{
    const int baseDirId = sanctuaryTempleDirId(rotateId, dir);
    SanctuaryTempleTextures result;
    if(baseDirId == 1)
        result.fBase = builTexs.fSanctuary[1].getTexture(stage);
    else if(baseDirId == 0) {
        result.fBase = builTexs.fSanctuary[2].getTexture(stage);
        if(stage == 2) result.fFlip = builTexs.fSanctuaryFlippedSW.getTexture(0);
    } else if(baseDirId == 2)
        result.fBase = builTexs.fSanctuary[0].getTexture(stage);
    else {
        result.fBase = builTexs.fSanctuary[3].getTexture(stage);
        if(stage == 2) result.fFlip = builTexs.fSanctuaryFlippedNW.getTexture(0);
    }
    if(stage == 2 && sanctuaryTempleFrontFacing(rotateId, dir) &&
       (baseDirId == 0 || baseDirId == 1)) {
        if(baseDirId == 0) {
            const auto& coll = builTexs.fSanctuaryHOverlay;
            result.fWoman = coll.getTexture(animFrame % coll.size());
        } else {
            const auto& coll = builTexs.fSanctuaryWOverlay;
            result.fWoman = coll.getTexture(animFrame % coll.size());
        }
    }
    return result;
}


std::shared_ptr<eTexture> sanctuaryStatueGetTexture(
    const BuildingTextures& builTexs,
    const GodType god,
    const int rotateId,
    const eWorldDirection dir)
{
    const auto coll = statueTextureCollection(builTexs, god);
    if(!coll) return nullptr;
    return coll->getTexture(sanctuaryFigureDirId(rotateId, dir));
}

std::shared_ptr<eTexture> sanctuaryMonumentGetTexture(
    const BuildingTextures& builTexs,
    const GodType god,
    const int rotateId,
    const eWorldDirection dir)
{
    const auto coll = monumentTextureCollection(builTexs, god);
    if(!coll) return nullptr;
    return coll->getTexture(sanctuaryFigureDirId(rotateId, dir));
}

std::shared_ptr<eTexture> sanctuaryAltarGetTexture(
    const BuildingTextures& builTexs,
    const int rotateId)
{
    return (rotateId % 2 == 1) ? builTexs.fSanctuaryAltarFlipped
                               : builTexs.fSanctuaryAltar;
}

void sanctuaryTempleDrawOrigin(
    const eBuildingType type,
    const int rotateId,
    const int xMin,
    const int yMin,
    int& outTx,
    int& outTy)
{
    const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, rotateId);
    const bool flipped = rotateId >= 2;
    for(const auto& column : h->fTiles) {
        for(auto ele : column) {
            if(ele.fType != eSanctEleType::sanctuary) continue;
            int previewX = ele.fX;
            int previewY = ele.fY;
            if(flipped) {
                ele = rotateId180(ele);
                ele.fX = h->fW - 1 - ele.fX;
                ele.fY = h->fH - 1 - ele.fY;
                previewX = ele.fX;
                previewY = ele.fY;
            }
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
            outTx = xMin + previewX;
            outTy = yMin + previewY;
            return;
        }
    }
    outTx = xMin;
    outTy = yMin;
}

void drawSanctuaryTempleBuildingPreview(
    GameBoard& board,
    eTilePainter& tp,
    const BuildingTextures& builTexs,
    const int worldTileX,
    const int worldTileY,
    const int altitude,
    const int placementRotateId,
    const eWorldDirection dir,
    const int animFrame,
    const bool canBuild,
    const int stage)
{
    if(placementRotateId < 0) {
        return;
    }
    const int rotateId = placementRotateId;

    double drawX;
    double drawY;
    previewDrawXY(board, worldTileX, worldTileY, drawX, drawY, 4, 4, altitude);
    applyPreviewSpanCameraOffset(drawX, drawY, 4, 4, dir);

    const auto drawLayer = [&](const std::shared_ptr<eTexture>& tex) {
        if(!tex) return;
        tp.drawTexture(drawX, drawY, tex, eAlignment::top);
        tex->clearColorMod();
    };

    const auto t = sanctuaryTempleGetTextures(builTexs, rotateId, dir, animFrame, stage);
    drawLayer(t.fBase);
    drawLayer(t.fFlip);
    if(t.fWoman) {
        const int r = placementRotateId % 4;
        const int d = static_cast<int>(dir);
        double womanDrawX, womanDrawY;
        previewDrawXY(board, worldTileX, worldTileY, womanDrawX, womanDrawY, 1, 1, altitude);
        womanDrawX += sanctuaryWomanTileDX(r, d);
        womanDrawY += sanctuaryWomanTileDY(r, d);
        tp.drawTexture(womanDrawX, womanDrawY, t.fWoman, eAlignment::bottom);
        t.fWoman->clearColorMod();
    }
}

void drawSanctuaryStatuePreview(
    GameBoard& board,
    eTilePainter& tp,
    const BuildingTextures& builTexs,
    const GodType god,
    const int statueTextureId,
    const int worldTileX,
    const int worldTileY,
    const int altitude,
    const eWorldDirection dir,
    const bool canBuild)
{
    const auto coll = statueTextureCollection(builTexs, god);
    if(!coll || statueTextureId < 0 || statueTextureId >= coll->size()) {
        return;
    }
    const int dirId = sanctuaryFigureDirId(statueTextureId, dir);
    double drawX;
    double drawY;
    previewDrawXY(board, worldTileX, worldTileY, drawX, drawY, 1, 1, altitude);
    const auto tex = coll->getTexture(dirId);
    tp.drawTexture(drawX, drawY, tex, eAlignment::top);
    tex->clearColorMod();
}

void drawSanctuaryMonumentPreview(
    GameBoard& board,
    eTilePainter& tp,
    const BuildingTextures& builTexs,
    const GodType god,
    const int monumentTextureId,
    const int worldTileX,
    const int worldTileY,
    const int altitude,
    const eWorldDirection dir,
    const bool canBuild)
{
    const auto coll = monumentTextureCollection(builTexs, god);
    if(!coll || monumentTextureId < 0 || monumentTextureId >= coll->size()) {
        return;
    }
    const int dirId = sanctuaryFigureDirId(monumentTextureId, dir);
    double drawX;
    double drawY;
    previewDrawXY(board, worldTileX, worldTileY, drawX, drawY, 2, 2, altitude);
    applyPreviewSpanCameraOffset(drawX, drawY, 2, 2, dir);
    const auto tex = coll->getTexture(dirId);
    tp.drawTexture(drawX, drawY, tex, eAlignment::top);
    tex->clearColorMod();
}

void drawSanctuaryAltarPreview(
    GameBoard& board,
    eTilePainter& tp,
    const BuildingTextures& builTexs,
    const int worldTileX,
    const int worldTileY,
    const int altitude,
    const eWorldDirection dir,
    const int rotateId,
    const bool canBuild)
{
    double drawX;
    double drawY;
    previewDrawXY(board, worldTileX, worldTileY, drawX, drawY, 2, 2, altitude);
    applyPreviewSpanCameraOffset(drawX, drawY, 2, 2, dir);
    drawX += gAltarOffsetX[rotateId % 4];
    drawY += gAltarOffsetY[rotateId % 4];
    const auto tex = (rotateId % 2 == 1) ? builTexs.fSanctuaryAltarFlipped
                                          : builTexs.fSanctuaryAltar;
    if(!tex) return;
    tp.drawTexture(drawX, drawY, tex, eAlignment::top);
    tex->clearColorMod();
}

void drawSanctuaryTorchPreview(
    GameBoard& board,
    eTilePainter& tp,
    const BuildingTextures& builTexs,
    const int worldTileX,
    const int worldTileY,
    const int altitude,
    const int animFrame)
{
    double drawX;
    double drawY;
    previewDrawXY(board, worldTileX, worldTileY, drawX, drawY, 1, 1, altitude);
    const auto& coll = builTexs.fSanctuaryFire;
    if(coll.size() == 0) return;
    const int texId = (animFrame / 4) % coll.size();
    const auto& tex = coll.getTexture(texId);
    tp.drawTexture(drawX + 0.5, drawY - 0.5, tex, eAlignment::bottom);
}

void drawSanctuaryTerrainPreview(
    GameBoard& board,
    eTilePainter& tp,
    const BuildingTextures& builTexs,
    const TerrainTextures& trrTexs,
    const eBuildingMode mode,
    const int rotateId,
    const int hoverTX,
    const int hoverTY,
    const eCityId viewedCityId,
    const SDL_Rect footprint,
    const eWorldDirection dir,
    const int boardWidth,
    const int boardHeight,
    const int animFrame,
    const bool canBuild)
{
    if (footprint.w <= 0 || footprint.h <= 0)
        return;

    const auto type = eBuildingModeHelpers::toBuildingType(mode);
    if(!eBuilding::sSanctuaryBuilding(type)) {
        return;
    }

    const auto h = eSanctBlueprints::sSanctuaryBlueprint(type, rotateId);
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

    sortByDrawOrder(tiles, dir, boardWidth, boardHeight);
    for (const auto& tile : tiles)
    {
        const auto tex = sanctuaryTerrainTexture(
            tile, board, viewedCityId, tp.size(), builTexs, trrTexs, dir);
        if(!tex) continue;
        double drawX;
        double drawY;
        previewDrawXY(board, tile.fX, tile.fY, drawX, drawY, 1, 1,
                      tile.fAltitude);
        tp.drawTexture(drawX, drawY, tex, eAlignment::top);
    }
}
