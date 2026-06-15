#ifndef PARK_TEXTURE_H
#define PARK_TEXTURE_H

class eTile;
enum class eWorldDirection;

namespace ParkTexture {
    void get(eTile* const tile,
             int& drawDim,
             const eWorldDirection dir);
};

#endif // PARK_TEXTURE_H
