#ifndef EHASNONBUSYRESOURCEOBJECT_H
#define EHASNONBUSYRESOURCEOBJECT_H

#include "ehasresourceobject.h"

class eSaveArchive;

class eHasNonBusyResourceObject : public eHasResourceObject {
public:
    eHasNonBusyResourceObject(const stdsptr<eHasResourceObject>& other);
    eHasNonBusyResourceObject();

    bool has(eTileBase* const t) const override;

    void read(eReadStream& src) override;
    void write(eWriteStream& dst) const override;
private:
    void serialize(eSaveArchive& ar);

    stdsptr<eHasResourceObject> mOther;
};

#endif // EHASNONBUSYRESOURCEOBJECT_H
