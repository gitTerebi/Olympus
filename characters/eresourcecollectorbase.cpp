#include "eresourcecollectorbase.h"
#include "fileIO/esavearchive.h"

void eResourceCollectorBase::read(eReadStream& src) {
    eCharacter::read(src);
    eSaveArchive ar(src);
    serialize(ar);
}

void eResourceCollectorBase::write(eWriteStream& dst) const {
    eCharacter::write(dst);
    eSaveArchive ar(dst);
    const_cast<eResourceCollectorBase*>(this)->serialize(ar);
}

void eResourceCollectorBase::serialize(eSaveArchive& ar) {
    ar.field("collectedResourceCount", mResCollected, 0);
}
