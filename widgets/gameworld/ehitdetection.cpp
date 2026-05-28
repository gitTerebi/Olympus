#include "ehitdetection.h"

#include "etilehelper.h"
#include "engine/etile.h"
#include "engine/e-game-board.h"

#include <vector>
#include <algorithm>
#include <cmath>

namespace {

int calc_adjust_with_percentage(int value, int percentage) {
    return value * percentage / 100;
}

}

void pixToId(int pixX, int pixY, int &idX, int &idY, int scale, int tileW, int tileH, int dx, int dy, int maxAlt, int minAlt, const GameBoard *board) {
    const int scaledX = calc_adjust_with_percentage(pixX, scale);
    const int scaledY = calc_adjust_with_percentage(pixY, scale);
    const double w = tileW;
    const double h = tileH;
    idX = std::round((scaledX - dx) / w + (scaledY - dy) / h - 0.5);
    idY = std::round(-(scaledX - dx) / w + (scaledY - dy) / h - 0.5);

    const auto dir = board->direction();
    const int width = board->width();
    const int height = board->height();

    struct Candidate {
        int x, y;
        int altitude;
        double dist;
        bool contains;
    };
    std::vector<Candidate> candidates;

    for (int x = idX + 2 * maxAlt; x >= idX + 2 * minAlt; x--)
    {
        for (int y = idY + 2 * maxAlt; y >= idY + 2 * minAlt; y--)
        {
            int rx;
            int ry;
            eTileHelper::rotatedTileIdToTileId(x, y, rx, ry, dir, width, height);
            const auto t = board->tile(rx, ry);
            if (!t)
                continue;
            const int a = t->altitude();
            const int ddx = 0;
            const int ddy = -a * 2 + 1;
            const int tpx = std::round(0.5 * (x - y + ddx) * tileW) + dx;
            const int tpy = std::round(0.5 * (x + y + ddy) * tileH) + dy;
            const double diffX = tpx - scaledX;
            const double diffY = tpy - scaledY;
            const double dist = std::sqrt(diffX * diffX + diffY * diffY);
            const double diamond =
                std::abs(diffX) / (0.5 * tileW) +
                std::abs(diffY) / (0.5 * tileH);
            candidates.push_back({x, y, a, dist, diamond <= 1.0});
        }
    }

    if (!candidates.empty())
    {
        auto minIt = std::min_element(candidates.begin(), candidates.end(), [](const Candidate &a, const Candidate &b) {
            if (a.contains != b.contains)
                return a.contains;
            if (a.contains && a.altitude != b.altitude)
                return a.altitude > b.altitude;
            return a.dist < b.dist;
        });
        idX = minIt->x;
        idY = minIt->y;
    }

    const int idXT = idX;
    const int idYT = idY;
    eTileHelper::rotatedTileIdToTileId(idXT, idYT, idX, idY,
                                       dir, width, height);
}
