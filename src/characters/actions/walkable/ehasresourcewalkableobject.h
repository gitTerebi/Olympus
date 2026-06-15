#ifndef EHASRESOURCEWALKABLEOBJECT_H
#define EHASRESOURCEWALKABLEOBJECT_H

#include "walkable-object.h"

class SaveArchive;

class eHasResourceWalkableObject : public WalkableObject {
public:
    eHasResourceWalkableObject(const stdsptr<eHasResourceObject>& hr,
                               const stdsptr<WalkableObject>& w);
    eHasResourceWalkableObject();

    bool walkable(eTileBase* const t) const override;

protected:
    void serializeFields(SaveArchive& ar) override;
private:
    stdsptr<eHasResourceObject> mHr;
    stdsptr<WalkableObject> mW;
};

#endif // EHASRESOURCEWALKABLEOBJECT_H
