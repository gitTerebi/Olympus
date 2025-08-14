#ifndef EWORLDREGION_H
#define EWORLDREGION_H

#include <string>

#include "fileIO/estreams.h"

#include "elanguage.h"

struct eWorldRegion {
    std::string fName;
    int fNameId;
    double fX;
    double fY;

    std::string getName() const {
        if(fNameId < 0 || fNameId > 20) return fName;
        return eLanguage::zeusText(196, fNameId);
    }

    void read(eReadStream& src) {
        src >> fName;
        src >> fNameId;
        src >> fX;
        src >> fY;
    }

    void write(eWriteStream& dst) const {
        dst << fName;
        dst << fNameId;
        dst << fX;
        dst << fY;
    }
};

#endif // EWORLDREGION_H
