#ifndef EWORLDREGION_H
#define EWORLDREGION_H

#include <string>

#include "fileIO/estreams.h"
#include "fileIO/esavearchive.h"
#include "fileIO/ejsonarchive.h"

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
        eSaveArchive ar(src);
        ar.field("name", fName);
        ar.field("nameId", fNameId);
        ar.field("x", fX);
        ar.field("y", fY);
    }

    void write(eWriteStream& dst) const {
        eSaveArchive ar(dst);
        ar.field("name", const_cast<std::string&>(fName));
        ar.field("nameId", const_cast<int&>(fNameId));
        ar.field("x", const_cast<double&>(fX));
        ar.field("y", const_cast<double&>(fY));
    }

    void serializeJson(eJsonArchive& ar) {
        ar.field("name",   fName);
        ar.field("nameId", fNameId);
        ar.field("x",      fX);
        ar.field("y",      fY);
    }
};

#endif // EWORLDREGION_H
