#ifndef STONES_TO_DRY_H
#define STONES_TO_DRY_H

class eTile;
enum class eWorldDirection;

namespace StonesToDry {
    void get(eTile* const tile,
             int& drawDim,
             const eWorldDirection dir);
};

#endif // STONES_TO_DRY_H
