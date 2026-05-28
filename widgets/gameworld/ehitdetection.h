#ifndef EHITDETECTION_H
#define EHITDETECTION_H

class GameBoard;

void pixToId(int pixX, int pixY, int &idX, int &idY, int scale, int tileW, int tileH, int dx, int dy, int maxAlt, int minAlt, const GameBoard *board);

#endif // EHITDETECTION_H
