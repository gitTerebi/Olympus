#include "egodeventvalue.h"
#include "fileIO/esavearchive.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

#include "estringhelpers.h"

#include "characters/gods/god.h"

void eGodEventValue::serialize(eSaveArchive& ar) {
    ar.field("god", mGod, GodType::zeus);
}

void eGodEventValue::longNameReplaceGod(
    const std::string& id, std::string& tmpl) const {
    const auto gtstr = God::sGodName(mGod);
    eStringHelpers::replace(tmpl, id, gtstr);
}
