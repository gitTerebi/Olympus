#include "egodreasoneventvalue.h"
#include "fileIO/save-archive.h"

#include "fileIO/write-stream.h"
#include "fileIO/read-stream.h"

void eGodReasonEventValue::serialize(SaveArchive& ar) {
    ar.field("godReason", mGodReason, false);
}
