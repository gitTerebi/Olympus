#ifndef EPAKHELPERS_H
#define EPAKHELPERS_H

#include <stdint.h>

enum class eResourceType;

namespace ePakHelpers {
    eResourceType pakResourceByteToType(
            const uint8_t byte, const bool newVersion);

    int pakBitmapIdConvert(const int id);
};

#endif // EPAKHELPERS_H
