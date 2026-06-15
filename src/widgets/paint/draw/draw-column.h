#ifndef DRAW_COLUMN_H
#define DRAW_COLUMN_H

class eTilePainter;
class TextureCollection;

void drawColumn(eTilePainter& tp, const int n,
                const double drawX, const double drawY,
                const TextureCollection& coll);

#endif // DRAW_COLUMN_H
