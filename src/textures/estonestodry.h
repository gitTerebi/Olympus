#ifndef ESTONESTODRY_H
#define ESTONESTODRY_H

class eTile;
enum class eWorldDirection;

namespace eStonesToDry {
    void get(eTile* const tile,
             int& drawDim,
             const eWorldDirection dir);
};

#endif // ESTONESTODRY_H
