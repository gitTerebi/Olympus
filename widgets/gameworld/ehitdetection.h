#ifndef EHITDETECTION_H
#define EHITDETECTION_H

class eGameBoard;

void pixToId(int pixX, int pixY, int &idX, int &idY, int scale, int tileW, int tileH, int dx, int dy, int maxAlt, int minAlt, const eGameBoard *board);

#endif // EHITDETECTION_H
