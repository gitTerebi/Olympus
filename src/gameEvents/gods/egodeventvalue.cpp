#include "egodeventvalue.h"
#include "fileIO/save-archive.h"

#include "fileIO/write-stream.h"
#include "fileIO/read-stream.h"

#include "string-helpers.h"

#include "characters/gods/god.h"

void eGodEventValue::serialize(SaveArchive& ar) {
    ar.field("god", mGod, GodType::zeus);
}

void eGodEventValue::longNameReplaceGod(
    const std::string& id, std::string& tmpl) const {
    const auto gtstr = God::sGodName(mGod);
    StringHelpers::replace(tmpl, id, gtstr);
}
