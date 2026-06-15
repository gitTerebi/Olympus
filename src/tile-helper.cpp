#include "tile-helper.h"

#include "iterate-square.h"
#include "engine/game-board.h"

void TileHelper::dtileIdToTileId(const int dTileX, const int dTileY,
                                  int& worldTileX, int& worldTileY) {
    worldTileX = dTileX + (dTileY + 1)/2;
    worldTileY = dTileY/2 - dTileX;
}

void TileHelper::tileIdToDTileId(const int worldTileX, const int worldTileY,
                                  int& dTileX, int& dTileY) {
    dTileY = worldTileX + worldTileY;
    dTileX = (worldTileX + worldTileY)/2 - worldTileY;
}

eTile* TileHelper::closestRoad(const int roadWorldTileX,
                                const int roadWorldTileY,
                                GameBoard& board, const int minLen) {
    const auto init = board.tile(roadWorldTileX, roadWorldTileY);
    const auto cid = init->cityId();
    eTile* roadTile = nullptr;
    eTile* plainTile = nullptr;
    const auto prcsTile = [&](const int i, const int j) {
        const int worldTileX = roadWorldTileX + i;
        const int worldTileY = roadWorldTileY + j;
        const auto tile = board.tile(worldTileX, worldTileY);
        if(!tile || tile->cityId() != cid) return false;
        if(tile->roadLength(minLen) >= minLen) {
            roadTile = tile;
            return true;
        } else if(!plainTile && tile->walkable()) {
            plainTile = tile;
        }
        return false;
    };

    for(int k = 0; k < 1000; k++) {
        IterateSquare::iterateSquare(k, prcsTile, 1);
        if(roadTile) break;
    }

    const auto tile = roadTile ? roadTile : plainTile;
    return tile;
}

void TileHelper::rotatedDTileIdToDTileId(const int viewDTileX,
                                          const int viewDTileY,
                                          int& dTileX, int& dTileY,
                                          const eWorldDirection dir,
                                          const int boardWidth,
                                          const int boardHeight) {
    if(dir == eWorldDirection::N) {
        dTileX = viewDTileX;
        dTileY = viewDTileY;
    } else if(dir == eWorldDirection::E) {
        dTileX = boardWidth - 1 - (viewDTileY + 1)/2;
        dTileY = 2*viewDTileX + (viewDTileY % 2);
    } else if(dir == eWorldDirection::S) {
        dTileX = boardWidth - viewDTileX - 1;
        dTileY = boardHeight - viewDTileY - 1;
    } else { // if(dir == eWorldDirection::W) {
        dTileX = viewDTileY/2;
        dTileY = (boardHeight - 2) - 2*viewDTileX - (viewDTileY % 2);
    }
}

void TileHelper::dTileIdToRotatedDTileId(const int dTileX, const int dTileY,
                                          int& viewDTileX, int& viewDTileY,
                                          const eWorldDirection dir,
                                          const int boardWidth,
                                          const int boardHeight) {
    if(dir == eWorldDirection::N) {
        viewDTileX = dTileX;
        viewDTileY = dTileY;
    } else if(dir == eWorldDirection::E) {
        viewDTileX = dTileY/2;
        viewDTileY = 2*boardWidth - 2 - 2*dTileX - (dTileY % 2);
    } else if(dir == eWorldDirection::S) {
        viewDTileX = boardWidth - dTileX - 1;
        viewDTileY = boardHeight - dTileY - 1;
    } else { // if(dir == eWorldDirection::W) {
        viewDTileX = (boardHeight - 1)/2 - dTileY/2 - (dTileY % 2);
        viewDTileY = 2*dTileX + (dTileY % 2);
    }
}

void TileHelper::tileIdToRotatedTileId(const int worldTileX,
                                        const int worldTileY,
                                        int& viewTileX, int& viewTileY,
                                        const eWorldDirection dir,
                                        const int boardWidth,
                                        const int boardHeight) {
    if(dir == eWorldDirection::N) {
        viewTileX = worldTileX;
        viewTileY = worldTileY;
        return;
    }
    int dTileX;
    int dTileY;
    tileIdToDTileId(worldTileX, worldTileY, dTileX, dTileY);
    int viewDTileX;
    int viewDTileY;
    dTileIdToRotatedDTileId(dTileX, dTileY, viewDTileX, viewDTileY,
                            dir, boardWidth, boardHeight);
    dtileIdToTileId(viewDTileX, viewDTileY, viewTileX, viewTileY);
}

void TileHelper::rotatedTileIdToTileId(const int viewTileX,
                                        const int viewTileY,
                                        int& worldTileX, int& worldTileY,
                                        const eWorldDirection dir,
                                        const int boardWidth,
                                        const int boardHeight) {
    if(dir == eWorldDirection::N) {
        worldTileX = viewTileX;
        worldTileY = viewTileY;
        return;
    }
    int viewDTileX;
    int viewDTileY;
    tileIdToDTileId(viewTileX, viewTileY, viewDTileX, viewDTileY);
    int dTileX;
    int dTileY;
    rotatedDTileIdToDTileId(viewDTileX, viewDTileY, dTileX, dTileY,
                            dir, boardWidth, boardHeight);
    dtileIdToTileId(dTileX, dTileY, worldTileX, worldTileY);
}

SDL_Rect TileHelper::toRotatedRect(const SDL_Rect& rect,
                                    const eWorldDirection dir,
                                    const int boardWidth,
                                    const int boardHeight) {
    if(dir == eWorldDirection::N) {
        return rect;
    } else if(dir == eWorldDirection::E) {
        const int worldTileX = rect.x + rect.w - 1;
        const int worldTileY = rect.y;
        int viewTileX;
        int viewTileY;
        tileIdToRotatedTileId(worldTileX, worldTileY, viewTileX, viewTileY,
                              dir, boardWidth, boardHeight);
        return SDL_Rect{viewTileX, viewTileY, rect.h, rect.w};
    } else if(dir == eWorldDirection::S) {
        const int worldTileX = rect.x + rect.w - 1;
        const int worldTileY = rect.y + rect.h - 1;
        int viewTileX;
        int viewTileY;
        tileIdToRotatedTileId(worldTileX, worldTileY, viewTileX, viewTileY,
                              dir, boardWidth, boardHeight);
        return SDL_Rect{viewTileX, viewTileY, rect.w, rect.h};
    } else if(dir == eWorldDirection::W) {
        const int worldTileX = rect.x;
        const int worldTileY = rect.y + rect.h - 1;
        int viewTileX;
        int viewTileY;
        tileIdToRotatedTileId(worldTileX, worldTileY, viewTileX, viewTileY,
                              dir, boardWidth, boardHeight);
        return SDL_Rect{viewTileX, viewTileY, rect.h, rect.w};
    }
    return rect;
}
