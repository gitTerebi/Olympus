#ifndef EATHENA_H
#define EATHENA_H

#include "eextendedgod.h"

class eApollo : public eExtendedGod {
public:
    eApollo(GameBoard& board);
};

class eArtemis : public eExtendedGod {
public:
    eArtemis(GameBoard& board);
};

class eAthena : public eExtendedGod {
public:
    eAthena(GameBoard& board);
};

class eAtlas : public eExtendedGod {
public:
    eAtlas(GameBoard& board);
};

class eZeus : public eExtendedGod {
public:
    eZeus(GameBoard& board);
};

#endif // EATHENA_H
