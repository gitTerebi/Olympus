#include "emonstereventvalue.h"
#include "fileIO/esavearchive.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

void eMonsterEventValue::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eMonsterEventValue*>(this)->serialize(ar);
}

void eMonsterEventValue::serialize(eSaveArchive& ar) {
    ar.value(mMonster);
}

void eMonsterEventValue::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}
