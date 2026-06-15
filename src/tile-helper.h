#ifndef TILE_HELPER_H
#define TILE_HELPER_H

class GameBoard;
class eTile;
enum class eWorldDirection;
struct SDL_Rect;

namespace TileHelper {
    void rotatedDTileIdToDTileId(const int viewDTileX, const int viewDTileY,
                                 int& dTileX, int& dTileY,
                                 const eWorldDirection dir,
                                 const int boardWidth, const int boardHeight);
    void dTileIdToRotatedDTileId(const int dTileX, const int dTileY,
                                 int& viewDTileX, int& viewDTileY,
                                 const eWorldDirection dir,
                                 const int boardWidth, const int boardHeight);
    void dtileIdToTileId(const int dTileX, const int dTileY,
                         int& worldTileX, int& worldTileY);
    void tileIdToDTileId(const int worldTileX, const int worldTileY,
                         int& dTileX, int& dTileY);
    void tileIdToRotatedTileId(const int worldTileX, const int worldTileY,
                               int& viewTileX, int& viewTileY,
                               const eWorldDirection dir,
                               const int boardWidth, const int boardHeight);
    void rotatedTileIdToTileId(const int viewTileX, const int viewTileY,
                               int& worldTileX, int& worldTileY,
                               const eWorldDirection dir,
                               const int boardWidth, const int boardHeight);

    eTile* closestRoad(const int roadWorldTileX, const int roadWorldTileY,
                       GameBoard& board, const int minLen = 1);

    SDL_Rect toRotatedRect(const SDL_Rect& rect,
                           const eWorldDirection dir,
                           const int boardWidth, const int boardHeight);
};

#endif // TILE_HELPER_H
