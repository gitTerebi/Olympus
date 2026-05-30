#ifndef DRAW_COLUMN_H
#define DRAW_COLUMN_H

class eTilePainter;
class eTextureCollection;

void drawColumn(eTilePainter& tp, const int n,
                const double rx, const double ry,
                const eTextureCollection& coll);

#endif // DRAW_COLUMN_H
