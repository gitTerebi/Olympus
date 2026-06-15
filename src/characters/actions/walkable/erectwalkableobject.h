#ifndef ERECTWALKABLEOBJECT_H
#define ERECTWALKABLEOBJECT_H

#include "walkable-object.h"

class SaveArchive;

class eRectWalkableObject : public WalkableObject {
public:
    eRectWalkableObject(const stdsptr<WalkableObject>& other,
                        const SDL_Rect& rect);
    eRectWalkableObject(const SDL_Rect& rect);
    eRectWalkableObject();

    bool walkable(eTileBase* const t) const override;
    eWalkableObjectType rootType() const override;
protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdsptr<WalkableObject> mOther;
    SDL_Rect mRect;
};

#endif // ERECTWALKABLEOBJECT_H
