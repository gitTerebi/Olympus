#ifndef ETHREADBOARD_H
#define ETHREADBOARD_H

#include "ethreadtile.h"

class eThreadBoard {
public:
    void initialize(const int w, const int h);
    void clear();

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    eThreadTile* tile(const int x, const int y);
    eThreadTile* dtile(const int x, const int y);

    int state() const { return mState; }
    void setState(const int s) { mState = s; }

    int allBuildingsState() const { return mAllBuildingsState; }
    void setAllBuildingsState(const int s) { mAllBuildingsState = s; }
private:
    void updateNeighbours();

    int mState = 0;
    int mAllBuildingsState = -1;

    int mWidth = 0;
    int mHeight = 0;
    std::vector<std::vector<eThreadTile>> mTiles;
};

#endif // ETHREADBOARD_H
