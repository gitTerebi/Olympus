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
#include "gamefile.h"

#include "pkwareinputstream.h"

GameFile::GameFile(const std::string& filename) :
    in(filename) {}

/*** Reads an uncompressed byte grid from the stream.
*/
Grid<uint8_t> *GameFile::readByteGrid() {
	if (!ok) return NULL;
    if (in.atEnd()) {
		ok = false;
		return NULL;
	}
	
    Grid<uint8_t> *g = new Grid<uint8_t>(MAX_MAPSIZE, MAX_MAPSIZE);
	
	for (int y = 0; y < MAX_MAPSIZE; y++) {
		for (int x = 0; x < MAX_MAPSIZE; x++) {
            g->set(x, y, readUByte());
		}
	}
	return g;
}

/**
* Reads an uncompressed short grid from the stream.
*/
Grid<uint16_t> *GameFile::readShortGrid() {
	if (!ok) return NULL;
    if (in.atEnd()) {
		ok = false;
		return NULL;
	}
	
    Grid<uint16_t> *g = new Grid<uint16_t>(MAX_MAPSIZE, MAX_MAPSIZE);
	
	for (int y = 0; y < MAX_MAPSIZE; y++) {
		for (int x = 0; x < MAX_MAPSIZE; x++) {
            g->set(x, y, readUShort());
		}
	}
	return g;
}

/**
* Reads an uncompressed int grid from the stream.
*/
Grid<uint32_t> *GameFile::readIntGrid() {
	if (!ok) return NULL;
    if (in.atEnd()) {
		ok = false;
		return NULL;
	}
	
    Grid<uint32_t> *g = new Grid<uint32_t>(MAX_MAPSIZE, MAX_MAPSIZE);
	
	for (int y = 0; y < MAX_MAPSIZE; y++) {
		for (int x = 0; x < MAX_MAPSIZE; x++) {
            g->set(x, y, readUInt());
		}
	}
	return g;
}

/**
* Reads a compressed byte grid from the stream. Compressed chunks
* consist of a length followed by a compressed chunk of that length
*/
Grid<uint8_t> *GameFile::readCompressedByteGrid() {
	if (!ok) return NULL;
    if (in.atEnd()) {
		ok = false;
		return NULL;
	}
	
    int length = readUInt();
    Grid<uint8_t> *g = new Grid<uint8_t>(MAX_MAPSIZE, MAX_MAPSIZE);
	
    PKWareInputStream pk(&in, length, false);
	for (int y = 0; y < MAX_MAPSIZE; y++) {
		if (pk.hasError()) break;
		for (int x = 0; x < MAX_MAPSIZE; x++) {
			g->set(x, y, pk.readByte());
		}
	}
	pk.empty();
	if (pk.hasError()) {
		delete g;
		return NULL;
	}
	return g;
}

/**
* Reads a compressed short grid from the stream. Compressed chunks
* consist of a length followed by a compressed chunk of that length
*/
Grid<uint16_t> *GameFile::readCompressedShortGrid() {
	if (!ok) return NULL;
    if (in.atEnd()) {
		ok = false;
		return NULL;
	}
	
    int length = readUInt();
    Grid<uint16_t> *g = new Grid<uint16_t>(MAX_MAPSIZE, MAX_MAPSIZE);
	
    PKWareInputStream pk(&in, length, false);
	for (int y = 0; y < MAX_MAPSIZE; y++) {
		if (pk.hasError()) break;
		for (int x = 0; x < MAX_MAPSIZE; x++) {
			g->set(x, y, pk.readShort());
		}
	}
	pk.empty();
	
	if (pk.hasError()) {
		delete g;
		ok = false;
		return NULL;
	}
	return g;
}

/**
* Reads a compressed int grid from the stream. Compressed chunks
* consist of a length followed by a compressed chunk of that length
*/
Grid<uint32_t> *GameFile::readCompressedIntGrid() {
	if (!ok) return NULL;
    if (in.atEnd()) {
		ok = false;
		return NULL;
	}
	
    int length = readUInt();
    Grid<uint32_t> *g = new Grid<uint32_t>(MAX_MAPSIZE, MAX_MAPSIZE);
	
    PKWareInputStream pk(&in, length, false);
	for (int y = 0; y < MAX_MAPSIZE; y++) {
		if (pk.hasError()) break;
		for (int x = 0; x < MAX_MAPSIZE; x++) {
			g->set(x, y, pk.readInt());
		}
	}
	pk.empty();
	
	if (pk.hasError()) {
		delete g;
		ok = false;
		return NULL;
	}
    return g;
}

