#include "framed-button.h"

#include "textures/game-textures.h"

#include <random>

void FramedButton::setRenderBg(const bool r) {
    if(mRenderBg == r) return;
    mRenderBg = r;
    resetFrameCache();
}

void FramedButton::setNoBorder(const bool b) {
    if(mNoBorder == b) return;
    mNoBorder = b;
    resetFrameCache();
}

void FramedButton::renderTargetsReset() {
    eButton::renderTargetsReset();
    resetFrameCache();
}

void FramedButton::resetFrameCache() {
    mFrameCache.reset();
    mHoverFrameCache.reset();
}

std::shared_ptr<Texture>& FramedButton::frameCache(const bool hover) {
    return hover ? mHoverFrameCache : mFrameCache;
}

void FramedButton::paintEvent(ePainter& p) {
    if(!enabled()) {
        if(mRenderBg) renderBg(p);
        // draw border normally
        int iRes; double mult; iResAndMult(iRes, mult);
        const int dim = GameTextures::interfaceTileDim();
        const auto& intrfc = GameTextures::interface()[iRes];
        if(intrfc.fLoaded) {
            const auto& coll = intrfc.fButtonFrame;
            const int iMax = width()/dim + 1;
            const int jMax = height()/dim + 1;
            const int lastX = width() - dim;
            const int lastY = height() - dim;
            for(int i = 0; i < iMax; i++) {
                const int x = i == iMax-1 ? lastX : dim*i;
                for(int j = 0; j < jMax; j++) {
                    int texId;
                    if(i==0)            texId = (j==0)?0:(j==jMax-1)?6:7;
                    else if(i==iMax-1)  texId = (j==0)?2:(j==jMax-1)?4:3;
                    else                texId = (j==0)?1:(j==jMax-1)?5:-1;
                    if(texId == -1) continue;
                    const int y = j == jMax-1 ? lastY : dim*j;
                    const auto& tex = coll.getTexture(texId);
                    tex->setAlpha(80);
                    p.drawTexture(x, y, tex);
                    tex->clearAlphaMod();
                }
            }
        }
        // draw text greyed
        const auto& tex = texture();
        if(tex) {
            tex->setColorMod(120, 120, 120);
            p.drawTexture(rect(), tex, textAlignment());
            tex->clearColorMod();
        }
        return;
    }
    if(mNoBorder) { eButton::paintEvent(p); return; }

    renderFrameCache(p, hovered());
    eButton::paintEvent(p);
}

void FramedButton::renderFrameCache(ePainter& p, const bool hover) {
    auto& cache = frameCache(hover);
    if(cache && cache->width() == width() && cache->height() == height()) {
        cache->render(p.renderer(), p.x(), p.y());
        return;
    }

    int iRes;
    double mult;
    iResAndMult(iRes, mult);
    const int dim = GameTextures::interfaceTileDim();
    const auto& intrfc = GameTextures::interface()[iRes];
    if(!intrfc.fLoaded) return;

    auto next = std::make_shared<Texture>();
    const auto r = p.renderer();
    if(!next->create(r, width(), height())) return;
    SDL_SetTextureBlendMode(next->tex(), SDL_BLENDMODE_BLEND);
    const auto prevTarget = SDL_GetRenderTarget(r);
    next->setAsRenderTarget(r);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
    SDL_RenderClear(r);
    ePainter cachePainter(r);

    if(mRenderBg) renderBg(cachePainter);

    const TextureCollection* coll = nullptr;
    if(hover) {
        coll = &intrfc.fButtonFrameHover;
    } else {
        coll = &intrfc.fButtonFrame;
    }

    const int iMax = width()/dim + 1;
    const int jMax = height()/dim + 1;

    const int lastX = width() - dim;
    const int lastY = height() - dim;

    const auto texCollId = [&](const int i, const int j) {
        int texId;
        if(i == 0) {
            if(j == 0) {
                texId = 0;
            } else if(j == jMax - 1) {
                texId = 6;
            } else {
                texId = 7;
            }
        } else if(i == iMax - 1) {
            if(j == 0) {
                texId = 2;
            } else if(j == jMax - 1) {
                texId = 4;
            } else {
                texId = 3;
            }
        } else if(j == 0) {
            texId = 1;
        } else if(j == jMax - 1) {
            texId = 5;
        } else {
            texId = -1;
        }
        return texId;
    };

    for(int i = 0; i < iMax; i++) {
        const int x = i == iMax - 1 ? lastX : dim*i;
        for(int j = 0; j < jMax; j++) {
            const int texId = texCollId(i, j);
            if(texId == -1) continue;
            const auto& tex = coll->getTexture(texId);
            const int y = j == jMax - 1 ? lastY : dim*j;
            cachePainter.drawTexture(x, y, tex);
        }
    }
    SDL_SetRenderTarget(r, prevTarget);
    cache = next;
    cache->render(r, p.x(), p.y());
}

void FramedButton::renderBg(ePainter& p) {
    int iRes;
    double mult;
    iResAndMult(iRes, mult);
    const int dim = GameTextures::interfaceTileDim();
    const auto& intrfc = GameTextures::interface()[iRes];
    if(!intrfc.fLoaded) return;

    const int iMax = width()/dim + 1;
    const int jMax = height()/dim + 1;

    const int lastX = width() - dim;
    const int lastY = height() - dim;

    const auto colls = &intrfc.fMessageBox;
    const auto& coll = (*colls)[4];
    std::default_random_engine rng{1};
    std::uniform_int_distribution<> dist{0, 100000};
    for(int i = 0; i < iMax; i++) {
        const int x = i == iMax - 1 ? lastX : dim*i;
        for(int j = 0; j < jMax; j++) {
            const int rand = dist(rng);
            const int texId = rand % coll.size();
            const auto& tex = coll.getTexture(texId);
            const int y = j == jMax - 1 ? lastY : dim*j;
            p.drawTexture(x, y, tex);
        }
    }
}
