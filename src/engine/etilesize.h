#ifndef ETILESIZE_H
#define ETILESIZE_H

// Only one tile size ships: 60x30 px tiles (s30).
// Historical 4-size support (s15/s30/s45/s60) was removed;
// legacy savegames stored s30 as ordinal 1 (handled in
// GameWidgetSettings::serialize).
enum class eTileSize : int {
    s30
};

#endif // ETILESIZE_H
