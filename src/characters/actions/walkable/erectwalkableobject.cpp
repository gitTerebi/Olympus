#include "erectwalkableobject.h"

#include "engine/etilebase.h"
#include "fileIO/esavearchive.h"

eRectWalkableObject::eRectWalkableObject(
        const stdsptr<eWalkableObject>& other,
        const SDL_Rect& rect) :
    eWalkableObject(eWalkableObjectType::rect),
    mOther(other), mRect(rect) {}

eRectWalkableObject::eRectWalkableObject(const SDL_Rect& rect) :
    eRectWalkableObject(nullptr, rect) {}

eRectWalkableObject::eRectWalkableObject() :
    eWalkableObject(eWalkableObjectType::rect) {}

bool eRectWalkableObject::walkable(
        eTileBase* const t) const {
    const SDL_Point p{t->x(), t->y()};
    const bool r = SDL_PointInRect(&p, &mRect);
    if(r) return true;
    if(mOther) return mOther->walkable(t);
    return false;
}

eWalkableObjectType eRectWalkableObject::rootType() const {
    if(!mOther) return eWalkableObjectType::rect;
    return mOther->rootType();
}

void eRectWalkableObject::serializeFields(eSaveArchive& ar) {
    ar.field("rect", mRect);
    ar.walkableField("other", mOther);
}
