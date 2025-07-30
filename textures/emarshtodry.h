#ifndef EMARSHTODRY_H
#define EMARSHTODRY_H

class eTile;
enum class eWorldDirection;

namespace eMarshToDry {
    int get(eTile* const tile, const eWorldDirection dir);
};

#endif // EMARSHTODRY_H
