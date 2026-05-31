#ifndef LUMBERJACK_H
#define LUMBERJACK_H

#include "eresourcecollector.h"

class Lumberjack : public eResourceCollector {
public:
    Lumberjack(GameBoard& board);
};

#endif // LUMBERJACK_H
