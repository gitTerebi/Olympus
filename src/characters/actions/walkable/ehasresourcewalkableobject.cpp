#include "ehasresourcewalkableobject.h"

#include "ehasresourceobject.h"
#include "fileIO/esavearchive.h"

eHasResourceWalkableObject::eHasResourceWalkableObject(
        const stdsptr<eHasResourceObject>& hr,
        const stdsptr<eWalkableObject>& w) :
    eWalkableObject(eWalkableObjectType::hasResource),
    mHr(hr), mW(w) {}

eHasResourceWalkableObject::eHasResourceWalkableObject() :
    eWalkableObject(eWalkableObjectType::hasResource) {}

bool eHasResourceWalkableObject::walkable(eTileBase* const t) const {
    return mHr->has(t) || mW->walkable(t);
}

void eHasResourceWalkableObject::serializeFields(eSaveArchive& ar) {
    ar.hasResourceField("hasResource", mHr);
    ar.walkableField("walkable", mW);
}
