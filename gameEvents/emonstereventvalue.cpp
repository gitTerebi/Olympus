#include "emonstereventvalue.h"

#include "fileIO/ewritestream.h"
#include "fileIO/ereadstream.h"

void eMonsterEventValue::write(eWriteStream& dst) const {
    dst << mMonster;
}

void eMonsterEventValue::read(eReadStream& src) {
    src >> mMonster;
}
