#ifndef ELABEL_H
#define ELABEL_H

#include "ewidget.h"
#include "elabelbase.h"

#include <algorithm>
#include <cmath>

class eLabel : public eWidget, public eLabelBase {
public:
    eLabel(MainWindow* const window);
    eLabel(const std::string& text,
           MainWindow* const window);

    void setTextureDrawScale(const double scale) {
        mTextureDrawScale = std::max(0.1, scale);
        mHasTextureDrawScale = true;
    }
    double textureDrawScale() const { return mTextureDrawScale; }
    bool hasTextureDrawScale() const { return mHasTextureDrawScale; }
    void setTextureDrawBleed(const int bleed) {
        mTextureDrawBleed = std::max(0, bleed);
    }
    void fitOptions(const std::vector<std::string>& options);
    void renderTargetsReset() override;
protected:
    void sizeHint(int& w, int& h) override;
    void paintEvent(ePainter& p) override;
    void paintTexture(ePainter& p,
                      const std::shared_ptr<Texture>& tex,
                      Alignment align);
    int scaledTextureWidth(const std::shared_ptr<Texture>& tex) const;
    int scaledTextureHeight(const std::shared_ptr<Texture>& tex) const;
private:
    double mTextureDrawScale = 1.0;
    bool mHasTextureDrawScale = false;
    int mTextureDrawBleed = 0;
};

class eScaledTextureLabel : public eLabel {
public:
    using eLabel::eLabel;

    void setDrawHeight(const int height) {
        const auto& tex = texture();
        if(!tex || tex->height() <= 0) return;
        mDrawHeight = height;
        mDrawWidth = tex->width()*height/tex->height();
        if(mFitToDrawSize) resize(mDrawWidth, mDrawHeight);
    }

    void setMaxDrawHeight(const int height) {
        const auto& tex = texture();
        if(!tex) return;
        setDrawHeight(std::min(tex->height(), height));
    }

    void setFitToDrawSize(const bool fit) {
        mFitToDrawSize = fit;
        if(fit && mDrawWidth > 0 && mDrawHeight > 0) {
            resize(mDrawWidth, mDrawHeight);
        }
    }

    void setDrawOffset(const int x, const int y) {
        mDrawOffsetX = x;
        mDrawOffsetY = y;
    }
protected:
    void paintEvent(ePainter& p) override {
        const auto& tex = texture();
        if(!tex) return;
        if(mDrawWidth <= 0 || mDrawHeight <= 0) {
            eLabel::paintEvent(p);
            return;
        }

        const SDL_Rect srcRect{tex->x(), tex->y(), tex->width(), tex->height()};
        const SDL_Rect dstRect{p.x() + (width() - mDrawWidth)/2 + mDrawOffsetX,
                               p.y() + (height() - mDrawHeight)/2 + mDrawOffsetY,
                               mDrawWidth,
                               mDrawHeight};
        if(mR != 255 || mG != 255 || mB != 255) {
            tex->setColorMod(mR, mG, mB);
        }
        tex->render(p.renderer(), srcRect, dstRect);
        if(mR != 255 || mG != 255 || mB != 255) {
            tex->clearColorMod();
        }
    }
private:
    int mDrawWidth = 0;
    int mDrawHeight = 0;
    int mDrawOffsetX = 0;
    int mDrawOffsetY = 0;
    bool mFitToDrawSize = false;
};

#endif // ELABEL_H
