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

void eHasNonBusyResourceObject::serializeFields(eSaveArchive& ar) {
    ar.hasResourceField("other", mOther);
}
