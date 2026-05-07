#include "egodeventvalue.h"
#include "fileIO/esavearchive.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

#include "estringhelpers.h"

#include "characters/gods/egod.h"

void eGodEventValue::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eGodEventValue*>(this)->serialize(ar);
}

void eGodEventValue::serialize(eSaveArchive& ar) {
    ar.value(mGod);
}

void eGodEventValue::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eGodEventValue::longNameReplaceGod(
    const std::string& id, std::string& tmpl) const {
    const auto gtstr = eGod::sGodName(mGod);
    eStringHelpers::replace(tmpl, id, gtstr);
}
