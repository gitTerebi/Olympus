#ifndef ESCROLLBAR_H
#define ESCROLLBAR_H

#include "ewidget.h"
#include "textures/game-textures.h"
#include "epainter.h"
#include "ebasicbutton.h"

#include <algorithm>
#include <functional>

class eScrollThumb : public eWidget {
public:
    using eScrollAction = std::function<void(int dy)>;

    eScrollThumb(MainWindow* const window) : eWidget(window) {}

    void setScrollAction(const eScrollAction& a) { mScrollAction = a; }

protected:
    void paintEvent(ePainter& p) override {
        int iRes, mult;
        iResAndMult(iRes, mult);
        const int dim = 8 * mult;
        const auto& intrfc = GameTextures::interface()[iRes];
        if(!intrfc.fLoaded) return;
        const auto& coll = mDragging ? intrfc.fButtonFrameHover : intrfc.fButtonFrame;
        const int iMax = width() / dim + 1;
        const int jMax = height() / dim + 1;
        const int lastX = width() - dim;
        const int lastY = height() - dim;
        for(int i = 0; i < iMax; i++) {
            const int x = i == iMax - 1 ? lastX : dim * i;
            for(int j = 0; j < jMax; j++) {
                int texId;
                if(i == 0) texId = (j == 0) ? 0 : (j == jMax-1) ? 6 : 7;
                else if(i == iMax-1) texId = (j == 0) ? 2 : (j == jMax-1) ? 4 : 3;
                else if(j == 0) texId = 1;
                else if(j == jMax-1) texId = 5;
                else continue;
                const int y = j == jMax - 1 ? lastY : dim * j;
                p.drawTexture(x, y, coll.getTexture(texId));
            }
        }
    }

    bool mousePressEvent(const eMouseEvent& e) override {
        mDragY = e.y();
        mDragging = true;
        return true;
    }

    bool mouseReleaseEvent(const eMouseEvent&) override {
        mDragging = false;
        return true;
    }

    bool mouseMoveEvent(const eMouseEvent& e) override {
        if(!mDragging) return false;
        const int delta = e.y() - mDragY;
        mDragY = e.y();
        if(mScrollAction) mScrollAction(delta);
        return true;
    }

private:
    bool mDragging = false;
    int mDragY = 0;
    eScrollAction mScrollAction;
};

class eScrollTrack : public eWidget {
public:
    using eClickAction = std::function<void(int clickY, int trackH)>;

    eScrollTrack(MainWindow* const window) : eWidget(window) {}

    void setClickAction(const eClickAction& a) { mClickAction = a; }

protected:
    bool mousePressEvent(const eMouseEvent& e) override {
        if(mClickAction) mClickAction(e.y(), height());
        return true;
    }

private:
    eClickAction mClickAction;
};

class eScrollViewport : public eWidget {
public:
    using eScrollCallback = std::function<void(int dy, int maxDy)>;

    using eWidget::eWidget;

    void setPage(eWidget* const page) {
        if(mPage) removeWidget(mPage);
        mPage = page;
        addWidget(mPage);
        clampDY();
    }

    void setScrollCallback(const eScrollCallback& cb) { mScrollCb = cb; }

    void scrollToTop() {
        mDy = 0;
        clampDY();
    }

    void scrollUp() {
        mDy -= scalePx(35);
        clampDY();
    }

    void scrollDown() {
        mDy += scalePx(35);
        clampDY();
    }

    void scrollToRatio(const float ratio) {
        if(!mPage) return;
        const int maxDy = std::max(0, mPage->height() - height());
        mDy = static_cast<int>(ratio * maxDy);
        clampDY();
    }

    void scrollByPixels(const int delta, const int trackH, const int thumbH) {
        if(!mPage) return;
        const int maxDy = std::max(0, mPage->height() - height());
        if(thumbH >= trackH || maxDy <= 0) return;
        const float ratio = static_cast<float>(maxDy) / (trackH - thumbH);
        mDy += static_cast<int>(delta * ratio);
        clampDY();
    }

    void clampDY() {
        if(!mPage) return;
        const int maxDy = std::max(0, mPage->height() - height());
        mDy = std::clamp(mDy, 0, maxDy);
        mPage->setY(-mDy);
        if(mScrollCb) mScrollCb(mDy, maxDy);
    }

protected:
    void paintEvent(ePainter& p) override {
        const auto r = rect();
        p.setClipRect(&r);
    }

    void postPaintEvent(ePainter& p) override {
        p.setClipRect(nullptr);
    }

    bool mouseWheelEvent(const eMouseWheelEvent& e) override {
        mDy -= scalePx(35) * e.dy();
        clampDY();
        return true;
    }

private:
    int mDy = 0;
    eWidget* mPage = nullptr;
    eScrollCallback mScrollCb;
};

class eScrollWidget;

class eScrollBar : public eWidget {
public:
    using eWidget::eWidget;

    void initialize(const int h) {
        setNoPadding();

        const auto probe = new eBasicButton(&InterfaceTextures::fBigUpButton, window());
        const int w = probe->width() / 2;
        probe->deleteLater();

        resize(w, h);
        mTrackH = h;

        const auto track = new eScrollTrack(window());
        track->setNoPadding();
        track->resize(w, h);
        track->move(0, 0);
        addWidget(track);

        mThumb = new eScrollThumb(window());
        mThumb->setNoPadding();
        mThumb->resize(w, h);
        mThumb->move(0, 0);
        addWidget(mThumb);

        mThumb->setScrollAction([this](const int delta) {
            if(mViewport) mViewport->scrollByPixels(delta, mTrackH, mThumb->height());
        });
        track->setClickAction([this](const int clickY, const int trackH) {
            if(!mViewport) return;
            const int thumbH = mThumb->height();
            const float ratio = static_cast<float>(clickY - thumbH / 2) / (trackH - thumbH);
            mViewport->scrollToRatio(std::clamp(ratio, 0.0f, 1.0f));
        });
    }

    void setViewport(eScrollViewport* const vp) {
        mViewport = vp;
        vp->setScrollCallback([this](const int dy, const int maxDy) {
            if(maxDy <= 0) {
                setVisible(false);
                return;
            }
            setVisible(true);
            const int thumbH = std::max(20, mTrackH * mTrackH / (mTrackH + maxDy));
            const int thumbY = static_cast<int>(static_cast<float>(dy) / maxDy * (mTrackH - thumbH));
            mThumb->setHeight(thumbH);
            mThumb->setY(thumbY);
        });
        vp->clampDY();
    }

    void setScrollWidget(eScrollWidget* const sw);
    void scrollUp();
    void scrollDown();

private:
    eScrollViewport* mViewport = nullptr;
    eScrollThumb* mThumb = nullptr;
    int mTrackH = 0;
    eScrollWidget* mSw = nullptr;
};

#endif // ESCROLLBAR_H
