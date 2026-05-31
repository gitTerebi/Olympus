#ifndef EHEATMAP_H
#define EHEATMAP_H

#include <vector>
#include <SDL2/SDL_rect.h>

// Vanilla Zeus ring formula:
//   ring N (N=1..) has appeal = fValue + (N-1)*fStepSize
//   ring N covers tiles dist (N-1)*fStepTiles+1 .. N*fStepTiles
//   capped at fRange tiles total.
// For nice buildings fStepSize is negative (fade out),
// for ugly buildings fStepSize is positive (penalty fades back to 0).
struct eHeat {
    int fValue;       // ring 1 strength
    int fStepTiles;   // tiles per ring (defaults to 1 if 0)
    int fStepSize;    // delta per ring (signed)
    int fRange;       // max range in tiles
};

class eHeatMap {
public:
    struct eHeatTile {
        bool fOutsideRange = false;
        bool fEnabled = false;
        double fAppeal = 0;
    };

    void add(const eHeatMap& other);
    void multiply(const eHeatMap& other);

    void reset();
    void initialize(const int dx, const int dy,
                    const int w, const int h);

    void addHeat(const eHeat& a,
                 const SDL_Rect& tileRect);

    void addHeat(const eHeat& a,
                 const int ax, const int ay,
                 const int sw, const int sh);

    void addHeat(const int x, const int y,
                 const double a);

    void setOutsideRange(const int x, const int y);
    bool enabled(const int x, const int y) const;
    double heat(const int x, const int y) const;

    int dx() const { return mDX; }
    int dy() const { return mDY; }

    int width() const { return mWidth; }
    int height() const { return mHeight; }

    void set(const int x, const int y,
             const bool e, const double h);
private:
    int mDX = 0;
    int mDY = 0;
    int mWidth = 0;
    int mHeight = 0;
    std::vector<std::vector<eHeatTile>> mMap;
};

#endif // EHEATMAP_H
