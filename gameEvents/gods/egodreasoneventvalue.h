#ifndef EGODREASONEVENTVALUE_H
#define EGODREASONEVENTVALUE_H

class eWriteStream;
class eReadStream;

#include "fileIO/ejsonarchive.h"
class eSaveArchive;
class eGodReasonEventValue {
public:
    bool godReason() const { return mGodReason; }
    void setGodReason(const bool r) { mGodReason = r; }

    void write(eWriteStream& dst) const;
    void serialize(eSaveArchive& ar);
    void read(eReadStream& src);
    virtual void serializeJson(eJsonArchive& ar) {
        ar.field("mGodReason", mGodReason);
    }
protected:
    bool mGodReason = false;
};

#endif // EGODREASONEVENTVALUE_H
