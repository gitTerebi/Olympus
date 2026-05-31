#ifndef ELAVATODRY_H
#define ELAVATODRY_H

class eTile;
enum class eWorldDirection;

namespace eLavaToDry {
    int get(eTile* const tile, const eWorldDirection dir);
};

#endif // ELAVATODRY_H
