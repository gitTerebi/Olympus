#include "etileterrainpainter.h"

#include "etexturecollection.h"

stdsptr<eTexture> eTileTerrainPainter::getTexture(const int frame) const {
    if(const auto coll = fColl) {
        const int ff = frame/20;
        const int id = ff % coll->size();
        return fColl->getTexture(id);
    } else {
        return fTex;
    }
}
