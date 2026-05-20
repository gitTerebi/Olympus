#include "ewarehouse.h"

#include "engine/e-game-board.h"
#include "etilehelper.h"
#include "textures/egametextures.h"

#include <algorithm>
#include <vector>

namespace {

struct eWarehouseSlot {
    int fX = 0;
    int fY = 0;
};

struct eWarehouseDrawSlot {
    eWarehouseSlot fSlot;
    int fStorageId = -1;
    bool fDoor = false;
};

eWarehouseSlot sWarehouseHomeDoorSlot() {
    return {-1, -3};
}

std::vector<eWarehouseSlot> sWarehouseStorageRealSlots() {
    return {{-1, -2},
            {-1, -1},
            {0, -3},
            {0, -2},
            {0, -1},
            {1, -3},
            {1, -2},
            {1, -1}};
}

std::pair<double, double> sSlotXY(const eWarehouseSlot& slot) {
    return {double(slot.fX), double(slot.fY)};
}

bool sSameSlot(const eWarehouseSlot& slot, const int x, const int y) {
    return slot.fX == x && slot.fY == y;
}

eWarehouseSlot sSlotFromLocalTile(const int x, const int y) {
    return {x - 1, y - 3};
}

std::pair<double, double> sSlotShiftFromHome(
        const eWarehouseSlot& slot) {
    const auto homeSlot = sWarehouseHomeDoorSlot();
    return {double(slot.fX - homeSlot.fX),
            double(slot.fY - homeSlot.fY)};
}

eWarehouseSlot sSlotFromRealTile(const SDL_Rect& rect,
                                 const eWorldDirection dir,
                                 const int boardW,
                                 const int boardH,
                                 SDL_Rect& rotatedRect,
                                 SDL_Point& realDoor,
                                 SDL_Point& rotatedDoor) {
    realDoor = {rect.x, rect.y};
    rotatedRect = eTileHelper::toRotatedRect(rect, dir, boardW, boardH);
    eTileHelper::tileIdToRotatedTileId(realDoor.x, realDoor.y,
                                       rotatedDoor.x, rotatedDoor.y,
                                       dir, boardW, boardH);
    const int localX = rotatedDoor.x - rotatedRect.x;
    const int localY = rotatedDoor.y - rotatedRect.y;
    return sSlotFromLocalTile(localX, localY);
}

eWarehouseSlot sRotatedSlotFromRealSlot(
        const SDL_Rect& rect,
        const SDL_Rect& rotatedRect,
        const eWorldDirection dir,
        const int boardW,
        const int boardH,
        const eWarehouseSlot& slot) {
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

std::vector<eWarehouseDrawSlot> sWarehouseStorageSlots(
        const SDL_Rect& rect,
        const SDL_Rect& rotatedRect,
        const eWorldDirection dir,
        const int boardW,
        const int boardH) {
    // Warehouse art is a 3x3 slot grid. The door occupies one slot;
    // storage piles draw in the other eight.
    const auto realSlots = sWarehouseStorageRealSlots();
    std::vector<eWarehouseDrawSlot> result;
    int storageId = 0;
    for(const auto& realSlot : realSlots) {
        auto& slot = result.emplace_back();
        slot.fSlot = sRotatedSlotFromRealSlot(rect, rotatedRect, dir,
                                              boardW, boardH, realSlot);
        slot.fStorageId = storageId++;
    }
    return result;
}

void sSortWarehouseDrawSlots(std::vector<eWarehouseDrawSlot>& slots) {
    std::sort(slots.begin(), slots.end(),
              [](const eWarehouseDrawSlot& a,
                 const eWarehouseDrawSlot& b) {
        if(a.fSlot.fY != b.fSlot.fY) {
            return a.fSlot.fY < b.fSlot.fY;
        }
        return a.fSlot.fX < b.fSlot.fX;
    });
}

}

eWarehouse::eWarehouse(eGameBoard& board, const eCityId cid) :
    eWarehouseBase(board, eBuildingType::warehouse, 3, 3, 12,
                   eResourceType::warehouse, cid) {
    setOverlayEnabledFunc([]() { return true; });
}

std::shared_ptr<eTexture> eWarehouse::getTexture(const eTileSize size) const {
    (void)size;
    return nullptr;
}

eTextureSpace eWarehouse::getTextureSpace(const int tx, const int ty,
                                          const eTileSize size) const {
    auto result = eBuilding::getTextureSpace(tx, ty, size);
    result.fClamp = false;
    return result;
}

std::vector<eOverlay> eWarehouse::getOverlays(const eTileSize size) const {
    std::vector<eOverlay> os;
    const int sizeId = static_cast<int>(size);
    const auto& blds = eGameTextures::buildings();
    const auto& texs = blds[sizeId];
    const eWorldDirection dir = getBoard().direction();
    const auto rect = tileRect();
    const int boardW = getBoard().width();
    const int boardH = getBoard().height();
    SDL_Rect rotatedRect;
    SDL_Point realDoor;
    SDL_Point rotatedDoor;
    const auto doorSlot = sSlotFromRealTile(rect, dir, boardW, boardH,
                                            rotatedRect, realDoor,
                                            rotatedDoor);
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
