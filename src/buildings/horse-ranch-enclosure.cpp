#include "horse-ranch-enclosure.h"

#include <memory>

#include "textures/egametextures.h"

#include "characters/horse.h"
#include "characters/actions/animal-action.h"

#include "ehorseranch.h"
#include "engine/game-board.h"
#include "fileIO/esavearchive.h"
#include "etilehelper.h"

namespace {

void horseDrawPosition(const eTile& tile, const double horseTileX,
                       const double horseTileY, const eWorldDirection dir,
                       const int boardWidth, const int boardHeight,
                       double& drawX, double& drawY) {
    int viewTileX;
    int viewTileY;
    eTileHelper::tileIdToRotatedTileId(tile.x(), tile.y(), viewTileX, viewTileY,
                                       dir, boardWidth, boardHeight);
    if(dir == eWorldDirection::N) {
        drawX = viewTileX + horseTileX + 0.5;
        drawY = viewTileY + horseTileY + 0.5;
    } else if(dir == eWorldDirection::E) {
        drawX = viewTileX + horseTileY + 0.5;
        drawY = viewTileY - horseTileX + 1.5;
    } else if(dir == eWorldDirection::S) {
        drawX = viewTileX - horseTileX + 1.5;
        drawY = viewTileY - horseTileY + 1.5;
    } else { // if(dir == eWorldDirection::W) {
        drawX = viewTileX - horseTileY + 1.5;
        drawY = viewTileY + horseTileX + 0.5;
    }
}

bool tileInRect(eTile* const tile, const SDL_Rect& rect) {
    if(!tile) return false;
    const SDL_Point p{tile->x(), tile->y()};
    return SDL_PointInRect(&p, &rect);
}

stdsptr<AnimalAction> createHorseAction(Horse* const horse,
                                        eTile* const tile,
                                        const SDL_Rect& rect) {
    const auto walkable = WalkableObject::sCreateRanch(rect);
    const auto a = e::make_shared<AnimalAction>(
                       horse, tile->x(), tile->y(), walkable);
    a->setLayTime(500);
    a->setWalkTime(1000);
    return a;
}

}

HorseRanchEnclosure::HorseRanchEnclosure(GameBoard& board,
                                         const eCityId cid) :
    eBuildingWithResource(board, eBuildingType::horseRanchEnclosure, 4, 4,
                          cid) {
    setEnabled(true);
}

HorseRanchEnclosure::~HorseRanchEnclosure() {
    for(const auto& h : mHorses) {
        h->kill();
    }
}

void HorseRanchEnclosure::erase() {
    if(mRanch) mRanch->eBuilding::erase();
    eBuilding::erase();
}

int HorseRanchEnclosure::provide(const eProvide p, const int n) {
    if(mRanch) mRanch->eEmployingBuilding::provide(p, n);
    return eBuildingWithResource::provide(p, n);
}

std::shared_ptr<eTexture> HorseRanchEnclosure::getTexture(
        const eTileSize size) const {
    const int sizeId = static_cast<int>(size);
    auto& blds = eGameTextures::buildings();
    return blds[sizeId].fHorseRanchEnclosure;
}

std::vector<BuildingContainedActorDraw>
HorseRanchEnclosure::getActorsDrawnAfterBuildingTexture(
        const eTileSize size) const {
    const auto& board = getBoard();
    const auto dir = board.direction();
    const int boardWidth = board.width();
    const int boardHeight = board.height();
    std::vector<BuildingContainedActorDraw> draws;
    for(const auto& h : mHorses) {
        const auto t = h->tile();
        if(!t) continue;
        auto& draw = draws.emplace_back();
        draw.fTexture = h->getTexture(size);
        horseDrawPosition(*t, h->x(), h->y(), dir, boardWidth, boardHeight,
                          draw.fViewTileX, draw.fViewTileY);
    }
    return draws;
}

void HorseRanchEnclosure::timeChanged(const int by) {
    (void)by;
    const auto tile = centerTile();
    if(!tile) return;
    const auto rect = tileRect();
    for(const auto& h : mHorses) {
        if(tileInRect(h->tile(), rect)) continue;
        h->changeTile(tile);
        h->setX(0.5);
        h->setY(0.5);
        h->setAction(createHorseAction(h.get(), tile, rect));
    }
}

int HorseRanchEnclosure::count(const eResourceType type) const {
    if(type == eResourceType::horse) return horseCount();
    return eBuildingWithResource::count(type);
}

int HorseRanchEnclosure::take(const eResourceType type, const int count) {
    if(type == eResourceType::horse) {
        const int max = horseCount();
        const int t = std::min(max, count);
        for(int i = 0; i < t; i++) {
            takeHorse();
        }
        return t;
    }
    return eBuildingWithResource::take(type, count);
}

bool HorseRanchEnclosure::spawnHorse() {
    if(mHorses.size() > 5) return false;
    const auto h = e::make_shared<Horse>(getBoard());
    h->setBothCityIds(cityId());
    const auto tile = centerTile();
    h->changeTile(tile);
    const auto a = createHorseAction(h.get(), tile, tileRect());
    h->setAction(a);
    mHorses.push_back(h);
    return true;
}

bool HorseRanchEnclosure::takeHorse() {
    if(mHorses.empty()) return false;
    auto& h = mHorses.back();
    h->kill();
    mHorses.pop_back();
    return true;
}

void HorseRanchEnclosure::setRanch(eHorseRanch* const ranch) {
    mRanch = ranch;
}

void HorseRanchEnclosure::serializeFields(eSaveArchive& ar) {
    eBuildingWithResource::serializeFields(ar);
    if(ar.reading()) {
        const stdptr<HorseRanchEnclosure> tptr(this);
        auto horses = std::make_shared<std::vector<std::shared_ptr<Horse*>>>();
        mHorses.clear();
        ar.countedArrayField("horses", 0,
            [this, horses](eSaveArchive& itemAr, const int i) {
                if(i >= static_cast<int>(horses->size())) horses->resize(i + 1);
                if(!(*horses)[i]) (*horses)[i] = std::make_shared<Horse*>(nullptr);
                itemAr.characterField("horse", &getBoard(), *(*horses)[i]);
            });
        ar.addPostFunc([tptr, horses]() {
            if(!tptr) return;
            tptr->mHorses.clear();
            for(const auto& hptr : *horses) {
                const auto h = hptr ? *hptr : nullptr;
                if(h) tptr->mHorses.push_back(h->ref<Horse>());
            }
        }, "HorseRanchEnclosure::horses");
    } else {
        const int nh = static_cast<int>(mHorses.size());
        ar.countedArrayField("horses", nh,
            [this](eSaveArchive& itemAr, const int i) {
                Horse* raw = mHorses[i].get();
                itemAr.characterField("horse", &getBoard(), raw);
            });
    }
    ar.buildingAsField("ranch", &getBoard(), mRanch);
}
