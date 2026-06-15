#ifndef EELITEHOUSINGRENDERER_H
#define EELITEHOUSINGRENDERER_H

#include "ebuildingrenderer.h"

class EliteHousing;

enum class eEliteRendererType {
    left, bottom, top, right
};

class eEliteHousingRenderer : public eBuildingRenderer {
public:
    eEliteHousingRenderer(const eEliteRendererType type,
                          const stdsptr<EliteHousing>& b);

    std::shared_ptr<Texture>
        getTexture(const eTileSize size) const;
    std::vector<Overlay>
        getOverlays(const eTileSize size) const;

    int spanW() const { return 2; }
    int spanH() const { return 2; }

    bool isMain() const { return false; }

    eEliteRendererType etype() const { return mType; }
private:
    const eEliteRendererType mType;
    const stdsptr<EliteHousing> mBuilding;
};

#endif // EELITEHOUSINGRENDERER_H
