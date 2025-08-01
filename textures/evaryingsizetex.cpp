#include "evaryingsizetex.h"

eTile* eVaryingSizeTex::hiddenByNeighbour(
        eTile* const tile,
        const eWorldDirection dir,
        int& dx, int& dy) {
    for(int x = 0; x > -3; x--) {
        for(int y = 0; y > -3; y--) {
            if(x == 0 && y == 0) continue;
            const auto t = tile->tileRelRotated<eTile>(x, y, dir);
            if(!t) continue;
            const int d = t->drawDim();
            if(d > -x && d > -y) {
                dx = -x;
                dy = -y;
                return t;
            }
        }
    }
    return nullptr;
}

void eVaryingSizeTex::get(eTile* const tile,
                          const eChecker& checker,
                          int& drawDim,
                          const eWorldDirection dir) {
    int dx;
    int dy;
    eTile* const under = hiddenByNeighbour(tile, dir, dx, dy);
    if(under) {
        tile->setUnderTile(under, dx, dy);
        drawDim = 0;
        return;
    }

    bool allFit = true;
    for(int x = 0; x < 3; x++) {
        for(int y = 0; y < 3; y++) {
            if(x == 0 && y == 0) continue;
            const auto t = tile->tileRelRotated<eTile>(x, y, dir);
            if(!t || !checker(t) || hiddenByNeighbour(t, dir, dx, dy)) {
                allFit = false;
                break;
            }
        }
        if(!allFit) break;
    }
    if(allFit) {
        drawDim = 3;
        return;
    }

    allFit = true;
    for(int x = 0; x < 2; x++) {
        for(int y = 0; y < 2; y++) {
            if(x == 0 && y == 0) continue;
            const auto t = tile->tileRelRotated<eTile>(x, y, dir);
            if(!t || !checker(t) || hiddenByNeighbour(t, dir, dx, dy)) {
                allFit = false;
                break;
            }
        }
        if(!allFit) break;
    }
    if(allFit) {
        drawDim = 2;
        return;
    }

    drawDim = 1;
}

std::shared_ptr<eTexture> eVaryingSizeTex::getVaryingTexture(
        const eVaryingFunc& func,
        eTile* const tile,
        const eTextureCollection& small,
        const eTextureCollection& large,
        const eTextureCollection& huge,
        int& drawDim,
        const eWorldDirection dir) {
    func(tile, drawDim, dir);
    if(drawDim == 0) {
        return std::shared_ptr<eTexture>();
    } else if(drawDim == 1) {
        const auto& coll = small;
        const int texId = tile->seed() % coll.size();
        return coll.getTexture(texId);
    } else if(drawDim == 2) {
        const auto& coll = large;
        const int texId = tile->seed() % coll.size();
        return coll.getTexture(texId);
    } else if(drawDim == 3) {
        const auto& coll = huge;
        const int texId = tile->seed() % coll.size();
        return coll.getTexture(texId);
    }
    return std::shared_ptr<eTexture>();
}
