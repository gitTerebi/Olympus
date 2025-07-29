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
#ifndef GAMEFILE_H
#define GAMEFILE_H

#include "qfile.h"

#include "grid.h"

class GameFile {
public:
    GameFile(const std::string &file);

    bool isOk() const { return ok; }

    Grid<uint8_t> *readByteGrid();
    Grid<uint16_t> *readShortGrid();
    Grid<uint32_t> *readIntGrid();

    Grid<uint8_t> *readCompressedByteGrid();
    Grid<uint16_t> *readCompressedShortGrid();
    Grid<uint32_t> *readCompressedIntGrid();

    int64_t pos();

    void seek(const int64_t pos);
    void skipBytes(const int bytes);
    void skipCompressed();

    uint8_t readUByte();
    uint16_t readUShort();
    uint32_t readUInt();

    int16_t readShort();

    void getDiamondBitmapCoordinates(int x, int y, int mapsize,
        int *x_out, int *y_out);
    void getBitmapCoordinates(int x, int y, int mapsize,
        int *x_out, int *y_out);

    bool searchPattern(const uint8_t pattern[], int length);

protected:
    QFile in;
    int MAX_MAPSIZE;
    bool ok = true;
};

#endif /* GAMEFILE_H */
