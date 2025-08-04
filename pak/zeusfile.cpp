/*
 *   Citybuilding Mappers - create minimaps from citybuilding game files
 *   Copyright (C) 2007, 2008  Bianca van Schaik
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "zeusfile.h"
#include "pkwareinputstream.h"

#include "buildings/eroad.h"
#include "etilehelper.h"

#include "spawners/eentrypoint.h"
#include "spawners/eexitpoint.h"
#include "spawners/elandinvasionpoint.h"
#include "spawners/eseainvasionpoint.h"
#include "spawners/edisembarkpoint.h"
#include "spawners/emonsterpoint.h"
#include "spawners/eboarspawner.h"
#include "spawners/edeerspawner.h"
#include "spawners/ewolfspawner.h"
#include "spawners/edisasterpoint.h"

ZeusFile::ZeusFile(const std::string &filename)
	: GameFile(filename) {
	MAX_MAPSIZE = 228;
    retrievedMaps = numMaps = 0;
}

void ZeusFile::readVersion() {
    in.seek(4);
    mNewVersion = readUByte() == 0x1a;
}

bool ZeusFile::isNewVersion() const {
    return mNewVersion;
}

void ZeusFile::readAtlantean() {
    seek(7788);
    mAtlantean = readUByte();
}

bool ZeusFile::isAtlantean() const {
    return mAtlantean;
}

int ZeusFile::getNumMaps() {
	// Check if file is open
    if(!in.isReadable()) {
		return 0;
	}
	
	char fourcc[5];
    in.read(fourcc, 4);
	fourcc[4] = 0;
	
    // Adventure, get positions of maps inside the file
    int i = 0;
    char pattern[] = "MAPS";
    // The first ~840000 bytes are the .pak and don't contain MAPS
    in.seek(800000);
    while (searchPattern((uint8_t*)pattern, 4)) {
        positions[i] = in.pos();
        i++;
    }
    numMaps = i;
    in.reset();

	return numMaps;
}

bool ZeusFile::loadBoard(eGameBoard& board, const eCityId cid) {
    if(retrievedMaps >= numMaps) {
        return false;
	}	

    in.seek(positions[retrievedMaps]);

    retrievedMaps++;

    // Read scenario info
    skipBytes(0x1778);
    skipCompressed(); // buildings grid: there are no placable buildings
    const auto edges = readCompressedByteGrid();
    const auto terrain = readCompressedIntGrid();
    skipCompressed(); // byte grid: 00 or 20
    readUInt(); // indicating start of random block (or perhaps "uncompressed" indicator?)
    const auto random = readByteGrid();
    skipCompressed(); // byte grid: all zeroes
    skipBytes(60);
    const int mapsize = readUInt(); // Poseidon or not doesn't matter here
    const int halfMapsize = mapsize/2;

    struct ePt {
        uint16_t fX;
        uint16_t fY;
    };

    skipBytes(604);

    const int maxWolfPts = 4;
    std::vector<ePt> wolfPts;
    wolfPts.resize(maxWolfPts);
    for(int i = 0; i < maxWolfPts; i++) {
        wolfPts[i].fX = readUShort();
    }
    for(int i = 0; i < maxWolfPts; i++) {
        wolfPts[i].fY = readUShort();
    }

    const int maxFishingPts = 8;
    std::vector<ePt> fishingPts;
    fishingPts.resize(maxFishingPts);
    for(int i = 0; i < maxFishingPts; i++) {
        fishingPts[i].fX = readUShort();
    }
    for(int i = 0; i < maxFishingPts; i++) {
        fishingPts[i].fY = readUShort();
    }

    const int maxUrchinPts = 8;
    std::vector<ePt> urchinPts;
    urchinPts.resize(maxUrchinPts);
    for(int i = 0; i < maxUrchinPts; i++) {
        urchinPts[i].fX = readUShort();
    }
    for(int i = 0; i < maxUrchinPts; i++) {
        urchinPts[i].fY = readUShort();
    }

    skipBytes(12);

    const int maxLandInvPts = 8;
    std::vector<ePt> landInvPts;
    landInvPts.resize(maxLandInvPts);
    for(int i = 0; i < maxLandInvPts; i++) {
        landInvPts[i].fX = readUShort();
    }

    const int maxSeaInvPts = 8;
    std::vector<ePt> seaInvPts;
    seaInvPts.resize(maxSeaInvPts);
    for(int i = 0; i < maxSeaInvPts; i++) {
        seaInvPts[i].fX = readUShort();
    }

    for(int i = 0; i < maxLandInvPts; i++) {
        landInvPts[i].fY = readUShort();
    }

    for(int i = 0; i < maxSeaInvPts; i++) {
        seaInvPts[i].fY = readUShort();
    }

    skipBytes(4);

    const int maxDeerPts = 4;
    std::vector<ePt> deerPts;
    deerPts.resize(maxDeerPts);
    for(int i = 0; i < maxDeerPts; i++) {
        deerPts[i].fX = readUShort();
    }
    for(int i = 0; i < maxDeerPts; i++) {
        deerPts[i].fY = readUShort();
    }

    skipBytes(80);

    const uint16_t entryPtX = readUShort();
    const uint16_t entryPtY = readUShort();
    const uint16_t exitPtX = readUShort();
    const uint16_t exitPtY = readUShort();

    const int maxDisasterPts = 8;
    std::vector<ePt> disasterPts;
    disasterPts.resize(maxDisasterPts);
    for(int i = 0; i < maxDisasterPts; i++) {
        disasterPts[i].fX = readUShort();
    }
    for(int i = 0; i < maxDisasterPts; i++) {
        disasterPts[i].fY = readUShort();
    }

    skipBytes(52);

    const int maxBoarPts = 4;
    std::vector<ePt> boarPts;
    boarPts.resize(maxBoarPts);
    for(int i = 0; i < maxBoarPts; i++) {
        boarPts[i].fX = readUShort();
        skipBytes(2);
    }
    for(int i = 0; i < maxBoarPts; i++) {
        boarPts[i].fY = readUShort();
        skipBytes(2);
    }

    skipBytes(216);

    const int maxMonsterInvPts = 3;
    std::vector<ePt> monsterInvPts;
    monsterInvPts.resize(maxMonsterInvPts);
    for(int i = 0; i < maxMonsterInvPts; i++) {
        monsterInvPts[i].fX = readUShort();
        skipBytes(2);
    }
    for(int i = 0; i < maxMonsterInvPts; i++) {
        monsterInvPts[i].fY = readUShort();
        skipBytes(2);
    }

    const int maxDisembarkPts = 3;
    std::vector<ePt> disembarkPts;
    disembarkPts.resize(maxDisembarkPts);
    for(int i = 0; i < maxDisembarkPts; i++) {
        disembarkPts[i].fX = readUShort();
        skipBytes(2);
    }
    for(int i = 0; i < maxDisembarkPts; i++) {
        disembarkPts[i].fY = readUShort();
        skipBytes(2);
    }

    skipBytes(736);
    const auto fertile = readCompressedByteGrid(); // meadow, 0-99
    skipBytes(18628);
    skipCompressed(); // not of proper length: 14400
    skipCompressed(); // not of proper length: 75168
    skipCompressed(); // byte grid: all ff's (counterpart of marble grid in sav?
    skipCompressed(); // 36 bytes
    skipBytes(144);
    const auto scrub = readCompressedByteGrid();
    const auto elevation = readCompressedByteGrid();

	// Extra sanity check
    if(!ok || mapsize > MAX_MAPSIZE) {
        if(terrain) delete terrain;
        if(edges) delete edges;
        if(random) delete random;
        if(fertile) delete fertile;
        if(scrub) delete scrub;
        if(elevation) delete elevation;
		
        return false;
	}

	// Transform it to something useful
    const int half = MAX_MAPSIZE / 2;
    const int border = (MAX_MAPSIZE - mapsize) / 2;
    const int max = border + mapsize;

    const int shift = mapsize/4;
    int boardW = mapsize/2;
    if(boardW % 2) boardW++;
    int boardH = mapsize + 1;
    if(boardH % 2) boardH++;
    board.initialize(boardW, boardH);
    board.assignAllTerritory(cid);

    struct eNullTile {
        eTile* fTile = nullptr;
    };

    std::map<int, std::map<int, eNullTile>> tileMap;
    std::vector<eTile*> halfSlopes;

    struct eZeroInt {
        int fV = 0;
    };

    const auto buildRoad = [&](eTile* const tile) {
        const auto cid = tile->cityId();
        const auto road = e::make_shared<eRoad>(board, cid);
        tile->setUnderBuilding(road);
        road->addUnderBuilding(tile);
        road->setCenterTile(tile);
        const int dx = tile->dx();
        const int dy = tile->dy();
        int x;
        int y;
        eTileHelper::dtileIdToTileId(dx, dy, x, y);
        road->setTileRect({x, y, 1, 1});
    };

    for(int y = border; y < max; y++) {
        const int start = (y < half) ? (border + half - y - 1) : (border + y - half);
        const int end = (y < half) ? (half + y + 1 - border) : (3*half - y - border);
        for(int x = start; x < end; x++) {
            const uint32_t t_terrain = terrain->get(x, y);
            const uint8_t t_random = random->get(x, y);
            const uint8_t t_meadow = fertile->get(x, y);
            const uint8_t t_scrub = scrub->get(x, y);
            const uint8_t t_elevation = elevation->get(x, y);

            const int dy = 2 + x + y - halfMapsize - (halfMapsize % 2 ? 0 : 1) - 2*border;
            const int dx = mapsize / 2 + (x - y + (dy % 2 ? 0 : 1))/2 - 1 - shift;

            const auto tile = board.dtile(dx, dy);

            tileMap[y - border][x - border].fTile = tile;

            if(!tile) continue;
            tile->setRainforest(mAtlantean);
            tile->setAltitude(t_elevation);

            if(t_terrain & 0x1) {
                tile->setTerrain(eTerrain::forest);
            } else if(t_terrain & 0x2) {
                if((t_terrain & 0x300002) == 0x100002) { // copper ore
                    tile->setTerrain(eTerrain::copper);
                } else if((t_terrain & 0x300002) == 0x200002) { // silver ore
                    tile->setTerrain(eTerrain::silver);
                } else { // normal (0x2) or cliff rock (0x300002)
                    // or black marble quarry (0x120000) ??
                    tile->setTerrain(eTerrain::flatStones);
                }
            } else if(t_terrain & 0x10000000 && (!(t_terrain & 0x8) || t_terrain & 0x40)) {
                // sanctuary or pyramid
            } else if(t_terrain & 0x8) { // building, fill in for boulevard or avenue
            } else if(t_terrain & 0x20) { // park
            } else if(t_terrain & 0x200) { // elevation
                if(t_terrain == 1600 ||
                   t_terrain == 2147485248) {
                    tile->setWalkableElev(true);
                    buildRoad(tile);
                } else if(t_terrain == 1536) {
                    tile->setWalkableElev(true);
                } else if(t_terrain == 2155873856) {
                    halfSlopes.push_back(tile);
                    tile->setWalkableElev(true);
                    buildRoad(tile);
                } else if(t_terrain == 8390144 ||
                          t_terrain == 8390208 ||
                          t_terrain == 2155873792) {
                    halfSlopes.push_back(tile);
                    tile->setWalkableElev(true);
                } else if(t_terrain == 2155872768 ||
                          t_terrain == 8389120) {
                    halfSlopes.push_back(tile);
                }
            } else if(t_terrain & 0x40) { // road
                buildRoad(tile);
            } else if(t_terrain & 0x4) { // water
                if(t_terrain & 0x4000000) { // deep water
                } else { // shallow water
                }
                tile->setTerrain(eTerrain::water);
            } else if(t_terrain & 0x20000) { // marble quarry
                if(t_terrain & 0x100000) { // black marble
                } else { // normal marble
                    tile->setTerrain(eTerrain::marble);
                }
            } else if((t_terrain & 0x300000) == 0x300000) { // orichalc
                tile->setTerrain(eTerrain::orichalc);
            } else if(t_terrain & 0x4000) { // wall
            } else if(t_terrain & 0x800) { // meadow
                tile->setTerrain(eTerrain::fertile);
                tile->setScrub(0.01*t_scrub);
            } else if(t_terrain & 0x80) { // beach / beach edge / scrub
                if(t_terrain & 0x10000) { // beach sand
                    tile->setTerrain(eTerrain::beach);
                } else { // beach edge OR scrub
                    if(t_scrub <= 0x18) {
                    } else if(t_scrub <= 0x38) {
                    } else if(t_scrub <= 0x48) {
                    } else if(t_scrub <= 0x50) {
                    } else {
                    }
                    tile->setScrub(0.01*t_scrub);
                }
            } else if(t_terrain & 0x40000) { // marshland
                tile->setTerrain(eTerrain::marsh);
            } else if(t_terrain & 0x1000000) { // molten lava
            } else { // empty land
                tile->setScrub(0.01*t_scrub);
            }
		}
	}

    for(const auto tile : halfSlopes) {
        const bool w = tile->walkableElev();
        const int a = tile->altitude();
        const auto ns = tile->diagonalNeighbours(nullptr);
        for(const auto& n : ns) {
            const auto ntile = static_cast<eTile*>(n.second);
            if(w && !tile->hasRoad() && ntile->hasRoad()) buildRoad(tile);
            const int na = ntile->altitude();
            if(na > a) tile->setDoubleAltitude(2*na - 1);
        }
    }

    if(terrain) delete terrain;
    if(random) delete random;
    if(fertile) delete fertile;
    if(scrub) delete scrub;
    if(elevation) delete elevation;
    if(edges) delete edges;

    const auto entryTile = tileMap[entryPtY][entryPtX].fTile;
    if(entryTile) {
        const auto b = std::make_shared<eEntryPoint>(
                           0, entryTile, board);
        entryTile->setBanner(b);
    }
    const auto exitTile = tileMap[exitPtY][exitPtX].fTile;
    if(exitTile) {
        const auto b = std::make_shared<eExitPoint>(
                           0, exitTile, board);
        exitTile->setBanner(b);
    }

    for(int i = 0; i < maxDisasterPts; i++) {
        const auto& pt = disasterPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        const auto b = std::make_shared<eDisasterPoint>(
                           i, tile, board);
        tile->setBanner(b);
    }

    for(int i = 0; i < maxLandInvPts; i++) {
        const auto& pt = landInvPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        const auto b = std::make_shared<eLandInvasionPoint>(
                           i, tile, board);
        tile->setBanner(b);
    }

    for(int i = 0; i < maxSeaInvPts; i++) {
        const auto& pt = seaInvPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        const auto b = std::make_shared<eSeaInvasionPoint>(
                           i + 8, tile, board);
        tile->setBanner(b);
    }

    for(int i = 0; i < maxWolfPts; i++) {
        const auto& pt = wolfPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        const auto b = std::make_shared<eWolfSpawner>(
                           i, tile, board);
        tile->setBanner(b);
    }

    for(int i = 0; i < maxDeerPts; i++) {
        const auto& pt = deerPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        const auto b = std::make_shared<eDeerSpawner>(
                           i, tile, board);
        tile->setBanner(b);
    }

    for(int i = 0; i < maxBoarPts; i++) {
        const auto& pt = boarPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        const auto b = std::make_shared<eBoarSpawner>(
                           i, tile, board);
        tile->setBanner(b);
    }

    for(int i = 0; i < maxMonsterInvPts; i++) {
        const auto& pt = monsterInvPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        const auto b = std::make_shared<eMonsterPoint>(
                           i, tile, board);
        tile->setBanner(b);
    }

    for(int i = 0; i < maxDisembarkPts; i++) {
        const auto& pt = disembarkPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        const auto b = std::make_shared<eDisembarkPoint>(
                           i, tile, board);
        tile->setBanner(b);
    }

    for(int i = 0; i < maxFishingPts; i++) {
        const auto& pt = fishingPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        tile->setHasFish(true);
    }

    for(int i = 0; i < maxUrchinPts; i++) {
        const auto& pt = urchinPts[i];
        const auto tile = tileMap[pt.fY][pt.fX].fTile;
        if(!tile) continue;
        tile->setHasUrchin(true);
    }

    for(int x = 0; x < board.width(); x++) {
        const auto tile = board.dtile(x, 0);
        const auto b = tile->bottom<eTile>();
        tile->setTerrain(b->terrain());
        tile->setScrub(b->scrub());
    }

    for(int y = 1; y < board.height(); y += 2) {
        const auto tile = board.dtile(board.width() - 1, y);
        const auto b = tile->left<eTile>();
        tile->setTerrain(b->terrain());
        tile->setScrub(b->scrub());
    }

    // Fix for the Odyssey 1. colony
    std::vector<eTile*> tiles;
    board.iterateOverAllTiles([&](eTile* const tile) {
        const auto terr = tile->terrain();
        if(terr != eTerrain::dry) return;
        const auto tr = tile->topRight<eTile>();
        const auto br = tile->bottomRight<eTile>();
        const auto bl = tile->bottomLeft<eTile>();
        const auto tl = tile->topLeft<eTile>();
        const auto trt = tr ? tr->terrain() : terr;
        const auto brt = br ? br->terrain() : terr;
        const auto blt = bl ? bl->terrain() : terr;
        const auto tlt = tl ? tl->terrain() : terr;
        int n = 0;
        if(trt == eTerrain::beach) n++;
        if(brt == eTerrain::beach) n++;
        if(blt == eTerrain::beach) n++;
        if(tlt == eTerrain::beach) n++;
        if(n > 1) {
            tiles.push_back(tile);
        }
    });
    for(const auto tile : tiles) {
        tile->setTerrain(eTerrain::beach);
    }

    return true;
}

/**
* Gets the map size from the saved game, which is
* buried deep into the game file
*/
int ZeusFile::getMapsize() {
	// Assume walkers have been read already
	// Three compressed blocks
	for (int i = 0; i < 3; i++) {
		skipCompressed();
	}
	
	// Three ints
	skipBytes(12);
	
	// Compressed block, followed by 72 bytes, followed by compressed
	skipCompressed();
	skipBytes(72);
	skipCompressed();
	
	// 68 bytes, followed by one compressed
	skipBytes(68);
	skipCompressed();
	
	// Start of data copied from the .map file!
	// 704 bytes to skip
	skipBytes(704);
	// Next int = map size
	int mapsize = readUInt();
	
	return mapsize;
}

