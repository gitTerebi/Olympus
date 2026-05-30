#include "warehouse.h"

#include "engine/game-board.h"
#include "etilehelper.h"
#include "textures/egametextures.h"

#include <algorithm>
#include <vector>

namespace {

struct WarehouseSlot {
    int fX = 0;
    int fY = 0;
};

struct WarehouseDrawSlot {
    WarehouseSlot fSlot;
    int fStorageId = -1;
    bool fDoor = false;
};

WarehouseSlot sWarehouseHomeDoorSlot() {
    return {-1, -3};
}

std::vector<WarehouseSlot> sWarehouseStorageRealSlots() {
    return {{-1, -2},
            {-1, -1},
            {0, -3},
            {0, -2},
            {0, -1},
            {1, -3},
            {1, -2},
            {1, -1}};
}

std::pair<double, double> sSlotXY(const WarehouseSlot& slot) {
    return {double(slot.fX), double(slot.fY)};
}

WarehouseSlot sSlotFromLocalTile(const int x, const int y) {
    return {x - 1, y - 3};
}

std::pair<double, double> sSlotShiftFromHome(
        const WarehouseSlot& slot) {
    const auto homeSlot = sWarehouseHomeDoorSlot();
    return {double(slot.fX - homeSlot.fX),
            double(slot.fY - homeSlot.fY)};
}

WarehouseSlot sSlotFromRealTile(const SDL_Rect& rect,
                                 const eWorldDirection dir,
                                 const int boardW,
                                 const int boardH,
                                 SDL_Rect& rotatedRect) {
    const SDL_Point realDoor{rect.x, rect.y};
    SDL_Point rotatedDoor;
    rotatedRect = eTileHelper::toRotatedRect(rect, dir, boardW, boardH);
    eTileHelper::tileIdToRotatedTileId(realDoor.x, realDoor.y,
                                       rotatedDoor.x, rotatedDoor.y,
                                       dir, boardW, boardH);
    const int localX = rotatedDoor.x - rotatedRect.x;
    const int localY = rotatedDoor.y - rotatedRect.y;
    return sSlotFromLocalTile(localX, localY);
}

WarehouseSlot sRotatedSlotFromRealSlot(
        const SDL_Rect& rect,
        const SDL_Rect& rotatedRect,
        const eWorldDirection dir,
        const int boardW,
        const int boardH,
        const WarehouseSlot& slot) {
    const int tileX = rect.x + slot.fX + 1;
    const int tileY = rect.y + slot.fY + 3;
    int rotatedX;
    int rotatedY;
    eTileHelper::tileIdToRotatedTileId(tileX, tileY,
                                       rotatedX, rotatedY,
                                       dir, boardW, boardH);
    const int localX = rotatedX - rotatedRect.x;
    const int localY = rotatedY - rotatedRect.y;
    return sSlotFromLocalTile(localX, localY);
}

std::vector<WarehouseDrawSlot> sWarehouseStorageSlots(
        const SDL_Rect& rect,
        const SDL_Rect& rotatedRect,
        const eWorldDirection dir,
        const int boardW,
        const int boardH) {
    // Warehouse art is a 3x3 slot grid. The door occupies one slot;
    // storage piles draw in the other eight.
    const auto realSlots = sWarehouseStorageRealSlots();
    std::vector<WarehouseDrawSlot> result;
    int storageId = 0;
    for(const auto& realSlot : realSlots) {
        auto& slot = result.emplace_back();
        slot.fSlot = sRotatedSlotFromRealSlot(rect, rotatedRect, dir,
                                              boardW, boardH, realSlot);
        slot.fStorageId = storageId++;
    }
    return result;
}

void sSortWarehouseDrawSlots(std::vector<WarehouseDrawSlot>& slots) {
    std::sort(slots.begin(), slots.end(),
              [](const WarehouseDrawSlot& a,
                 const WarehouseDrawSlot& b) {
        if(a.fSlot.fY != b.fSlot.fY) {
            return a.fSlot.fY < b.fSlot.fY;
        }
        return a.fSlot.fX < b.fSlot.fX;
    });
}

}

Warehouse::Warehouse(GameBoard& board, const eCityId cid) :
    WarehouseBase(board, eBuildingType::warehouse, 3, 3, 12,
                   eResourceType::warehouse, cid) {
    setOverlayEnabledFunc([]() { return true; });
}

std::shared_ptr<eTexture> Warehouse::getTexture(const eTileSize size) const {
    (void)size;
    return nullptr;
}

eTextureSpace Warehouse::getTextureSpace(const int tx, const int ty,
                                          const eTileSize size) const {
    return eBuilding::getTextureSpace(tx, ty, size);
}

std::vector<eOverlay> Warehouse::getOverlays(const eTileSize size) const {
    std::vector<eOverlay> os;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& texs = blds[sizeId];
    const eWorldDirection dir = getBoard().direction();
    const auto rect = tileRect();
    const int boardW = getBoard().width();
    const int boardH = getBoard().height();
    SDL_Rect rotatedRect;
    const auto doorSlot = sSlotFromRealTile(rect, dir, boardW, boardH,
                                            rotatedRect);
    const auto doorSlotShift = sSlotShiftFromHome(doorSlot);
    auto drawSlots = sWarehouseStorageSlots(rect, rotatedRect, dir,
                                            boardW, boardH);
    auto& doorSlotInfo = drawSlots.emplace_back();
    doorSlotInfo.fSlot = doorSlot;
    doorSlotInfo.fDoor = true;
    sSortWarehouseDrawSlots(drawSlots);

    for(const auto& slot : drawSlots) {
        if(slot.fDoor) {
            auto& door = os.emplace_back();
            door.fTex = texs.fWarehouseDoor;
            door.fX = doorSlotShift.first;
            door.fY = doorSlotShift.second;
            door.fAlignTop = true;
            if(enabled()) {
                const auto& coll = texs.fWarehouseOverlay;
                const int texId = textureTime() % coll.size();
                auto& man = os.emplace_back();
                man.fTex = coll.getTexture(texId);
                man.fX = doorSlotShift.first - 1.24;
                man.fY = doorSlotShift.second - 4.32;
            }
        } else {
            getSpaceOverlay(size, os, sSlotXY(slot.fSlot),
                            slot.fStorageId);
        }
    }

    return os;
}
