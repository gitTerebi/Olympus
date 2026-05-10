#include "egodreasoneventvalue.h"
#include "fileIO/esavearchive.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

void eGodReasonEventValue::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eGodReasonEventValue*>(this)->serialize(ar);
}

void eGodReasonEventValue::serialize(eSaveArchive& ar) {
    ar.field("mGodReason", mGodReason);
}

void eGodReasonEventValue::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}
