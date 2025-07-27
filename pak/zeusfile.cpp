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

ZeusFile::ZeusFile(const std::string &filename)
	: GameFile(filename) {
	MAX_MAPSIZE = 228;
	MAX_WALKERS = 2000;
	MAX_BUILDINGS = 4000;
	retrievedMaps = numMaps = 0;
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

bool ZeusFile::loadBoard(eGameBoard& board) {
    if(retrievedMaps >= numMaps) {
        return false;
	}
	
	ok = true;
	
    Grid<uint32_t> *terrain = NULL;
    Grid<uint8_t> *edges = NULL, *random = NULL, *fertile = NULL,
                  *scrub = NULL, *elevation = NULL;
	
	int mapsize;
	bool is_poseidon = false;

    in.seek(positions[retrievedMaps]);

    retrievedMaps++;
		
    // Read scenario info
    skipBytes(0x1778);
    skipCompressed(); // buildings grid: there are no placable buildings
    edges = readCompressedByteGrid();
    terrain = readCompressedIntGrid();
    skipCompressed(); // byte grid: 00 or 20
    readInt(); // indicating start of random block (or perhaps "uncompressed" indicator?)
    random = readByteGrid();
    skipCompressed(); // byte grid: all zeroes
    skipBytes(60);
    mapsize = readInt(); // Poseidon or not doesn't matter here
    skipBytes(1984);
    fertile = readCompressedByteGrid(); // meadow, 0-99
    skipBytes(18628);
    skipCompressed(); // not of proper length: 14400
    skipCompressed(); // not of proper length: 75168
    skipCompressed(); // byte grid: all ff's (counterpart of marble grid in sav?
    skipCompressed(); // 36 bytes
    skipBytes(144);
    scrub = readCompressedByteGrid();
    elevation = readCompressedByteGrid();

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
	int half = MAX_MAPSIZE / 2;
	int border = (MAX_MAPSIZE - mapsize) / 2;
	int max = border + mapsize;
	int start, end;
    uint8_t t_random, t_meadow, t_scrub, t_elevation;
    uint32_t t_terrain;

    const int shift = mapsize/4;
    board.initialize(mapsize/2 - 1, mapsize);

    for (int y = border; y < max; y++) {
        start = (y < half) ? (border + half - y - 1) : (border + y - half);
        end = (y < half) ? (half + y + 1 - border) : (3*half - y - border);
        for (int x = start; x < end; x++) {
            t_terrain  = terrain->get(x, y);
			t_random = random->get(x, y);
			t_meadow = fertile->get(x, y);
			t_scrub = scrub->get(x, y);
            t_elevation = elevation->get(x, y);

            const int dy = 1 + x + y - mapsize / 2;
            const int dx = mapsize / 2 + (x - y + (dy % 2 ? 0 :  1))/2 - 2 - shift;
            const auto tile = board.dtile(dx, dy);
            if(!tile) continue;
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
            } else if(t_terrain & 0x40) { // road
                const auto cid = tile->cityId();
                const auto road = e::make_shared<eRoad>(board, cid);
                tile->setUnderBuilding(road);
                road->addUnderBuilding(tile);
                road->setCenterTile(tile);
                int x;
                int y;
                eTileHelper::dtileIdToTileId(dx, dy, x, y);
                road->setTileRect({x, y, 1, 1});
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
            } else if(t_terrain & 0x1000000) { // molten lava
            } else { // empty land
                tile->setScrub(0.01*t_scrub);
            }
		}
	}
    if(terrain) delete terrain;
    if(random) delete random;
    if(fertile) delete fertile;
    if(scrub) delete scrub;
    if(elevation) delete elevation;
    if(edges) delete edges;
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
	int mapsize = readInt();
	
	return mapsize;
}

