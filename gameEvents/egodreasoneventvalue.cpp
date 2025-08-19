#include "egodreasoneventvalue.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

void eGodReasonEventValue::write(eWriteStream& dst) const {
    dst << mGodReason;
}

void eGodReasonEventValue::read(eReadStream& src) {
    src >> mGodReason;
}
