#ifndef EHASRESOURCEWALKABLEOBJECT_H
#define EHASRESOURCEWALKABLEOBJECT_H

#include "ewalkableobject.h"

class eSaveArchive;

class eHasResourceWalkableObject : public eWalkableObject {
public:
    eHasResourceWalkableObject(const stdsptr<eHasResourceObject>& hr,
                               const stdsptr<eWalkableObject>& w);
    eHasResourceWalkableObject();

    bool walkable(eTileBase* const t) const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    stdsptr<eHasResourceObject> mHr;
    stdsptr<eWalkableObject> mW;
};

#endif // EHASRESOURCEWALKABLEOBJECT_H
