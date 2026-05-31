#include "egodreasoneventvalue.h"
#include "fileIO/esavearchive.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

void eGodReasonEventValue::serialize(eSaveArchive& ar) {
    ar.field("godReason", mGodReason, false);
}
