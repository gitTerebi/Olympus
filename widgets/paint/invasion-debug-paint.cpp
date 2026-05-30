#include "widgets/paint/invasion-debug-paint.h"

#include "engine/game-board.h"
#include "engine/etile.h"
#include "etilehelper.h"
#include "gameEvents/invasions/invasion-handler.h"
#include "widgets/epainter.h"

#include <cmath>
#include <vector>

namespace {

using InvasionHandlers = std::vector<eInvasionHandler*>;

InvasionHandlers activeInvasionHandlers(GameBoard& board)
{
    InvasionHandlers result;
    for(int i = 0; i < 10; i++) {
        const auto cid = static_cast<eCityId>(i);
        const auto handlers = board.invasionHandlers(cid);
        for(const auto h : handlers) {
            if(h) result.push_back(h);
        }
    }
    return result;
}

SDL_Point tilePoint(GameBoard& board,
                    eTile* const tile,
                    const int tileW,
                    const int tileH)
{
    int rtx;
    int rty;
    eTileHelper::tileIdToRotatedTileId(tile->x(), tile->y(), rtx, rty,
                                       board.direction(),
                                       board.width(), board.height());
    const int a = tile->altitude();
    return {
        (rtx - rty)*tileW/2,
        (rtx + rty - 2*a)*tileH/2
    };
}

void drawDiamond(ePainter& p,
                 const SDL_Point c,
                 const int tileW,
                 const int tileH,
                 const SDL_Color color)
{
    const int w = std::max(18, tileW);
    const int h = std::max(12, tileH);
    const std::vector<SDL_Point> pts{
        {c.x, c.y - h/2},
        {c.x + w/2, c.y},
        {c.x, c.y + h/2},
        {c.x - w/2, c.y},
        {c.x, c.y - h/2}
    };
    p.drawPolygon(pts, color);
}

void drawCross(ePainter& p,
               const SDL_Point c,
               const int size,
               const SDL_Color color)
{
    p.drawPolygon({{c.x - size, c.y}, {c.x + size, c.y}}, color);
    p.drawPolygon({{c.x, c.y - size}, {c.x, c.y + size}}, color);
}

void drawLine(ePainter& p,
              const SDL_Point a,
              const SDL_Point b,
              const SDL_Color color)
{
    const double dx = b.x - a.x;
    const double dy = b.y - a.y;
    const double len = std::sqrt(dx*dx + dy*dy);
    if(len < 1.0) return;
    const double px = -dy/len;
    const double py = dx/len;
    const int width = 22;
    const std::vector<SDL_Point> shaft{
        {int(a.x + px*width), int(a.y + py*width)},
        {int(b.x + px*width), int(b.y + py*width)},
        {int(b.x - px*width), int(b.y - py*width)},
        {int(a.x - px*width), int(a.y - py*width)}
    };
    p.drawPolygon(shaft, color);
}

void drawLabel(ePainter& p,
               const SDL_Point c,
               const std::string& text,
               const SDL_Color bg)
{
    const SDL_Rect rect{c.x - 26, c.y - 38, 52, 18};
    p.fillRect(rect, bg);
    p.drawText(rect, text, eFontColor::light,
               eAlignment::hcenter | eAlignment::vcenter);
}

}

void paintInvasionDebugTargets(GameBoard& board,
                               const eCityId viewedCity,
                               ePainter& p,
                               const int tileW,
                               const int tileH,
                               const int animFrame)
{
    (void)viewedCity;
    for(const auto invasion : activeInvasionHandlers(board)) {
        if(!invasion) continue;
        // Draw the last order the general issued: FROM = where the army was when
        // it was given, GOAL = where the order sends them (half-step or building).
        // Both are captured per-order so the arrow re-aims on every new move.
        const auto from = invasion->moveFromTile();
        const auto target = invasion->moveToTile();
        if(!from || !target) continue;

        const auto a = tilePoint(board, from, tileW, tileH);
        const auto b = tilePoint(board, target, tileW, tileH);
        const SDL_Color line{255, 25, 25, 220};
        const SDL_Color start{255, 230, 40, 230};
        const SDL_Color end{255, 25, 25, 255};
        drawLine(p, a, b, line);
        drawDiamond(p, a, tileW, tileH, start);
        drawDiamond(p, b, tileW, tileH, end);
        drawLabel(p, a, "FROM", SDL_Color{80, 70, 0, 210});
        drawLabel(p, b, "GOAL", SDL_Color{90, 0, 0, 220});

        const int pulse = 10 + (animFrame / 8) % 8;
        drawCross(p, b, pulse, SDL_Color{255, 255, 255, 240});
    }
}
