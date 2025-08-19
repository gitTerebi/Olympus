#include "egodeventvalue.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

#include "estringhelpers.h"

#include "characters/gods/egod.h"

void eGodEventValue::write(eWriteStream& dst) const {
    dst << mGod;
}

void eGodEventValue::read(eReadStream& src) {
    src >> mGod;
}

void eGodEventValue::longNameReplaceGod(
    const std::string& id, std::string& tmpl) const {
    const auto gtstr = eGod::sGodName(mGod);
    eStringHelpers::replace(tmpl, id, gtstr);
}
