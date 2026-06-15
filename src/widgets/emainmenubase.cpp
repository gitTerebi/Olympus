#include "emainmenubase.h"

#include "textures/game-textures.h"

void eMainMenuBase::initialize() {
    const auto& intrfc = GameTextures::interface();
    const auto res = resolution();
    const int iRes = static_cast<int>(res.uiScale());
    const auto& texs = intrfc[iRes];
    setTexture(texs.fMainMenuImage);
}

#include <algorithm>

void eMainMenuBase::renderTargetsReset() {
    eLabel::renderTargetsReset();
    mBackgroundCache.reset();
}

void eMainMenuBase::updateBackgroundCache(ePainter& p) {
    const auto& tex = texture();
    if(!tex) return;

    const int ww = width();
    const int wh = height();
    if(ww <= 0 || wh <= 0) return;

    if(mBackgroundCache &&
       mBackgroundCache->width() == ww &&
       mBackgroundCache->height() == wh) {
        return;
    }

    int tw;
    int th;
    textureSize(tw, th);
    if(tw <= 0 || th <= 0) return;

    auto cache = std::make_shared<Texture>();
    const auto r = p.renderer();
    if(!cache->create(r, ww, wh)) return;
    SDL_SetTextureBlendMode(cache->tex(), SDL_BLENDMODE_NONE);

    const auto prevTarget = SDL_GetRenderTarget(r);
    SDL_BlendMode prevBlendMode;
    SDL_GetRenderDrawBlendMode(r, &prevBlendMode);
    cache->setAsRenderTarget(r);
    SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
    SDL_RenderClear(r);

    const float scale = std::max((float)ww / tw, (float)wh / th);
    const int dw = (int)(tw * scale);
    const int dh = (int)(th * scale);
    const int x = (ww - dw) / 2;
    const int y = (wh - dh) / 2;
    const SDL_Rect srcRect{0, 0, tw, th};
    const SDL_Rect dstRect{x, y, dw, dh};
    tex->render(r, srcRect, dstRect);

    SDL_SetRenderTarget(r, prevTarget);
    SDL_SetRenderDrawBlendMode(r, prevBlendMode);
    mBackgroundCache = cache;
}

void eMainMenuBase::paintEvent(ePainter& p) {
    updateBackgroundCache(p);
    if(mBackgroundCache) {
        mBackgroundCache->render(p.renderer(), p.x(), p.y());
        return;
    }

    const auto& tex = texture();
    if(tex) {
        const int ww = width();
        const int wh = height();
        int tw, th;
        textureSize(tw, th);
        if(tw > 0 && th > 0) {
            const float scale = std::max((float)ww / tw, (float)wh / th);
            const int dw = (int)(tw * scale);
            const int dh = (int)(th * scale);
            const int x = (ww - dw) / 2 + p.x();
            const int y = (wh - dh) / 2 + p.y();
            const SDL_Rect srcRect{0, 0, tw, th};
            const SDL_Rect dstRect{x, y, dw, dh};
            tex->render(p.renderer(), srcRect, dstRect);
        }
    }
}
