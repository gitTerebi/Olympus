#ifndef ETILETERRAINPAINTER_H
#define ETILETERRAINPAINTER_H

class Texture;
class TextureCollection;

#include "pointers/estdselfref.h"

struct eTileTerrainPainter {
    stdsptr<Texture> fTex = nullptr;
    const TextureCollection* fColl = nullptr;
    int fDrawDim = 1;

    stdsptr<Texture> getTexture(const int frame) const;
};

#endif // ETILETERRAINPAINTER_H
