#include "ebitmapwidget.h"

#include "textures/game-textures.h"

void eBitmapWidget::sizeHint(int& w, int& h) {
    const auto res = resolution();
    const double mult = res.multiplier();
    w = std::round(mult*mScaling*400);
    h = std::round(mult*mScaling*300);
}

void eBitmapWidget::paintEvent(ePainter& p) {
    const auto res = resolution();
    const double mult = res.multiplier();
    const auto tex = texture();
    const int tw = tex->width();
    const int th = tex->height();
    const SDL_Rect srcRect{0, 0, tw, th};
    const int w = std::round(mult*mScaling*400);
    const int h = std::round(mult*mScaling*300);
    const SDL_Rect dstRect{p.x(), p.y(), w, h};
    SDL_RenderCopy(p.renderer(), tex->tex(), &srcRect, &dstRect);
}

stdsptr<Texture> eBitmapWidget::texture() const {
    const auto res = resolution();
    const auto uiScale = res.uiScale();
    const int iRes = static_cast<int>(uiScale);
    const auto& intrfc = GameTextures::interface()[iRes];
    if(mBitmap == 0) {
        return intrfc.fLoadImage1;
    } else if(mBitmap == 1) {
        return intrfc.fLoadImage2;
    } else if(mBitmap == 2) {
        return intrfc.fLoadImage3;
    } else if(mBitmap == 3) {
        return intrfc.fLoadImage4;
    } else if(mBitmap == 4) {
        return intrfc.fLoadImage5;
    } else if(mBitmap == 5) {
        return intrfc.fLoadImage6;
    } else if(mBitmap == 6) {
        return intrfc.fLoadImage7;
    } else if(mBitmap == 7) {
        return intrfc.fLoadImage8;
    } else if(mBitmap == 8) {
        return intrfc.fLoadImage9;
    } else if(mBitmap == 9) {
        return intrfc.fLoadImage10;
    } else if(mBitmap == 10) {
        return intrfc.fLoadImage11;
    } else if(mBitmap == 11) {
        return intrfc.fLoadImage12;
    } else if(mBitmap == 12) {
        GameTextures::loadPoseidonCampaign1();
        return intrfc.fPoseidonCampaign1;
    } else if(mBitmap == 13) {
        GameTextures::loadPoseidonCampaign2();
        return intrfc.fPoseidonCampaign2;
    } else if(mBitmap == 14) {
        GameTextures::loadPoseidonCampaign3();
        return intrfc.fPoseidonCampaign3;
    } else if(mBitmap == 15) {
        GameTextures::loadPoseidonCampaign4();
        return intrfc.fPoseidonCampaign4;
    } else if(mBitmap == 16) {
        GameTextures::loadPoseidonCampaign5();
        return intrfc.fPoseidonCampaign5;
    } else if(mBitmap == 17) {
        GameTextures::loadPoseidonCampaign6();
        return intrfc.fPoseidonCampaign6;
    }
    return intrfc.fLoadImage1;
}
