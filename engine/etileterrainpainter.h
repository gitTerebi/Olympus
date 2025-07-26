#ifndef ETILETERRAINPAINTER_H
#define ETILETERRAINPAINTER_H

class eTexture;
class eTextureCollection;

#include "pointers/estdselfref.h"

struct eTileTerrainPainter {
    stdsptr<eTexture> fTex = nullptr;
    const eTextureCollection* fColl = nullptr;
    int fDrawDim = 1;

    stdsptr<eTexture> getTexture(const int frame) const;
};

#endif // ETILETERRAINPAINTER_H
