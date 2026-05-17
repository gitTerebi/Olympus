#include "ehasnonbusyresourceobject.h"

#include "engine/etilebase.h"
#include "fileIO/esavearchive.h"

eHasNonBusyResourceObject::eHasNonBusyResourceObject(
        const stdsptr<eHasResourceObject>& other) :
    eHasResourceObject(eHasResourceObjectType::nonBusy),
    mOther(other) {}

eHasNonBusyResourceObject::eHasNonBusyResourceObject() :
    eHasResourceObject(eHasResourceObjectType::nonBusy) {}

bool eHasNonBusyResourceObject::has(eTileBase* const t) const {
    return !t->busy() && mOther->has(t);
}

void eHasNonBusyResourceObject::read(eReadStream& src) {
    eSaveArchive ar(src);
    serialize(ar);
}

void eHasNonBusyResourceObject::write(eWriteStream& dst) const {
    eSaveArchive ar(dst);
    const_cast<eHasNonBusyResourceObject*>(this)->serialize(ar);
}

void eHasNonBusyResourceObject::serialize(eSaveArchive& ar) {
    ar.hasResourceField("other", mOther);
}
