#ifndef EWORLDREGION_H
#define EWORLDREGION_H

#include <string>

#include "fileIO/streams.h"
#include "fileIO/save-archive.h"

#include "language.h"

struct eWorldRegion {
    std::string fName;
    int fNameId;
    double fX;
    double fY;

    std::string getName() const {
        if(fNameId < 0 || fNameId > 20) return fName;
        return Language::zeusText(196, fNameId);
    }

    void serialize(SaveArchive& ar) {
        ar.field("name", fName);
        ar.field("nameId", fNameId);
        ar.field("x", fX);
        ar.field("y", fY);
    }
};

#endif // EWORLDREGION_H