int64_t GameFile::pos() {
    return in.pos();
}

void GameFile::seek(const int64_t pos) {
    if (!ok) return;

    in.seek(pos);
}

/**
* Skips some bytes
*/
void GameFile::skipBytes(const int bytes) {
	if (!ok) return;
	
    in.seek(in.pos() + bytes);
}

/**
* Skips a compressed data block
*/
void GameFile::skipCompressed() {
	if (!ok) return;
	
    int skip = readUInt();
    in.seek(in.pos() + skip);
}

/**
* Reads a byte from the stream
*/
uint8_t GameFile::readUByte() {
	if (!ok) return 0;
	
	char data;
    if (!in.getChar(&data)) {
		ok = false;
		return 0;
	}
    return (uint8_t) data;
}

/**
* Reads a short from the stream
*/
uint16_t GameFile::readUShort() {
	if (!ok) return 0;
	
	char data[2];
    if (in.read(data, 2) != 2) {
		ok = false;
		return 0;
	}
    return (uint16_t)((uint8_t)data[0] | (((uint8_t)data[1]) << 8));
}

/**
* Reads an integer from the stream
*/
uint32_t GameFile::readUInt() {
	if (!ok) return 0;
	
	char data[4];
    uint32_t number = 0;
	
    if (in.read(data, 4) != 4) {
		ok = false;
		return 0;
	}
	
	for (int i = 0; i < 4; i++) {
        number |= ((uint8_t)data[i] << (i*8));
	}
    return number;
}

int16_t GameFile::readShort() {
    if (!ok) return 0;

    char data[2];
    if (in.read(data, 2) != 2) {
        ok = false;
        return 0;
    }
    return (int16_t)((int8_t)data[0] | (((int8_t)data[1]) << 8));
}

/**
* Returns the bitmap coordinates for a given map coordinate, for a
* diamond-shaped minimap (Caesar 3)
*/
void GameFile::getDiamondBitmapCoordinates(int x, int y, int mapsize,
		int *x_out, int *y_out) {
	*x_out = x + mapsize - y - 1;
	*y_out = x + y;
}

/**
* Returns the bitmap coordinates for a given map coordinate
*/
void GameFile::getBitmapCoordinates(int x, int y, int mapsize,
		int *x_out, int *y_out) {
	*x_out = mapsize / 2 + x - y - 1;
	*y_out = 1 + x + y - mapsize / 2;
}

/**
* Searches the stream for a pattern. Returns true if the pattern has been
* found; the file pointer is set to the first byte *after* the pattern.
* Returns false if the pattern wasn't found; the file pointer remains at the
* place where it was before the function call
* @param pattern Pattern to search for
* @param length Pattern length
*/
bool GameFile::searchPattern(const uint8_t pattern[], int length) {
    if (in.atEnd() || length <= 0 || length > 256) {
		return false;
	}
	int bad_char_skip[256];
	
	for (int i = 0; i < 256; i++) {
		bad_char_skip[i] = length;
	}
	
	int last = length - 1;
	for (int i = 0; i < last; i++) {
		bad_char_skip[pattern[i]] = last - i;
	}
	
    int startpos = in.pos();
	int returnpos = startpos;
	int bufsize = 16384;
    uint8_t buffer[16384];
    uint8_t *haystack;
	int buflength;
	do  {
        buflength = in.read((char*)buffer, bufsize);
		int hlen = buflength;
		haystack = buffer;
		
		while (hlen >= length) {
			/* scan from the end of the needle */
			for (int i = last; haystack[i] == pattern[i]; i--) {
				if (i == 0) { /* If the first byte matches, we've found it. */
					// Seek to just after the matched pattern
					startpos += buflength - hlen + length;
                    in.seek(startpos);
					return true;
				}
			}
			
			hlen     -= bad_char_skip[haystack[last]];
			haystack += bad_char_skip[haystack[last]];
		}
		
		// Leave "last" bytes for the next block: they might contain a match
		startpos += buflength - last;
        in.seek(startpos);
		
	} while (buflength == bufsize);
	
    in.seek(returnpos);
	return false;
}

