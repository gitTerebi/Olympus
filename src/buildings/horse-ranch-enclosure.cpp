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

void rotatedHorsePosition(const eTile& tile, const double x, const double y,
                          const eWorldDirection dir, const int boardW,
                          const int boardH, double& rx, double& ry) {
    int rtx;
    int rty;
    eTileHelper::tileIdToRotatedTileId(tile.x(), tile.y(), rtx, rty,
                                       dir, boardW, boardH);
    if(dir == eWorldDirection::N) {
        rx = rtx + x + 0.5;
        ry = rty + y + 0.5;
    } else if(dir == eWorldDirection::E) {
        rx = rtx + y + 0.5;
        ry = rty - x + 1.5;
    } else if(dir == eWorldDirection::S) {
        rx = rtx - x + 1.5;
        ry = rty - y + 1.5;
    } else { // if(dir == eWorldDirection::W) {
        rx = rtx - y + 1.5;
        ry = rty + x + 0.5;
    }
}

void enclosureDrawPosition(const SDL_Rect& rect, const eWorldDirection dir,
                           const int boardW, const int boardH,
                           double& x, double& y) {
    const auto rotatedRect = eTileHelper::toRotatedRect(rect, dir,
                                                       boardW, boardH);
    x = rotatedRect.x + rotatedRect.w - 2;
    y = rotatedRect.y + rotatedRect.h + 2;
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

std::vector<eOverlay> HorseRanchEnclosure::getOverlays(
        const eTileSize size) const {
    const auto& board = getBoard();
    const auto dir = board.direction();
    const int boardW = board.width();
    const int boardH = board.height();
    double drawX;
    double drawY;
    enclosureDrawPosition(tileRect(), dir, boardW, boardH, drawX, drawY);
    std::vector<eOverlay> os;
    for(const auto& h : mHorses) {
        const auto t = h->tile();
        if(!t) continue;
        auto& o = os.emplace_back();
        o.fTex = h->getTexture(size);
        double horseX;
        double horseY;
        rotatedHorsePosition(*t, h->x(), h->y(), dir, boardW, boardH,
                             horseX, horseY);
        o.fX = horseX - drawX;
        o.fY = horseY - drawY;
    }
    return os;
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
