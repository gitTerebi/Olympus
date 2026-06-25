#include "etilepainter.h"

eTilePainter::eTilePainter(ePainter& p,
                           const eTileSize size,
                           const int tw, const int th) :
    mP(p), mSize(size), mTileW(tw), mTileH(th) {
    mScheduled.reserve(128);
    mSaves.reserve(16);
}

void eTilePainter::save() {
    eTilePainterSave save;
    save.fX = mX;
    save.fY = mY;
    mSaves.emplace(mSaves.end(), save);
}

void eTilePainter::restore() {
    if(mSaves.empty()) return;
    const auto save = mSaves.back();
    mSaves.pop_back();
    mX = save.fX;
    mY = save.fY;
}

void eTilePainter::translate(const double x, const double y) {
    mX += x;
    mY += y;
}

void eTilePainter::drawTexture(const double x, const double y,
                               const std::shared_ptr<Texture>& tex,
                               const Alignment align) const {
    drawTexture(x, y, tex.get(), align);
}

void eTilePainter::drawTexture(const double x, const double y,
                               Texture* tex,
                               const Alignment align) const {
    if(!tex) return;
    const double dx = mTileH*tex->offsetX()/30.;
    const double dy = mTileH*tex->offsetY()/30.;
    int pixX;
    int pixY;
    drawPositon(x, y, pixX, pixY, dx, dy);
    mP.drawTexture(pixX, pixY, tex, align);
}

void eTilePainter::drawTexture(const double x, const double y,
                               const std::shared_ptr<Texture>& tex) const {
    drawTexture(x, y, tex.get());
}

void eTilePainter::drawTexture(const double x, const double y,
                               Texture* tex) const {
    if(!tex) return;
    const double dx = mTileH*tex->offsetX()/30.;
    const double dy = mTileH*tex->offsetY()/30.;
    int pixX;
    int pixY;
    drawPositon(x, y, pixX, pixY, dx, dy);
    mP.drawTexture(pixX, pixY, tex);
}

void eTilePainter::scheduleDrawTexture(const double x, const double y,
                                       const std::shared_ptr<Texture>& tex) {
    auto& s = mScheduled.emplace_back();
    s.fX = x;
    s.fY = y;
    s.fTex = tex.get();
}

void eTilePainter::scheduleDrawTexture(const double x, const double y,
                                       const std::shared_ptr<Texture>& tex,
                                       const Alignment align) {
    auto& s = mScheduled.emplace_back();
    s.fX = x;
    s.fY = y;
    s.fTex = tex.get();
    s.fHasAlign = true;
    s.fAlign = align;
}

void eTilePainter::scheduleDrawTexture(const double x, const double y,
                                       const std::shared_ptr<Texture>& tex,
                                       const Alignment align,
                                       const SDL_Color& colorMod) {
    auto& s = mScheduled.emplace_back();
    s.fX = x;
    s.fY = y;
    s.fTex = tex.get();
    s.fHasAlign = true;
    s.fAlign = align;
    s.fHasColorMod = true;
    s.fColorMod = colorMod;
}

void eTilePainter::handleScheduledDraw() {
    for(const auto& s : mScheduled) {
        if(s.fHasColorMod && s.fTex) {
            s.fTex->setColorMod(s.fColorMod.r, s.fColorMod.g, s.fColorMod.b);
        }
        if(s.fHasAlign) {
            drawTexture(s.fX, s.fY, s.fTex, s.fAlign);
        } else {
            drawTexture(s.fX, s.fY, s.fTex);
        }
        if(s.fHasColorMod && s.fTex) {
            s.fTex->clearColorMod();
        }
    }
    mScheduled.clear();
}

void eTilePainter::drawPolygon(const std::vector<SDL_Point>& pts,
                               const SDL_Color& color) const {
    std::vector<SDL_Point> ppts;
    ppts.reserve(pts.size());
    for(const auto& pt : pts) {
        int pixX;
        int pixY;
        drawPositon(pt.x, pt.y, pixX, pixY);
        ppts.push_back({pixX, pixY});
    }
    mP.drawPolygon(ppts, color);
}

void eTilePainter::fillRectCenter(const double x, const double y,
                                  const int w, const int h,
                                  const SDL_Color& color) const {
    int pixX;
    int pixY;
    drawPositon(x, y, pixX, pixY);
    mP.fillRect(SDL_Rect{pixX - w/2, pixY - h/2, w, h}, color);
}

void eTilePainter::fillRect(const double x, const double y,
                            const int w, const int h,
                            const SDL_Color& color) const {
    int pixX;
    int pixY;
    drawPositon(x, y, pixX, pixY);
    mP.fillRect(SDL_Rect{pixX, pixY, w, h}, color);
}

void eTilePainter::drawPositon(const double x, const double y,
                               int& pixX, int& pixY,
                               const double dx, const double dy) const {
    const double rx = x + mX;
    const double ry = y + mY;
    pixX = std::round(0.5 * (rx - ry) * mTileW - dx);
    pixY = std::round(0.5 * (rx + ry) * mTileH - dy);
}
