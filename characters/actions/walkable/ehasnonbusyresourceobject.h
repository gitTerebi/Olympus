#ifndef EHASNONBUSYRESOURCEOBJECT_H
#define EHASNONBUSYRESOURCEOBJECT_H

#include "ehasresourceobject.h"

class eSaveArchive;

class eHasNonBusyResourceObject : public eHasResourceObject {
public:
    eHasNonBusyResourceObject(const stdsptr<eHasResourceObject>& other);
    eHasNonBusyResourceObject();

    bool has(eTileBase* const t) const override;

protected:
    void serializeFields(eSaveArchive& ar) override;
private:
    stdsptr<eHasResourceObject> mOther;
};

#endif // EHASNONBUSYRESOURCEOBJECT_H
