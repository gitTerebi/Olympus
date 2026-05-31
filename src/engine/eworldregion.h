#ifndef EWORLDREGION_H
#define EWORLDREGION_H

#include <string>

#include "fileIO/estreams.h"
#include "fileIO/esavearchive.h"

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

    void serialize(eSaveArchive& ar) {
        ar.field("name", fName);
        ar.field("nameId", fNameId);
        ar.field("x", fX);
        ar.field("y", fY);
    }
};

#endif // EWORLDREGION_H
