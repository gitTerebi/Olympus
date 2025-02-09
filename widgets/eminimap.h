#ifndef EMINIMAP_H
#define EMINIMAP_H

#include "ewidget.h"

#include "engine/egameboard.h"

struct eDirectionTexture {
    bool fTotalUpdateScheduled = true;
    bool fUpdateScheduled = false;
    std::shared_ptr<eTexture> fTexture;
    std::vector<eTile*> fTilesToUpdate;
};

class eMiniMap : public eWidget {
public:
    eMiniMap(eMainWindow* const window);

    void renderTargetsReset() override;

    void setBoard(eGameBoard* const board);

    void setDirection(const eWorldDirection dir);

    void setChangeAction(const eAction& act);

    void viewFraction(const double fx, const double fy);
    void viewedFraction(double& fx, double& fy);

    void viewTile(const int tileX, const int tileY);
    void viewedTile(int& tileX, int& tileY) const;

    void setViewBoxSize(const double fx, const double fy);

    void scheduleUpdate();
    void scheduleTotalUpdate();
    void scheduleTilesUpdate(const std::vector<eTile*>& tiles);
protected:
    bool mousePressEvent(const eMouseEvent& e) override;
    bool mouseMoveEvent(const eMouseEvent& e) override;
    bool mouseReleaseEvent(const eMouseEvent& e) override;

    void paintEvent(ePainter& p) override;
private:
    void updateTexture(const eCityId cid);
    void viewRelPix(const int pixX, const int pixY);
    void viewAbsPix(const int px, const int py);

    int mTime = 0;
    int mCityCounter = 0;

    int mMouseX = 0;
    int mMouseY = 0;

    int mCenterX = 0;
    int mCenterY = 0;

    int mDrawX = 0;
    int mDrawY = 0;

    double mViewBoxW = 0.2;
    double mViewBoxH = 0.2;

    int mTDim = 2;

    eGameBoard* mBoard = nullptr;

    std::map<eWorldDirection, eDirectionTexture> mTextures;
    eWorldDirection mDir = eWorldDirection::N;
    eDirectionTexture* mTexture = nullptr;

    eAction mChangeAction;
};

#endif // EMINIMAP_H
