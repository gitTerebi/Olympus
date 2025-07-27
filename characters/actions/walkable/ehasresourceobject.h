#ifndef EHASRESOURCEOBJECT_H
#define EHASRESOURCEOBJECT_H

#include "fileIO/estreams.h"
#include "engine/etilebase.h"

enum class eHasResourceObjectType {
    nonBusy,
    fish,
    urchin,
    silver,
    copper,
    orichalc,
    forest,
    marble,
    blackMarble
};

class eHasResourceObject {
public:
    eHasResourceObject(const eHasResourceObjectType t) :
        mType(t) {}

    virtual bool has(eTileBase* const t) const {
        switch(mType) {
        case eHasResourceObjectType::fish:
            return t->hasFish();
        case eHasResourceObjectType::urchin:
            return t->hasUrchin();
        case eHasResourceObjectType::silver:
            return t->terrain() == eTerrain::silver;
        case eHasResourceObjectType::copper:
            return t->terrain() == eTerrain::copper;
        case eHasResourceObjectType::orichalc:
            return t->terrain() == eTerrain::orichalc;
        case eHasResourceObjectType::forest:
            return t->terrain() == eTerrain::forest;
        case eHasResourceObjectType::marble: {
            const int r = t->resource();
            if(r <= 0) return false;
            return t->terrain() == eTerrain::marble;
        } break;
        case eHasResourceObjectType::blackMarble: {
            const int r = t->resource();
            if(r <= 0) return false;
            return t->terrain() == eTerrain::blackMarble;
        } break;
        case eHasResourceObjectType::nonBusy:
            return false;
        }
    }

    virtual void read(eReadStream&) {}
    virtual void write(eWriteStream&) const {}

    eHasResourceObjectType type() const { return mType; }

    static stdsptr<eHasResourceObject> sCreate(
            const eHasResourceObjectType type);

    static stdsptr<eHasResourceObject> sCreateNonBusy(
            const stdsptr<eHasResourceObject>& other);
private:
    const eHasResourceObjectType mType;
};

#endif // EHASRESOURCEOBJECT_H
